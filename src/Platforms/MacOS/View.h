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

#ifndef __VIEW_H__
#define __VIEW_H__

// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#import <Cocoa/Cocoa.h>

#include <GraphicsPipeline/Types.h>

@interface View : NSOpenGLView
{
  gp_target*          mTarget;
}

- (id) initWithFrame:(NSRect)frame;
- (void) reshape;
- (void) drawRect:(NSRect)rect;
- (void) update;
- (void) setTarget:(gp_target*)target;
@end

#endif // __VIEW_H__
