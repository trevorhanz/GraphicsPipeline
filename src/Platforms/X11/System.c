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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Target.h>
#include <GraphicsPipeline/X11.h>
#include "X11.h"
#include "API/GL/GL.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/select.h>

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  gp_list_init(&system->mTargets);
  system->mEvent = _gp_event_new();
  
  char* display = getenv("DISPLAY");
  system->mDisplay = XOpenDisplay(display);
  if(!system->mDisplay)
  {
    gp_log_error("Unable to connect to X11 server (%s).", (display)?display:":0");
    free(system);
    return NULL;
  }
  
  system->mDeleteMessage = XInternAtom(system->mDisplay, "WM_DELETE_WINDOW", False);
  
  return system;
}

void gp_system_free(gp_system* system)
{
  assert(system != NULL);
  
  XCloseDisplay(system->mDisplay);
  
  _gp_event_free(system->mEvent);
  free(system);
}

void _gp_system_process_events(gp_io* io)
{
  gp_system* system = (gp_system*)gp_io_get_userdata(io);
  
  XEvent event;
  
  while(XPending(system->mDisplay))
  {
    XNextEvent(system->mDisplay, &event);
    
    if(event.type == Expose)
    {
      gp_list_node* node = gp_list_front(&system->mTargets);
      while(node != NULL)
      {
        gp_target* target = (gp_target*)node;
        if(target->mWindow == event.xexpose.window)
        {
          gp_target_redraw(target);
        }
        node = gp_list_node_next(node);
      }
    }
    
    if(event.type == ClientMessage)
    {
      if(event.xclient.data.l[0] == system->mDeleteMessage)
      {
        gp_system_stop(system);
      }
    }
  }
}

void gp_system_run(gp_system* system)
{
  assert(system != NULL);
  
  int fd = XConnectionNumber(system->mDisplay);
  
  gp_io* io = gp_system_io_read_new(system, fd);
  gp_io_set_callback(io, _gp_system_process_events);
  gp_io_set_userdata(io, (void*)system);
  
  _gp_event_run(system->mEvent);
  
  gp_io_free(io);
}

void gp_system_stop(gp_system* system)
{
  _gp_event_stop(system->mEvent);
}

gp_timer* gp_system_timer_new(gp_system* system)
{
  return _gp_event_timer_new(system->mEvent);
}

gp_io* gp_system_io_read_new(gp_system* system, int fd)
{
  return _gp_event_io_read_new(system->mEvent, fd);
}

gp_io* gp_system_io_write_new(gp_system* system, int fd)
{
  return _gp_event_io_write_new(system->mEvent, fd);
}


