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

#include "View.h"
#include "WindowDelegate.h"
#include "Platforms/Defaults.h"

void _gp_window_free(gp_object* object)
{
  gp_window* window = (gp_window*)object;
  
  _gp_pipeline_free(window->mPipeline);
  free(window);
}

gp_window* gp_window_new(gp_context* context)
{
  gp_window* window = malloc(sizeof(gp_window));
  _gp_object_init(&window->mObject, _gp_window_free);
  window->mParent = context;
  window->mPipeline = _gp_pipeline_new();
  window->mClickCB = NULL;
  window->mClickData = NULL;
  window->mMoveCB = NULL;
  window->mMoveData = NULL;
  window->mEnterCB = NULL;
  window->mEnterData = NULL;
  window->mKeyCB = NULL;
  window->mKeyData = NULL;
  window->mResizeCB = NULL;
  window->mResizeData = NULL;
  
  NSUInteger windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable |
                           NSWindowStyleMaskMiniaturizable;
  window->mWindow = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT)
  styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [window->mWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  [window->mWindow setTitle:@"GP Window"];
  
  // Set delegate
  id delegate = [[WindowDelegate alloc] initWithWindow:window];
  [window->mWindow setDelegate:delegate];
  
  // Create OpenGL view
  window->mView = [[View alloc] initWithFrame:NSMakeRect( 0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT ) pixelFormat:context->mPixelFormat];
  NSOpenGLContext* newContext = [ [ NSOpenGLContext alloc ] initWithFormat:context->mPixelFormat shareContext:context->mShare ];
  [window->mView setOpenGLContext: newContext];
  [window->mWindow setContentView:window->mView];
  [window->mWindow makeFirstResponder:window->mView];
  [window->mWindow setInitialFirstResponder:window->mView];
  [window->mWindow setAcceptsMouseMovedEvents:YES];
  [window->mWindow makeKeyAndOrderFront:window->mWindow];
  [window->mView setWindow:window];
  [window->mView display];
  [[window->mView openGLContext] makeCurrentContext];
  
  _gp_api_init_context();
  
  return window;
}

gp_pipeline* gp_window_get_pipeline(gp_window* window)
{
  return window->mPipeline;
}

void gp_window_redraw(gp_window* window)
{
  [window->mView setNeedsDisplay:YES];
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  NSRect rect = [window->mView frame];
  if(width) *width = rect.size.width;
  if(height) *height = rect.size.height;
}

#define _GP_SET_WINDOW_CALLBACK(name, cb, data)\
  void gp_window_set_ ## name ## _callback(gp_window* window, gp_event_ ## name ## _callback_t callback, gp_pointer* userData)\
  {\
    if(window->data) gp_object_unref((gp_object*)window->data);\
    \
    window->cb = callback;\
    window->data = userData;\
    \
    if(window->data) gp_object_ref((gp_object*)window->data);\
  }

_GP_SET_WINDOW_CALLBACK(click, mClickCB, mClickData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)