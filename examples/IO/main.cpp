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

#include <stdlib.h>
#include <stdio.h>

#ifdef GP_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

using namespace GP;

void IOCallback(gp_io* io)
{
  char buffer[256];
  read(0, buffer, 256);
  printf("IO: %s\n", buffer);
}

int main(int argc, char* argv[])
{
  System* system = new System();
  
  IO* io = system->CreateReadIO(0); // STDIN;
  io->SetCallback(IOCallback);
  
  system->Run();
  
  return EXIT_SUCCESS;
}
