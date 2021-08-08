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

void _gp_context_free(gp_object* object)
{
  gp_context* context = (gp_context*)object;
  
  free(context);
}

gp_context* gp_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
  _gp_object_init(&context->mObject, _gp_context_free);
  context->mParent = system;
  
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

void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  work(data);
  join(data);
}

void _gp_api_context_make_current(gp_context* context)
{
  // TODO: Implement
}
