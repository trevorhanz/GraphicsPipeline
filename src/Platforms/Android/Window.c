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

#include "Android.h"

#include <stdlib.h>

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  _gp_pipeline_free(window->mPipeline);
  
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
  return NULL;
}

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mPipeline;
}

void gp_window_redraw(gp_window* window)
{
}

gp_window* gp_window_new_from_native(gp_context* context, ANativeWindow* awindow)
{
  gp_window* window = malloc(sizeof(gp_window));
  _gp_object_init(&window->mObject, _gp_window_free);
  window->mWindow = awindow;
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
  
  ANativeWindow_setBuffersGeometry(awindow, 0, 0, context->mFormat);

  if (!(window->mSurface = eglCreateWindowSurface(context->mParent->mDisplay, context->mConfig, awindow, 0))) {
    gp_log_error("eglCreateWindowSurface() returned error %d", eglGetError());
    free(window);
    return NULL;
  }
  
  const EGLint contextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
  };
  
  if (!(window->mContext = eglCreateContext(context->mParent->mDisplay, context->mConfig, context->mShare, contextAttribs))) {
    gp_log_error("eglCreateContext() returned error %d", eglGetError());
    free(window);
    return NULL;
  }
  
  if (!eglMakeCurrent(context->mParent->mDisplay, window->mSurface, window->mSurface, window->mContext)) {
    gp_log_error("eglMakeCurrent() returned error %d", eglGetError());
    free(window);
    return NULL;
  }
  
  window->mPipeline = _gp_pipeline_new();
  
  return window;
}

void gp_window_set_title(gp_window* window, const char* title)
{
  // TODO: Implement
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  // TODO: Implement
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  // TODO: Implement
}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  // TODO: Implement
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  // TODO: fillout with correct values.
  if(width) *width = 0;
  if(height) *height = 0;
}

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  // NOTE: Has no effect on Android.
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  if(x) *x = 0;
  if(y) *y = 0;
}

void gp_window_show(gp_window* window)
{
  // TODO: Implement
}

void gp_window_hide(gp_window* window)
{
  // TODO: Implement
}

int gp_window_get_shown(gp_window* window)
{
  // TODO: Implement
  return 1;
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
