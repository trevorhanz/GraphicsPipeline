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
#include "Context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Shader
 * \{
 */

/*!
 * Create a new gp_shader_source object.
 * \return Newly created shader source.
 */
GP_EXPORT gp_shader_source* gp_shader_source_new();

/*!
 * Increase shader source ref count.
 * \param shader Shader source to have its ref count increased.
 */
GP_EXPORT void gp_shader_source_ref(gp_shader_source* source);

/*!
 * Decreate shader source ref count.
 * \param shader Shader source to have its ref count decreased.
 */
GP_EXPORT void gp_shader_source_unref(gp_shader_source* source);

/*!
 * Store a shader source string in this shader source obect.
 * \param source The shader source object for which to add the source code.
 * \param type The type shader stage for which the string should be compiled.
 * \param str Text containing shader source code.
 */
GP_EXPORT void gp_shader_source_add_from_string(gp_shader_source* source, GP_SHADER_SOURCE_TYPE type, const char* str);

/*!
 * Create a new gp_shader object tied to a context.
 * \param context Context object used to create shader.
 * \return Newly created shader.
 */
GP_EXPORT gp_shader* gp_shader_new(gp_context* context);

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
 * \param source Object containing source code.
 */
GP_EXPORT void gp_shader_compile(gp_shader* shader, gp_shader_source* source);

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
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_texture_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_float_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_vec2_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_vec3_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_vec4_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_mat3_new_by_name(gp_shader* shader, const char* name);

/*!
 * Create a new gp_uniform for a gp_shader object.
 * \param shader Shader object used to create the gp_uniform object.
 * \param name Name of the uniform variable in the shader code.
 * \return Newly created gp_uniform object.
 */
GP_EXPORT gp_uniform* gp_uniform_mat4_new_by_name(gp_shader* shader, const char* name);

/*!
 * Set texture data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param texture Texture object to be used.
 */
GP_EXPORT void gp_uniform_texture_set(gp_uniform* uniform, gp_texture* texture);

/*!
 * Set float data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_float_set(gp_uniform* uniform, float data);

/*!
 * Set float[2] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_vec2_set(gp_uniform* uniform, float* data);

/*!
 * Set float[3] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_vec3_set(gp_uniform* uniform, float* data);

/*!
 * Set float[4] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_vec4_set(gp_uniform* uniform, float* data);

/*!
 * Set float[3][3] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_mat3_set(gp_uniform* uniform, float* data);

/*!
 * Set float[4][4] data into a gp_uniform object.
 * \param uniform Uniform object to have data loaded.
 * \param data Pointer to array of data to be loaded.
 */
GP_EXPORT void gp_uniform_mat4_set(gp_uniform* uniform, float* data);

/*!
 * Get texture object from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT gp_texture* gp_uniform_texture_get(gp_uniform* uniform);

/*!
 * Get float data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float gp_uniform_float_get(gp_uniform* uniform);

/*!
 * Get float[2] data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float* gp_uniform_vec2_get(gp_uniform* uniform);

/*!
 * Get float[3] data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float* gp_uniform_vec3_get(gp_uniform* uniform);

/*!
 * Get float[4] data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float* gp_uniform_vec4_get(gp_uniform* uniform);

/*!
 * Get float[3][3] data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float* gp_uniform_mat3_get(gp_uniform* uniform);

/*!
 * Get float[4][4] data from a gp_uniform object.
 * \param uniform Uniform object of type float.
 */
GP_EXPORT float* gp_uniform_mat4_get(gp_uniform* uniform);

//! \} // Shader

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Uniform;
  
  /*!
   * \brief Wrapper class for ::gp_shader_source.
   */
  class ShaderSource
  {
  public:
    //! Constructor
    inline ShaderSource();
    
    //! Copy Constructor
    inline ShaderSource(const ShaderSource& other);
    
    //! Destructor
    inline ~ShaderSource();
    
    /*!
     * Store a shader source string in this shader source obect.
     * \param type The type shader stage for which the string should be compiled.
     * \param str Text containing shader source code.
     */
    inline void AddString(GP_SHADER_SOURCE_TYPE type, const char* str);
    
    //! Equal operator
    inline const ShaderSource& operator = (const ShaderSource& other);
    
  private:
    gp_shader_source* mSource;
    
    friend class Shader;
  };
  
  /*!
   * \brief Wrapper class for ::gp_shader.
   */
  class Shader
  {
  public:
    //! Constructor
    inline Shader(const Context& context);
    
    //! Copy Constructor
    inline Shader(const Shader& other);
    
    //! Destructor
    inline ~Shader();
    
    /*!
     * Compile this shader program with vertex and fragment source code.
     * \param source The shader source object to be compiled.
     */
    inline void Compile(const ShaderSource& source);
    
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
    
    friend class Pipeline;
    friend class DrawOperation;
    friend class Uniform;
  };
  
  /*!
   * \brief Wrapper class for ::gp_uniform
   */
  class Uniform
  {
  protected:
    //! Constructor
    inline Uniform(gp_uniform* uniform);
    
  public:
    //! Copy Constructor
    inline Uniform(const Uniform& other);
    
    //! Destructor
    inline ~Uniform();
    
    //! Equal operator
    inline const Uniform& operator = (const Uniform& other);
    
  protected:
    inline gp_shader* GetShader(const Shader& shader);
    
    gp_uniform*       mUniform;
    
    friend class Shader;
    friend class DrawOperation;
  };
  
#define CXX_UNIFORM(CXXname, Cname, type)\
  /*!\
   * \brief Uniform specialized for CXXname\
   */\
  class Uniform##CXXname : public Uniform\
  {\
  public:\
    /*! Constructor */\
    inline Uniform##CXXname(const Shader& shader, const char* name);\
    \
    /*!\
     * Set Cname data into Uniform object\
     */\
    inline void Set(type data);\
    \
    /*!\
     * Get Cname data into Uniform object\
     */\
    inline type Get();\
  };\
  Uniform##CXXname::Uniform##CXXname(const Shader& shader, const char* name) : Uniform(gp_uniform_##Cname##_new_by_name(GetShader(shader), name)) {}\
  void Uniform##CXXname::Set(type data) {gp_uniform_##Cname##_set(mUniform, data);}\
  type Uniform##CXXname::Get() {return gp_uniform_##Cname##_get(mUniform);}
  
  CXX_UNIFORM(Texture, texture, gp_texture*)
  CXX_UNIFORM(Float, float, float)
  CXX_UNIFORM(Vec2, vec2, float*)
  CXX_UNIFORM(Vec3, vec3, float*)
  CXX_UNIFORM(Vec4, vec4, float*)
  CXX_UNIFORM(Mat3, mat3, float*)
  CXX_UNIFORM(Mat4, mat4, float*)
  
#undef CXX_UNIFORM
  
  //
  // Implementation
  //
  ShaderSource::ShaderSource() : mSource(gp_shader_source_new()) {}
  ShaderSource::ShaderSource(const ShaderSource& other)
  {
    mSource = other.mSource;
    gp_shader_source_ref(mSource);
  }
  ShaderSource::~ShaderSource() {gp_shader_source_unref(mSource);}
  void ShaderSource::AddString(GP_SHADER_SOURCE_TYPE type, const char* str) {gp_shader_source_add_from_string(mSource, type, str);}
  const ShaderSource& ShaderSource::operator = (const ShaderSource& other)
  {
    gp_shader_source_unref(mSource);
    mSource = other.mSource;
    gp_shader_source_ref(mSource);
    return *this;
  }
  
  Shader::Shader(const Context& context) : mShader(gp_shader_new(context.mContext)) {}
  Shader::Shader(const Shader& other)
  {
    mShader = other.mShader;
    gp_shader_ref(mShader);
  }
  Shader::~Shader() {gp_shader_unref(mShader);}
  void Shader::Compile(const ShaderSource& source) {gp_shader_compile(mShader, source.mSource);}
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
  const Uniform& Uniform::operator = (const Uniform& other)
  {
    gp_uniform_unref(mUniform);
    mUniform = other.mUniform;
    gp_uniform_ref(mUniform);
    return *this;
  }
  gp_shader* Uniform::GetShader(const Shader& shader) {return shader.mShader;}
}
#endif // __cplusplus

#endif // __GP_SHADER_H__
