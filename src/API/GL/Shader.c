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

typedef struct
{
  gp_list_node                mNode;
  GP_SHADER_SOURCE_TYPE       mType;
  const char*                 mText;
} _gp_shader_source_node;

gp_shader_source* gp_shader_source_new()
{
  gp_shader_source* source = malloc(sizeof(gp_shader_source));
  
  gp_list_init(&source->mSource);
  gp_ref_init(&source->mRef);
  
  return source;
}

void gp_shader_source_ref(gp_shader_source* source)
{
  gp_ref_inc(&source->mRef);
}

void gp_shader_source_unref(gp_shader_source* source)
{
  if(gp_ref_dec(&source->mRef))
  {
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
}

void gp_shader_source_add_from_string(gp_shader_source* source, GP_SHADER_SOURCE_TYPE type, const char* str)
{
  _gp_shader_source_node* node = malloc(sizeof(_gp_shader_source_node));
  node->mType = type;
  node->mText = strdup(str);
  
  gp_list_push_front(&source->mSource, (gp_list_node*)node);
}

gp_shader* gp_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(gp_shader));
  
  shader->mProgram = 0;
  shader->mAttribute = 0;
  gp_ref_init(&shader->mRef);
  
  return shader;
}

void gp_shader_ref(gp_shader* shader)
{
  gp_ref_inc(&shader->mRef);
}

void gp_shader_unref(gp_shader* shader)
{
  if(gp_ref_dec(&shader->mRef))
  {
    glDeleteProgram(shader->mProgram);
    free(shader);
  }
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

gp_uniform* gp_shader_uniform_new_by_name(gp_shader* shader, const char* name)
{
  gp_uniform* uniform = malloc(sizeof(gp_uniform));
  uniform->mLocation = glGetUniformLocation(shader->mProgram, name);
  uniform->mOperation = 0;
  uniform->mData = 0;
  gp_ref_init(&uniform->mRef);
  
  return uniform;
}

void gp_uniform_ref(gp_uniform* uniform)
{
  gp_ref_inc(&uniform->mRef);
}

void gp_uniform_unref(gp_uniform* uniform)
{
  if(gp_ref_dec(&uniform->mRef))
  {
    if(uniform->mData)
      free(uniform->mData);
    free(uniform);
  }
}

void _gp_uniform_load_texture(gp_uniform* uniform)
{
  glBindTexture(GL_TEXTURE_2D, *(int*)uniform->mData);
  glUniform1i(uniform->mLocation, 0);
}
void _gp_uniform_load_int(gp_uniform* uniform) {glUniform1i(uniform->mLocation, *(int*)uniform->mData);}
void _gp_uniform_load_float(gp_uniform* uniform) {glUniform1f(uniform->mLocation, *(float*)uniform->mData);}
void _gp_uniform_load_vec2(gp_uniform* uniform)
{
  float* data = uniform->mData;
  glUniform2f(uniform->mLocation, data[0], data[1]);
}
void _gp_uniform_load_vec3(gp_uniform* uniform)
{
  float* data = uniform->mData;
  glUniform3f(uniform->mLocation, data[0], data[1], data[2]);
}
void _gp_uniform_load_vec4(gp_uniform* uniform)
{
  float* data = uniform->mData;
  glUniform4f(uniform->mLocation, data[0], data[1], data[2], data[3]);
}
void _gp_uniform_load_mat3(gp_uniform* uniform)
{
  glUniformMatrix3fv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}
void _gp_uniform_load_mat4(gp_uniform* uniform)
{
  glUniformMatrix4fv(uniform->mLocation, 1, GL_FALSE, uniform->mData);
}

#define UNIFORM_SET(op, size)\
  if(uniform->mOperation != op)\
  {\
    if(uniform->mOperation)\
      free(uniform->mData);\
    uniform->mData = malloc(size);\
  }\
  uniform->mOperation = op;

void gp_uniform_set_texture(gp_uniform* uniform, gp_texture* texture)
{
  UNIFORM_SET(_gp_uniform_load_texture, sizeof(GLuint));
  memcpy(uniform->mData, &texture->mTexture, sizeof(GLuint));
}

void gp_uniform_set_float(gp_uniform* uniform, float data)
{
  UNIFORM_SET(_gp_uniform_load_float, sizeof(float)*1);
  memcpy(uniform->mData, &data, sizeof(float));
}

void gp_uniform_set_vec2(gp_uniform* uniform, float* data)
{
  UNIFORM_SET(_gp_uniform_load_vec2, sizeof(float)*2);
  memcpy(uniform->mData, data, sizeof(float)*2);
}

void gp_uniform_set_vec3(gp_uniform* uniform, float* data)
{
  UNIFORM_SET(_gp_uniform_load_vec3, sizeof(float)*3);
  memcpy(uniform->mData, data, sizeof(float)*3);
}

void gp_uniform_set_vec4(gp_uniform* uniform, float* data)
{
  UNIFORM_SET(_gp_uniform_load_vec4, sizeof(float)*4);
  memcpy(uniform->mData, data, sizeof(float)*4);
}

void gp_uniform_set_mat3(gp_uniform* uniform, float* data)
{
  UNIFORM_SET(_gp_uniform_load_mat3, sizeof(float)*9);
  memcpy(uniform->mData, data, sizeof(float)*9);
}

void gp_uniform_set_mat4(gp_uniform* uniform, float* data)
{
  UNIFORM_SET(_gp_uniform_load_mat4, sizeof(float)*16);
  memcpy(uniform->mData, data, sizeof(float)*16);
}
