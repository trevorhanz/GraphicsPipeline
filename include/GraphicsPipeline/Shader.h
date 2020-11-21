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

#ifndef __GP_SHADER_H__
#define __GP_SHADER_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif
  
  GP_EXPORT void gp_shader_free(gp_shader* shader);
  GP_EXPORT void gp_shader_compile(gp_shader* shader, const char* vertex, const char* fragment);
  
#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Shader
  {
  private:
    inline Shader(gp_shader* shader);
  public:
    inline ~Shader();
    
    inline void Compile(const char* vertex, const char* fragment);
    
  private:
    gp_shader*        mShader;
    
    friend class Context;
    friend class Pipeline;
  };
  
  //
  // Implementation
  //
  Shader::Shader(gp_shader* shader) : mShader(shader) {}
  Shader::~Shader() {gp_shader_free(mShader);}
  void Shader::Compile(const char* vertex, const char* fragment) {gp_shader_compile(mShader, vertex, fragment);}
}
#endif // __cplusplus

#endif // __GP_SHADER_H__
