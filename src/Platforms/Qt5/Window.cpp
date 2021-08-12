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

#include "Qt.h"
#include "Window.h"
#include "Platforms/Defaults.h"

_Window::_Window(gp_context* parent, QOpenGLContext* share)
  : mClickCB(NULL),
  mClickData(NULL),
  mMoveCB(NULL),
  mMoveData(NULL),
  mEnterCB(NULL),
  mEnterData(NULL),
  mKeyCB(NULL),
  mKeyData(NULL),
  mResizeCB(NULL),
  mResizeData(NULL),
  mParent(parent),
  mContext(new QOpenGLContext(this)),
  mPipeline(_gp_pipeline_new())
{
  setSurfaceType(QWindow::OpenGLSurface);
  setFormat(share->format());
  
  mContext->setFormat(share->format());
  mContext->setShareContext(share);
  mContext->create();
  
  mContainer = QWidget::createWindowContainer(this);
  mContainer->resize(GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT);
  mContainer->show();
  
  mContext->makeCurrent(this);
  
  _gp_api_init_context();
}

_Window::~_Window()
{
  if(mClickData) gp_object_unref((gp_object*)mClickData);
  if(mMoveData) gp_object_unref((gp_object*)mMoveData);
  if(mEnterData) gp_object_unref((gp_object*)mEnterData);
  if(mKeyData) gp_object_unref((gp_object*)mKeyData);
  if(mResizeData) gp_object_unref((gp_object*)mResizeData);
  
  _gp_pipeline_free(mPipeline);
}

QWidget* _Window::GetWidget()
{
  return mContainer;
}

gp_pipeline* _Window::GetPipeline()
{
  return mPipeline;
}

void _Window::Draw()
{
  if(!isExposed()) return;
  
  mContext->makeCurrent(this);
  
  glViewport(0, 0, width(), height());
  
  _gp_pipeline_execute(mPipeline);
  
  mContext->swapBuffers(this);
}

void _Window::exposeEvent(QExposeEvent* event)
{
  Draw();
}

void _Window::mouseMoveEvent(QMouseEvent* event)
{
  QPointF point = event->localPos();
  
  gp_event_move_t input;
  input.x = point.x();
  input.y = point.y();
  if(mMoveCB)
  {
    mMoveCB(&input, mMoveData);
  }
}

void _Window::mousePressEvent(QMouseEvent* event)
{
  HandleMouseClick(event, true);
}

void _Window::mouseReleaseEvent(QMouseEvent* event)
{
  HandleMouseClick(event, false);
}

void _Window::HandleMouseClick(QMouseEvent* event, bool state)
{
  QPointF point = event->localPos();
  if(mClickCB)
  {
    gp_event_click_t input;
    input.x = point.x();
    input.y = point.y();
    input.state = state;
    
    switch(event->button())
    {
      case Qt::LeftButton:
        input.button = GP_BUTTON_LEFT;
        break;
      case Qt::MiddleButton:
        input.button = GP_BUTTON_MIDDLE;
        break;
      case Qt::RightButton:
        input.button = GP_BUTTON_RIGHT;
        break;
      default:
        return;
    }
    
    mClickCB(&input, mClickData);
  }
}

void _Window::keyPressEvent(QKeyEvent* event)
{
  if(mKeyCB)
  {
    gp_event_key_t input;
    auto key = mParent->mKeyMap.find(event->key());
    if(key != mParent->mKeyMap.end())
      input.key = key->second;
    else
      input.key = GP_KEY_UNKNOWN;
    input.state = 1;
    
    mKeyCB(&input, mKeyData);
  }
}

void _Window::keyReleaseEvent(QKeyEvent* event)
{
  if(mKeyCB)
  {
    gp_event_key_t input;
    auto key = mParent->mKeyMap.find(event->key());
    if(key != mParent->mKeyMap.end())
      input.key = key->second;
    else
      input.key = GP_KEY_UNKNOWN;
    input.state = 0;
    
    mKeyCB(&input, mKeyData);
  }
}

void _Window::resizeEvent(QResizeEvent* event)
{
  if(mResizeCB)
  {
    gp_event_resize_t input;
    input.width = event->size().width();
    input.height = event->size().height();
    
    mResizeCB(&input, mResizeData);
  }
}

bool _Window::event(QEvent* event)
{
  switch(event->type())
  {
    case QEvent::Enter:
    case QEvent::Leave:
    {
      if(mEnterCB)
      {
        gp_event_enter_t input;
        input.enter = (event->type()==QEvent::Enter)?1:0;
        mEnterCB(&input, mEnterData);
      }
    }
    default:
      break;
  }
  
  return QWindow::event(event);
}

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  delete window;
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = new gp_window();
  _gp_object_init(&window->mObject, _gp_window_free);
  window->mWindow = new _Window(context, context->mShare);
  
  return window;
}

#define _GP_SET_WINDOW_CALLBACK(name, cb, data)\
  void gp_window_set_ ## name ## _callback(gp_window* window, gp_event_ ## name ## _callback_t callback, gp_pointer* userData)\
  {\
    if(window->mWindow->data) gp_object_unref((gp_object*)window->mWindow->data);\
    \
    window->mWindow->cb = callback;\
    window->mWindow->data = userData;\
    \
    if(window->mWindow->data) gp_object_ref((gp_object*)window->mWindow->data);\
  }

_GP_SET_WINDOW_CALLBACK(click, mClickCB, mClickData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mWindow->GetPipeline();
}

void gp_window_redraw(gp_window* window)
{
  window->mWindow->Draw();
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  window->mWindow->GetWidget()->setMinimumSize(QSize(width, height));
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  window->mWindow->GetWidget()->setMaximumSize(QSize(width, height));
}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  window->mWindow->GetWidget()->resize(width, height);
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  if(width) *width = window->mWindow->width();
  if(height) *height = window->mWindow->height();
}




