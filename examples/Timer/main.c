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

void TimerCallback(gp_timer* timer)
{
  printf("Timeout\n");
  gp_timer_arm(timer, 1.0);
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_timer* timer = gp_system_timer_new(system);
  gp_timer_set_callback(timer, TimerCallback);
  gp_timer_arm(timer, 1.0);
  
  gp_system_run(system);
  
  gp_timer_free(timer);
  gp_system_free(system);
  
  return EXIT_SUCCESS;
}
