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
#include "Texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Shader
 * \{
 */

/*!
 * Increase shader ref count.
 * \param shader Shader to have its ref count increased.
 */
GP_EXPORT void gp_shader_ref(gp_shader* shader);

/*!
 * Decreate shader ref count.
 * \param shader Shader to have its ref count decreased.
 */
GP_EXPORT void gp_shader_unref(gp_shader* shader);

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
 * Increase uniform ref count.
 * \param uniform Uniform to have its ref count increased.
 */
GP_EXPORT void gp_uniform_ref(gp_uniform* uniform);

/*!
 * Decreate uniform ref count.
 * \param uniform Uniform to have its ref count decreased.
 */
GP_EXPORT void gp_uniform_unref(gp_uniform* uniform);

/*!
 * Set textire data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_texture(gp_uniform* uniform, gp_texture* texture);

/*!
 * Set float data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_float(gp_uniform* uniform, float data);

/*!
 * Set float[2] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_vec2(gp_uniform* uniform, float* data);

/*!
 * Set float[3] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_vec3(gp_uniform* uniform, float* data);

/*!
 * Set float[4] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_vec4(gp_uniform* uniform, float* data);

/*!
 * Set float[3][3] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_mat3(gp_uniform* uniform, float* data);

/*!
 * Set float[4][4] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_set_mat4(gp_uniform* uniform, float* data);

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
    //! Copy Constructor
    inline Shader(const Shader& other);
    
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
    inline Uniform CreateUniform(const char* name);
    
    //! Equal operator
    inline const Shader& operator = (const Shader& other);
    
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
    //! Copy Constructor
    inline Uniform(const Uniform& other);
    
    //! Destructor
    inline ~Uniform();
    
    /*!
     * Set float data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetTexture(Texture* texture);
    
    /*!
     * Set float data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetFloat(float data);
    
    /*!
     * Set float[2] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetVec2(float* data);
    
    /*!
     * Set float[3] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetVec3(float* data);
    
    /*!
     * Set float[4] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetVec4(float* data);
    
    /*!
     * Set float[3][3] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetMat3(float* data);
    
    /*!
     * Set float[4][4] data into a Uniform object.
     * \param data Pointer to array of data to be loaded.
     */
    inline void SetMat4(float* data);
    
    //! Equal operator
    inline const Uniform& operator = (const Uniform& other);
    
  private:
    gp_uniform*       mUniform;
    
    friend class Shader;
    friend class DrawOperation;
  };
  
  //
  // Implementation
  //
  Shader::Shader(gp_shader* shader) : mShader(shader) {}
  Shader::Shader(const Shader& other)
  {
    mShader = other.mShader;
    gp_shader_ref(mShader);
  }
  Shader::~Shader() {gp_shader_unref(mShader);}
  void Shader::Compile(const char* vertex, const char* fragment) {gp_shader_compile(mShader, vertex, fragment);}
  Uniform Shader::CreateUniform(const char* name) {return Uniform(gp_shader_uniform_new_by_name(mShader, name));}
  const Shader& Shader::operator = (const Shader& other)
  {
    gp_shader_unref(mShader);
    mShader = other.mShader;
    gp_shader_ref(mShader);
    return *this;
  }
  
  Uniform::Uniform(gp_uniform* uniform) : mUniform(uniform) {}
  Uniform::Uniform(const Uniform& other)
  {
    mUniform = other.mUniform;
    gp_uniform_ref(mUniform);
  }
  Uniform::~Uniform() {gp_uniform_unref(mUniform);}
  void Uniform::SetTexture(Texture* texture) {gp_uniform_set_texture(mUniform, texture->mTexture);}
  void Uniform::SetFloat(float data) {gp_uniform_set_float(mUniform, data);}
  void Uniform::SetVec2(float* data) {gp_uniform_set_vec2(mUniform, data);}
  void Uniform::SetVec3(float* data) {gp_uniform_set_vec3(mUniform, data);}
  void Uniform::SetVec4(float* data) {gp_uniform_set_vec4(mUniform, data);}
  void Uniform::SetMat3(float* data) {gp_uniform_set_mat3(mUniform, data);}
  void Uniform::SetMat4(float* data) {gp_uniform_set_mat4(mUniform, data);}
  const Uniform& Uniform::operator = (const Uniform& other)
  {
    gp_uniform_unref(mUniform);
    mUniform = other.mUniform;
    gp_uniform_ref(mUniform);
    return *this;
  }
}
#endif // __cplusplus

#endif // __GP_SHADER_H__
