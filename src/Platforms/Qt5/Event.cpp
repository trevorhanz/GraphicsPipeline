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

extern "C" void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback, gp_pointer* userdata)
{
  timer->mTimerCallback->mCallback = callback;
  
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

extern "C" void gp_timer_arm(gp_timer* timer, double timeout)
{
  timer->mTimer->start(timeout*1000);
}

extern "C" void gp_timer_disarm(gp_timer* timer)
{
  timer->mTimer->stop();
}

extern "C" void gp_io_set_callback(gp_io* io, gp_io_callback callback, gp_pointer* userdata)
{
  io->mIOCallback->mCallback = callback;
  
  if(io->mUserData)
  {
    gp_object_unref((gp_object*)io->mUserData);
  }
  
  io->mUserData = userdata;
  
  if(io->mUserData)
  {
    gp_object_ref((gp_object*)io->mUserData);
  }
}


