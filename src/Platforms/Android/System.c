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
#include <GraphicsPipeline/Logging.h>
#include "Android.h"

#include <stdlib.h>

void _gp_system_free(gp_object* object)
{
  gp_system* system = (gp_system*)object;
  
  free(system);
}

gp_system* gp_system_new()
{
  gp_system* system = malloc(sizeof(gp_system));
  _gp_object_init(&system->mObject, _gp_system_free);
  
  if((system->mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    gp_log_error("eglGetDisplay() returned error %d", eglGetError());
    free(system);
    return NULL;
  }
  if(!eglInitialize(system->mDisplay, 0, 0)) {
    gp_log_error("eglInitialize() returned error %d", eglGetError());
    free(system);
    return NULL;
  }
  
  return system;
}

void gp_system_run(gp_system* system)
{
}

void gp_system_stop(gp_system* system)
{
}

gp_timer* gp_timer_new(gp_system* system)
{
  return NULL;
}

gp_io* gp_io_read_new(gp_system* system, int fd)
{
  return NULL;
}

gp_io* gp_io_write_new(gp_system* system, int fd)
{
  return NULL;
}
