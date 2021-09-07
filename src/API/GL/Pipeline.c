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
#include <string.h>

#define GP_OBJECT_FROM_LIST_NODE(node) (gp_object*)(((char*)node)-sizeof(gp_object))

#define GP_PIPELINE_RESORT        0x01

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

void _gp_notification_null(gp_operation* self)
{
}

void gp_operation_set_priority(gp_operation* operation, int priority)
{
  if(operation->mPipeline && operation->mPriority != priority)
  {
    operation->mPipeline->mState |= GP_PIPELINE_RESORT;
  }
  operation->mPriority = priority;
}

int gp_operation_get_priority(gp_operation* operation)
{
  return operation->mPriority;
}

typedef struct
{
  gp_operation            mOperation;
  float                   mColor[4]; // RGBA
} _gp_operation_clear;

void _gp_operation_clear_func(gp_operation* self, _gp_draw_context* context)
{
  _gp_operation_clear* clear = (_gp_operation_clear*)self;
  
  glClearColor(clear->mColor[0], clear->mColor[1], clear->mColor[2], clear->mColor[3]);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

gp_operation* gp_operation_clear_new()
{
  _gp_operation_clear* operation = malloc(sizeof(_gp_operation_clear));
  _gp_object_init(&operation->mOperation.mObject, (gp_object_free)free);
  operation->mOperation.mFunc = _gp_operation_clear_func;
  operation->mOperation.mPipeline = 0;
  operation->mOperation.mAdded = _gp_notification_null;
  operation->mOperation.mRemoved = _gp_notification_null;
  operation->mOperation.mPriority = 0;
  
  operation->mColor[0] = 0;
  operation->mColor[1] = 0;
  operation->mColor[2] = 0;
  operation->mColor[3] = 1;
  
  return (gp_operation*)operation;
}

void gp_operation_clear_set_color(gp_operation* operation, float r, float g, float b, float a)
{
  _gp_operation_clear* self = (_gp_operation_clear*)operation;
  self->mColor[0] = r;
  self->mColor[1] = g;
  self->mColor[2] = b;
  self->mColor[3] = a;
}

typedef struct _gp_array_list gp_array_list;

struct _gp_array_list
{
  gp_list_node            mNode;
  gp_array*               mArray;
  int                     mIndex;
  int                     mComponents;
  GLuint                  mType;
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
  gp_operation            mOperation;
#ifndef GP_GLES2
  GLuint                  mVAO;
  uint8_t                 mDirty;
#endif
  gp_shader*              mShader;
  gp_list                 mArrays;
  gp_list                 mUniforms;
  unsigned int            mVerticies;
  GP_DRAW_MODE            mMode;
} _gp_operation_draw;

void _gp_operation_draw_func(gp_operation* operation, _gp_draw_context* context)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  
  CHECK_GL_ERROR();
  
  if(context->mShader != self->mShader)
  {
    glUseProgram(self->mShader->mProgram);
    context->mShader = self->mShader;
  }
  
  gp_list_node* node = gp_list_front(&self->mUniforms);
  while(node != gp_list_end(&self->mUniforms))
  {
    gp_uniform_list* uniform = (gp_uniform_list*)node;
    uniform->mUniform->mOperation(uniform->mUniform, context);
    
    node = gp_list_node_next(node);
  }
  
#ifndef GP_GLES2
  if(self->mVAO == 0)
    glGenVertexArrays(1, &self->mVAO);
  
  glBindVertexArray(self->mVAO);
  if(self->mDirty == 1)
  {
#endif
  node = gp_list_front(&self->mArrays);
  while(node != gp_list_end(&self->mArrays))
  {
    gp_array_list* array = (gp_array_list*)node;
    glBindBuffer(GL_ARRAY_BUFFER, array->mArray->mVBO);
    
    CHECK_GL_ERROR();
    
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(array->mIndex);
#ifdef GP_GL
    if(array->mType == GL_DOUBLE)
    {
      glVertexAttribLPointer(array->mIndex, array->mComponents, array->mType, array->mStride, (void*)array->mOffset);
    }
    else
#endif
      glVertexAttribPointer(array->mIndex, array->mComponents, array->mType, GL_FALSE, array->mStride, (void*)array->mOffset);
    
    node = gp_list_node_next(node);
  }
#ifndef GP_GLES2
  self->mDirty = 0;
  }
#endif
  
  static const GLuint draw_modes[] =
  {
    GL_POINTS,
    GL_TRIANGLES,
    GL_TRIANGLE_STRIP,
    GL_LINES,
    GL_LINE_STRIP
  };
  
  glDrawArrays(draw_modes[self->mMode], 0, self->mVerticies);
  CHECK_GL_ERROR();
}

