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

//! \file Window.h

#ifndef __GP_WINDOW_H__
#define __GP_WINDOW_H__

#include "Common.h"
#include "Types.h"
#include "Context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Window
 * \{
 */

/*!
 * Create a new gp_window object tied to a context.
 * \param context Context object used to create window.
 * \return Newly created window.
 */
GP_EXPORT gp_window* gp_window_new(gp_context* context);

/*!
 * Increase window object reference count.
 * \param window Window object for which to increase the ref count.
 */
GP_EXPORT void gp_window_ref(gp_window* window);

/*!
 * Decrease window object reference count.
 * \param window Window object for which to decrease the ref count.
 */
GP_EXPORT void gp_window_unref(gp_window* window);

/*!
 * Get the gp_pipeline tied to a gp_window.
 * \param window Pointer to window object.
 * \return Pointer to pipeline object.
 */
GP_EXPORT gp_pipeline* gp_window_get_pipeline(gp_window* window);

/*!
 * Schedule the window to be redrawn.
 * \param window Pointer to window object.
 */
GP_EXPORT void gp_window_redraw(gp_window* window);

//! \} // Window

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_window.
   */
  class Window
  {
  protected:
    //! Constructor
    inline Window(gp_window* window);
    
  public:
    //! Constructor
    inline Window(const Context& context);
    
    //! Copy Constructor
    inline Window(const Window& other);
    
    //! Destructor
    inline ~Window();
    
    /*!
     * Retrieve GP::Pipeline object.
     * \return GP::Pipeline used by this window.
     */
    inline Pipeline GetPipeline();
    
    /*!
     * Schedules the window to be redrawn.
     */
    inline void Redraw();
    
    //! Equal operator
    inline const Window& operator = (const Window& other);
    
  protected:
    inline static gp_context* GetContext(const Context& context);
    
  private:
    gp_window*        mWindow;
  };
  
  //
  // Implementation
  //
  Window::Window(gp_window* window) : mWindow(window) {}
  Window::Window(const Context& context) : mWindow(gp_window_new(context.mContext)) {}
  Window::Window(const Window& other)
  {
    mWindow = other.mWindow;
    gp_window_ref(mWindow);
  }
  Window::~Window() {gp_window_unref(mWindow);}
  Pipeline Window::GetPipeline() {return Pipeline(gp_window_get_pipeline(mWindow));}
  void Window::Redraw() {gp_window_redraw(mWindow);}
  const Window& Window::operator = (const Window& other)
  {
    gp_window_unref(mWindow);
    mWindow = other.mWindow;
    gp_window_ref(mWindow);
    return *this;
  }
  gp_context* Window::GetContext(const Context& context) {return context.mContext;}
}

#endif // __cplusplus

#endif // __GP_WINDOW_H__
