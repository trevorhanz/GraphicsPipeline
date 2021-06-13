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

#include "Android.h"

#include <stdlib.h>

gp_context* gp_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
  context->mParent = system;
  gp_ref_init(&context->mRef);
  
  const EGLint attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_NONE
  };
  EGLint numConfigs;
  
  if(!eglChooseConfig(system->mDisplay, attribs, &context->mConfig, 1, &numConfigs)) {
    gp_log_error("eglChooseConfig() returned error %d", eglGetError());
    free(context);
    return NULL;
  }

  if(!eglGetConfigAttrib(system->mDisplay, context->mConfig, EGL_NATIVE_VISUAL_ID, &context->mFormat)) {
    gp_log_error("eglGetConfigAttrib() returned error %d", eglGetError());
    free(context);
    return NULL;
  }
  
  const EGLint contextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
  };
  
  if(!(context->mShare = eglCreateContext(system->mDisplay, context->mConfig, 0, contextAttribs))) {
    gp_log_error("eglCreateContext() returned error %d", eglGetError());
    free(context);
    return NULL;
  }
  
  const EGLint surfaceAttribs[] = {
    EGL_NONE
  };
  
  if(!(context->mShareSurface = eglCreatePbufferSurface(system->mDisplay, context->mConfig, surfaceAttribs))) {
    gp_log_error("eglCreatePbufferSurface() returned error %d", eglGetError());
    free(context);
    return NULL;
  }
  
  if(!eglMakeCurrent(system->mDisplay, context->mShareSurface, context->mShareSurface, context->mShare)) {
      gp_log_error("eglMakeCurrent() returned error %d", eglGetError());
      free(context);
      return NULL;
    }
  
  gp_log_error("GL Version: %s", glGetString(GL_VERSION));
  
  return context;
}

void gp_context_ref(gp_context* context)
{
  gp_ref_inc(&context->mRef);
}

void gp_context_unref(gp_context* context)
{
  if(gp_ref_dec(&context->mRef))
  {
    free(context);
  }
}

gp_window* gp_window_new(gp_context* context)
{
  return NULL;
}

void gp_window_ref(gp_window* window)
{
  gp_ref_inc(&window->mRef);
}

void gp_window_unref(gp_window* window)
{
  if(gp_ref_dec(&window->mRef))
  {
    _gp_pipeline_free(window->mPipeline);
    free(window);
  }
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
  window->mWindow = awindow;
  gp_ref_init(&window->mRef);
  
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

void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  work(data);
  join(data);
}
