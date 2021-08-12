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

#ifndef __GP_WEB_COMMON_H__
#define __GP_WEB_COMMON_H__

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>
#include <GraphicsPipeline/Window.h>

#include <API/GL/GL.h>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "../../Utils/RefCounter.h"

#include <stdint.h>

struct _gp_system
{
  uint8_t                               mCanvasIndex;
  uint8_t                               mTargetIndex;
  int                                   mState;
};

struct _gp_context
{
  gp_object                             mObject;
  gp_system*                            mParent;
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mShare;
  char*                                 mID;
  
  gp_timer*                             mWorkTimer;
  gp_list                               mWork;
};

struct _gp_window
{
  gp_object                             mObject;
  char*                                 mID;
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mContext;
  GLuint                                vbo;
  GLuint                                shaderProgram;
  GLint                                 posAttrib;
  gp_pipeline*                          mPipeline;
  gp_event_click_callback_t             mClickCB;
  gp_pointer*                           mClickData;
  gp_event_move_callback_t              mMoveCB;
  gp_pointer*                           mMoveData;
  gp_event_enter_callback_t             mEnterCB;
  gp_pointer*                           mEnterData;
  gp_event_key_callback_t               mKeyCB;
  gp_pointer*                           mKeyData;
  gp_event_resize_callback_t            mResizeCB;
  gp_pointer*                           mResizeData;
};

struct _gp_timer
{
  gp_object                             mObject;
  int                                   mTimerID;
  gp_timer_callback                     mCallback;
  gp_pointer*                           mUserData;
};

struct _gp_io
{
};

int _gp_canvas_get_width(const char* id);
int _gp_canvas_get_height(const char* id);
void _gp_canvas_set_width(const char* id, int value);
void _gp_canvas_set_height(const char* id, int value);
void _gp_canvas_set_min_width(const char* id, int value);
void _gp_canvas_set_min_height(const char* id, int value);
void _gp_canvas_set_max_width(const char* id, int value);
void _gp_canvas_set_max_height(const char* id, int value);

#endif // __GP_WEB_COMMON_H__
