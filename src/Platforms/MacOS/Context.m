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

#include "MacOS.h"

#include <unistd.h>

#include <OpenGL/gl3.h>

#include "View.h"
#include "Platforms/Defaults.h"

void gp_context_ref(gp_context* context)
{
  gp_ref_inc(&context->mRef);
}

void gp_context_unref(gp_context* context)
{
  if(gp_ref_dec(&context->mRef))
  {
    free(context);
  }
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(gp_target));
  target->mParent = context;
  target->mPipeline = _gp_pipeline_new();
  gp_ref_init(&target->mRef);
  
  NSUInteger windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable |
                           NSWindowStyleMaskMiniaturizable;
  target->mWindow = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT)
  styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [target->mWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  [target->mWindow setTitle:@"GP Window"];
  [target->mWindow makeKeyAndOrderFront:nil];

  // Create OpenGL view
  target->mView = [[View alloc] initWithFrame:NSMakeRect( 0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT ) pixelFormat:context->mPixelFormat];
  NSOpenGLContext* newContext = [ [ NSOpenGLContext alloc ] initWithFormat:context->mPixelFormat shareContext:context->mShare ];
  [target->mView setOpenGLContext: newContext];
  [target->mWindow setContentView:target->mView];
  [target->mView setTarget:target];
  [target->mView display];

  [[target->mView openGLContext] makeCurrentContext];

  return target;
}

void gp_target_ref(gp_target* target)
{
  gp_ref_inc(&target->mRef);
}

void gp_target_unref(gp_target* target)
{
  if(gp_ref_dec(&target->mRef))
  {
    _gp_pipeline_free(target->mPipeline);
    free(target);
  }
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
  [target->mView setNeedsDisplay:YES];
}

