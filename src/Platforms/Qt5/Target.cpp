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

#include <GraphicsPipeline/Logging.h>

#include "Target.h"

Target::Target(QOpenGLContext* share)
  : mContext(new QOpenGLContext(this)),
  mPipeline(_gp_pipeline_new())
{
  setSurfaceType(QWindow::OpenGLSurface);
  setFormat(share->format());
  
  mContext->setFormat(share->format());
  mContext->setShareContext(share);
  mContext->create();
  
  mContainer = QWidget::createWindowContainer(this);
  mContainer->show();
  
  mContext->makeCurrent(this);
}

Target::~Target()
{
  delete mPipeline;
}

QWidget* Target::GetWidget()
{
  return mContainer;
}

gp_pipeline* Target::GetPipeline()
{
  return mPipeline;
}

void Target::Draw()
{
  mContext->makeCurrent(this);
  
  glViewport(0, 0, width(), height());
  
  _gp_pipeline_execute(mPipeline);
  
  mContext->swapBuffers(this);
}

void Target::exposeEvent(QExposeEvent* event)
{
  gp_log_debug("expose");
  
  Draw();
}

