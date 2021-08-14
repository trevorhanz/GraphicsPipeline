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

#ifndef __GP_WINDOWS_COMMON_H__
#define __GP_WINDOWS_COMMON_H__

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/Input.h>
#include <GraphicsPipeline/Window.h>
#include <API/GL/GL.h>

#include "../../Utils/Object.h"
#include "../../Utils/RefCounter.h"

#include <windows.h>

#define WM_SOCKET WM_USER + 1
#define WM_WORK_DONE WM_USER + 2

struct _gp_system
{
  gp_object             mObject;
  HWND                  mInternalWindow;
};

struct _gp_context
{
  gp_object             mObject;
  gp_system*            mParent;
  HGLRC                 mShare;
  HWND                  mWindow;
  unsigned int          mPixelFormat;

  HGLRC                 mWorkContext;
  HWND                  mWorkWindow;
  HANDLE                mWorkThread;
  CRITICAL_SECTION      mWorkMutex;
  CONDITION_VARIABLE    mWorkCV;
  gp_list               mWork;
  gp_list               mFinished;
};

struct _gp_window
{
  gp_object             mObject;
  gp_context*           mParent;
  HGLRC                 mContext;
  HWND                  mWindow;
  gp_pipeline*          mPipeline;
  int                   mMinWidth;
  int                   mMinHeight;
  int                   mMaxWidth;
  int                   mMaxHeight;
  
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

  unsigned char               mMouseEntered;
};

struct _gp_timer
{
  gp_object             mObject;
  gp_system*            mSystem;
  gp_timer_callback     mCallback;
  gp_pointer*           mUserData;
};

struct _gp_io
{
  gp_object             mObject;
  gp_system*            mSystem;
  gp_io_callback        mCallback;
  gp_pointer*           mUserData;
};

void _gp_work_done(void* data);

#endif // __GP_WINDOWS_COMMON_H__
