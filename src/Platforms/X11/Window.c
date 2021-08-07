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

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = malloc(sizeof(struct _gp_window));
  window->mParent = context;
  window->mPipeline = _gp_pipeline_new();
  window->mDirty = 1;
  window->mClickCB = NULL;
  window->mClickData = NULL;
  window->mMoveCB = NULL;
  window->mMoveData = NULL;
  window->mEnterCB = NULL;
  window->mEnterData = NULL;
  window->mKeyCB = NULL;
  window->mKeyData = NULL;
  window->mResizeCB = NULL;
  window->mResizeData = NULL;
  gp_ref_init(&window->mRef);
  gp_list_push_back(&context->mParent->mTargets, (gp_list_node*)window);
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
  XStoreName(context->mDisplay, window->mWindow, "GraphicsPipeline");
  XFlush(context->mDisplay);
  glXMakeCurrent(context->mDisplay, window->mWindow, window->mContext);
  XFlush(context->mDisplay);
  XMapRaised(context->mDisplay, window->mWindow);
  XFlush(context->mDisplay);
  
  XSetWMProtocols(context->mDisplay, window->mWindow, &context->mParent->mDeleteMessage, 1);
  
  _gp_api_init_context();
  
  return window;
}

void gp_window_ref(gp_window* window)
{
  gp_ref_inc(&window->mRef);
}

void gp_window_unref(gp_window* window)
{
  if(gp_ref_dec(&window->mRef))
  {
    glXDestroyContext(window->mParent->mDisplay, window->mContext);
    
    gp_list_remove(&window->mParent->mParent->mTargets, (gp_list_node*)window);
    _gp_pipeline_free(window->mPipeline);
    
    gp_object_unref((gp_object*)window->mWake);
    
    close(window->mPipe[0]);
    close(window->mPipe[1]);
    
    if(window->mClickData) gp_object_unref((gp_object*)window->mClickData);
    if(window->mMoveData) gp_object_unref((gp_object*)window->mMoveData);
    if(window->mEnterData) gp_object_unref((gp_object*)window->mEnterData);
    if(window->mKeyData) gp_object_unref((gp_object*)window->mKeyData);
    if(window->mResizeData) gp_object_unref((gp_object*)window->mResizeData);
    
    free(window);
  }
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

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  XWindowAttributes attributes;
  XGetWindowAttributes(window->mParent->mDisplay, window->mWindow, &attributes);
  if(width) *width = attributes.width;
  if(height) *height = attributes.height;
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
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)

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
