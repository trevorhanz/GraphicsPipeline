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

#ifndef __GP_WINDOWS_COMMON_H__
#define __GP_WINDOWS_COMMON_H__

#include <GraphicsPipeline/Types.h>
#include <API/GL/GL.h>

#include <windows.h>

#define WM_SOCKET WM_USER + 1

struct _gp_system
{
  HWND                  mInternalWindow;
};

struct _gp_context
{
  gp_system*            mParent;
  HGLRC                 mShare;
  HWND                  mWindow;
  unsigned int          mPixelFormat;
};

struct _gp_target
{
  gp_context*           mParent;
  HGLRC                 mContext;
  HWND                  mWindow;
  gp_pipeline*          mPipeline;
};

struct _gp_timer
{
  gp_system*            mSystem;
  gp_timer_callback     mCallback;
  void*                 mUserData;
};

struct _gp_io
{
  gp_system*            mSystem;
  gp_io_callback        mCallback;
  void*                 mUserData;
};

#endif // __GP_WINDOWS_COMMON_H__