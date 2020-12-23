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

using namespace GP;

void TimerCallback(gp_timer* timer)
{
  printf("Timeout\n");
  gp_timer_arm(timer, 1.0);
}

int main(int argc, char* argv[])
{
  System* system = new System();
  
  Timer* timer = system->CreateTimer();
  timer->SetCallback(TimerCallback);
  timer->Arm(1.0);
  
  system->Run();
  
  return EXIT_SUCCESS;
}
