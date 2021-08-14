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

- (BOOL) acceptsFirstResponder
{
  return YES;
}

- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
  return YES;
}

- (BOOL) canBecomeKeyView
{
  return YES;
}

- (id) initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat *)format
{
  self = [super initWithFrame:frame pixelFormat:format];
  
  return self;
}

- (void) reshape
{
  [super reshape];
}

- (void) drawRect:(NSRect)rect
{
  [super drawRect:rect];
  
  [[self openGLContext] makeCurrentContext];
  
  _gp_pipeline_execute(mWindow->mPipeline);
  
  [[self openGLContext] flushBuffer];
}

- (void) update
{
  [super update];
}

- (void) setWindow:(gp_window*)window
{
  mWindow = window;
}

- (void) mouseDown:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_LEFT :GP_PRESSED :[event locationInWindow]];
}

- (void) mouseUp:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_LEFT :GP_RELEASED :[event locationInWindow]];
}

- (void) rightMouseDown:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_RIGHT :GP_PRESSED :[event locationInWindow]];
}

- (void) rightMouseUp:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_RIGHT :GP_RELEASED :[event locationInWindow]];
}

- (void) otherMouseDown:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_MIDDLE :GP_PRESSED :[event locationInWindow]];
}

- (void) otherMouseUp:(NSEvent *)event
{
  [self handleMouseClick:GP_BUTTON_MIDDLE :GP_RELEASED :[event locationInWindow]];
}

- (void) mouseMoved:(NSEvent *)event
{
  if(mWindow->mTrackCB)
  {
    NSRect frame = [mWindow->mView frame];
    
    gp_event_track_t move;
    move.x = [event locationInWindow].x;
    move.y = frame.size.height - [event locationInWindow].y;
    mWindow->mTrackCB(&move, mWindow->mTrackData);
  }
}

- (void) mouseEntered:(NSEvent *)event
{
  if(mWindow->mEnterCB)
  {
    gp_event_enter_t enter;
    enter.enter = 1;
    mWindow->mEnterCB(&enter, mWindow->mEnterData);
  }
}

- (void) mouseExited:(NSEvent *)event
{
  if(mWindow->mEnterCB)
  {
    gp_event_enter_t enter;
    enter.enter = 0;
    mWindow->mEnterCB(&enter, mWindow->mEnterData);
  }
}

- (void) keyDown:(NSEvent *)event
{
  if(mWindow->mKeyCB)
  {
    // TODO: Create keyCode mapping
    gp_event_key_t key;
    key.key = event.keyCode;
    key.state = GP_PRESSED;
    mWindow->mKeyCB(&key, mWindow->mKeyData);
  }
}

- (void) keyUp:(NSEvent *)event
{
  if(mWindow->mKeyCB)
  {
    // TODO: Create keyCode mapping
    gp_event_key_t key;
    key.key = event.keyCode;
    key.state = GP_RELEASED;
    mWindow->mKeyCB(&key, mWindow->mKeyData);
  }
}

- (void) handleMouseClick:(gp_button_t)button :(gp_state_t)state :(NSPoint)position
{
  if(mWindow->mClickCB)
  {
    NSRect frame = [mWindow->mView frame];
    
    gp_event_click_t click;
    click.button = button;
    click.state = state;
    click.x = position.x;
    click.y = frame.size.height - position.y;
    mWindow->mClickCB(&click, mWindow->mClickData);
  }
}

@end
