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

#ifndef __GP_MACOS_COMMON_H__
#define __GP_MACOS_COMMON_H__

// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>
#include <API/GL/GL.h>

#import <Cocoa/Cocoa.h>
#include <pthread.h>

#include "View.h"
#include "Timer.h"
#include "../../Utils/Object.h"
#include "../../Utils/RefCounter.h"

struct _gp_system
{
  gp_object               mObject;
  gp_window*              mTarget;
};

struct _gp_monitor_list
{
  gp_object               mObject;
};

struct _gp_context
{
  gp_object               mObject;
  gp_system*              mParent;
  NSOpenGLContext*        mShare;
  NSOpenGLPixelFormat*    mPixelFormat;
  
  NSOpenGLContext*        mWorkContext;
  pthread_t               mWorkThread;
  pthread_mutex_t         mWorkMutex;
  pthread_cond_t          mWorkCV;
  int                     mWorkPipe[2];
  gp_io*                  mWorkIO;
  gp_list                 mWork;
  gp_list                 mFinished;
};

struct _gp_window
{
  gp_object               mObject;
  gp_context*             mParent;
  gp_pipeline*            mPipeline;
  NSWindow*               mWindow;
  View*                   mView;
  
  gp_event_click_callback_t   mClickCB;
  gp_pointer*                 mClickData;
  gp_event_scroll_callback_t  mScrollCB;
  gp_pointer*                 mScrollData;
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

struct _gp_timer
{
  gp_object               mObject;
  Timer*                  mTimer;
};

struct _gp_io
{
  gp_object               mObject;
  NSFileHandle*           mHandle;
  gp_io_callback          mCallback;
  gp_pointer*             mUserData;
};

#endif // __MACOS_COMMON_H__
