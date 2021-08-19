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

#include "Web.h"

#include <stdlib.h>

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback, gp_pointer* userdata)
{
  timer->mCallback = callback;
  
  if(timer->mUserData)
  {
    gp_object_unref((gp_object*)timer->mUserData);
  }
  
  timer->mUserData = userdata;
  
  if(timer->mUserData)
  {
    gp_object_ref((gp_object*)timer->mUserData);
  }
}

void _gp_timer_timeout(void* userdata)
{
  gp_timer* timer = (gp_timer*)userdata;
  timer->mTimerID = -1;
  timer->mCallback(timer, timer->mUserData);
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
  timer->mTimerID = emscripten_set_timeout(_gp_timer_timeout, timeout*1000, timer);
}

void gp_timer_disarm(gp_timer* timer)
{
  if(timer->mTimerID < 0) return;
  
  EM_ASM({
    clearTimeout($0)
  }, timer->mTimerID);
}

void gp_io_set_callback(gp_io* io, gp_io_callback callback, gp_pointer* userdata)
{
}
