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
#include "Object.h"
#include "Monitor.h"

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

/*!
 * Structure for window movement events.
 */
typedef struct
{
  int         x;
  int         y;
} gp_event_move_t;

typedef void(*gp_event_click_callback_t)(const gp_event_click_t* click, gp_pointer* userData);
typedef void(*gp_event_scroll_callback_t)(const gp_event_scroll_t* scroll, gp_pointer* userData);
typedef void(*gp_event_track_callback_t)(const gp_event_track_t* move, gp_pointer* userData);
typedef void(*gp_event_enter_callback_t)(const gp_event_enter_t* enter, gp_pointer* userData);
typedef void(*gp_event_key_callback_t)(const gp_event_key_t* key, gp_pointer* userData);
typedef void(*gp_event_resize_callback_t)(const gp_event_resize_t* resize, gp_pointer* userData);
typedef void(*gp_event_move_callback_t)(const gp_event_move_t* resize, gp_pointer* userData);

/*!
 * Create a new gp_window object tied to a context.
 * \param context Context object used to create window.
 * \return Newly created window.
 */
GP_EXPORT gp_window* gp_window_new(gp_context* context);

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
 * Set the window's title.
 * \param window Pointer to window object.
 * \param title New window title.
 */
GP_EXPORT void gp_window_set_title(gp_window* window, const char* title);

/*!
 * Set the window's type.  The default is GP_WINDOW_TYPE_NORMAL.
 * \param window Pointer to window object.
 * \param type New window type.
 */
GP_EXPORT void gp_window_set_type(gp_window* window, GP_WINDOW_TYPE type);

/*!
 * Set the minimum size of the window.
 * \param window Pointer to window object.
 * \param width New minimum width value pixels. Use -1 for no minimum.
 * \param height New minimum height value in pixels. Use -1 for no minimum.
 */
GP_EXPORT void gp_window_set_min_size(gp_window* window, int width, int height);

/*!
 * Set the maximum size of the window.
 * \param window Pointer to window object.
 * \param width New maximum width value pixels. Use -1 for no maximum.
 * \param height New maximum height value in pixels. Use -1 for no maximum.
 */
GP_EXPORT void gp_window_set_max_size(gp_window* window, int width, int height);

/*!
 * Set the size of the window.
 * \param window Pointer to window object.
 * \param width New width value pixels.
 * \param height New height value in pixels.
 */
GP_EXPORT void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height);

/*!
 * Retrieve the size of the window.
 * \param window Pointer to window object.
 * \param width Pointer where the window width value will be set.
 * \param height Pointer where the window height value will be set.
 */
GP_EXPORT void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height);

/*!
 * Set the position of the window.
 * \param window Pointer to window object.
 * \param x New X position of the window.
 * \param y New Y position of the window.
 */
GP_EXPORT void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y);

/*!
 * Retrieve the position of the window.
 * \param window Pointer to window object.
 * \param x Pointer where the window's x position value will be set.
 * \param y Pointer where the window's y position value will be set.
 */
GP_EXPORT void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y);

/*!
 * Make the window visible.  Nothing happens if the window is already visible.
 * \param window Pointer to window object.
 */
GP_EXPORT void gp_window_show(gp_window* window);

/*!
 * Make the window hidden.  Nothing happens if the window is already hidden.
 * \param window Pointer to window object.
 */
GP_EXPORT void gp_window_hide(gp_window* window);

/*!
 * Retrieve whether or not the window is shown.
 * \param window Pointer to window object.
 * \return Returns 1 if the window is visible, else it returns 0.
 */
GP_EXPORT int gp_window_get_shown(gp_window* window);

/*!
 * Set the window to be fullscreened to the specified monitor.
 * \param window Pointer to window object.
 * \param monitor Monitor onto which to be fullscreened.  Set to NULL to remove fullscreen.
 */
GP_EXPORT void gp_window_set_fullscreen(gp_window* window, gp_monitor* monitor);

/*!
 * Retrieve the current fullscreened monitor.
 * \param window Pointer to window object.
 * \return The monitor for which the window is currently fullscreened or NULL if not fullscreened.
 */
GP_EXPORT gp_monitor* gp_window_get_fullscreen(gp_window* window);

