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

#include <GraphicsPipeline/Shader.h>
#include <GraphicsPipeline/Logging.h>

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#endif // GP_GL
#include "GL.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct
{
  gp_list_node                mNode;
  GP_SHADER_SOURCE_TYPE       mType;
  const char*                 mText;
} _gp_shader_source_node;

void _gp_shader_source_free(gp_object* object)
{
  gp_shader_source* source = (gp_shader_source*)object;
  
  gp_list_node* node = gp_list_front(&source->mSource);
  while(node != NULL)
  {
    _gp_shader_source_node* s = (_gp_shader_source_node*)node;
    free((char*)s->mText);
    
    node = gp_list_node_next(node);
    
    free(s);
  }
  
  free(source);
}

gp_shader_source* gp_shader_source_new()
{
  gp_shader_source* source = malloc(sizeof(gp_shader_source));
  _gp_object_init(&source->mObject, _gp_shader_source_free);
  gp_list_init(&source->mSource);
  
  return source;
}

void gp_shader_source_add_from_string(gp_shader_source* source, GP_SHADER_SOURCE_TYPE type, const char* str)
{
  _gp_shader_source_node* node = malloc(sizeof(_gp_shader_source_node));
  node->mType = type;
  node->mText = strdup(str);
  
  gp_list_push_front(&source->mSource, (gp_list_node*)node);
}

void _gp_shader_free(gp_object* object)
{
  gp_shader* self = (gp_shader*)object;
  
  glDeleteProgram(self->mProgram);
  free(self);
}

gp_shader* gp_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(gp_shader));
  _gp_object_init(&shader->mObject, _gp_shader_free);
  shader->mProgram = 0;
  shader->mAttribute = 0;
  
  return shader;
}

int _check_shader_status(unsigned int shader, GP_SHADER_SOURCE_TYPE type)
{
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    GLchar* errorLog = malloc(sizeof(GLchar)*maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(shader); // Don't leak the shader.
    switch(type)
    {
      case GP_SHADER_SOURCE_VERTEX:
        gp_log_error("Vertex Error: %s\n", errorLog);
        break;
      case GP_SHADER_SOURCE_FRAGMENT:
        gp_log_error("Fragment Error: %s\n", errorLog);
        break;
    }
    free(errorLog);

    return 0;
  }
  
  return 1;
}

void gp_shader_compile(gp_shader* shader, gp_shader_source* source)
{
  shader->mProgram = glCreateProgram();
  
  _gp_shader_source_node* node = (_gp_shader_source_node*)gp_list_front(&source->mSource);
  while(node != NULL)
  {
    int gl_type;
    switch(node->mType)
    {
      case GP_SHADER_SOURCE_VERTEX:
        gl_type = GL_VERTEX_SHADER;
        break;
      case GP_SHADER_SOURCE_FRAGMENT:
        gl_type = GL_FRAGMENT_SHADER;
        break;
    }
    GLuint s = glCreateShader(gl_type);
    glShaderSource(s, 1, &node->mText, 0);
    glCompileShader(s);
    
    if(!_check_shader_status(s, node->mType)) return;
    
    glAttachShader(shader->mProgram, s);
    
    node = (_gp_shader_source_node*)gp_list_node_next((gp_list_node*)node);
  }
  
  glLinkProgram(shader->mProgram);
  glUseProgram(shader->mProgram);
  
  int linkStatus = 0;
  glGetProgramiv(shader->mProgram, GL_LINK_STATUS, &linkStatus);
  if(!linkStatus)
  {
    int length;
    glGetProgramiv(shader->mProgram, GL_INFO_LOG_LENGTH, &length);
    if(length > 0)
    {
      GLchar* infoLog = malloc(sizeof(GLchar)*length);
      glGetProgramInfoLog(shader->mProgram, length, &length, infoLog);
      gp_log_error("ShaderError: %s\n", infoLog);
      free(infoLog);
    }
  }
  
  shader->mAttribute = glGetAttribLocation(shader->mProgram, "position");
}

