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
#include "Context.h"
#include "MacOS.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

gp_system* gp_system_new()
{
  gp_system* system = malloc(sizeof(gp_system));
  
  return system;
}

void gp_system_free(gp_system* system)
{
  free(system);
}

gp_context* gp_system_context_new(gp_system* system)
{
  gp_context* context = malloc(sizeof(gp_context));
  
  NSOpenGLPixelFormatAttribute pixelAttribs[ 16 ];
  int pixNum = 0;

  pixelAttribs[ pixNum++ ] = NSOpenGLPFAOpenGLProfile;
  pixelAttribs[ pixNum++ ] = NSOpenGLProfileVersion3_2Core;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFADoubleBuffer;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAAccelerated;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAColorSize;
  pixelAttribs[ pixNum++ ] = 32;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFADepthSize;
  pixelAttribs[ pixNum++ ] = 16;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFAMultisample,
  pixelAttribs[ pixNum++ ] = NSOpenGLPFASampleBuffers;
  pixelAttribs[ pixNum++ ] = 1;
  pixelAttribs[ pixNum++ ] = NSOpenGLPFASamples;
  pixelAttribs[ pixNum++ ] = 4;
  pixelAttribs[ pixNum ] = 0;
  context->mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelAttribs];

  context->mShare = [[NSOpenGLContext alloc] initWithFormat:context->mPixelFormat shareContext:NULL];
  [context->mShare makeCurrentContext];

  // NOTE: GL functions need a context to be bound to get information from
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);

  GLboolean shaderSupport;
  glGetBooleanv(GL_SHADER_COMPILER, &shaderSupport);
  gp_log_info("ShaderSupport: %s", (shaderSupport)?"TRUE":"FALSE");
  
  return context;
}

void gp_system_run(gp_system* system)
{
  [NSApplication sharedApplication];
  [NSApp run];
}

gp_timer* gp_system_timer_new(gp_system* system)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  timer->mTimer = [[Timer alloc] init:timer];
  
  return timer;
}

gp_io* gp_system_io_read_new(gp_system*  system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mHandle = [[NSFileHandle alloc] initWithFileDescriptor:fd];
  io->mHandle.readabilityHandler = ^(NSFileHandle* fh){
    io->mCallback(io);
  };
  
  return io;
}

gp_io* gp_system_io_write_new(gp_system* system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mHandle = [[NSFileHandle alloc] initWithFileDescriptor:fd];
  io->mHandle.writeabilityHandler = ^(NSFileHandle* fh){
    io->mCallback(io);
  };
  
  return io;
}
