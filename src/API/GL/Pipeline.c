/************************************************************************
* Copyright (C) 2020 Trevor Hanz
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
************************************************************************/

#include <GraphicsPipeline/Pipeline.h>
#include <GraphicsPipeline/Logging.h>

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#include "GL.h"

#ifndef __APPLE__
#include <GL/glew.h>
#else // __APPLE__
// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif // __APPLE__
#else // GP_GL
#include "GL.h"
#endif

#include <stdlib.h>
#include <stdint.h>

const gp_draw_mode GP_MODE_TRIANGLES = GL_TRIANGLES;
const gp_draw_mode GP_MODE_TRIANGLE_STRIP = GL_TRIANGLE_STRIP;

// Apple doesn't seem to have support for debug callbacks
#if !defined(__APPLE__) && defined(GP_GL)
void DebugCallbackFunction(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* userParam)
{
  switch(type)
  {
  case GL_DEBUG_TYPE_ERROR:
    gp_log_error(message);
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
  case GL_DEBUG_TYPE_PORTABILITY:
    gp_log_warn(message);
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
  case GL_DEBUG_TYPE_OTHER:
    gp_log_debug(message);
    break;
  }
}
#endif

#define CHECK_GL_ERROR() \
{\
  GLenum err;\
  while((err = glGetError()) != GL_NO_ERROR) {\
    gp_log_error("GL ERROR (%d) at: %s:%d", err, __FILE__, __LINE__);\
  }\
}

void gp_operation_ref(gp_operation* operation)
{
  gp_ref_inc(&operation->mRef);
}

void gp_operation_unref(gp_operation* operation)
{
  if(gp_ref_dec(&operation->mRef))
  {
    operation->mData->free(operation->mData);
    free(operation);
  }
}

typedef struct
{
  _gp_operation_data      mData;
  float                   mColor[4]; // RGBA
} _gp_operation_clear_data;

