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

#include "Android.h"

void gp_timer_free(gp_timer* timer)
{
}

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback)
{
}

void gp_timer_set_userdata(gp_timer* timer, void* userdata)
{
}

void* gp_timer_get_userdata(gp_timer* timer)
{
  return NULL;
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
}

void gp_timer_disarm(gp_timer* timer)
{
}

void gp_io_free(gp_io* io)
{
}

void gp_io_set_callback(gp_io* io, gp_io_callback callback)
{
}

void gp_io_set_userdata(gp_io* io, void* userdata)
{
}

void* gp_io_get_userdata(gp_io* io)
{
  return NULL;
}
