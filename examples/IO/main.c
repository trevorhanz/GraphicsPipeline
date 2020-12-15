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
#include <unistd.h>

void IOCallback(gp_io* io)
{
  char buffer[256];
  read(0, buffer, 256);
  printf("IO: %s\n", buffer);
}

int main(int argc, char* argv[])
{
  gp_system* system = gp_system_new();
  
  gp_io* io = gp_system_io_read_new(system, 0); // STDIN
  gp_io_set_callback(io, IOCallback);
  
  gp_system_run(system);
  
  return EXIT_SUCCESS;
}
