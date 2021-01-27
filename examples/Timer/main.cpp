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

#include <iostream>

using namespace GP;

int main(int argc, char* argv[])
{
  System system;
  
  Timer* timer = system.CreateTimer();
  timer->SetCallback([](Timer* timer){std::cout << "Timeout" << std::endl; timer->Arm(1.0);});
  timer->Arm(1.0);
  
  system.Run();
  
  delete timer;
  
  return EXIT_SUCCESS;
}
