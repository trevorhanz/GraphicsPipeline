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

#include <QStackedLayout>

_Window::_Window(gp_context* parent, QOpenGLContext* share)
  : mClickCB(NULL),
  mClickData(NULL),
  mTrackCB(NULL),
  mTrackData(NULL),
  mEnterCB(NULL),
  mEnterData(NULL),
  mKeyCB(NULL),
  mKeyData(NULL),
  mResizeCB(NULL),
  mResizeData(NULL),
  mMoveCB(NULL),
  mMoveData(NULL),
  mParent(parent),
  mContext(new QOpenGLContext(this)),
  mPipeline(_gp_pipeline_new())
{
  setSurfaceType(QWindow::OpenGLSurface);
  setFormat(share->format());
  
  mContext->setFormat(share->format());
  mContext->setShareContext(share);
  mContext->create();
  
  mContainer = new _WindowContainer(this);
  mContainer->resize(GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT);
  mContainer->setWindowTitle(GP_DEFAULT_WINDOW_TITLE);
  
  QWidget* base = QWidget::createWindowContainer(this, mContainer);
  
  QLayout* layout = new QStackedLayout();
  layout->addWidget(base);
  mContainer->setLayout(layout);
  
  mContext->makeCurrent(this);
  
  _gp_api_init_context();
}

_Window::~_Window()
{
  if(mClickData) gp_object_unref((gp_object*)mClickData);
  if(mTrackData) gp_object_unref((gp_object*)mTrackData);
  if(mEnterData) gp_object_unref((gp_object*)mEnterData);
  if(mKeyData) gp_object_unref((gp_object*)mKeyData);
  if(mResizeData) gp_object_unref((gp_object*)mResizeData);
  if(mMoveData) gp_object_unref((gp_object*)mMoveData);
  
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
  
  _gp_api_prepare_window(width(), height());
  
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
  
  gp_event_track_t input;
  input.x = point.x();
  input.y = point.y();
  if(mTrackCB)
  {
    mTrackCB(&input, mTrackData);
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
    } break;
    default:
      break;
  }
  
  return QWindow::event(event);
}

_WindowContainer::_WindowContainer(_Window* window)
  : mWindow(window)
{
}

void _WindowContainer::moveEvent(QMoveEvent* event)
{
  if(mWindow->mMoveCB)
  {
    QMoveEvent* move = (QMoveEvent*)event;
    gp_event_move_t input;
    input.x = move->pos().x();
    input.y = move->pos().y();
    mWindow->mMoveCB(&input, mWindow->mMoveData);
  }
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
_GP_SET_WINDOW_CALLBACK(track, mTrackCB, mTrackData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mWindow->GetPipeline();
}

void gp_window_redraw(gp_window* window)
{
  window->mWindow->Draw();
}

void gp_window_set_title(gp_window* window, const char* title)
{
  window->mWindow->GetWidget()->setWindowTitle(title);
}

void gp_window_set_type(gp_window* window, GP_WINDOW_TYPE type)
{
  switch(type)
  {
    case GP_WINDOW_TYPE_NORMAL:
      window->mWindow->GetWidget()->setWindowFlags(Qt::Widget);
      break;
    case GP_WINDOW_TYPE_UTILITY:
      window->mWindow->GetWidget()->setWindowFlags(Qt::Popup);
      break;
  }
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

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  window->mWindow->GetWidget()->move(x, y);
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  if(x) *x = window->mWindow->GetWidget()->geometry().x();
  if(y) *y = window->mWindow->GetWidget()->geometry().y();
}

void gp_window_show(gp_window* window)
{
  window->mWindow->GetWidget()->show();
}

void gp_window_hide(gp_window* window)
{
  window->mWindow->GetWidget()->hide();
}

int gp_window_get_shown(gp_window* window)
{
  return !window->mWindow->GetWidget()->isHidden();
}

