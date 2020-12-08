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

#ifndef __GP_QT_COMMON_H__
#define __GP_QT_COMMON_H__

#include <GraphicsPipeline/GP.h>

#include <QApplication>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QOpenGLShaderProgram>
#include <QOpenGLWindow>

#include "Target.h"

struct _gp_system
{
  QApplication*           mApp;
};

struct _gp_context
{
  QOpenGLContext*         mShare;
  QOffscreenSurface*      mSurface;
};

struct _gp_target
{
  Target*                 mTarget;
};

#endif // __GP_QT_COMMON_H__
