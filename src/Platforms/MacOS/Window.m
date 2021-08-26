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
  
  if(window->mClickData) gp_object_unref((gp_object*)window->mClickData);
  if(window->mScrollData) gp_object_unref((gp_object*)window->mScrollData);
  if(window->mTrackData) gp_object_unref((gp_object*)window->mTrackData);
  if(window->mEnterData) gp_object_unref((gp_object*)window->mEnterData);
  if(window->mKeyData) gp_object_unref((gp_object*)window->mKeyData);
  if(window->mResizeData) gp_object_unref((gp_object*)window->mResizeData);
  if(window->mMoveData) gp_object_unref((gp_object*)window->mMoveData);
  
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
  window->mScrollCB = NULL;
  window->mScrollData = NULL;
  window->mTrackCB = NULL;
  window->mTrackData = NULL;
  window->mEnterCB = NULL;
  window->mEnterData = NULL;
  window->mKeyCB = NULL;
  window->mKeyData = NULL;
  window->mResizeCB = NULL;
  window->mResizeData = NULL;
  window->mMoveCB = NULL;
  window->mMoveData = NULL;
  
  NSUInteger windowStyle = NSWindowStyleMaskTitled |
                           NSWindowStyleMaskClosable |
                           NSWindowStyleMaskResizable |
                           NSWindowStyleMaskMiniaturizable;
  window->mWindow = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT)
  styleMask:windowStyle backing:NSBackingStoreBuffered defer:NO]
  autorelease];
  [window->mWindow cascadeTopLeftFromPoint:NSMakePoint(20,20)];
  [window->mWindow setTitle:@GP_DEFAULT_WINDOW_TITLE];
  
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
  //[window->mWindow makeKeyAndOrderFront:window->mWindow];
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

void gp_window_set_title(gp_window* window, const char* title)
{
  [window->mWindow setTitle:[NSString stringWithCString:title encoding:NSUTF8StringEncoding]];
}

void gp_window_set_type(gp_window* window, GP_WINDOW_TYPE type)
{
  switch(type)
  {
    case GP_WINDOW_TYPE_NORMAL:
      window->mWindow.styleMask |= NSWindowStyleMaskTitled;
      window->mWindow.styleMask &= ~NSWindowStyleMaskBorderless;
      break;
    case GP_WINDOW_TYPE_UTILITY:
      window->mWindow.styleMask &= ~NSWindowStyleMaskTitled;
      window->mWindow.styleMask |= NSWindowStyleMaskBorderless;
      break;
  }
}

void gp_window_set_min_size(gp_window* window, int width, int height)
{
  NSSize size = [window->mWindow minSize];
  size.width = width;
  size.height = height;
  [window->mWindow setMinSize:size];
}

void gp_window_set_max_size(gp_window* window, int width, int height)
{
  NSSize size = [window->mWindow maxSize];
  size.width = width;
  size.height = height;
  [window->mWindow setMaxSize:size];}

void gp_window_set_size(gp_window* window, unsigned int width, unsigned int height)
{
  NSRect frame = [window->mWindow frame];
  frame.size.width = width;
  frame.size.height = height;
  [window->mWindow setFrame: frame display: YES];
}

void gp_window_get_size(gp_window* window, unsigned int* width, unsigned int* height)
{
  NSRect rect = [window->mView frame];
  if(width) *width = rect.size.width;
  if(height) *height = rect.size.height;
}

void gp_window_set_position(gp_window* window, unsigned int x, unsigned int y)
{
  // TODO: Implement
}

void gp_window_get_position(gp_window* window, unsigned int* x, unsigned int* y)
{
  // TODO: Implement
}

void gp_window_show(gp_window* window)
{
  [window->mWindow setIsVisible:YES];
}

void gp_window_hide(gp_window* window)
{
  [window->mWindow setIsVisible:NO];
}

int gp_window_get_shown(gp_window* window)
{
  return [window->mWindow isVisible];
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
_GP_SET_WINDOW_CALLBACK(scroll, mScrollCB, mScrollData)
_GP_SET_WINDOW_CALLBACK(track, mTrackCB, mTrackData)
_GP_SET_WINDOW_CALLBACK(enter, mEnterCB, mEnterData)
_GP_SET_WINDOW_CALLBACK(key, mKeyCB, mKeyData)
_GP_SET_WINDOW_CALLBACK(resize, mResizeCB, mResizeData)
_GP_SET_WINDOW_CALLBACK(move, mMoveCB, mMoveData)
