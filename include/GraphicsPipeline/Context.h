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
#include "Types.h"
#include "System.h"
#include "Object.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Context
 * \{
 */

/*!
 * Create a new context object tied to an existing system object.
 * \param system Pointer to existing system object.
 * \return Pointer to new context object.
 */
GP_EXPORT gp_context* gp_context_new(gp_system* system);

//! \} // Context

#ifdef __cplusplus
} // extern "C"

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_context
   */
  class Context : public Object
  {
  public:
    //! Constructor
    inline Context(gp_context* context);
    
    //! Constructor
    inline Context(const System& system);
  };
  
  /*
   * Implementation
   */
  Context::Context(gp_context* context) : Object((gp_object*)context) {}
  Context::Context(const System& system) : Object((void*)gp_context_new((gp_system*)system.GetObject())) {}
}
#endif

#endif // __GP_CONTEXT_H__
