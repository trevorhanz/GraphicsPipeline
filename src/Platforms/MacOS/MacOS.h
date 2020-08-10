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

#ifndef __GP_MACOS_COMMON_H__
#define __GP_MACOS_COMMON_H__

// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/Window.h>
#include <GraphicsPipeline/System.h>
#include <API/GL/Pipeline.h>

#import <Cocoa/Cocoa.h>

#include "View.h"

namespace GP
{
  class WindowUserData : public UserData
  {
  public:
    TargetPtr         mTarget;
  };
  typedef std::shared_ptr<WindowUserData> WindowUserDataPtr;
  
  class TargetUserData : public GL::TargetUserData
  {
  public:
    NSWindow*         mWindow;
    View*             mView;
    
    void MakeCurrent() override {[[mView openGLContext] makeCurrentContext];}
    void Present() override {[[mView openGLContext] flushBuffer];}
  };
  typedef std::shared_ptr<TargetUserData> TargetUserDataPtr;
}

#endif // __MACOS_COMMON_H__
