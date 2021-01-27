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

#include <iostream>

using namespace GP;

int main(int argc, char* argv[])
{
  System system;
  
  IO* io = system.CreateReadIO(0); // STDIN;
  io->SetCallback([](IO*)
  {
    std::string in;
    std::cin >> in;
    std::cout << "IO: " << in << std::endl;
  });
  
  system.Run();
  
  delete io;
  
  return EXIT_SUCCESS;
}
