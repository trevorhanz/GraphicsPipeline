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

#include <GL/glew.h>
#include <GL/wglew.h>

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Windows.h>
#include <GraphicsPipeline/Logging.h>
#include "API/GL/GL.h"

#include "Windows.h"

void gp_timer_free(gp_timer* timer)
{
  free(timer);
}

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback)
{
  timer->mCallback = callback;
}

void gp_timer_set_userdata(gp_timer* timer, void* userdata)
{
  timer->mUserData = userdata;
}

void* gp_timer_get_userdata(gp_timer* timer)
{
  return timer->mUserData;
}

void _gp_timer_callback(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD dwTime)
{
  gp_timer* timer = (gp_timer*)timerId;
  KillTimer(hwnd, timerId);

  timer->mCallback(timer);
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
  SetTimer(timer->mSystem->mInternalWindow, (UINT_PTR)timer, timeout * 1000, &_gp_timer_callback);
}

void gp_timer_disarm(gp_timer* timer)
{
  KillTimer(timer->mSystem->mInternalWindow, (UINT_PTR)timer);
}

void gp_io_free(gp_io* io)
{
  free(io);
}

void gp_io_set_callback(gp_io* io, gp_io_callback callback)
{
  io->mCallback = callback;
}

void gp_io_set_userdata(gp_io* io, void* userdata)
{
  io->mUserData = userdata;
}

void* gp_io_get_userdata(gp_io* io)
{
  return io->mUserData;
}

