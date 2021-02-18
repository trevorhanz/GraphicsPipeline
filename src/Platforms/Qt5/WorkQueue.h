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

#ifndef __GP_WORKQUEUE_H__
#define __GP_WORKQUEUE_H__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include "../../Utils/List.h"

class WorkQueue : public QThread
{
  Q_OBJECT
public:
  WorkQueue(QOpenGLContext* context);
  void run() override;
  
  void AddWork(void(*work)(void*), void(*join)(void*), void* data);
  
  void Finalize();
  
signals:
  void workFinished();
  
private:
  QOpenGLContext*             mContext;
  QOffscreenSurface*          mSurface;
  QMutex                      mMutex;
  QWaitCondition              mCV;
  gp_list                     mWork;
  gp_list                     mFinished;
};


#endif // __GP_WORKQUEUE_H__

