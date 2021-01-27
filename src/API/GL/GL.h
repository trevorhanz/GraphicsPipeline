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

#ifndef __GP_GL_H__
#define __GP_GL_H__

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/Common.h>

#include "../../Utils/List.h"
#include "../../Utils/RefCounter.h"

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/gl.h>
#else // __APPLE__
// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif // __APPLE__
#else // GP_GL
#include <GLES3/gl3.h>
#endif // GP_GL

struct _gp_array
{
  GLuint                  mVBO;
  gp_refcounter           mRef;
};

struct _gp_texture
{
  GLuint                  mTexture;
  gp_refcounter           mRef;
};

struct _gp_shader
{
  GLuint                  mProgram;
  GLuint                  mAttribute;
  gp_refcounter           mRef;
};

typedef void(*LoadUniform)(gp_uniform* uniform);

struct _gp_uniform
{
  GLuint                  mLocation;
  LoadUniform             mOperation;
  void*                   mData;
  gp_refcounter           mRef;
};

typedef struct __gp_operation_data _gp_operation_data;

struct __gp_operation_data
{
  void (*free)(_gp_operation_data* data);
};

struct _gp_operation
{
  void (*func)(_gp_operation_data* data);
  _gp_operation_data*     mData;
  gp_refcounter           mRef;
};

typedef struct _gp_operation_list gp_operation_list;

struct _gp_operation_list
{
  gp_list_node            mNode;
  gp_operation*           mOperation;
};

struct _gp_pipeline
{
  gp_list                 mOperations;
};

#ifdef __cplusplus
extern "C" {
#endif

gp_pipeline* _gp_pipeline_new();

void _gp_pipeline_free(gp_pipeline* pipeline);

void _gp_pipeline_execute(gp_pipeline* pipeline);

void _gp_api_init();

void _gp_generate_shader(gp_shader* shader);

void _gp_generate_array(gp_array* array);

void _gp_generate_texture(gp_texture* texture);

#ifdef __cplusplus
}
#endif

#endif // __GP_GL_H__
