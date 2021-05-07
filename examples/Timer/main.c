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

#include <GraphicsPipeline/GP.h>
#include <GraphicsPipeline/Desktop.h>

#include <stdlib.h>
#include <stdio.h>

// Time between two consecutive timers
#define TIMER_DELAY 0.1

int num_timers;

typedef struct
{
  int index;
} timeout_data;

void TimerCallback(gp_timer* timer, gp_pointer* userdata)
{
  timeout_data* data = (timeout_data*)gp_pointer_get_pointer(userdata);
  
  gp_log("Timeout: %d", data->index);
  gp_timer_arm(timer, TIMER_DELAY*num_timers);
}

int main(int argc, char* argv[])
{
  num_timers = 1;
  if(argc == 2)
    num_timers = atoi(argv[1]);
  
  gp_system* system = gp_system_new();
  gp_timer** timers = malloc(sizeof(gp_timer*)*num_timers);
  
  for(int i=0; i<num_timers; ++i)
  {
    timeout_data* data = malloc(sizeof(timeout_data));
    data->index = i;
    
    gp_pointer* pointer = gp_pointer_new(data, free);
    timers[i] = gp_timer_new(system);
    gp_timer_set_callback(timers[i], TimerCallback, pointer);
    gp_timer_arm(timers[i], TIMER_DELAY*(i+1));
    gp_object_unref((gp_object*)pointer);
  }
  
  gp_system_run(system);
  
  for(int i=0; i<num_timers; ++i)
  {
    gp_object_unref((gp_object*)timers[i]);
  }
  free(timers);
  gp_object_unref((gp_object*)system);
  
  return EXIT_SUCCESS;
}
