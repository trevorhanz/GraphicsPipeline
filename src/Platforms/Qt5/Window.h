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

#include <QOpenGLContext>
#include <QWindow>
#include <QWidget>

#include "GL.h"

class Window : public QWindow
{
  Q_OBJECT
public:
  Window(QOpenGLContext* share);
  virtual ~Window();
  
  QWidget* GetWidget();
  gp_pipeline* GetPipeline();
  
  void Draw();
  
protected:
  void exposeEvent(QExposeEvent* event) override;
  
private:
  QOpenGLContext*       mContext;
  QWidget*              mContainer;
  gp_pipeline*          mPipeline;
};