void _gp_uniform_load_texture(gp_uniform* uniform, _gp_draw_context* context)
{
  //
  // Find texture index if already bound
  //
  int index = -1;
  gp_texture_cache_list* node = (gp_texture_cache_list*)gp_list_back(&context->mTextureCache);
  while(node != NULL && node->mTexture != 0)
  {
    if(node->mTexture == (gp_texture*)uniform->mData)
    {
      index = node->mIndex;
      break;
    }
    node = (gp_texture_cache_list*)gp_list_node_prev((gp_list_node*)node);
  }
  if(index == -1)
  {
    node = (gp_texture_cache_list*)gp_list_front(&context->mTextureCache);
    index = node->mIndex;
    node->mTexture = (gp_texture*)uniform->mData;
    gp_list_remove(&context->mTextureCache, (gp_list_node*)node);
    gp_list_push_back(&context->mTextureCache, (gp_list_node*)node);
  }
  
  //
  // Bind texture and set uniform value
  //
  gp_texture* texture = ((gp_texture*)uniform->mData);
  glActiveTexture(GL_TEXTURE0+index);
  glBindTexture(texture->mDimensions, texture->mTexture);
  glUniform1i(uniform->mLocation, index);
  
  CHECK_GL_ERROR()
}
void _gp_uniform_load_int(gp_uniform* uniform, _gp_draw_context* context) {glUniform1i(uniform->mLocation, *(int*)uniform->mData);}
void _gp_uniform_load_float(gp_uniform* uniform, _gp_draw_context* context) {glUniform1f(uniform->mLocation, *(float*)uniform->mData);}
void _gp_uniform_load_vec2(gp_uniform* uniform, _gp_draw_context* context)
{
  float* data = uniform->mData;
  glUniform2f(uniform->mLocation, data[0], data[1]);
}
void _gp_uniform_load_vec3(gp_uniform* uniform, _gp_draw_context* context)
{
  float* data = uniform->mData;
  glUniform3f(uniform->mLocation, data[0], data[1], data[2]);
}
void _gp_uniform_load_vec4(gp_uniform* uniform, _gp_draw_context* context)
{
  float* data = uniform->mData;
  glUniform4f(uniform->mLocation, data[0], data[1], data[2], data[3]);
}
void _gp_uniform_load_mat3(gp_uniform* uniform, _gp_draw_context* context)
{
  glUniformMatrix3fv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}
void _gp_uniform_load_mat4(gp_uniform* uniform, _gp_draw_context* context)
{
  glUniformMatrix4fv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}

#ifdef GP_GL
void _gp_uniform_load_double(gp_uniform* uniform, _gp_draw_context* context) {glUniform1d(uniform->mLocation, *(double*)uniform->mData);}
void _gp_uniform_load_dvec2(gp_uniform* uniform, _gp_draw_context* context)
{
  double* data = uniform->mData;
  glUniform2d(uniform->mLocation, data[0], data[1]);
}
void _gp_uniform_load_dvec3(gp_uniform* uniform, _gp_draw_context* context)
{
  double* data = uniform->mData;
  glUniform3d(uniform->mLocation, data[0], data[1], data[2]);
}
void _gp_uniform_load_dvec4(gp_uniform* uniform, _gp_draw_context* context)
{
  double* data = uniform->mData;
  glUniform4d(uniform->mLocation, data[0], data[1], data[2], data[3]);
}
void _gp_uniform_load_dmat3(gp_uniform* uniform, _gp_draw_context* context)
{
  glUniformMatrix3dv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}
void _gp_uniform_load_dmat4(gp_uniform* uniform, _gp_draw_context* context)
{
  glUniformMatrix4dv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}
#endif

void _gp_uniform_default_free(gp_object* object)
{
  gp_uniform* uniform = (gp_uniform*)object;
  
  free(uniform->mData);
  free(uniform);
}

#define UNIFORM_NEW_BY_NAME(type, size)\
  gp_uniform* gp_uniform_##type##_new_by_name(gp_shader* shader, const char* name)\
  {\
    gp_uniform* uniform = malloc(sizeof(gp_uniform));\
    _gp_object_init(&uniform->mObject, _gp_uniform_default_free);\
    uniform->mLocation = glGetUniformLocation(shader->mProgram, name);\
    uniform->mOperation = _gp_uniform_load_##type;\
    uniform->mData = malloc(size);\
    memset(uniform->mData, 0, size);\
    return uniform;\
  }

void _gp_uniform_texture_free(gp_object* object)
{
  gp_uniform* uniform = (gp_uniform*)object;
  
  if(uniform->mData)
    gp_object_unref((gp_object*)uniform->mData);
  
  free(uniform);
}

gp_uniform* gp_uniform_texture_new_by_name(gp_shader* shader, const char* name)
{
  gp_uniform* uniform = malloc(sizeof(gp_uniform));
  _gp_object_init(&uniform->mObject, _gp_uniform_texture_free);
  uniform->mLocation = glGetUniformLocation(shader->mProgram, name);
  uniform->mOperation = _gp_uniform_load_texture;
  uniform->mData = 0;
  return uniform;
}