/*!
 * Connect a callback for mouse click input events.
 * \param callback Function callback to be used for input mouse click events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_click_callback(gp_window* window, gp_event_click_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for scroll input events.
 * \param callback Function callback to be used for input scroll events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_scroll_callback(gp_window* window, gp_event_scroll_callback_t callback, gp_pointer* userData);

/*!
 * Connect a callback for mouse move input events.
 * \param callback Function callback to be used for input mouse move events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_track_callback(gp_window* window, gp_event_track_callback_t callback, gp_pointer* userData);

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

/*!
 * Connect a callback for window movement events.
 * \param callback Function callback to be used for window movement events. Set to NULL to remove callback.
 * \param userData Pointer to user defined data.
 */
GP_EXPORT void gp_window_set_move_callback(gp_window* window, gp_event_move_callback_t callback, gp_pointer* userData);

//! \} // Window

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_window.
   */
  class Window : public Object
  {
  public:
    inline Window();
    
    //! Constructor
    inline Window(gp_window* window);
    
    //! Constructor
    inline Window(const Context& context);
    
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
     * Set the window's title.
     * \param width New window title.
     */
    inline void SetTitle(const char* title);
    
    /*!
     * Set the window's type.
     * \param type New window type.
     */
    inline void SetType(GP_WINDOW_TYPE type);
    
    /*!
     * Set the minimum size of the window.
     * \param width New minimum width value pixels. Use -1 for no minimum.
     * \param height New minimum height value in pixels. Use -1 for no minimum.
     */
    inline void SetMinSize(int width, int height);
    
    /*!
     * Set the maximum size of the window.
     * \param width New maximum width value pixels. Use -1 for no maximum.
     * \param height New maximum height value in pixels. Use -1 for no maximum.
     */
    inline void SetMaxSize(int width, int height);
    
    /*!
     * Set the size of the window.
     * \param width New width value pixels.
     * \param height New height value in pixels.
     */
    inline void SetSize(unsigned int width, unsigned int height);
    
    /*!
     * Retrieve the size of the window
     * \param width Pointer where the window width value will be set.
     * \param height Pointer where the window height value will be set.
     */
    inline void GetSize(unsigned int* width, unsigned int* height);
    
    /*!
     * Set the position of the window.
     * \param x New X position of the window.
     * \param y New Y position of the window.
     */
    inline void SetPosition(unsigned int x, unsigned int y);
    
    /*!
     * Retrieve the position of the window.
     * \param x Pointer where the window's x position value will be set.
     * \param y Pointer where the window's y position value will be set.
     */
    inline void GetPosition(unsigned int* x, unsigned int* y);
    
    /*!
     * Make the window visible.  Nothing happens if the window is already visible.
     */
    inline void Show();
    
    /*!
     * Make the window hidden.  Nothing happens if the window is already hidden.
     */
    inline void Hide();
    
    /*!
     * Retrieve whether or not the window is shown.
     * \return Returns true if the window is visible.
     */
    inline bool GetShown();
    
    /*!
     * Set the window to be fullscreened to the specified monitor.
     * \param monitor Monitor onto which to be fullscreened.
     */
    inline void SetFullscreen(Monitor monitor);
    
    /*!
     * Retrieve the current fullscreened monitor.
     * \return The monitor for which the window is currently fullscreened.
     */
    inline Monitor GetFullscreen();
    
    /*!
     * Set the callback to be used for cursor click events.
     * \param callback Callback to be used.
     */
    inline void SetClickCallback(std::function<void(const gp_event_click_t*)> callback);
    
    /*!
     * Set the callback to be used for scroll events.
     * \param callback Callback to be used.
     */
    inline void SetScrollCallback(std::function<void(const gp_event_scroll_t*)> callback);
    
    /*!
     * Set the callback to be used for cursor move events.
     * \param callback Callback to be used.
     */
    inline void SetTrackCallback(std::function<void(const gp_event_track_t*)> callback);
    
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
    
    /*!
     * Set the callback to be used for cursor move events.
     * \param callback Callback to be used.
     */
    inline void SetMoveCallback(std::function<void(const gp_event_move_t*)> callback);
    
  private:
    typedef std::function<void(const gp_event_click_t*)>    ClickCallback;
    typedef std::function<void(const gp_event_scroll_t*)>   ScrollCallback;
    typedef std::function<void(const gp_event_track_t*)>    TrackCallback;
    typedef std::function<void(const gp_event_enter_t*)>    EnterCallback;
    typedef std::function<void(const gp_event_key_t*)>      KeyCallback;
    typedef std::function<void(const gp_event_resize_t*)>   ResizeCallback;
    typedef std::function<void(const gp_event_move_t*)>     MoveCallback;
    
    template <typename T>
    static void HandleCallback(const T* input, gp_pointer* userData);
    
    template <typename T>
    struct CallbackData
    {
      T               mCallback;
    };
  };
  
  //
  // Implementation
  //
  Window::Window() : Object((void*)0) {}
  Window::Window(gp_window* window) : Object((gp_object*)window) {}
  Window::Window(const Context& context) : Object((void*)gp_window_new((gp_context*)GetObject(context))) {}
  Pipeline Window::GetPipeline() {return Pipeline(gp_window_get_pipeline((gp_window*)GetObject(*this)));}
  void Window::Redraw() {gp_window_redraw((gp_window*)GetObject(*this));}
  void Window::SetTitle(const char* title) {gp_window_set_title((gp_window*)GetObject(*this), title);}
  void Window::SetType(GP_WINDOW_TYPE type) {gp_window_set_type((gp_window*)GetObject(*this), type);}
  void Window::SetMinSize(int width, int height) {gp_window_set_min_size((gp_window*)GetObject(*this), width, height);}
  void Window::SetMaxSize(int width, int height) {gp_window_set_max_size((gp_window*)GetObject(*this), width, height);}
  void Window::SetSize(unsigned int width, unsigned int height) {gp_window_set_size((gp_window*)GetObject(*this), width, height);}
  void Window::GetSize(unsigned int* width, unsigned int* height) {gp_window_get_size((gp_window*)GetObject(*this), width, height);}
  void Window::SetPosition(unsigned int x, unsigned int y) {gp_window_set_position((gp_window*)GetObject(*this), x, y);}
  void Window::GetPosition(unsigned int* x, unsigned int* y) {gp_window_get_position((gp_window*)GetObject(*this), x, y);}
  void Window::Show() {gp_window_show((gp_window*)GetObject(*this));}
  void Window::Hide() {gp_window_hide((gp_window*)GetObject(*this));}
  bool Window::GetShown() {return (bool)gp_window_get_shown((gp_window*)GetObject(*this));}
  void Window::SetFullscreen(Monitor monitor) {gp_window_set_fullscreen((gp_window*)GetObject(*this), (gp_monitor*)GetObject(monitor));}
  Monitor Window::GetFullscreen()
  {
    gp_monitor* monitor = gp_window_get_fullscreen((gp_window*)GetObject(*this));
    Monitor result(monitor);
    gp_object_unref((gp_object*)monitor);
    return result;
  }
  void Window::SetClickCallback(std::function<void(const gp_event_click_t*)> callback)
  {
    auto data = new CallbackData<ClickCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_click_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_click_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetScrollCallback(std::function<void(const gp_event_scroll_t*)> callback)
  {
    auto data = new CallbackData<ScrollCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_scroll_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_scroll_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetTrackCallback(std::function<void(const gp_event_track_t*)> callback)
  {
    auto data = new CallbackData<TrackCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_track_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_track_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetEnterCallback(std::function<void(const gp_event_enter_t*)> callback)
  {
    auto data = new CallbackData<EnterCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_enter_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_enter_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetKeyCallback(std::function<void(const gp_event_key_t*)> callback)
  {
    auto data = new CallbackData<KeyCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_key_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_key_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetResizeCallback(std::function<void(const gp_event_resize_t*)> callback)
  {
    auto data = new CallbackData<ResizeCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_resize_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_resize_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  void Window::SetMoveCallback(std::function<void(const gp_event_move_t*)> callback)
  {
    auto data = new CallbackData<MoveCallback>();
    data->mCallback = callback;
    auto pointer = Pointer(data).GetObject();
    gp_window_set_move_callback((gp_window*)GetObject(*this), HandleCallback<gp_event_move_t>, (gp_pointer*)pointer);
    gp_object_unref(pointer);
  }
  template <typename T>
  void Window::HandleCallback(const T* input, gp_pointer* userData)
  {
    auto data = (CallbackData<std::function<void(const T*)> >*)gp_pointer_get_pointer(userData);
    data->mCallback(input);
  }
}

#endif // __cplusplus

#endif // __GP_WINDOW_H__
