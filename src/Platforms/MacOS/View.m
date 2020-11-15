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

#import <Foundation/Foundation.h>

#import "View.h"

// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>

#include "API/GL/GL.h"
#include "MacOS.h"

@implementation View

- (id) initWithFrame:(NSRect)frame
{
  self = [super initWithFrame:frame];
}

- (void) reshape
{
  [super reshape];
}

- (void) drawRect:(NSRect)rect
{
  [super drawRect:rect];
  
  [[self openGLContext] makeCurrentContext];
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  _gp_pipeline_execute(mTarget->mPipeline);
  
  [[self openGLContext] flushBuffer];
}

- (void) update
{
  [super update];
}

- (void) setTarget:(gp_target*)target
{
  mTarget = target;
}

@end
