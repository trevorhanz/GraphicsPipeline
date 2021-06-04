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

gp_context* sContext = 0;

void gp_target_redraw(gp_target* target);

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

typedef struct
{
  gp_list_node  mNode;
  GLsync        mFence;
  void(*mJoin)(void*);
  void*         mData;
} _gp_work_item;

void _gp_work_timeout(gp_timer* timer)
{
  gp_context* context = (gp_context*)gp_timer_get_userdata(timer);
  
  _gp_work_item* work = (_gp_work_item*)gp_list_front(&context->mWork);
  if(work != NULL)
  {
    GLenum ret = glClientWaitSync(work->mFence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
    if(ret == GL_TIMEOUT_EXPIRED)
    {
      if(timer->mTimerID < 0)
        gp_timer_arm(timer, .01);
    }
    else
    {
      gp_list_remove(&context->mWork, (gp_list_node*)work);
      work->mJoin(work->mData);
      glDeleteSync(work->mFence);
      free(work);
      if(gp_list_front(&context->mWork) != NULL
         && timer->mTimerID < 0)
        gp_timer_arm(timer, .01);
    }
  }
}

void _gp_context_build(gp_context* context)
{
  context->mWorkTimer = gp_timer_new(system);
  gp_timer_set_callback(context->mWorkTimer, _gp_work_timeout);
  gp_timer_set_userdata(context->mWorkTimer, context);
  gp_list_init(&context->mWork);
  
  sContext = context;
  
  _gp_canvas_set_width(&context->mID[1], GP_DEFAULT_WINDOW_WIDTH);
  _gp_canvas_set_height(&context->mID[1], GP_DEFAULT_WINDOW_HEIGHT);
  
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;
  
  context->mShare = emscripten_webgl_create_context(context->mID, &attrs);
  
  EMSCRIPTEN_RESULT res = emscripten_webgl_make_context_current(context->mShare);
  assert(res == EMSCRIPTEN_RESULT_SUCCESS);
  assert(emscripten_webgl_get_current_context() == context->mShare);
  
  const GLubyte* glVersion = glGetString(GL_VERSION);
  gp_log_info("GL Version: %s", glVersion);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
}

gp_context* gp_context_new_from_id(gp_system* system, const char* id)
{
  gp_context* context = malloc(sizeof(gp_context));
  context->mParent = system;
  gp_ref_init(&context->mRef);
  
  int size = strlen(id);
  context->mID = malloc(sizeof(char)*size);
  memcpy(context->mID, id, sizeof(char)*size);
  
  _gp_context_build(context);
  
  return context;
}

gp_context* gp_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
  context->mParent = system;
  gp_ref_init(&context->mRef);
  
  context->mID = malloc(sizeof(char)*15);
  
  int index = context->mParent->mCanvasIndex++;
  snprintf(context->mID, 15, "#_gp_canvas_%d", index);
  
  EM_ASM({
    let element = document.createElement("canvas");
    element.id = UTF8ToString($0);
    document.body.appendChild(element);
  }, &context->mID[1]);
  
  _gp_context_build(context);
  
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

void _gp_target_build(gp_target* target)
{
  gp_ref_init(&target->mRef);
  
  target->mPipeline = _gp_pipeline_new();
  
  gp_target_redraw(target);
}

gp_target* gp_target_new_from_id(gp_context* context, const char* id)
{
  gp_target* target = malloc(sizeof(gp_target));
  
  int size = strlen(id);
  target->mID = malloc(sizeof(char)*size);
  memcpy(target->mID, id, sizeof(char)*size);
  
  _gp_target_build(target);
  
  return target;
}

gp_target* gp_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(gp_target));
  target->mID = malloc(sizeof(char)*15);
  
  int index = context->mParent->mTargetIndex++;
  snprintf(target->mID, 15, "#_gp_target_%d", index);
  
  EM_ASM({
    let element = document.createElement("div");
    element.id = UTF8ToString($0);
    document.body.appendChild(element);
  }, &target->mID[1]);
  
  _gp_target_build(target);
  
  return target;
}

void gp_target_ref(gp_target* target)
{
  gp_ref_inc(&target->mRef);
}

void gp_target_unref(gp_target* target)
{
  if(gp_ref_dec(&target->mRef))
  {
    _gp_pipeline_free(target->mPipeline);
    free(target);
  }
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void _gp_target_redraw_callback(void* data)
{
  gp_target* target = (gp_target*)data;
  
  _gp_pipeline_execute(target->mPipeline);
}

void gp_target_redraw(gp_target* target)
{
  emscripten_async_call(_gp_target_redraw_callback, target, -1);
}

void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  work(data);
  
  _gp_work_item* node = malloc(sizeof(_gp_work_item));
  node->mFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  node->mJoin = join;
  node->mData = data;
  gp_list_push_back(&sContext->mWork, (gp_list_node*)node);
  
  if(sContext->mWorkTimer->mTimerID<0)
    gp_timer_arm(sContext->mWorkTimer, .1);
}
