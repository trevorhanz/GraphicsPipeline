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

#include "WorkQueue.h"
#include <GraphicsPipeline/Logging.h>
#include "GL.h"

#include <QOpenGLFunctions>

struct _gp_work_node
{
  gp_list_node          mNode;
  void (*mWork)(void*);
  void (*mJoin)(void*);
  void*                 mData;
};

WorkQueue::WorkQueue(QOpenGLContext* context)
{
  mContext = new QOpenGLContext();
  mContext->setShareContext(context);
  mContext->setFormat(context->format());
  mContext->moveToThread(this);
  
  mSurface = new QOffscreenSurface;
  mSurface->setFormat(context->format());
  mSurface->moveToThread(this);
  
  gp_list_init(&mWork);
  gp_list_init(&mFinished);
}

void WorkQueue::run()
{
  if(!mContext->create())
  {
    gp_log_error("WorkQueue context creation failed.");
  }
  mSurface->create();
  if(!mContext->makeCurrent(mSurface))
  {
    gp_log_error("WorkQueue context failed to be made current.");
  }
  
  _gp_api_init_context();
  
  mMutex.lock();
  
  while(1)
  {
    while(gp_list_front(&mWork) != NULL)
    {
      _gp_work_node* node = (_gp_work_node*)gp_list_front(&mWork);
      gp_list_remove(&mWork, (gp_list_node*)node);
      mMutex.unlock();
      
      node->mWork(node->mData);
      
      mMutex.lock();
      gp_list_push_back(&mFinished, (gp_list_node*)node);
      emit workFinished();
    }
    
    mCV.wait(&mMutex);
  }
}

void WorkQueue::AddWork(void (*work)(void*), void (*join)(void*), void* data)
{
  _gp_work_node* node = new _gp_work_node;
  node->mWork = work;
  node->mJoin = join;
  node->mData = data;
  
  mMutex.lock();
  gp_list_push_back(&mWork, (gp_list_node*)node);
  mMutex.unlock();
  mCV.notify_one();
}

void WorkQueue::Finalize()
{
  mMutex.lock();
  
  _gp_work_node* node;
  while(gp_list_front(&mFinished) != NULL)
  {
    node = (_gp_work_node*)gp_list_front(&mFinished);
    gp_list_remove(&mFinished, (gp_list_node*)node);
    mMutex.unlock();
    
    node->mJoin(node->mData);
    
    mMutex.lock();
  }
  
  mMutex.unlock();
}



