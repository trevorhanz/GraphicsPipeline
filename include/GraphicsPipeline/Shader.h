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
#include "Object.h"

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
 * Compile a gp_shader object with a vertext and fragment source code.
 * \param shader Shader object to be compiled.
 * \param source Object containing source code.
 */
GP_EXPORT void gp_shader_compile(gp_shader* shader, gp_shader_source* source);

#define GP_UNIFORM_DEFINITION(uname, typeName, description)\
  /*!\
   Create a new gp_uniform for a gp_shader object with type of uname.\
   \param shader Shader object used to create the gp_uniform object.\
   \param name Name of the uniform variable in the shader code.\
   \return Newly created gp_uniform object.\
   */\
  GP_EXPORT gp_uniform* gp_uniform_##uname##_new_by_name(gp_shader* shader, const char* name);\
  \
  /*!\
   Set description data into a gp_uniform object of type uname. \
   \param uniform Uniform object to have data loaded. \
   \param data Pointer to array of data to be loaded. \
   */\
  GP_EXPORT void gp_uniform_##uname##_set(gp_uniform* uniform, typeName data);\
  \
  /*!\
   Get description data from a gp_uniform object of type uname.\
   \param uniform Uniform object of typeName uname.\
   */\
  GP_EXPORT typeName gp_uniform_##uname##_get(gp_uniform* uniform);

GP_UNIFORM_DEFINITION(texture, gp_texture*, gp_texture)

GP_UNIFORM_DEFINITION(float, float, float)
GP_UNIFORM_DEFINITION(vec2, float*, float[2])
GP_UNIFORM_DEFINITION(vec3, float*, float[3])
GP_UNIFORM_DEFINITION(vec4, float*, float[4])
GP_UNIFORM_DEFINITION(mat3, float*, float[3][3])
GP_UNIFORM_DEFINITION(mat4, float*, float[4][4])

GP_UNIFORM_DEFINITION(double, double, double)
GP_UNIFORM_DEFINITION(dvec2, double*, double[2])
GP_UNIFORM_DEFINITION(dvec3, double*, double[3])
GP_UNIFORM_DEFINITION(dvec4, double*, double[4])
GP_UNIFORM_DEFINITION(dmat3, double*, double[3][3])
GP_UNIFORM_DEFINITION(dmat4, double*, double[4][4])

#undef GP_UNIFORM_DEFINITION

//! \} // Shader

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  class Uniform;
  
  /*!
   * \brief Wrapper class for ::gp_shader_source.
   */
  class ShaderSource : public Object
  {
  public:
    //! Constructor
    inline ShaderSource(gp_shader_source* source);
    
    //! Constructor
    inline ShaderSource();
    
    /*!
     * Store a shader source string in this shader source obect.
     * \param type The type shader stage for which the string should be compiled.
     * \param str Text containing shader source code.
     */
    inline void AddString(GP_SHADER_SOURCE_TYPE type, const char* str);
  };
  
  /*!
   * \brief Wrapper class for ::gp_shader.
   */
  class Shader : public Object
  {
  public:
    inline Shader();
    
    //! Constructor
    inline Shader(gp_shader* shader);
    
    //! Constructor
    inline Shader(const Context& context);
    
    /*!
     * Compile this shader program with vertex and fragment source code.
     * \param source The shader source object to be compiled.
     */
    inline void Compile(const ShaderSource& source);
  };
  
  /*!
   * \brief Wrapper class for ::gp_uniform
   */
  class Uniform : public Object
  {
  protected:
    //! Constructor. Only for internal use.
    inline Uniform(void* uniform);
    
  public:
    //! Constructor
    inline Uniform(gp_uniform* uniform);
    
    //! Copy Constructor
    inline Uniform(const Object& other);
  };
  
#define CXX_UNIFORM(CXXname, Cname, type)\
  /*!\
   * \brief Uniform specialized for CXXname\
   */\
  class Uniform##CXXname : public Uniform\
  {\
  public:\
    inline Uniform##CXXname();\
    \
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
  Uniform##CXXname::Uniform##CXXname() : Uniform((void*)0) {}\
  Uniform##CXXname::Uniform##CXXname(const Shader& shader, const char* name) : Uniform((void*)gp_uniform_##Cname##_new_by_name((gp_shader*)GetObject(shader), name)) {}\
  void Uniform##CXXname::Set(type data) {gp_uniform_##Cname##_set((gp_uniform*)GetObject(*this), data);}\
  type Uniform##CXXname::Get() {return gp_uniform_##Cname##_get((gp_uniform*)GetObject(*this));}
  
  /*!
   * \brief Uniform specialized for Texture
   */
  class UniformTexture : public Uniform
  {
  public:
    inline UniformTexture();
    
    /*! Constructor */
    inline UniformTexture(const Shader& shader, const char* name);
    
    /*!
     * Set texture data into Uniform object
     */
    inline void Set(const Texture& texture);
    
    /*!
     * Get texture data into Uniform object
     */
    inline gp_texture* Get();
  };
  UniformTexture::UniformTexture() : Uniform((void*)0) {}
  UniformTexture::UniformTexture(const Shader& shader, const char* name) : Uniform((void*)gp_uniform_texture_new_by_name((gp_shader*)GetObject(shader), name)) {}
  void UniformTexture::Set(const Texture& texture) {gp_uniform_texture_set((gp_uniform*)GetObject(*this), (gp_texture*)GetObject(texture));}
  gp_texture* UniformTexture::Get() {return 0;}
  
  CXX_UNIFORM(Float, float, float)
  CXX_UNIFORM(Vec2, vec2, float*)
  CXX_UNIFORM(Vec3, vec3, float*)
  CXX_UNIFORM(Vec4, vec4, float*)
  CXX_UNIFORM(Mat3, mat3, float*)
  CXX_UNIFORM(Mat4, mat4, float*)
  
  CXX_UNIFORM(Double, double, double)
  CXX_UNIFORM(DVec2, dvec2, double*)
  CXX_UNIFORM(DVec3, dvec3, double*)
  CXX_UNIFORM(DVec4, dvec4, double*)
  CXX_UNIFORM(DMat3, dmat3, double*)
  CXX_UNIFORM(DMat4, dmat4, double*)
  
#undef CXX_UNIFORM
  
  //
  // Implementation
  //
  ShaderSource::ShaderSource(gp_shader_source* source) : Object((gp_object*)source) {}
  ShaderSource::ShaderSource() : Object((void*)gp_shader_source_new()) {}
  void ShaderSource::AddString(GP_SHADER_SOURCE_TYPE type, const char* str)
  {
    gp_shader_source_add_from_string((gp_shader_source*)GetObject(*this), type, str);
  }
  
  Shader::Shader() : Object((void*)0) {}
  Shader::Shader(gp_shader* shader) : Object((gp_object*)shader) {}
  Shader::Shader(const Context& context) : Object((void*)gp_shader_new((gp_context*)GetObject(context))) {}
  void Shader::Compile(const ShaderSource& source)
  {
    gp_shader_compile((gp_shader*)GetObject(*this), (gp_shader_source*)GetObject(source));
  }
  
  Uniform::Uniform(void* uniform) : Object((void*)uniform) {}
  Uniform::Uniform(gp_uniform* uniform) : Object((gp_object*)uniform) {}
  Uniform::Uniform(const Object& other) : Object(other) {}
}
#endif // __cplusplus

#endif // __GP_SHADER_H__
