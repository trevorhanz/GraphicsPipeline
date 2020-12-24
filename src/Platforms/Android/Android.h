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

#ifndef __GP_ANDROID_COMMON_H__
#define __GP_ANDROID_COMMON_H__

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>

#include <android/native_window.h>

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/Logging.h>
#include <API/GL/GL.h>

struct _gp_system
{
  EGLDisplay                            mDisplay;
  gp_target*                            mTarget;
};

struct _gp_context
{
  gp_system*                            mParent;
  EGLConfig                             mConfig;
  EGLint                                mFormat;
  EGLContext                            mShare;
  EGLSurface                            mShareSurface;
};

struct _gp_target
{
  EGLContext                            mContext;
  ANativeWindow*                        mWindow;
  EGLSurface                            mSurface;
  GLuint                                vbo;
  GLuint                                shaderProgram;
  GLint                                 posAttrib;
  gp_pipeline*                          mPipeline;
};

struct _gp_timer
{
};

struct _gp_io
{
};

#endif // __GP_ANDROID_COMMON_H__
