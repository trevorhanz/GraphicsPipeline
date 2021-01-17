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

typedef struct
{
  _gp_operation_data      mData;
} _gp_operation_clear_data;

void _gp_operation_clear(_gp_operation_data* data)
{
  _gp_operation_clear_data* d = (_gp_operation_clear_data*)data;
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

gp_operation* gp_operation_clear_new()
{
  gp_operation* operation = malloc(sizeof(gp_operation));
  operation->func = _gp_operation_clear;
  operation->mData = malloc(sizeof(_gp_operation_clear_data));
  
  return operation;
}

typedef struct _gp_array_list gp_array_list;

struct _gp_array_list
{
  gp_array_list*          mNext;
  GLuint                  mVBO;
  int                     mIndex;
  int                     mComponents;
  int                     mStride;
  uintptr_t               mOffset;
};

typedef struct _gp_uniform_list gp_uniform_list;

struct _gp_uniform_list
{
  gp_uniform_list*        mNext;
  gp_uniform*             mUniform;
};

typedef struct
{
  _gp_operation_data      mData;
#ifdef GP_GL
  GLuint                  mVAO;
  uint8_t                 mDirty;
#endif
  gp_shader*              mShader;
  gp_array_list*          mArrays;
  gp_uniform_list*        mUniforms;
  unsigned int            mVerticies;
  gp_draw_mode            mMode;
} _gp_operation_draw_data;

void _gp_operation_draw(_gp_operation_data* data)
{
  _gp_operation_draw_data* d = (_gp_operation_draw_data*)data;
  
  CHECK_GL_ERROR();
  
  glUseProgram(d->mShader->mProgram);
  
  gp_uniform_list* uniform = d->mUniforms;
  while(uniform != NULL)
  {
    uniform->mUniform->mOperation(uniform->mUniform);
    
    uniform = uniform->mNext;
  }
  
#ifdef GP_GL
  if(d->mVAO == 0)
    glGenVertexArrays(1, &d->mVAO);
  
  glBindVertexArray(d->mVAO);
  if(d->mDirty == 1)
  {
#endif
  gp_array_list* array = d->mArrays;
  while(array != NULL)
  {
    glBindBuffer(GL_ARRAY_BUFFER, array->mVBO);
    
    CHECK_GL_ERROR();
    
    // Specify the layout of the vertex data
    glEnableVertexAttribArray(array->mIndex);
    glVertexAttribPointer(array->mIndex, array->mComponents, GL_FLOAT, GL_FALSE, array->mStride, (void*)array->mOffset);
    
    array = array->mNext;
  }
#ifdef GP_GL
  d->mDirty = 0;
  }
#endif
  
  glDrawArrays(d->mMode, 0, d->mVerticies);
}

gp_operation* gp_operation_draw_new()
{
  gp_operation* operation = malloc(sizeof(gp_operation));
  operation->func = _gp_operation_draw;
  
  operation->mData = malloc(sizeof(_gp_operation_draw_data));
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  data->mUniforms = NULL;
  data->mArrays = NULL;
  data->mShader = NULL;
  #ifdef GP_GL
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
  data->mShader = shader;
  
#ifdef GP_GL
  data->mDirty = 1;
#endif
}

void gp_operation_draw_add_array_by_index(gp_operation* operation, gp_array* array, int index, int components, int stride, int offset)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  
  gp_array_list* a = malloc(sizeof(gp_array_list));
  a->mNext = data->mArrays;
  a->mVBO = array->mVBO;
  a->mIndex = index;
  a->mComponents = components;
  a->mStride = stride;
  a->mOffset = offset;
  data->mArrays = a;
  
#ifdef GP_GL
  data->mDirty = 1;
#endif
}

void gp_operation_draw_set_uniform(gp_operation* operation, gp_uniform* uniform)
{
  _gp_operation_draw_data* data = (_gp_operation_draw_data*)operation->mData;
  
  gp_uniform_list* u = malloc(sizeof(gp_uniform_list));
  u->mNext = data->mUniforms;
  u->mUniform = uniform;
  data->mUniforms = u;
  
#ifdef GP_GL
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

void gp_pipeline_add_operation(gp_pipeline* pipeline, gp_operation* operation)
{
  gp_operation_list* list = malloc(sizeof(gp_operation_list));
  
  list->mOperation = operation;
  list->mNext = NULL;
  if(pipeline->mOperationsEnd != NULL)
  {
    pipeline->mOperationsEnd->mNext = list;
  }
  else
  {
    pipeline->mOperations = list;
    pipeline->mOperationsEnd = list;
  }
}

gp_pipeline* _gp_pipeline_new()
{
  gp_pipeline* pipeline = malloc(sizeof(gp_pipeline));
  pipeline->mOperations = NULL;
  pipeline->mOperationsEnd = NULL;
  
  return pipeline;
}

void _gp_pipeline_execute(gp_pipeline* pipeline)
{
  gp_operation_list* list = pipeline->mOperations;
  while(list != NULL)
  {
    list->mOperation->func(list->mOperation->mData);
    
    list = list->mNext;
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
  
#if GP_DEBUG
  // Apple doesn't seem to have support for debug callbacks
#ifndef __APPLE__
  if(glDebugMessageCallback)
  {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(DebugCallbackFunction, 0);
  }
#endif // __APPLE__
#endif // GP_DEBUG
#endif // GP_GL
}
