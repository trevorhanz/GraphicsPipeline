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

#include "Target.h"
#include "TimerCallback.h"
#include "IOCallback.h"
#include "WorkQueue.h"

#include "../../Utils/RefCounter.h"

struct _gp_system
{
  QApplication*           mApp;
};

struct _gp_context
{
  QOpenGLContext*         mShare;
  QOffscreenSurface*      mSurface;
  gp_refcounter           mRef;
  
  WorkQueue*             mWorkQueue;
};

struct _gp_target
{
  Target*                 mTarget;
  gp_refcounter           mRef;
};

struct _gp_timer
{
  QTimer*                 mTimer;
  TimerCallback*          mTimerCallback;
  void*                   mUserData;
};

struct _gp_io
{
  QSocketNotifier*        mSocketNotifier;
  IOCallback*             mIOCallback;
  void*                   mUserData;
};

#endif // __GP_QT_COMMON_H__