void _gp_operation_clear(_gp_operation_data* data)
{
  _gp_operation_clear_data* d = (_gp_operation_clear_data*)data;
  
  glClearColor(d->mColor[0], d->mColor[1], d->mColor[2], d->mColor[3]);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

gp_operation* gp_operation_clear_new()
{
  gp_operation* operation = malloc(sizeof(gp_operation));
  operation->func = _gp_operation_clear;
  operation->mData = malloc(sizeof(_gp_operation_clear_data));
  operation->mData->free = (void(*)(_gp_operation_data*))free;
  gp_ref_init(&operation->mRef);
  
  _gp_operation_clear_data* d = (_gp_operation_clear_data*)operation->mData;
  d->mColor[0] = 0;
  d->mColor[1] = 0;
  d->mColor[2] = 0;
  d->mColor[3] = 1;
  
  return operation;
}

void gp_operation_clear_set_color(gp_operation* operation, float r, float g, float b, float a)
{
  _gp_operation_clear_data* d = (_gp_operation_clear_data*)operation->mData;
  d->mColor[0] = r;
  d->mColor[1] = g;
  d->mColor[2] = b;
  d->mColor[3] = a;
}

typedef struct _gp_array_list gp_array_list;

struct _gp_array_list
{
  gp_list_node            mNode;
  gp_array*               mArray;
  int                     mIndex;
  int                     mComponents;
  int                     mStride;
  uintptr_t               mOffset;
};

typedef struct _gp_uniform_list gp_uniform_list;

struct _gp_uniform_list
{
  gp_list_node            mNode;
  gp_uniform*             mUniform;
};

typedef struct
{
  _gp_operation_data      mData;
#ifndef GP_GLES2
  GLuint                  mVAO;
  uint8_t                 mDirty;
#endif
  gp_shader*              mShader;
  gp_list                 mArrays;
  gp_list                 mUniforms;
  unsigned int            mVerticies;
  gp_draw_mode            mMode;
} _gp_operation_draw_data;

void _gp_operation_draw(_gp_operation_data* data)
{
  _gp_operation_draw_data* d = (_gp_operation_draw_data*)data;
  
  CHECK_GL_ERROR();
  
  glUseProgram(d->mShader->mProgram);
  
  gp_list_node* node = gp_list_front(&d->mUniforms);
  while(node != NULL)
  {
    gp_uniform_list* uniform = (gp_uniform_list*)node;
    uniform->mUniform->mOperation(uniform->mUniform);
    
    node = gp_list_node_next(node);
  }
  
#ifndef GP_GLES2
  if(d->mVAO == 0)
    glGenVertexArrays(1, &d->mVAO);
  
  glBindVertexArray(d->mVAO);
  if(d->mDirty == 1)
  {
#endif
  node = gp_list_front(&d->mArrays);
  while(node != NULL)
  {
    gp_array_list* array = (gp_array_list*)node;
    glBindBuffer(GL_ARRAY_BUFFER, array->mArray->mVBO);
    
    CHECK_GL_ERROR();
    
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(array->mIndex);
    glVertexAttribPointer(array->mIndex, array->mComponents, GL_FLOAT, GL_FALSE, array->mStride, (void*)array->mOffset);
    
    node = gp_list_node_next(node);
  }
#ifndef GP_GLES2
  d->mDirty = 0;
  }
#endif
  
  glDrawArrays(d->mMode, 0, d->mVerticies);
  CHECK_GL_ERROR();
}

void _gp_operation_draw_free(_gp_operation_data* data)
{
  _gp_operation_draw_data* d = (_gp_operation_draw_data*)data;
  
  if(d->mShader)
    gp_shader_unref(d->mShader);
  
  // TODO - free VAO
  gp_list_node* node = gp_list_front(&d->mArrays);
  while(node != NULL)
  {
    gp_array_list* array = (gp_array_list*)node;
    gp_array_unref(array->mArray);
    
    node = gp_list_node_next(node);
    free(array);
  }
  
  node = gp_list_front(&d->mUniforms);
  while(node != NULL)
  {
    gp_uniform_list* uniform = (gp_uniform_list*)node;
    gp_uniform_unref(uniform->mUniform);
    
    node = gp_list_node_next(node);
    free(uniform);
  }
  
  free(d);
}

gp_operation* gp_operation_draw_new()
{
  gp_operation* operation = malloc(sizeof(gp_operation));
  operation->func = _gp_operation_draw;
  gp_ref_init(&operation->mRef);
  operation->mData = malloc(sizeof(_gp_operation_draw_data));
  operation->mData->free = _gp_operation_draw_free;
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  gp_list_init(&data->mUniforms);
  gp_list_init(&data->mArrays);
  data->mShader = NULL;
#ifndef GP_GLES2
  data->mVAO = 0;
  data->mDirty = 1;
#endif
  data->mVerticies = 0;
  data->mMode = GL_TRIANGLES;
  
  return operation;
}

void gp_operation_draw_set_shader(gp_operation* operation, gp_shader* shader)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  if(data->mShader)
    gp_shader_unref(data->mShader);
  data->mShader = shader;
  gp_shader_ref(data->mShader);
  
#ifndef GP_GLES2
  data->mDirty = 1;
#endif
}

void gp_operation_draw_add_array_by_index(gp_operation* operation, gp_array* array, int index, int components, int stride, int offset)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  
  gp_array_list* a = malloc(sizeof(gp_array_list));
  a->mArray = array;
  a->mIndex = index;
  a->mComponents = components;
  a->mStride = stride;
  a->mOffset = offset;
  gp_list_push_back(&data->mArrays, (gp_list_node*)a);
  
  gp_array_ref(array);
  
#ifndef GP_GLES2
  data->mDirty = 1;
#endif
}

void gp_operation_draw_set_uniform(gp_operation* operation, gp_uniform* uniform)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  
  gp_uniform_list* u = malloc(sizeof(gp_uniform_list));
  u->mUniform = uniform;
  gp_list_push_back(&data->mUniforms, (gp_list_node*)u);
  
  gp_uniform_ref(uniform);
  
#ifndef GP_GLES2
  data->mDirty = 1;
#endif
}

void gp_operation_draw_set_verticies(gp_operation* operation, int count)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  data->mVerticies = count;
}

void gp_operation_draw_set_mode(gp_operation* operation, gp_draw_mode mode)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  data->mMode = mode;
}

typedef struct
{
  _gp_operation_data      mData;
  gp_pipeline*            mPipeline;
  int                     mX;
  int                     mY;
  int                     mWidth;
  int                     mHeight;
} _gp_operation_viewport_data;

