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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>
#include "Android.h"

#include <stdlib.h>

gp_system* gp_system_new()
{
  gp_system* system = malloc(sizeof(gp_system));
  
  if((system->mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    gp_log_error("eglGetDisplay() returned error %d", eglGetError());
    free(system);
    return NULL;
  }
  if(!eglInitialize(system->mDisplay, 0, 0)) {
    gp_log_error("eglInitialize() returned error %d", eglGetError());
    free(system);
    return NULL;
  }
  
  return system;
}

void gp_system_free(gp_system* system)
{
  free(system);
}

gp_context* gp_system_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
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

void gp_system_run(gp_system* system)
{
}
