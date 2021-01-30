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

//! \file Context.h

#ifndef __GP_CONTEXT_H__
#define __GP_CONTEXT_H__

#include "Common.h"
#include "Pipeline.h"
#include "Types.h"
#include "Target.h"
#include "Array.h"
#include "Shader.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Context
 * \{
 */

/*!
 * Increase context object reference count.
 * \param context Context object for which to increase the ref count.
 */
GP_EXPORT void gp_context_ref(gp_context* context);

/*!
 * Decrease context object reference count.
 * \param context Context object for which to decrease the ref count.
 */
GP_EXPORT void gp_context_unref(gp_context* context);

/*!
 * Create a new gp_target object tied to a context.
 * \param context Context object used to create target.
 * \return Newly created target.
 */
GP_EXPORT gp_target* gp_context_target_new(gp_context* context);

/*!
 * Create a new gp_array object tied to a context.
 * \param context Context object used to create array.
 * \return Newly created array.
 */
GP_EXPORT gp_array* gp_context_array_new(gp_context* context);

/*!
 * Create a new gp_texture object tied to a context.
 * \param context Context object used to create texture.
 * \return Newly created texture.
 */
GP_EXPORT gp_texture* gp_context_texture_new(gp_context* context);

/*!
 * Create a new gp_shader object tied to a context.
 * \param context Context object used to create shader.
 * \return Newly created shader.
 */
GP_EXPORT gp_shader* gp_context_shader_new(gp_context* context);

//! \} // Context

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_context
   */
  class Context
  {
  protected:
    //! Constructor
    inline Context(gp_context* context);
  public:
    //! Copy Constructor
    inline Context(const Context& other);
    
    //! Destructor
    inline ~Context();
    
    /*!
     * Create a Target object tied to this Context.
     * \return Newly created Target object.
     */
    inline Target* CreateTarget();
    
    /*!
     * Create an Array object tied to this Context.
     * \return Newly created Array object.
     */
    inline Array CreateArray();
    
    /*!
     * Create a Shader object tied to this Context.
     * \return Newly created Shader object.
     */
    inline Shader CreateShader();
    
    //! Equal operator
    inline const Context& operator = (const Context& other);
    
  protected:
    gp_context*           mContext;
    
    friend class System;
  };
  
  /*
   * Implementation
   */
  Context::Context(gp_context* context) : mContext(context) {}
  Context::Context(const Context& other)
  {
    mContext = other.mContext;
    gp_context_ref(mContext);
  }
  Context::~Context() {gp_context_unref(mContext);}
  Target* Context::CreateTarget() {return new Target(gp_context_target_new(mContext));}
  Array Context::CreateArray() {return Array(gp_context_array_new(mContext));}
  Shader Context::CreateShader() {return Shader(gp_context_shader_new(mContext));}
  const Context& Context::operator = (const Context& other)
  {
    gp_context_unref(mContext);
    mContext = other.mContext;
    gp_context_ref(mContext);
    return *this;
  }
}
#endif

#endif // __GP_CONTEXT_H__
