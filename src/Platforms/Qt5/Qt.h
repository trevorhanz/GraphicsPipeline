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

#ifndef __GP_QT_COMMON_H__
#define __GP_QT_COMMON_H__

#include <GraphicsPipeline/GP.h>

#include <QApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QTimer>
#include <QSocketNotifier>
#include <QThread>
#include <QScreen>

#include "Window.h"
#include "TimerCallback.h"
#include "IOCallback.h"
#include "WorkQueue.h"

#include "../../Utils/Object.h"
#include "../../Utils/RefCounter.h"

struct _gp_system
{
  gp_object               mObject;
  QCoreApplication*       mApp;
  bool                    mManageApp;
};

struct _gp_monitor_list
{
  gp_object               mObject;
};

struct _gp_monitor
{
  gp_object               mObject;
  QScreen*                mScreen;
};

struct _gp_context
{
  gp_object               mObject;
  QOpenGLContext*         mShare;
  QOffscreenSurface*      mSurface;
  std::map<int, gp_key_t> mKeyMap;
  WorkQueue*              mWorkQueue;
};

struct _gp_window
{
  gp_object               mObject;
  _Window*                mWindow;
  gp_monitor*             mMonitor;
};

struct _gp_timer
{
  gp_object               mObject;
  QTimer*                 mTimer;
  TimerCallback*          mTimerCallback;
  gp_pointer*             mUserData;
};

struct _gp_io
{
  gp_object               mObject;
  QSocketNotifier*        mSocketNotifier;
  IOCallback*             mIOCallback;
  gp_pointer*             mUserData;
};

#endif // __GP_QT_COMMON_H__