UNIFORM_NEW_BY_NAME(float, sizeof(float))
UNIFORM_NEW_BY_NAME(vec2, sizeof(float)*2)
UNIFORM_NEW_BY_NAME(vec3, sizeof(float)*3)
UNIFORM_NEW_BY_NAME(vec4, sizeof(float)*4)
UNIFORM_NEW_BY_NAME(mat3, sizeof(float)*9)
UNIFORM_NEW_BY_NAME(mat4, sizeof(float)*16)

#ifdef GP_GL
UNIFORM_NEW_BY_NAME(double, sizeof(double))
UNIFORM_NEW_BY_NAME(dvec2, sizeof(double)*2)
UNIFORM_NEW_BY_NAME(dvec3, sizeof(double)*3)
UNIFORM_NEW_BY_NAME(dvec4, sizeof(double)*4)
UNIFORM_NEW_BY_NAME(dmat3, sizeof(double)*9)
UNIFORM_NEW_BY_NAME(dmat4, sizeof(double)*16)
#endif

#undef UNIFORM_NEW_BY_NAME

#define UNIFORM_SET_FLOATPTR(type, size)\
  void gp_uniform_##type##_set(gp_uniform* uniform, float* data)\
  {\
    assert(uniform->mOperation == _gp_uniform_load_##type);\
    memcpy(uniform->mData, data, size);\
  }
  
#define UNIFORM_SET_DOUBLEPTR(type, size)\
  void gp_uniform_##type##_set(gp_uniform* uniform, double* data)\
  {\
    assert(uniform->mOperation == _gp_uniform_load_##type);\
    memcpy(uniform->mData, data, size);\
  }

#define UNIFORM_GET(type, datatype)\
  datatype gp_uniform_##type##_get(gp_uniform* uniform)\
  {\
    assert(uniform->mOperation == _gp_uniform_load_##type);\
    return (datatype)uniform->mData;\
  }

void gp_uniform_texture_set(gp_uniform* uniform, gp_texture* texture)
{
  assert(uniform->mOperation == _gp_uniform_load_texture);
  
  if(uniform->mData) gp_object_unref((gp_object*)uniform->mData);
  uniform->mData = texture;
  if(uniform->mData) gp_object_ref((gp_object*)uniform->mData);
}

void gp_uniform_float_set(gp_uniform* uniform, float data)
{
  assert(uniform->mOperation == _gp_uniform_load_float);
  memcpy(uniform->mData, &data, sizeof(float));
}

float gp_uniform_float_get(gp_uniform* uniform)
{
  assert(uniform->mOperation == _gp_uniform_load_float);
  return *(float*)uniform->mData;
}

#ifdef GP_GL
void gp_uniform_double_set(gp_uniform* uniform, double data)
{
  assert(uniform->mOperation == _gp_uniform_load_double);
  memcpy(uniform->mData, &data, sizeof(double));
}

double gp_uniform_double_get(gp_uniform* uniform)
{
  assert(uniform->mOperation == _gp_uniform_load_double);
  return *(double*)uniform->mData;
}
#endif

UNIFORM_SET_FLOATPTR(vec2, sizeof(float)*2)
UNIFORM_SET_FLOATPTR(vec3, sizeof(float)*3)
UNIFORM_SET_FLOATPTR(vec4, sizeof(float)*4)
UNIFORM_SET_FLOATPTR(mat3, sizeof(float)*9)
UNIFORM_SET_FLOATPTR(mat4, sizeof(float)*16)

UNIFORM_GET(texture, gp_texture*)
UNIFORM_GET(vec2, float*)
UNIFORM_GET(vec3, float*)
UNIFORM_GET(vec4, float*)
UNIFORM_GET(mat3, float*)
UNIFORM_GET(mat4, float*)

#ifdef GP_GL
UNIFORM_SET_DOUBLEPTR(dvec2, sizeof(double)*2)
UNIFORM_SET_DOUBLEPTR(dvec3, sizeof(double)*3)
UNIFORM_SET_DOUBLEPTR(dvec4, sizeof(double)*4)
UNIFORM_SET_DOUBLEPTR(dmat3, sizeof(double)*9)
UNIFORM_SET_DOUBLEPTR(dmat4, sizeof(double)*16)

UNIFORM_GET(dvec2, double*)
UNIFORM_GET(dvec3, double*)
UNIFORM_GET(dvec4, double*)
UNIFORM_GET(dmat3, double*)
UNIFORM_GET(dmat4, double*)
#endif

#undef UNIFORM_SET_FLOATPTR
