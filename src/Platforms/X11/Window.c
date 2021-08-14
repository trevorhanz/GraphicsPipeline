/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#include "X11.h"

#include <X11/Xatom.h>

#include <unistd.h>
#include <stdlib.h>

#include "../../API/GL/GL.h"
#include "Platforms/Defaults.h"

void _gp_window_wake_callback(gp_io* io)
{
  gp_window* window = (gp_window*)gp_pointer_get_pointer(gp_io_get_userdata(io));
  for(;;)
  {
    char ch;
    if(read(window->mPipe[0], &ch, 1) == -1)
      break;
  }
  
  if(window->mDirty)
  {
    _gp_window_draw(window);
    window->mDirty = 0;
  }
}

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  glXDestroyContext(window->mParent->mDisplay, window->mContext);

  gp_list_remove(&window->mParent->mParent->mWindows, &window->mNode);
  _gp_pipeline_free(window->mPipeline);

  gp_object_unref((gp_object*)window->mWake);

  close(window->mPipe[0]);
  close(window->mPipe[1]);

  if(window->mClickData) gp_object_unref((gp_object*)window->mClickData);
  if(window->mTrackData) gp_object_unref((gp_object*)window->mTrackData);
  if(window->mEnterData) gp_object_unref((gp_object*)window->mEnterData);
  if(window->mKeyData) gp_object_unref((gp_object*)window->mKeyData);
  if(window->mResizeData) gp_object_unref((gp_object*)window->mResizeData);
  if(window->mMoveData) gp_object_unref((gp_object*)window->mMoveData);

  free(window);
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = malloc(sizeof(struct _gp_window));
  _gp_object_init(&window->mObject, _gp_window_free);
  window->mParent = context;
  window->mPipeline = _gp_pipeline_new();
  window->mDirty = 1;
  window->mClickCB = NULL;
  window->mClickData = NULL;
  window->mTrackCB = NULL;
  window->mTrackData = NULL;
  window->mEnterCB = NULL;
  window->mEnterData = NULL;
  window->mKeyCB = NULL;
  window->mKeyData = NULL;
  window->mResizeCB = NULL;
  window->mResizeData = NULL;
  window->mMoveCB = NULL;
  window->mMoveData = NULL;
  gp_list_push_back(&context->mParent->mWindows, &window->mNode);
  _gp_event_pipe_new(context->mParent->mEvent, window->mPipe);
  
  gp_pointer* pointer = gp_pointer_new(window, 0);
  window->mWake = gp_io_read_new(context->mParent, window->mPipe[0]);
  gp_io_set_callback(window->mWake, _gp_window_wake_callback);
  gp_io_set_userdata(window->mWake, pointer);
  gp_object_unref((gp_object*)pointer);
  
  XSetWindowAttributes attr;
  attr.colormap = context->mColorMap;
  attr.border_pixel = 0;
  attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
  StructureNotifyMask | DestroyNotify | PointerMotionMask | FocusChangeMask;
  
  window->mWindow = XCreateWindow(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen),
                                  0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT,
                                  0, context->mVisualInfo->depth, InputOutput, context->mVisualInfo->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &attr);
  
  int context_attribs[] =
  {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
    GLX_CONTEXT_MINOR_VERSION_ARB, 4,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,
    None
  };
  
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");
  window->mContext = glXCreateContextAttribsARB(context->mDisplay, context->mConfig, context->mShare,
                                              True, context_attribs );
  XStoreName(context->mDisplay, window->mWindow, GP_DEFAULT_WINDOW_TITLE);
  XFlush(context->mDisplay);
  glXMakeCurrent(context->mDisplay, window->mWindow, window->mContext);
  XFlush(context->mDisplay);
  XMapRaised(context->mDisplay, window->mWindow);
  XFlush(context->mDisplay);
  
  XSetWMProtocols(context->mDisplay, window->mWindow, &context->mParent->mDeleteMessage, 1);
  
  _gp_api_init_context();
  
  return window;
}

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mPipeline;
}

void gp_window_redraw(gp_window* window)
{
  size_t r = write(window->mPipe[1], "x", 1);
  
  window->mDirty = 1;
}

void gp_window_set_title(gp_window* window, const char* title)
{
  XStoreName(window->mParent->mDisplay, window->mWindow, title);
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  XSizeHints* hints = XAllocSizeHints();
  XGetSizeHints(window->mParent->mDisplay, window->mWindow, hints, XA_WM_NORMAL_HINTS);
  hints->flags |= PMinSize;
  hints->min_width = width;
  hints->min_height = height;
  XSetWMSizeHints(window->mParent->mDisplay, window->mWindow, hints, XA_WM_NORMAL_HINTS);
  free(hints);
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  XSizeHints* hints = XAllocSizeHints();
  XGetSizeHints(window->mParent->mDisplay, window->mWindow, hints, XA_WM_NORMAL_HINTS);
  hints->flags |= PMaxSize;
  hints->max_width = width;
  hints->max_height = height;
  XSetWMNormalHints(window->mParent->mDisplay, window->mWindow, hints);
  free(hints);
}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  XWindowChanges changes;
  changes.width = width;
  changes.height = height;
  XConfigureWindow(window->mParent->mDisplay, window->mWindow, CWWidth | CWHeight, &changes);
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  XWindowAttributes attributes;
  XGetWindowAttributes(window->mParent->mDisplay, window->mWindow, &attributes);
  if(width) *width = attributes.width;
  if(height) *height = attributes.height;
}

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  XWindowChanges changes;
  changes.x = x;
  changes.y = y;
  XConfigureWindow(window->mParent->mDisplay, window->mWindow, CWX | CWY, &changes);
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  Window root = RootWindow(window->mParent->mDisplay, window->mParent->mVisualInfo->screen);
  Window child;
  int X, Y;
  XTranslateCoordinates(window->mParent->mDisplay, window->mWindow, root,
                        0, 0,
                        &X, &Y,
                        &child);
  if(x) *x = X;
  if(y) *y = X;
}

#define _GP_SET_WINDOW_CALLBACK(name, cb, data)\
  void gp_window_set_ ## name ## _callback(gp_window* window, gp_event_ ## name ## _callback_t callback, gp_pointer* userData)\
  {\
    if(window->data) gp_object_unref((gp_object*)window->data);\
    \
    window->cb = callback;\
    window->data = userData;\
    \
    if(window->data) gp_object_ref((gp_object*)window->data);\
  }

_GP_SET_WINDOW_CALLBACK(click, mClickCB, mClickData)
_GP_SET_WINDOW_CALLBACK(track, mTrackCB, mTrackData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)

void _gp_window_draw(gp_window* window)
{
  glXMakeCurrent(window->mParent->mDisplay, window->mWindow, window->mContext);
  
  unsigned int width, height;
  gp_window_get_size(window, &width, &height);
  _gp_api_prepare_window(width, height);
  
  _gp_pipeline_execute(window->mPipeline);
  
  glXSwapBuffers(window->mParent->mDisplay, window->mWindow);
  
  window->mDirty = 0;
}
