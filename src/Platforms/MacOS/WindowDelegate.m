/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#import "WindowDelegate.h"

#include "MacOS.h"
#include <GraphicsPipeline/Window.h>
#include <GraphicsPipeline/Logging.h>

@implementation WindowDelegate

- (instancetype) initWithWindow:(gp_window *)window
{
  self = [super init];
  
  mWindow = window;
  
  return self;
}

- (void) windowDidResize:(NSNotification *)notification
{
  if(mWindow->mResizeCB)
  {
    NSRect rect = [mWindow->mView frame];
    
    gp_event_resize_t resize;
    resize.width = rect.size.width;
    resize.height = rect.size.height;
    mWindow->mResizeCB(&resize, mWindow->mResizeData);
  }
}

- (void) windowDidMove:(NSNotification *)notification
{
  if(mWindow->mMoveCB)
  {
    NSRect screen = [[NSScreen mainScreen] frame];
    NSRect rect = [mWindow->mView frame];
    NSPoint position = [mWindow->mWindow convertRectToScreen:rect].origin;
    
    gp_event_move_t move;
    move.x = position.x;
    move.y = screen.size.height-(position.y+rect.size.height);
    mWindow->mMoveCB(&move, mWindow->mMoveData);
  }
}

@end