void _gp_operation_viewport(_gp_operation_data* data)
{
  _gp_operation_viewport_data* d = (_gp_operation_viewport_data*)data;
  
  glViewport(d->mX, d->mY, d->mWidth, d->mHeight);
  
  _gp_pipeline_execute(d->mPipeline);
}

void _gp_operation_viewport_free(_gp_operation_data* data)
{
  _gp_operation_viewport_data* d = (_gp_operation_viewport_data*)data;
  _gp_pipeline_free(d->mPipeline);
  free(d);
}

gp_operation* gp_operation_viewport_new()
{
  gp_operation* operation = malloc(sizeof(gp_operation));
  operation->func = _gp_operation_viewport;
  gp_ref_init(&operation->mRef);
  operation->mData = malloc(sizeof(_gp_operation_viewport_data));
  operation->mData->free = _gp_operation_viewport_free;
  _gp_operation_viewport_data* data = (_gp_operation_viewport_data*)operation->mData;
  data->mPipeline = _gp_pipeline_new();
  data->mX = 0;
  data->mY = 0;
  data->mWidth = 0;
  data->mHeight = 0;
  
  return operation;
}

gp_pipeline* gp_operation_viewport_get_pipeline(gp_operation* operation)
{
  _gp_operation_viewport_data* data = (_gp_operation_viewport_data*)operation->mData;
  return data->mPipeline;
}

void gp_operation_viewport_set_dimesions(gp_operation* operation, int x, int y, int width, int height)
{
  _gp_operation_viewport_data* data = (_gp_operation_viewport_data*)operation->mData;
  data->mX = x;
  data->mY = y;
  data->mWidth = width;
  data->mHeight = height;
}


void gp_pipeline_add_operation(gp_pipeline* pipeline, gp_operation* operation)
{
  gp_operation_list* list = malloc(sizeof(gp_operation_list));
  
  list->mOperation = operation;
  gp_operation_ref(operation);
  gp_list_push_back(&pipeline->mOperations, (gp_list_node*)list);
}

int _gp_list_find_operation(gp_list_node* node, void* userdata)
{
  return ((gp_operation_list*)node)->mOperation == (gp_operation*)userdata;
}

void gp_pipeline_remove_operation(gp_pipeline* pipeline, gp_operation* operation)
{
  gp_list_node* node = gp_list_find(&pipeline->mOperations, _gp_list_find_operation, (void*)operation);
  gp_list_remove(&pipeline->mOperations, node);
  gp_operation_unref(operation);
  free(node);
}

gp_pipeline* _gp_pipeline_new()
{
  gp_pipeline* pipeline = malloc(sizeof(gp_pipeline));
  gp_list_init(&pipeline->mOperations);
  
  return pipeline;
}

void _gp_pipeline_free(gp_pipeline* pipeline)
{
  gp_list_node* node = gp_list_front(&pipeline->mOperations);
  while(node)
  {
    gp_operation_list* op = (gp_operation_list*)node;
    node = gp_list_node_next(node);
    gp_operation_unref(op->mOperation);
    free(op);
  }
  free(pipeline);
}

void _gp_pipeline_execute(gp_pipeline* pipeline)
{
  gp_operation_list* list = (gp_operation_list*)gp_list_front(&pipeline->mOperations);
  while(list != NULL)
  {
    list->mOperation->func(list->mOperation->mData);
    
    list = (gp_operation_list*)gp_list_node_next((gp_list_node*)list);
  }
}

void _gp_api_init()
{
#ifdef GP_GL
#ifndef __APPLE__
  // NOTE: GLEW must be initialize with an active OpenGL context
  GLenum err = glewInit();
  if(GLEW_OK != err)
  {
    gp_log_error("GLEW: %s", glewGetErrorString(err));
  }
  gp_log_info("GLEW Version: %s", glewGetString(GLEW_VERSION));
#endif // __APPLE__
#endif // GP_GL
}

void _gp_api_init_context()
{
  // Apple doesn't seem to have support for debug callbacks
#if defined(GP_GL) && defined(GP_DEBUG) && !defined(__APPLE__)
  if(glDebugMessageCallback)
  {
    int flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(!(flags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
      gp_log_warn("Context doesn't have debug support");
    }
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback((GLDEBUGPROC)DebugCallbackFunction, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, 0, GL_FALSE);
  }
#endif // defined(GP_GL) && defined(GP_DEBUG) && !defined(__APPLE__)
}
