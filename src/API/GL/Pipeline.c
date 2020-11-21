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

#define CHECK_GL_ERROR() \
{\
  GLenum err;\
  while((err = glGetError()) != GL_NO_ERROR) {\
    LogE("GL ERROR (%d) at: %s:%d", err, __FILE__, __LINE__);\
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
