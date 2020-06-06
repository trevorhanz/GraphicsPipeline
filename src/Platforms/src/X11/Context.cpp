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
#include "X11.h"

#include <iostream>
#include <unistd.h>

using namespace GP::GL;
using namespace std;

Context::Context(Display* display, XVisualInfo* vi, ::Window window)
  : mDisplay(display),
  mVisualInfo(NULL),
  mWindow(window)
{
  // Get a matching FB config
  static int attrList[] =
  {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    GLX_SAMPLE_BUFFERS  , 1,
    GLX_SAMPLES         , 4,
    None
  };
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(mDisplay, DefaultScreen(mDisplay), attrList, &fbcount );
  if ( !fbc )
  {
    cerr << "Failed to retrieve a framebuffer config" << endl;
    return;
  }
  int samp_buf, samples, best_fbc;
  for(int i=0; i<fbcount; i++)
  {
    XVisualInfo *vi;
    vi = glXGetVisualFromFBConfig(mDisplay, fbc[i] );
    if (vi)
    {
      glXGetFBConfigAttrib(mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib(mDisplay, fbc[i], GLX_SAMPLES       , &samples  );
      if(samp_buf > 0 && samples > 4)
      {
        best_fbc = i;
        mVisualInfo = vi;
        break;
      }
    }
  }
  XFree(fbc);
  
  mColorMap = XCreateColormap(mDisplay, RootWindow(mDisplay, mVisualInfo->screen), mVisualInfo->visual, AllocNone);
  
  mShare = glXCreateContext(mDisplay, mVisualInfo, NULL, True);
  
  // NOTE: Context needs window first time it is made current.
  glXMakeCurrent(mDisplay, mWindow, mShare);
  
  // NOTE: GLEW must be initialize with an active OpenGL context
  glewInit();
  
  // NOTE: GL functions need a context to be bound to get information from
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  cout << "OpenGL Version: " << major << "." << minor << endl;
  
  cout << "Direct Rendering: " << ((glXIsDirect(mDisplay, mShare)) ? "YES" : "NO") << endl << endl;
}

Context::~Context()
{
  XFreeColormap(mDisplay, mColorMap);
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<PipelineBase>(this);
}

TargetUserDataPtr Context::CreateTarget()
{
  XSetWindowAttributes attr;
  attr.colormap = mColorMap;
  attr.border_pixel = 0;
  attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
  StructureNotifyMask | DestroyNotify | PointerMotionMask | FocusChangeMask;
  
  auto s = DefaultScreen(mDisplay);
  ::Window window = XCreateWindow(mDisplay, RootWindow(mDisplay, mVisualInfo->screen),
                          0, 0, 1024, 768, 0, mVisualInfo->depth, InputOutput, mVisualInfo->visual,
                          CWBorderPixel | CWColormap | CWEventMask, &attr);
  
  GP::TargetUserDataPtr ptr = std::make_shared<GP::TargetUserData>();
  ptr->mDisplay = mDisplay;
  ptr->mWindow = window;
  ptr->mContext = glXCreateContext(mDisplay, mVisualInfo, mShare, True);
  XStoreName(mDisplay, ptr->mWindow, "GraphicsPipeline");
  XFlush(mDisplay);
  glXMakeCurrent(mDisplay, ptr->mWindow, ptr->mContext);
  XFlush(mDisplay);
  XMapRaised(ptr->mDisplay, ptr->mWindow);
  XFlush(mDisplay);
  sleep(1);
  
  return ptr;
}


