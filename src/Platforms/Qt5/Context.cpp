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

#include "Qt.h"
#include <GraphicsPipeline/Logging.h>

#include <QOpenGLFunctions>

#include "GL.h"

gp_context* sContext = 0;

static const struct { int major, minor; } _gl_versions[] = {
  {4, 6},
  {4, 5},
  {4, 4},
  {4, 3},
  {4, 2},
  {4, 1},
  {4, 0},

  {3, 3},
  {3, 2},
  {3, 1},
  
  {0, 0} /* end of list */
};

void _gp_context_free(gp_object* object)
{
  gp_context* context = (gp_context*)object;
  
  delete context;
}

extern "C" gp_context* gp_qt_context_new()
{
  gp_context* context = new gp_context;
  _gp_object_init(&context->mObject, _gp_context_free);
  sContext = context;
  
  QSurfaceFormat format;
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setSamples(4);
  format.setOption(QSurfaceFormat::DebugContext);
  
  QOffscreenSurface* surface  = new QOffscreenSurface();
  for(int i=0; _gl_versions[i].major!=0; ++i)
  {
    format.setMajorVersion(_gl_versions[i].major);
    format.setMinorVersion(_gl_versions[i].minor);
    
    surface->setFormat(format);
    surface->create();
    if(surface->isValid()) break;
  }
  
  context->mSurface = surface;
  
  context->mShare = new QOpenGLContext();
  context->mShare->setFormat(format);
  if(!context->mShare->create())
  {
    delete context->mShare;
    delete context;
    gp_log_error("Unable to create context");
    return 0;
  }
  context->mShare->makeCurrent(context->mSurface);
  
  QOpenGLFunctions* functions = context->mShare->functions();
  
  gp_log_info("Qt Version: %s", qVersion());
  
  int major, minor;
  functions->glGetIntegerv(GL_MAJOR_VERSION, &major);
  functions->glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  _gp_api_init();
  
  _gp_api_init_context();
  
  context->mWorkQueue = new WorkQueue(context->mShare);
  QObject::connect(context->mWorkQueue, &WorkQueue::workFinished, context->mWorkQueue, &WorkQueue::Finalize);
  context->mWorkQueue->start();
  
  //
  // Generate key map
  //
  std::map<int, gp_key_t>& keys = context->mKeyMap;
  keys[Qt::Key_A] = GP_KEY_A;
  keys[Qt::Key_B] = GP_KEY_B;
  keys[Qt::Key_C] = GP_KEY_C;
  keys[Qt::Key_D] = GP_KEY_D;
  keys[Qt::Key_E] = GP_KEY_E;
  keys[Qt::Key_F] = GP_KEY_F;
  keys[Qt::Key_G] = GP_KEY_G;
  keys[Qt::Key_H] = GP_KEY_H;
  keys[Qt::Key_I] = GP_KEY_I;
  keys[Qt::Key_J] = GP_KEY_J;
  keys[Qt::Key_K] = GP_KEY_K;
  keys[Qt::Key_L] = GP_KEY_L;
  keys[Qt::Key_M] = GP_KEY_M;
  keys[Qt::Key_N] = GP_KEY_N;
  keys[Qt::Key_O] = GP_KEY_O;
  keys[Qt::Key_P] = GP_KEY_P;
  keys[Qt::Key_Q] = GP_KEY_Q;
  keys[Qt::Key_R] = GP_KEY_R;
  keys[Qt::Key_S] = GP_KEY_S;
  keys[Qt::Key_T] = GP_KEY_T;
  keys[Qt::Key_U] = GP_KEY_U;
  keys[Qt::Key_V] = GP_KEY_V;
  keys[Qt::Key_W] = GP_KEY_W;
  keys[Qt::Key_X] = GP_KEY_X;
  keys[Qt::Key_Y] = GP_KEY_Y;
  keys[Qt::Key_Z] = GP_KEY_Z;
  
  keys[Qt::Key_0] = GP_KEY_0;
  keys[Qt::Key_1] = GP_KEY_1;
  keys[Qt::Key_2] = GP_KEY_2;
  keys[Qt::Key_3] = GP_KEY_3;
  keys[Qt::Key_4] = GP_KEY_4;
  keys[Qt::Key_5] = GP_KEY_5;
  keys[Qt::Key_6] = GP_KEY_6;
  keys[Qt::Key_7] = GP_KEY_7;
  keys[Qt::Key_8] = GP_KEY_8;
  keys[Qt::Key_9] = GP_KEY_9;
  
  keys[Qt::Key_F1] = GP_KEY_F1;
  keys[Qt::Key_F2] = GP_KEY_F2;
  keys[Qt::Key_F3] = GP_KEY_F3;
  keys[Qt::Key_F4] = GP_KEY_F4;
  keys[Qt::Key_F5] = GP_KEY_F5;
  keys[Qt::Key_F6] = GP_KEY_F6;
  keys[Qt::Key_F7] = GP_KEY_F7;
  keys[Qt::Key_F8] = GP_KEY_F8;
  keys[Qt::Key_F9] = GP_KEY_F8;
  keys[Qt::Key_F10] = GP_KEY_F10;
  keys[Qt::Key_F11] = GP_KEY_F11;
  keys[Qt::Key_F12] = GP_KEY_F12;
  keys[Qt::Key_F13] = GP_KEY_F13;
  keys[Qt::Key_F14] = GP_KEY_F14;
  keys[Qt::Key_F15] = GP_KEY_F15;
  keys[Qt::Key_F16] = GP_KEY_F16;
  keys[Qt::Key_F17] = GP_KEY_F17;
  keys[Qt::Key_F18] = GP_KEY_F18;
  keys[Qt::Key_F19] = GP_KEY_F19;
  keys[Qt::Key_F20] = GP_KEY_F20;
  keys[Qt::Key_F21] = GP_KEY_F21;
  keys[Qt::Key_F22] = GP_KEY_F22;
  keys[Qt::Key_F23] = GP_KEY_F23;
  keys[Qt::Key_F24] = GP_KEY_F24;
  keys[Qt::Key_F25] = GP_KEY_F25;
  
  keys[Qt::Key_Up] = GP_KEY_UP;
  keys[Qt::Key_Down] = GP_KEY_DOWN;
  keys[Qt::Key_Left] = GP_KEY_LEFT;
  keys[Qt::Key_Right] = GP_KEY_RIGHT;
  
  keys[Qt::Key_PageUp] = GP_KEY_PAGE_UP;
  keys[Qt::Key_PageDown] = GP_KEY_PAGE_DOWN;
  keys[Qt::Key_Home] = GP_KEY_HOME;
  keys[Qt::Key_End] = GP_KEY_END;
  
  keys[Qt::Key_Escape] = GP_KEY_ESCAPE;
  keys[Qt::Key_Tab] = GP_KEY_TAB;
  keys[Qt::Key_Backspace] = GP_KEY_BACKSPACE;
  keys[Qt::Key_Return] = GP_KEY_ENTER;
  keys[Qt::Key_Enter] = GP_KEY_KP_ENTER;
  keys[Qt::Key_Insert] = GP_KEY_INSERT;
  keys[Qt::Key_Delete] = GP_KEY_DELETE;
  keys[Qt::Key_Pause] = GP_KEY_PAUSE;
  keys[Qt::Key_Print] = GP_KEY_PRINT_SCREEN;
  
  keys[Qt::Key_Shift] = GP_KEY_LEFT_SHIFT;
  keys[Qt::Key_Control] = GP_KEY_LEFT_CONTROL;
  keys[Qt::Key_Alt] = GP_KEY_LEFT_ALT;
  keys[Qt::Key_Meta] = GP_KEY_MENU;
  
  keys[Qt::Key_CapsLock] = GP_KEY_CAPS_LOCK;
  keys[Qt::Key_NumLock] = GP_KEY_NUM_LOCK;
  keys[Qt::Key_ScrollLock] = GP_KEY_SCROLL_LOCK;
  
  keys[Qt::Key_Space] = GP_KEY_SPACE;
  
  return context;
}

QWidget* gp_window_get_qwidget(gp_window* window)
{
  return window->mWindow->GetWidget();
}

extern "C" gp_context* gp_context_new(gp_system* system)
{
  return gp_qt_context_new();
}

extern "C" void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  sContext->mWorkQueue->AddWork(work, join, data);
}

extern "C" void _gp_api_context_make_current(gp_context* context)
{
  context->mShare->makeCurrent(context->mSurface);
}