void _gp_operation_draw_free(gp_object* object)
{
  _gp_operation_draw* d = (_gp_operation_draw*)object;
  
  if(d->mShader)
    gp_object_unref((gp_object*)d->mShader);
  
  // TODO - free VAO
  gp_list_node* node = gp_list_front(&d->mArrays);
  while(node != gp_list_end(&d->mArrays))
  {
    gp_array_list* array = (gp_array_list*)node;
    gp_object_unref((gp_object*)array->mArray);
    
    node = gp_list_node_next(node);
    free(array);
  }
  
  node = gp_list_front(&d->mUniforms);
  while(node != gp_list_end(&d->mUniforms))
  {
    gp_uniform_list* uniform = (gp_uniform_list*)node;
    gp_object_unref((gp_object*)uniform->mUniform);
    
    node = gp_list_node_next(node);
    free(uniform);
  }
  
  free(d);
}

void _gp_operation_draw_removed(gp_operation* self)
{
  _gp_operation_draw* d = (_gp_operation_draw*)self;
  d->mVAO = 0;  // TODO: delete VAO
  d->mDirty = 1;
}

gp_operation* gp_operation_draw_new()
{
  _gp_operation_draw* operation = malloc(sizeof(_gp_operation_draw));
  _gp_object_init(&operation->mOperation.mObject, _gp_operation_draw_free);
  operation->mOperation.mFunc = _gp_operation_draw_func;
  operation->mOperation.mPipeline = 0;
  operation->mOperation.mAdded = _gp_notification_null;
  operation->mOperation.mRemoved = _gp_operation_draw_removed;
  operation->mOperation.mPriority = 0;
  gp_list_init(&operation->mUniforms);
  gp_list_init(&operation->mArrays);
  operation->mShader = NULL;
#ifndef GP_GLES2
  operation->mVAO = 0;
  operation->mDirty = 1;
#endif
  operation->mVerticies = 0;
  operation->mMode = GP_MODE_TRIANGLES;
  
  return (gp_operation*)operation;
}

void gp_operation_draw_set_shader(gp_operation* operation, gp_shader* shader)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  if(self->mShader)
    gp_object_unref((gp_object*)self->mShader);
  self->mShader = shader;
  gp_object_ref((gp_object*)self->mShader);
  
#ifndef GP_GLES2
  self->mDirty = 1;
#endif
}

void gp_operation_draw_add_array_by_index(gp_operation* operation,
                                          gp_array* array,
                                          int index,
                                          int components,
                                          GP_DATA_TYPE type,
                                          int stride,
                                          int offset)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  
  gp_array_list* a = NULL;
  
  // Search for existing array at index first.
  gp_list_node* node = gp_list_front(&self->mArrays);
  while(node != gp_list_end(&self->mArrays))
  {
    a = (gp_array_list*)node;
    if(a->mIndex == index)
    {
      gp_object_unref((gp_object*)a->mArray);
      break;
    }
    
    node = gp_list_node_next(node);
  }
  
  // Create a new node if one wasn't found.
  if(node == gp_list_end(&self->mArrays))
  {
    a = malloc(sizeof(gp_array_list));
    gp_list_push_back(&self->mArrays, (gp_list_node*)a);
  }
  
  static const GLuint types[] =
  {
    GL_UNSIGNED_BYTE,
    GL_INT,
    GL_FLOAT,
#ifdef GP_GL
    GL_DOUBLE
#else
    GL_FLOAT
#endif
  };
  
  a->mArray = array;
  a->mIndex = index;
  a->mComponents = components;
  a->mType = types[type];
  a->mStride = stride;
  a->mOffset = offset;
  
  gp_object_ref((gp_object*)array);
  
#ifndef GP_GLES2
  self->mDirty = 1;
#endif
}

void gp_operation_draw_set_uniform(gp_operation* operation, gp_uniform* uniform)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  
  gp_uniform_list* u = malloc(sizeof(gp_uniform_list));
  u->mUniform = uniform;
  gp_list_push_back(&self->mUniforms, (gp_list_node*)u);
  
  gp_object_ref((gp_object*)uniform);
  
#ifndef GP_GLES2
  self->mDirty = 1;
#endif
}

void gp_operation_draw_set_verticies(gp_operation* operation, int count)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  self->mVerticies = count;
}

void gp_operation_draw_set_mode(gp_operation* operation, GP_DRAW_MODE mode)
{
  _gp_operation_draw* self = (_gp_operation_draw*)operation;
  self->mMode = mode;
}

typedef struct
{
  gp_operation            mOperation;
  gp_pipeline*            mPipeline;
  int                     mRect[4];     // x, y, width, height
  int                     mOrigin[4];   // x, y, width, height
} _gp_operation_viewport;

void _gp_operation_viewport_func(gp_operation* operation, _gp_draw_context* context)
{
  _gp_operation_viewport* self = (_gp_operation_viewport*)operation;
  
  glGetIntegerv(GL_VIEWPORT, self->mOrigin);
  
  glViewport(self->mRect[0], self->mRect[1], self->mRect[2], self->mRect[3]);
  
  _gp_pipeline_execute_with_context(self->mPipeline, context);
  
  glViewport(self->mOrigin[0], self->mOrigin[1], self->mOrigin[2], self->mOrigin[3]);
}

