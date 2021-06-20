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
#include <GraphicsPipeline/MacOS.h>
#include "MacOS.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

void _gp_system_free(gp_object* object)
{
  gp_system* system = (gp_system*)object;
  
  free(system);
}

gp_system* gp_system_new()
{
  gp_system* system = malloc(sizeof(gp_system));
  _gp_object_init(&system->mObject, _gp_system_free);
  
  return system;
}

void gp_system_run(gp_system* system)
{
  [NSApplication sharedApplication];
  [NSApp run];
}

void gp_system_stop(gp_system* system)
{
  
}

void _gp_timer_free(gp_object* object)
{
  gp_timer* timer = (gp_timer*)object;
  
  free(timer);
}

gp_timer* gp_timer_new(gp_system* system)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  _gp_object_init(&timer->mObject, _gp_timer_free);
  timer->mTimer = [[Timer alloc] init:timer];
  
  return timer;
}

void _gp_io_free(gp_object* object)
{
  gp_io* io = (gp_io*)object;
  
  if(io->mUserData) gp_object_unref((gp_object*)io->mUserData);
  
  free(io);
}

gp_io* gp_io_read_new(gp_system*  system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  _gp_object_init(&io->mObject, _gp_io_free);
  io->mHandle = [[NSFileHandle alloc] initWithFileDescriptor:fd];
  io->mHandle.readabilityHandler = ^(NSFileHandle* fh){
    io->mCallback(io);
  };
  io->mUserData = NULL;
  
  return io;
}

gp_io* gp_io_write_new(gp_system* system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mHandle = [[NSFileHandle alloc] initWithFileDescriptor:fd];
  io->mHandle.writeabilityHandler = ^(NSFileHandle* fh){
    io->mCallback(io);
  };
  
  return io;
}
