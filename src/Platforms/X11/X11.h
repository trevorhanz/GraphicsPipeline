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

#ifndef __GP_X11_COMMON_H__
#define __GP_X11_COMMON_H__

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>
#include <GraphicsPipeline/Window.h>

#include "../../Utils/List.h"
#include "../../Utils/Object.h"
#include "../../Utils/RefCounter.h"

#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include <pthread.h>

#define GP_OBJECT_FROM_LIST_NODE(node) (gp_object*)(((char*)node)-sizeof(gp_object))

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

typedef struct __gp_event _gp_event;

typedef struct
{
  gp_key_t                key;
} _gp_key_map;

struct _gp_system
{
  gp_object               mObject;
  Display*                mDisplay;
  gp_list                 mWindows;
  _gp_event*              mEvent;
  Atom                    mDeleteMessage;
  struct
  {
    int                   mMinCode;
    int                   mMaxCode;
    _gp_key_map*          mKeys;
  } mKeyMap;
};

struct _gp_context
{
  gp_object               mObject;
  gp_system*              mParent;
  Display*                mDisplay;
  XVisualInfo*            mVisualInfo;
  GLXFBConfig             mConfig;
  Window                  mWindow;
  Colormap                mColorMap;
  GLXContext              mShare;
  
  GLXContext              mWorkCtx;
  pthread_t               mWorkThread;
  pthread_mutex_t         mWorkMutex;
  pthread_cond_t          mWorkCV;
  gp_list                 mWork;
  gp_list                 mFinished;
  gp_io*                  mWorkIO;
  int                     mWorkPipe[2];
  int                     mState;
};

struct _gp_window
{
  gp_object               mObject;
  gp_list_node            mNode;
  gp_context*             mParent;
  gp_pipeline*            mPipeline;
  Window                  mWindow;
  GLXContext              mContext;
  uint8_t                 mDirty;
  int                     mPipe[2];
  gp_io*                  mWake;
  gp_event_click_callback_t   mClickCB;
  gp_pointer*                 mClickData;
  gp_event_track_callback_t   mTrackCB;
  gp_pointer*                 mTrackData;
  gp_event_enter_callback_t   mEnterCB;
  gp_pointer*                 mEnterData;
  gp_event_key_callback_t     mKeyCB;
  gp_pointer*                 mKeyData;
  gp_event_resize_callback_t  mResizeCB;
  gp_pointer*                 mResizeData;
  gp_event_move_callback_t    mMoveCB;
  gp_pointer*                 mMoveData;
};

void _gp_window_draw(gp_window* window);

_gp_event* _gp_event_new();

void _gp_event_free(_gp_event* event);

void _gp_event_run(_gp_event* event);

void _gp_event_stop(_gp_event* event);

void _gp_event_wake(_gp_event* event);

gp_timer* _gp_event_timer_new(_gp_event* event);

gp_io* _gp_event_io_read_new(_gp_event* event, int fd);

gp_io* _gp_event_io_write_new(_gp_event* event, int fd);

void _gp_event_pipe_new(_gp_event* event, int* fds);

#endif // __GP_X11_COMMON_H__
