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

#include <API/GL/GL.h>
#include "Web.h"
#include "Platforms/Defaults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

EM_JS(int, _gp_canvas_get_width, (const char* id), {
  return document.getElementById(UTF8ToString(id)).width;
});

EM_JS(int, _gp_canvas_get_height, (const char* id), {
  return document.getElementById(UTF8ToString(id)).height;
});

EM_JS(void, _gp_canvas_set_width, (const char* id, int value), {
  document.getElementById(UTF8ToString(id)).width = value;
});

EM_JS(void, _gp_canvas_set_height, (const char* id, int value), {
  document.getElementById(UTF8ToString(id)).height = value;
});

void gp_context_free(gp_context* context)
{
  free(context);
}

void _gp_target_build(gp_target* target)
{
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  
  _gp_canvas_set_width(&target->mID[1], GP_DEFAULT_WINDOW_WIDTH);
  _gp_canvas_set_height(&target->mID[1], GP_DEFAULT_WINDOW_HEIGHT);
  
  target->mContext = emscripten_webgl_create_context(target->mID, &attrs);
  
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(target->mContext);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == target->mContext);
  
  target->mPipeline = _gp_pipeline_new();
  
  const GLubyte* glVersion = glGetString(GL_VERSION);
  gp_log_info("GL Version: %s", glVersion);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  gp_target_redraw(target);
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(gp_target));
  target->mID = malloc(sizeof(char)*15);
  
  int index = context->mParent->mCanvasIndex++;
  snprintf(target->mID, 15, "#_gp_canvas_%d", index);
  
  EM_ASM({
    let div = document.createElement("div");
    
    let element = document.createElement("canvas");
    element.id = UTF8ToString($0);
    document.body.appendChild(element);
  }, &target->mID[1]);
  
  _gp_target_build(target);
  
  return target;
}

gp_target* gp_context_target_new_from_id(gp_context* context, const char* id)
{
  gp_target* target = malloc(sizeof(gp_target));
  
  int size = strlen(id);
  target->mID = malloc(sizeof(char)*size);
  memcpy(target->mID, id, sizeof(char)*size);
  
  _gp_target_build(target);
  
  return target;
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

void _gp_target_redraw_callback(void* data)
{
  gp_target* target = (gp_target*)data;
  
  emscripten_webgl_make_context_current(target->mContext);
  
  _gp_pipeline_execute(target->mPipeline);
}

void gp_target_redraw(gp_target* target)
{
  emscripten_async_call(_gp_target_redraw_callback, target, -1);
}
