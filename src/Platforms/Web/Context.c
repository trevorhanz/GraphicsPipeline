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

#include <API/GLES/GLES.h>
#include "Web.h"

#include <stdlib.h>
#include <assert.h>

void gp_context_free(gp_context* context)
{
  free(context);
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(gp_target));
  
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  
  target->mContext = emscripten_webgl_create_context("#canvas1", &attrs);
  
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(target->mContext);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == target->mContext);
  
  target->mPipeline = malloc(sizeof(struct _gp_pipeline));
  target->mPipeline->mOperations = NULL;
  
  context->mParent->mTarget = target;
  
  const GLubyte* glVersion = glGetString(GL_VERSION);
  gp_log_info("GL Version: %s", glVersion);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  return target;
}

gp_array* gp_context_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(struct _gp_array));
  
  glGenBuffers(1, &array->mVBO);
  
  return array;
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
