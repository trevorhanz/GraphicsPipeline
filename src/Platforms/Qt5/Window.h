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

#ifndef __GP_QT_WINDOW_H__
#define __GP_QT_WINDOW_H__

#include <QOpenGLContext>
#include <QWindow>
#include <QWidget>
#include <QMouseEvent>

#include "GL.h"

#include <GraphicsPipeline/GP.h>

class _Window : public QWindow
{
  Q_OBJECT
public:
  _Window(gp_context* parent, QOpenGLContext* share);
  virtual ~_Window();
  
  QWidget* GetWidget();
  gp_pipeline* GetPipeline();
  
  void Draw();
  
  gp_event_click_callback_t   mClickCB;
  gp_pointer*                 mClickData;
  gp_event_move_callback_t    mMoveCB;
  gp_pointer*                 mMoveData;
  gp_event_enter_callback_t   mEnterCB;
  gp_pointer*                 mEnterData;
  gp_event_key_callback_t     mKeyCB;
  gp_pointer*                 mKeyData;
  gp_event_resize_callback_t  mResizeCB;
  gp_pointer*                 mResizeData;
  
protected:
  void exposeEvent(QExposeEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  bool event(QEvent* event) override;
  
private:
  void HandleMouseClick(QMouseEvent* event, bool state);
  
  gp_context*           mParent;
  QOpenGLContext*       mContext;
  QWidget*              mContainer;
  gp_pipeline*          mPipeline;
};

#endif // __GP_QT_WINDOW_H__
