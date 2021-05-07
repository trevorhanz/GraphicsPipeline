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
#include <string.h>

#ifdef GP_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

void IOCallback(gp_io* io, gp_pointer* userdata)
{
  char buffer[256];
  memset(buffer, 0, sizeof(buffer));
  read(0, buffer, 256);
  if(strcmp(buffer, "exit\n") == 0)
  {
    gp_system* system = (gp_system*)gp_pointer_get_pointer(userdata);
    gp_system_stop(system);
    return;
  }
  printf("Read: %s\n", buffer);
  printf("Input: ");
  fflush(stdout);
}

int main(int argc, char* argv[])
{
  gp_log_info("IO example");
  gp_log("Type 'exit' to quit.\n");
  
  printf("Input: ");
  fflush(stdout);
  
  gp_system* system = gp_system_new();
  
  gp_pointer* pointer = gp_pointer_new(system, 0);
  gp_io* io = gp_io_read_new(system, 0); // STDIN
  gp_io_set_callback(io, IOCallback, pointer);
  gp_object_unref((gp_object*)pointer);
  
  gp_system_run(system);
  
  gp_object_unref((gp_object*)io);
  gp_object_unref((gp_object*)system);
  
  return EXIT_SUCCESS;
}
