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

extern "C" gp_context* gp_qt_context_new()
{
  gp_context* context = new gp_context;
  
  QSurfaceFormat format;
  format.setMajorVersion(4);
  format.setMinorVersion(2);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
  format.setSamples(4);
  
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
  
  int major, minor;
  functions->glGetIntegerv(GL_MAJOR_VERSION, &major);
  functions->glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  _gp_api_init();
  
  return context;
}

QWidget* gp_target_get_qwidget(gp_target* target)
{
  return target->mTarget->GetWidget();
}

void gp_context_free(gp_context* context)
{
  delete context;
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = new gp_target();
  target->mTarget = new Target(context->mShare);
//   target->mTarget->GetWidget()->show();
  
  return target;
}

gp_array* gp_context_array_new(gp_context* context)
{
  QOpenGLFunctions* functions = context->mShare->functions();
  
  gp_array* array = new gp_array();
  functions->glGenBuffers(1, &array->mVBO);
  
  return array;
}

gp_shader* gp_context_shader_new(gp_context* context)
{
  gp_shader* shader = new gp_shader;
  
  return shader;
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mTarget->GetPipeline();
}

void gp_target_redraw(gp_target* target)
{
  target->mTarget->Draw();
}
