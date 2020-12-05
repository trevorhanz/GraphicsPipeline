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

#ifndef __APPLE__
#include <GL/glew.h>
#endif
#include "GL.h"

#ifndef __APPLE__
#include <GL/glew.h>
#else
// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif

#include <stdlib.h>

// Apple doesn't seem to have support for debug callbacks
#ifndef __APPLE__
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
  GLuint                  mVAO;
  gp_shader*              mShader;
  gp_array*               mArray;
} _gp_operation_draw_data;

void _gp_operation_draw(_gp_operation_data* data)
{
  _gp_operation_draw_data* d = (_gp_operation_draw_data*)data;
  
  CHECK_GL_ERROR();
  
  glBindVertexArray(d->mVAO);
  
  glUseProgram(d->mShader->mProgram);
  
  glBindBuffer(GL_ARRAY_BUFFER, d->mArray->mVBO);
  
  CHECK_GL_ERROR();
  
  // Specify the layout of the vertex data
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void gp_pipeline_add_draw(gp_pipeline* pipeline, gp_shader* shader, gp_array* array)
{
  gp_operation_list* list = malloc(sizeof(gp_operation_list));
  
  list->mOperation = malloc(sizeof(gp_operation));
  list->mOperation->func = &_gp_operation_draw;
  
  _gp_operation_draw_data* data = malloc(sizeof(_gp_operation_draw_data));
  data->mShader = shader;
  data->mArray = array;
  glGenVertexArrays(1, &data->mVAO);
  list->mOperation->mData = (_gp_operation_data*)data;
  
  list->mNext = pipeline->mOperations;
  pipeline->mOperations = list;
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
}
