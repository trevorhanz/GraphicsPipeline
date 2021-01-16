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

//! \file Shader.h

#ifndef __GP_SHADER_H__
#define __GP_SHADER_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Shader
 * \{
 */

/*!
 * Free gp_shader object.
 * \param shader Shader object to be freed.
 */
GP_EXPORT void gp_shader_free(gp_shader* shader);

/*!
 * Compile a gp_shader object with a vertext and fragment source code.
 * \param shader Shader object to be compiled.
 * \param vertex Vertex shader source code.
 * \param fragment Fragment shader source code.
 */
GP_EXPORT void gp_shader_compile(gp_shader* shader, const char* vertex, const char* fragment);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_shader_uniform_new_by_name(gp_shader* shader, const char* name);

/*!
 * Set float[4] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_vec4(gp_uniform* uniform, float* data);

//! \} // Shader

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Uniform;
  
  /*!
   * \brief Wrapper class for ::gp_shader.
   */
  class Shader
  {
  private:
    //! Constructor
    inline Shader(gp_shader* shader);
  public:
    //! Destructor
    inline ~Shader();
    
    /*!
     * Compile this shader program with vertex and fragment source code.
     * \param vertex Vertex shader source code.
     * \param fragment Fragment shader source code.
     */
    inline void Compile(const char* vertex, const char* fragment);
    
    /*!
     * Create a new Uniform for a Shader object.
     * \param name Name of the uniform variable in the shader code.
     * \return Newly created Uniform object.
     */
    inline Uniform* CreateUniform(const char* name);
    
  private:
    gp_shader*        mShader;
    
    friend class Context;
    friend class Pipeline;
    friend class DrawOperation;
  };
  
  /*!
   * \brief Wrapper class for ::gp_uniform
   */
  class Uniform
  {
  private:
    //! Constructor
    inline Uniform(gp_uniform* uniform);
    
  public:
    //! Destructor
    inline ~Uniform();
    
    /*!
     * Set float[4] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetVec4(float* data);
    
  private:
    gp_uniform*       mUniform;
    
    friend class Shader;
    friend class DrawOperation;
  };
  
  //
  // Implementation
  //
  Shader::Shader(gp_shader* shader) : mShader(shader) {}
  Shader::~Shader() {gp_shader_free(mShader);}
  void Shader::Compile(const char* vertex, const char* fragment) {gp_shader_compile(mShader, vertex, fragment);}
  Uniform* Shader::CreateUniform(const char* name) {return new Uniform(gp_shader_uniform_new_by_name(mShader, name));}
  
  Uniform::Uniform(gp_uniform* uniform) : mUniform(uniform) {}
  Uniform::~Uniform() {}
  void Uniform::SetVec4(float* data) {gp_uniform_set_vec4(mUniform, data);}
}
#endif // __cplusplus

#endif // __GP_SHADER_H__
