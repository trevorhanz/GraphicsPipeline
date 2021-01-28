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

//! \file Web.h

#ifndef __GP_WEB_H__
#define __GP_WEB_H__

#include "Common.h"
#include "Context.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Web
 * \{
 */

/*!
 * Creates a target object tied to this context using a canvas id.
 * \param context Context object used to create target.
 * \param id ID string of canvas object to be used.
 * \return Newly created target.
 */
GP_EXPORT gp_target* gp_context_target_new_from_id(gp_context* context, const char* id);

//! \} // Web

#ifdef __cplusplus
}

namespace GP
{
namespace Web
{
  /*!
   * \brief Wrapper class for ::gp_context extended for Web features.
   */
  class Context : public GP::Context
  {
  protected:
    //! Constructor
    inline Context(gp_context* context);
    
  public:
    //! Copy Constructor
    inline Context(const GP::Context& other);
    
    /*!
     * Creates a target object tied to this context using a canvas id.
     * \param id ID string of canvas object to be used.
     * \return Newly created Target.
     */
    inline Target* CreateTarget(const char* id);
  };
  
  /*
   * Implementation
   */
  Context::Context(gp_context* context) : GP::Context(context) {}
  Context::Context(const GP::Context& other) : GP::Context(other) {}
  Target* Context::CreateTarget(const char* id) {return new Target(gp_context_target_new_from_id(mContext, id));}
}
}

#endif

#endif // __GP_WEB_H__
