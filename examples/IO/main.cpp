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
  gp_log_info("IO example");
  gp_log("Type 'exit' to quit.\n");
  
  std::cout << "Input: ";
  std::cout.flush();
  
  System system;
  
  ReadIO io(system, 0); // STDIN;
  io.SetCallback([&](IO&)
  {
    std::string in;
    std::cin >> in;
    if(in == "exit")
    {
      system.Stop();
      return;
    }
    std::cout << "Read: " << in << std::endl << std::endl;
    std::cout << "Input: ";
    std::cout.flush();
  });
  
  system.Run();
  
  return EXIT_SUCCESS;
}
