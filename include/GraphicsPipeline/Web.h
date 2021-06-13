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
#include "Window.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Web
 * \{
 */

/*!
 * Creates a new context object tied to an existing system object using a canvas id.
 * \param system System object used to create context.
 * \param id ID string of canvas object to be used.
 * \return Newly created window.
 */
GP_EXPORT gp_context* gp_context_new_from_id(gp_system* system, const char* id);

/*!
 * Creates a window object tied to this context using a canvas id.
 * \param context Context object used to create window.
 * \param id ID string of element to be used.
 * \return Newly created window.
 */
GP_EXPORT gp_window* gp_window_new_from_id(gp_context* context, const char* id);

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
  public:
    //! Constructor
    inline Context(const GP::System& system, const char* id);
    
    //! Copy Constructor
    inline Context(const GP::Context& other);
  };
  
  /*!
   * \brief Wrapper class for ::gp_window extended for Web features.
   */
  class Window : public GP::Window
  {
  public:
    //! Constructor
    inline Window(const GP::Context& context, const char* id);
    
    //! Copy Constructor
    inline Window(const GP::Window& other);
  };
  
  /*
   * Implementation
   */
  Context::Context(const GP::System& system, const char* id) : GP::Context(gp_context_new_from_id(GetSystem(system), id)) {}
  
  Window::Window(const GP::Context& context, const char* id) : GP::Window(gp_window_new_from_id(GetContext(context), id)) {}
}
}

#endif

#endif // __GP_WEB_H__
