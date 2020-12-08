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
#include "GLES.h"

int _check_shader_status(unsigned int shader, const char* type)
{
  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if(isCompiled == GL_FALSE)
  {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    GLchar errorLog[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);

    // Provide the infolog in whatever manor you deem best.
    // Exit with failure.
    glDeleteShader(shader); // Don't leak the shader.
    gp_log_error("%s Error: %s\n", type, errorLog);
    return 0;
  }
  
  return 1;
}

void gp_shader_compile(gp_shader* shader, const char* vertex, const char* fragment)
{
  // Create and compile the vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex, 0);
  glCompileShader(vertexShader);
  
  if(!_check_shader_status(vertexShader, "Vertex")) return;
  
  // Create and compile the fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragment, 0);
  glCompileShader(fragmentShader);
  
  if(!_check_shader_status(fragmentShader, "Fragment")) return;
  
  // Link the vertex and fragment shader into a shader program
  shader->mProgram = glCreateProgram();
  glAttachShader(shader->mProgram, vertexShader);
  glAttachShader(shader->mProgram, fragmentShader);
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
      GLchar infoLog[length];
      glGetProgramInfoLog(shader->mProgram, length, &length, infoLog);
      gp_log_error("ShaderError: %s\n", infoLog);
    }
  }
  
  shader->mAttribute = glGetAttribLocation(shader->mProgram, "position");
}
