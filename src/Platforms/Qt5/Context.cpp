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

extern "C" gp_context* gp_qt_context_new()
{
  gp_context* context = new gp_context;
  gp_ref_init(&context->mRef);
  sContext = context;
  
  QSurfaceFormat format;
  format.setMajorVersion(4);
  format.setMinorVersion(2);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setSamples(4);
  format.setOption(QSurfaceFormat::DebugContext);
  
  context->mSurface = new QOffscreenSurface();
  context->mSurface->setFormat(format);
  context->mSurface->create();
  
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

void gp_context_ref(gp_context* context)
{
  gp_ref_inc(&context->mRef);
}

void gp_context_unref(gp_context* context)
{
  if(gp_ref_dec(&context->mRef))
  {
    delete context;
  }
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = new gp_window();
  window->mWindow = new Window(context->mShare);
  gp_ref_init(&window->mRef);
  
  return window;
}

void gp_window_ref(gp_window* window)
{
  gp_ref_inc(&window->mRef);
}

void gp_window_unref(gp_window* window)
{
  if(gp_ref_dec(&window->mRef))
  {
    free(window);
  }
}

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mWindow->GetPipeline();
}

void gp_window_redraw(gp_window* window)
{
  window->mWindow->Draw();
}

extern "C" void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data)
{
  sContext->mWorkQueue->AddWork(work, join, data);
}
