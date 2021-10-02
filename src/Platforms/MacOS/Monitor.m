/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#include "MacOS.h"

#include <stdlib.h>

gp_monitor_list* gp_monitor_list_new(gp_system* system)
{
  gp_monitor_list* monitors = (gp_monitor_list*)malloc(sizeof(gp_monitor_list));
  _gp_object_init(&monitors->mObject, (gp_object_free)free);
  
  return monitors;
}

int gp_monitor_list_get_count(gp_monitor_list* monitors)
{
  return 0;
}

gp_monitor* gp_monitor_list_get_by_index(gp_monitor_list* monitors, int index)
{
  return NULL;
}

gp_monitor* gp_monitor_list_get_primary(gp_monitor_list* monitors)
{
  return NULL;
}

void gp_monitor_get_size(gp_monitor* monitor, int* width, int* height)
{
  
}

