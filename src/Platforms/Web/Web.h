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

#ifndef __GP_WEB_COMMON_H__
#define __GP_WEB_COMMON_H__

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <emscripten.h>
#include <emscripten/html5.h>

struct _gp_system
{
  gp_target*                            mTarget;
};

struct _gp_context
{
  gp_system*                            mParent;
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mShare;
};

struct _gp_target
{
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mContext;
  GLuint                                vbo;
  GLuint                                shaderProgram;
  GLint                                 posAttrib;
  gp_pipeline*                          mPipeline;
};

struct _gp_timer
{
  int                                   mTimerID;
  gp_timer_callback                     mCallback;
  void*                                 mUserData;
};

struct _gp_io
{
};

#endif // __GP_WEB_COMMON_H__
