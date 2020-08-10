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

#include "Context.h"
#include "MacOS.h"

#include <iostream>
#include <unistd.h>

#include <OpenGL/gl3.h>

#include "View.h"

using namespace GP::GL;
using namespace std;

Context::Context(void(*callback)())
  : mCallback(callback)
{
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
  mPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelAttribs];
  
  mShare = [[NSOpenGLContext alloc] initWithFormat:mPixelFormat shareContext:NULL];
  [mShare makeCurrentContext];
  
  // NOTE: GL functions need a context to be bound to get information from
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  cout << "OpenGL Version: " << major << "." << minor << endl;
  LogD("Test");
  LogD("Test");
  
  GLboolean shaderSupport;
  glGetBooleanv(GL_SHADER_COMPILER, &shaderSupport);
  cout << "ShaderSupport: " << ((shaderSupport)?"TRUE":"FALSE") << endl;
}

Context::~Context()
{
  
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<Pipeline>(this);
}

TargetUserDataPtr Context::CreateTarget()
{
  GP::TargetUserDataPtr ptr = std::make_shared<GP::TargetUserData>();
  NSUInteger windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable | NSWindowStyleMaskMiniaturizable;
  ptr->mWindow = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 640, 480)
  styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [ptr->mWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  [ptr->mWindow setTitle:@"GP Window"];
  [ptr->mWindow makeKeyAndOrderFront:nil];
  
  // Create OpenGL view
  ptr->mView = [[View alloc] initWithFrame:NSMakeRect( 0, 0, 800, 600 ) pixelFormat:mPixelFormat];
  NSOpenGLContext* newContext = [ [ NSOpenGLContext alloc ] initWithFormat:mPixelFormat shareContext:mShare ];
  [ptr->mView setOpenGLContext: newContext];
  [ptr->mView setCallback:mCallback];
  [ptr->mWindow setContentView:ptr->mView];
  [ptr->mView display];
  
  ptr->MakeCurrent();
  
  return ptr;
}

void Context::Bind(GP::TargetPtr target)
{
}


