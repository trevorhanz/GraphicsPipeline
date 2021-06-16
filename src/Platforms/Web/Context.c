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
#include <GraphicsPipeline/Window.h>
#include "Web.h"
#include "Platforms/Defaults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

gp_context* sContext = 0;

void gp_window_redraw(gp_window* window);

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
  gp_pointer* pointer = gp_pointer_new(context, 0);
  context->mWorkTimer = gp_timer_new(context->mParent);
  gp_timer_set_callback(context->mWorkTimer, _gp_work_timeout);
  gp_timer_set_userdata(context->mWorkTimer, pointer);
  gp_list_init(&context->mWork);
  gp_object_unref((gp_object*)pointer);
  
  sContext = context;
  
  _gp_canvas_set_width(&context->mID[1], GP_DEFAULT_WINDOW_WIDTH);
  _gp_canvas_set_height(&context->mID[1], GP_DEFAULT_WINDOW_HEIGHT);
  
  EM_ASM({
    let element = document.getElementById(UTF8ToString($0));
    element.style.position = "absolute";
  }, &context->mID[1]);
  
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

EM_BOOL _gp_window_click_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  if(window->mClickCB)
  {
    gp_event_click_t input;
    input.x = mouseEvent->targetX;
    input.y = mouseEvent->targetY;
    input.state = (eventType == EMSCRIPTEN_EVENT_MOUSEUP);
    switch(mouseEvent->button)
    {
      case 0: input.button = GP_BUTTON_LEFT; break;
      case 1: input.button = GP_BUTTON_MIDDLE; break;
      case 2: input.button = GP_BUTTON_RIGHT; break;
    }
    window->mClickCB(&input, window->mClickData);
  }
  
  return EM_TRUE;
}

EM_BOOL _gp_window_move_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  gp_event_move_t input;
  input.x = mouseEvent->targetX;
  input.y = mouseEvent->targetY;
  if(window->mMoveCB)
  {
    window->mMoveCB(&input, window->mMoveData);
  }
  
  return EM_TRUE;
}

EM_BOOL _gp_window_enter_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  if(window->mEnterCB)
  {
    gp_event_enter_t input;
    input.enter = (eventType == EMSCRIPTEN_EVENT_MOUSEENTER)?1:0;
    window->mEnterCB(&input, window->mEnterData);
  }
  
  return EM_TRUE;
}

EM_BOOL _gp_window_key_callback(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  if(window->mKeyCB)
  {
    // TODO: Create key mapping
    gp_event_key_t input;
    input.key = keyEvent->key[0];
    input.code = keyEvent->code[0];
    input.state = (eventType == EMSCRIPTEN_EVENT_KEYDOWN);
    
    window->mKeyCB(&input, window->mKeyData);
  }
  
  return EM_TRUE;
}

EM_BOOL _gp_window_resize_callback(int eventType, const EmscriptenUiEvent* uiEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  if(window->mResizeCB)
  {
    gp_event_resize_t input;
    input.width = uiEvent->windowInnerWidth;
    input.height = uiEvent->windowInnerHeight;
    
    window->mResizeCB(&input, window->mResizeData);
  }
  
  return EM_TRUE;
}

void _gp_window_build(gp_window* window)
{
  gp_ref_init(&window->mRef);
  
  window->mPipeline = _gp_pipeline_new();
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
  
  EM_ASM({
    let element = document.getElementById(UTF8ToString($0));
    element.style.display = "block";
    element.style.position = "relative";
    element.style.width = "1024px";
    element.style.height = "768px";
    element.oncontextmenu = function(e) {e.preventDefault();}; // Prevent context menu from poping up
  }, &window->mID[1]);
  
  emscripten_set_mouseup_callback(window->mID, window, EM_TRUE, _gp_window_click_callback);
  emscripten_set_mousedown_callback(window->mID, window, EM_TRUE, _gp_window_click_callback);
  emscripten_set_mousemove_callback(window->mID, window, EM_TRUE, _gp_window_move_callback);
  emscripten_set_mouseenter_callback(window->mID, window, EM_TRUE, _gp_window_enter_callback);
  emscripten_set_mouseleave_callback(window->mID, window, EM_TRUE, _gp_window_enter_callback);
  emscripten_set_keyup_callback(window->mID, window, EM_TRUE, _gp_window_key_callback);
  emscripten_set_keydown_callback(window->mID, window, EM_TRUE, _gp_window_key_callback);
  emscripten_set_resize_callback(window->mID, window, EM_TRUE, _gp_window_resize_callback);
  
  gp_window_redraw(window);
}

gp_window* gp_window_new_from_id(gp_context* context, const char* id)
{
  gp_window* window = malloc(sizeof(gp_window));
  
  int size = strlen(id);
  window->mID = malloc(sizeof(char)*size);
  memcpy(window->mID, id, sizeof(char)*size);
  
  _gp_window_build(window);
  
  return window;
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = malloc(sizeof(gp_window));
  window->mID = malloc(sizeof(char)*15);
  
  int index = context->mParent->mTargetIndex++;
  snprintf(window->mID, 15, "#_gp_window_%d", index);
  
  EM_ASM({
    let element = document.createElement("div");
    element.id = UTF8ToString($0);
    document.body.appendChild(element);
  }, &window->mID[1]);
  
  _gp_window_build(window);
  
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
    _gp_pipeline_free(window->mPipeline);
    
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

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  if(width) *width = _gp_canvas_get_width(&window->mID[1]);
  if(height) *height = _gp_canvas_get_height(&window->mID[1]);
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

void _gp_window_redraw_callback(void* data)
{
  gp_window* window = (gp_window*)data;
  
  _gp_pipeline_execute(window->mPipeline);
}

void gp_window_redraw(gp_window* window)
{
  emscripten_async_call(_gp_window_redraw_callback, window, -1);
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
