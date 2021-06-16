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
#include "Input.h"

#ifdef __cplusplus
#include <functional>

extern "C" {
#endif

/*!
 * \defgroup Window
 * \{
 */

/*!
 * Structure for window resize events.
 */
typedef struct
{
  int         width;
  int         height;
} gp_event_resize_t;

typedef void(*gp_event_click_callback_t)(const gp_event_click_t* click, gp_pointer* userData);
typedef void(*gp_event_move_callback_t)(const gp_event_move_t* move, gp_pointer* userData);
typedef void(*gp_event_enter_callback_t)(const gp_event_enter_t* enter, gp_pointer* userData);
typedef void(*gp_event_key_callback_t)(const gp_event_key_t* key, gp_pointer* userData);
typedef void(*gp_event_resize_callback_t)(const gp_event_resize_t* resize, gp_pointer* userData);

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

/*!
 * Retrieve the size of the window
 * \param window Pointer to window object.
 * \param width Pointer where the window width value will be set.
 * \param height Pointer where the window height value will be set.
 */
GP_EXPORT void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height);

/*!
 * Connect a callback for mouse click input events.
 * \param callback Function callback to be used for input mouse click events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_click_callback(gp_window* window, gp_event_click_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for mouse move input events.
 * \param callback Function callback to be used for input mouse move events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_move_callback(gp_window* window, gp_event_move_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for mouse enter/leave input event.
 * \param callback Function callback to be used for input mouse enter events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_enter_callback(gp_window* window, gp_event_enter_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for keyboard input event.
 * \param callback Function callback to be used for input key events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_key_callback(gp_window* window, gp_event_key_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for mouse enter/leave input event.
 * \param callback Function callback to be used for window resize events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_resize_callback(gp_window* window, gp_event_resize_callback_t callback, gp_pointer* userData);

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
    
    /*!
     * Retrieve the size of the window
     * \param width Pointer where the window width value will be set.
     * \param height Pointer where the window height value will be set.
     */
    inline void GetSize(unsigned int* width, unsigned int* height);
    
    /*!
     * Set the callback to be used for cursor click events.
     * \param callback Callback to be used.
     */
    inline void SetClickCallback(std::function<void(const gp_event_click_t*)> callback);
    
    /*!
     * Set the callback to be used for cursor move events.
     * \param callback Callback to be used.
     */
    inline void SetMoveCallback(std::function<void(const gp_event_move_t*)> callback);
    
    /*!
     * Set the callback to be used for cursor enter events.
     * \param callback Callback to be used.
     */
    inline void SetEnterCallback(std::function<void(const gp_event_enter_t*)> callback);
    
    /*!
     * Set the callback to be used for key events.
     * \param callback Callback to be used.
     */
    inline void SetKeyCallback(std::function<void(const gp_event_key_t*)> callback);
    
    /*!
     * Set the callback to be used for resize events.
     * \param callback Callback to be used.
     */
    inline void SetResizeCallback(std::function<void(const gp_event_resize_t*)> callback);
    
    //! Equal operator
    inline const Window& operator = (const Window& other);
    
  protected:
    inline static gp_context* GetContext(const Context& context);
    inline gp_window* GetWindow() {return mWindow;}
    
  private:
    typedef std::function<void(const gp_event_click_t*)>    ClickCallback;
    typedef std::function<void(const gp_event_move_t*)>     MoveCallback;
    typedef std::function<void(const gp_event_enter_t*)>    EnterCallback;
    typedef std::function<void(const gp_event_key_t*)>      KeyCallback;
    typedef std::function<void(const gp_event_resize_t*)>   ResizeCallback;
    
    template <typename T>
    static void HandleCallback(const T* click, gp_pointer* userData);
    
    template <typename T>
    struct CallbackData
    {
      T               mCallback;
    };
    
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
  void Window::GetSize(unsigned int* width, unsigned int* height) {gp_window_get_size(mWindow, width, height);}
  void Window::SetClickCallback(std::function<void(const gp_event_click_t*)> callback)
  {
    auto data = new CallbackData<ClickCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_click_callback(mWindow, HandleCallback<gp_event_click_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetMoveCallback(std::function<void(const gp_event_move_t*)> callback)
  {
    auto data = new CallbackData<MoveCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_move_callback(mWindow, HandleCallback<gp_event_move_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetEnterCallback(std::function<void(const gp_event_enter_t*)> callback)
  {
    auto data = new CallbackData<EnterCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_enter_callback(mWindow, HandleCallback<gp_event_enter_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetKeyCallback(std::function<void(const gp_event_key_t*)> callback)
  {
    auto data = new CallbackData<KeyCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_key_callback(mWindow, HandleCallback<gp_event_key_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetResizeCallback(std::function<void(const gp_event_resize_t*)> callback)
  {
    auto data = new CallbackData<ResizeCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_resize_callback(mWindow, HandleCallback<gp_event_resize_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  template <typename T>
  void Window::HandleCallback(const T* input, gp_pointer* userData)
  {
    auto data = (CallbackData<std::function<void(const T*)> >*)gp_pointer_get_pointer(userData);
    data->mCallback(input);
  }
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