void _gp_operation_viewport_free(gp_object* object)
{
  _gp_operation_viewport* self = (_gp_operation_viewport*)object;
  _gp_pipeline_free(self->mPipeline);
  free(self);
}

gp_operation* gp_operation_viewport_new()
{
  _gp_operation_viewport* operation = malloc(sizeof(_gp_operation_viewport));
  _gp_object_init(&operation->mOperation.mObject, _gp_operation_viewport_free);
  operation->mOperation.mFunc = _gp_operation_viewport_func;
  operation->mOperation.mPipeline = 0;
  operation->mOperation.mAdded = _gp_notification_null;
  operation->mOperation.mRemoved = _gp_notification_null;
  operation->mPipeline = _gp_pipeline_new();
  operation->mOperation.mPriority = 0;
  memset(operation->mRect, 0, sizeof(int)*4);
  
  return (gp_operation*)operation;
}

gp_pipeline* gp_operation_viewport_get_pipeline(gp_operation* operation)
{
  _gp_operation_viewport* self = (_gp_operation_viewport*)operation;
  return self->mPipeline;
}

void gp_operation_viewport_set_dimesions(gp_operation* operation, int x, int y, int width, int height)
{
  _gp_operation_viewport* self = (_gp_operation_viewport*)operation;
  self->mRect[0] = x;
  self->mRect[1] = y;
  self->mRect[2] = width;
  self->mRect[3] = height;
}

void gp_pipeline_add_operation(gp_pipeline* pipeline, gp_operation* operation)
{
  if(operation->mPipeline)
  {
    gp_pipeline_remove_operation(operation->mPipeline, operation);
  }
  
  operation->mPipeline = pipeline;
  operation->mAdded(operation);
  pipeline->mState |= GP_PIPELINE_RESORT;
  
  gp_object_ref((gp_object*)operation);
  gp_list_push_back(&pipeline->mOperations, (gp_list_node*)&operation->mNode);
}

void gp_pipeline_remove_operation(gp_pipeline* pipeline, gp_operation* operation)
{
  if(operation->mPipeline == pipeline)
  {
    operation->mRemoved(operation);
    operation->mPipeline = 0;
    
    gp_list_remove(&pipeline->mOperations, &operation->mNode);
    gp_object_unref((gp_object*)operation);
  }
}

gp_pipeline* _gp_pipeline_new()
{
  gp_pipeline* pipeline = malloc(sizeof(gp_pipeline));
  gp_list_init(&pipeline->mOperations);
  pipeline->mState = 0;
  
  return pipeline;
}

void _gp_pipeline_free(gp_pipeline* pipeline)
{
  gp_list_node* node = gp_list_front(&pipeline->mOperations);
  while(node)
  {
    gp_operation* op = (gp_operation*)GP_OBJECT_FROM_LIST_NODE(node);
    node = gp_list_node_next(node);
    gp_object_unref((gp_object*)op);
  }
  free(pipeline);
}

void _gp_pipeline_execute(gp_pipeline* pipeline)
{
  _gp_draw_context* context = malloc(sizeof(_gp_draw_context));
  context->mShader = 0;
  gp_list_init(&context->mTextureCache);
  
  int texture_units;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
  
  int i;
  for(i=0; i<texture_units; ++i)
  {
    gp_texture_cache_list* node = malloc(sizeof(gp_texture_cache_list));
    node->mTexture = 0;
    node->mIndex = i;
    gp_list_push_back(&context->mTextureCache, (gp_list_node*)node);
  }
  
  _gp_pipeline_execute_with_context(pipeline, context);
  
  gp_list_node* node = gp_list_front(&context->mTextureCache);
  while(node != NULL)
  {
    gp_list_node* temp = node;
    node = gp_list_node_next(node);
    free(temp);
  }
  free(context);
}

int _gp_pipeline_sort(gp_list_node* first, gp_list_node* second)
{
  int p1 = ((gp_operation*)GP_OBJECT_FROM_LIST_NODE(first))->mPriority;
  int p2 = ((gp_operation*)GP_OBJECT_FROM_LIST_NODE(second))->mPriority;
  return p1 < p2;
}

void _gp_pipeline_execute_with_context(gp_pipeline* pipeline, _gp_draw_context* context)
{
  if(pipeline->mState & GP_PIPELINE_RESORT)
  {
    gp_list_sort(&pipeline->mOperations, _gp_pipeline_sort);
    
    pipeline->mState &= ~GP_PIPELINE_RESORT;
  }
  
  gp_list_node* node = gp_list_front(&pipeline->mOperations);
  while(node != gp_list_end(&pipeline->mOperations))
  {
    gp_operation* op = (gp_operation*)GP_OBJECT_FROM_LIST_NODE(node);
    op->mFunc(op, context);
    
    node = gp_list_node_next((gp_list_node*)&op->mNode);
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

void _gp_api_prepare_window(unsigned int width, unsigned int height)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
#ifdef GP_GL
  glEnable(GL_PROGRAM_POINT_SIZE);
#endif
  
  glViewport(0, 0, width, height);
}
