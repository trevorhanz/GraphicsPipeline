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

#include <System.h>
#include "Context.h"
#include "X11.h"

#include <GL/glew.h>
#include <GL/glx.h>
// #include <X11/extensions/Xrandr.h>

#include <iostream>

using namespace GP;
using namespace std;

class System::Data
{
public:
  Display*                                  mDisplay;
  XVisualInfo*                              mVisualInfo;
  
  ::Window CreateWindow()
  {
    XSetWindowAttributes attr;
    Colormap cmap = XCreateColormap(mDisplay, RootWindow(mDisplay, mVisualInfo->screen), mVisualInfo->visual, AllocNone);
    attr.colormap = cmap;
    attr.border_pixel = 0;
    attr.event_mask = StructureNotifyMask;
    attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
  StructureNotifyMask | DestroyNotify | PointerMotionMask | FocusChangeMask;
    
    auto s = DefaultScreen(mDisplay);
    ::Window window = XCreateWindow(mDisplay, RootWindow(mDisplay, mVisualInfo->screen),
                            0, 0, 1024, 768, 0, mVisualInfo->depth, InputOutput, mVisualInfo->visual,
                            CWBorderPixel | CWColormap | CWEventMask, &attr);
    
    XFreeColormap(mDisplay, cmap);
    
    return window;
  }
};

System::System()
  : mData(new Data())
{
  mData->mDisplay = XOpenDisplay(0);
  
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
  GLXFBConfig *fbc = glXChooseFBConfig(mData->mDisplay, DefaultScreen(mData->mDisplay), attrList, &fbcount );
  if ( !fbc )
  {
    cerr << "Failed to retrieve a framebuffer config" << endl;
    return;
  }
  int samp_buf, samples, best_fbc;
  XVisualInfo* visualInfo = NULL;
  for(int i=0; i<fbcount; i++)
  {
    XVisualInfo *vi;
    vi = glXGetVisualFromFBConfig(mData->mDisplay, fbc[i] );
    if (vi)
    {
      glXGetFBConfigAttrib(mData->mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib(mData->mDisplay, fbc[i], GLX_SAMPLES       , &samples  );
      if(samp_buf > 0 && samples > 4)
      {
        best_fbc = i;
        visualInfo = vi;
        mData->mVisualInfo = vi;
        break;
      }
    }
  }
//   XFree(fbc);
}

System::~System()
{
  delete mData;
}

WindowPtr System::CreateWindow(const std::string& title, int width, int height)
{
  auto data = std::make_shared<WindowUserData>();
  return std::make_shared<X11::Window>();
}

ContextPtr System::CreateContext()
{
  return std::make_shared<GL::Context>(mData->mDisplay, mData->mVisualInfo, mData->CreateWindow());
}

void System::Poll()
{
  XEvent event;
  
  while(XPending(mData->mDisplay))
  {
    XNextEvent(mData->mDisplay, &event);
    
    if(event.type == Expose)
    {
      printf("Expose\n");
    }
  }
}

void System::Run()
{
  while(true)
  {
    Poll();
  }
}
