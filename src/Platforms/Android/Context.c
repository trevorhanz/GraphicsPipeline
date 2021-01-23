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

void gp_context_free(gp_context* context)
{
  free(context);
}

gp_target* gp_context_target_new(gp_context* context)
{
  return NULL;
}

gp_array* gp_context_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(struct _gp_array));
  
  glGenBuffers(1, &array->mVBO);
  
  return array;
}

gp_texture* gp_context_texture_new(gp_context* context)
{
  gp_texture* texture = malloc(sizeof(gp_texture));
  
  _gp_generate_texture(texture);
  
  return texture;
}

gp_shader* gp_context_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(struct _gp_shader));
  
  return shader;
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
}

gp_target* gp_context_target_from_native(gp_context* context, ANativeWindow* window)
{
  gp_target* target = malloc(sizeof(gp_target));
  target->mWindow = window;
  
  ANativeWindow_setBuffersGeometry(window, 0, 0, context->mFormat);

  if (!(target->mSurface = eglCreateWindowSurface(context->mParent->mDisplay, context->mConfig, window, 0))) {
    gp_log_error("eglCreateWindowSurface() returned error %d", eglGetError());
    free(target);
    return NULL;
  }
  
  const EGLint contextAttribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 2,
    EGL_NONE
  };
  
  if (!(target->mContext = eglCreateContext(context->mParent->mDisplay, context->mConfig, context->mShare, contextAttribs))) {
    gp_log_error("eglCreateContext() returned error %d", eglGetError());
    free(target);
    return NULL;
  }
  
  if (!eglMakeCurrent(context->mParent->mDisplay, target->mSurface, target->mSurface, target->mContext)) {
    gp_log_error("eglMakeCurrent() returned error %d", eglGetError());
    free(target);
    return NULL;
  }
  
  target->mPipeline = _gp_pipeline_new();
  
  return target;
}
