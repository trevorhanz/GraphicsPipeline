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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Web.h>
#include <API/GL/GL.h>
#include "Web.h"

#include <stdlib.h>

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  system->mCanvasIndex = 0;
  
  return system;
}

void gp_system_free(gp_system* system)
{
  free(system);
}

void gp_system_run(gp_system* system)
{
  
}

gp_timer* gp_timer_new(gp_system* system)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  timer->mTimerID = -1;
  timer->mCallback = NULL;
  timer->mUserData = NULL;
  
  return timer;
}

gp_io* gp_system_io_read_new(gp_system* system, int fd)
{
  gp_log_warn("IO watchers not implemented");
  
  return NULL;
}

gp_io* gp_system_io_write_new(gp_system* system, int fd)
{
  gp_log_warn("IO watchers not implemented");
  
  return NULL;
}
