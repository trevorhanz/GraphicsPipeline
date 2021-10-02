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

#include <API/GL/GL.h>
#include <GraphicsPipeline/Window.h>
#include "Web.h"
#include "Platforms/Defaults.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

EM_BOOL _gp_window_track_callback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
  gp_window* window = (gp_window*)userData;
  
  gp_event_track_t input;
  input.x = mouseEvent->targetX;
  input.y = mouseEvent->targetY;
  if(window->mTrackCB)
  {
    window->mTrackCB(&input, window->mTrackData);
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

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  _gp_pipeline_free(window->mPipeline);
  
  if(window->mClickData) gp_object_unref((gp_object*)window->mClickData);
  if(window->mScrollData) gp_object_unref((gp_object*)window->mScrollData);
  if(window->mTrackData) gp_object_unref((gp_object*)window->mTrackData);
  if(window->mEnterData) gp_object_unref((gp_object*)window->mEnterData);
  if(window->mKeyData) gp_object_unref((gp_object*)window->mKeyData);
  if(window->mResizeData) gp_object_unref((gp_object*)window->mResizeData);
  if(window->mMoveData) gp_object_unref((gp_object*)window->mMoveData);
  
  free(window);
}

void _gp_window_build(gp_window* window)
{
  _gp_object_init(&window->mObject, _gp_window_free);
  
  window->mPipeline = _gp_pipeline_new();
  window->mClickCB = NULL;
  window->mClickData = NULL;
  window->mScrollCB = NULL;
  window->mScrollData = NULL;
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
  emscripten_set_mousemove_callback(window->mID, window, EM_TRUE, _gp_window_track_callback);
  emscripten_set_mouseenter_callback(window->mID, window, EM_TRUE, _gp_window_enter_callback);
  emscripten_set_mouseleave_callback(window->mID, window, EM_TRUE, _gp_window_enter_callback);
  emscripten_set_keyup_callback(window->mID, window, EM_TRUE, _gp_window_key_callback);
  emscripten_set_keydown_callback(window->mID, window, EM_TRUE, _gp_window_key_callback);
  emscripten_set_resize_callback(window->mID, window, EM_TRUE, _gp_window_resize_callback);
  // TODO: Get canvas position changes.
  
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

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mPipeline;
}

void gp_window_set_title(gp_window* window, const char* title)
{
  // NOTE: Not applicable to Web.
}

void gp_window_set_type(gp_window* window, GP_WINDOW_TYPE type)
{
  // NOTE: Not applicable to Web.
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  _gp_canvas_set_min_width(&window->mID[1], width);
  _gp_canvas_set_min_height(&window->mID[1], height);
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  _gp_canvas_set_max_width(&window->mID[1], width);
  _gp_canvas_set_max_height(&window->mID[1], height);
}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  _gp_canvas_set_width(&window->mID[1], width);
  _gp_canvas_set_height(&window->mID[1], height);
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  if(width) *width = _gp_canvas_get_width(&window->mID[1]);
  if(height) *height = _gp_canvas_get_height(&window->mID[1]);
}

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  _gp_canvas_set_left(&window->mID[1], x);
  _gp_canvas_set_top(&window->mID[1], y);
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  if(x) *x = _gp_canvas_get_left(&window->mID[1]);
  if(y) *y = _gp_canvas_get_top(&window->mID[1]);
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

void gp_window_set_fullscreen(gp_window* window, gp_monitor* monitor)
{
  // TODO: Implement
}

gp_monitor* gp_window_get_fullscreen(gp_window* window)
{
  // TODO: Implement
  return NULL;
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
_GP_SET_WINDOW_CALLBACK(scroll, mScrollCB, mScrollData)
_GP_SET_WINDOW_CALLBACK(track, mTrackCB, mTrackData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)

void _gp_window_redraw_callback(void* data)
{
  gp_window* window = (gp_window*)data;
  
  _gp_pipeline_execute(window->mPipeline);
}

void gp_window_redraw(gp_window* window)
{
  emscripten_async_call(_gp_window_redraw_callback, window, -1);
}
