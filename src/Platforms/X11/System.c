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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/X11.h>
#include "X11.h"
#include "API/GL/GL.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <stdlib.h>
#include <stdio.h>

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  system->mDisplay = XOpenDisplay(0);
  
  return system;
}

void gp_system_free(gp_system* system)
{
  free(system);
}

gp_context* gp_system_context_new(gp_system* system)
{
  gp_context* context = (gp_context*)malloc(sizeof(struct _gp_context));
  context->mParent = system;
  context->mDisplay = system->mDisplay;
  
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
  GLXFBConfig *fbc = glXChooseFBConfig(context->mDisplay, DefaultScreen(context->mDisplay), attrList, &fbcount );
  if ( !fbc )
  {
//     cerr << "Failed to retrieve a framebuffer config" << endl;
//     return;
  }
  int samp_buf, samples, best_fbc;
  for(int i=0; i<fbcount; i++)
  {
    XVisualInfo *vi;
    vi = glXGetVisualFromFBConfig(context->mDisplay, fbc[i] );
    if (vi)
    {
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLES       , &samples  );
      if(samp_buf > 0 && samples > 4)
      {
        best_fbc = i;
        context->mVisualInfo = vi;
        break;
      }
    }
  }
  XFree(fbc);
  
  context->mColorMap = XCreateColormap(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen), context->mVisualInfo->visual, AllocNone);
  
  context->mShare = glXCreateContext(context->mDisplay, context->mVisualInfo, NULL, True);
  
  // NOTE: Context needs window first time it is made current.
  glXMakeCurrent(context->mDisplay, context->mWindow, context->mShare);
  
  // NOTE: GL functions need a context to be bound to get information from
  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  gp_log_info("OpenGL Version: %d.%d", major, minor);
  gp_log_info("Direct Rendering: %s", ((glXIsDirect(context->mDisplay, context->mShare)) ? "YES" : "NO"));
  
  _gp_api_init();
  
  return context;
}

void gp_system_run(gp_system* system)
{
  while(1)
  {
    XEvent event;
    
    while(XPending(system->mDisplay))
    {
      XNextEvent(system->mDisplay, &event);
      
      if(event.type == Expose)
      {
        _gp_target_draw(system->mTarget);
      }
    }
  }
}
