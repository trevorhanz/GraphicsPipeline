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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/select.h>

gp_system* gp_system_new()
{
  gp_system* system = (gp_system*)malloc(sizeof(struct _gp_system));
  gp_list_init(&system->mTargets);
  system->mEvent = _gp_event_new();
  
  char* display = getenv("DISPLAY");
  system->mDisplay = XOpenDisplay(display);
  if(!system->mDisplay)
  {
    gp_log_error("Unable to connect to X11 server (%s).", (display)?display:":0");
    free(system);
    return NULL;
  }
  
  system->mDeleteMessage = XInternAtom(system->mDisplay, "WM_DELETE_WINDOW", False);
  
  return system;
}

void gp_system_free(gp_system* system)
{
  assert(system != NULL);
  
  XCloseDisplay(system->mDisplay);
  
  _gp_event_free(system->mEvent);
  free(system);
}

gp_context* gp_system_context_new(gp_system* system)
{
  assert(system != NULL);
  
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
    None
  };
  int fbcount;
  GLXFBConfig *fbc = glXChooseFBConfig(context->mDisplay, DefaultScreen(context->mDisplay), attrList, &fbcount );
  if ( !fbc )
  {
//     cerr << "Failed to retrieve a framebuffer config" << endl;
//     return;
  }
  int samp_buf, samples, best_samples;
  best_samples = -1;
  for(int i=0; i<fbcount; i++)
  {
    XVisualInfo *vi;
    vi = glXGetVisualFromFBConfig(context->mDisplay, fbc[i] );
    if (vi)
    {
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
      glXGetFBConfigAttrib(context->mDisplay, fbc[i], GLX_SAMPLES       , &samples  );
      if(samples > best_samples)
      {
        best_samples = samples;
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
  
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  gp_log_info("GLSL Version: %s", glslVersion);
  
  gp_log_info("Direct Rendering: %s", ((glXIsDirect(context->mDisplay, context->mShare)) ? "YES" : "NO"));
  
  _gp_api_init();
  
  return context;
}

void _gp_system_process_events(gp_io* io)
{
  gp_system* system = (gp_system*)gp_io_get_userdata(io);
  
  XEvent event;
  
  while(XPending(system->mDisplay))
  {
    XNextEvent(system->mDisplay, &event);
    
    if(event.type == Expose)
    {
      gp_list_node* node = gp_list_front(&system->mTargets);
      while(node != NULL)
      {
        gp_target* target = (gp_target*)node;
        if(target->mWindow == event.xexpose.window)
        {
          gp_target_redraw(target);
        }
        node = gp_list_node_next(node);
      }
    }
    
    if(event.type == ClientMessage)
    {
      if(event.xclient.data.l[0] == system->mDeleteMessage)
      {
        gp_system_stop(system);
      }
    }
  }
}

void gp_system_run(gp_system* system)
{
  assert(system != NULL);
  
  int fd = XConnectionNumber(system->mDisplay);
  
  gp_io* io = gp_system_io_read_new(system, fd);
  gp_io_set_callback(io, _gp_system_process_events);
  gp_io_set_userdata(io, (void*)system);
  
  _gp_event_run(system->mEvent);
  
  gp_io_free(io);
}

void gp_system_stop(gp_system* system)
{
  _gp_event_stop(system->mEvent);
}

gp_timer* gp_system_timer_new(gp_system* system)
{
  return _gp_event_timer_new(system->mEvent);
}

gp_io* gp_system_io_read_new(gp_system* system, int fd)
{
  return _gp_event_io_read_new(system->mEvent, fd);
}

gp_io* gp_system_io_write_new(gp_system* system, int fd)
{
  return _gp_event_io_write_new(system->mEvent, fd);
}


