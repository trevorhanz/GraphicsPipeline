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

#include "X11.h"

#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "../../API/GL/GL.h"
#include "Platforms/Defaults.h"

void _gp_target_wake_callback(gp_io* io)
{
  gp_target* target = (gp_target*)gp_io_get_userdata(io);
  for(;;)
  {
    char ch;
    if(read(target->mPipe[0], &ch, 1) == -1)
      break;
  }
  
  if(target->mDirty)
  {
    _gp_target_draw(target);
    target->mDirty = 0;
  }
}

gp_context* gp_context_new(gp_system* system)
{
  assert(system != NULL);
  
  gp_context* context = (gp_context*)malloc(sizeof(struct _gp_context));
  context->mParent = system;
  context->mDisplay = system->mDisplay;
  gp_ref_init(&context->mRef);
  
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

void gp_context_ref(gp_context* context)
{
  gp_ref_inc(&context->mRef);
}

void gp_context_unref(gp_context* context)
{
  if(gp_ref_dec(&context->mRef))
  {
    glXDestroyContext(context->mDisplay, context->mShare);
    XFreeColormap(context->mDisplay, context->mColorMap);
    XFree(context->mVisualInfo);
    free(context);
  }
}

gp_target* gp_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(struct _gp_target));
  target->mParent = context;
  target->mPipeline = _gp_pipeline_new();
  target->mDirty = 1;
  gp_ref_init(&target->mRef);
  gp_list_push_back(&context->mParent->mTargets, (gp_list_node*)target);
  _gp_event_pipe_new(context->mParent->mEvent, target->mPipe);
  
  target->mWake = gp_io_read_new(context->mParent, target->mPipe[0]);
  gp_io_set_callback(target->mWake, _gp_target_wake_callback);
  gp_io_set_userdata(target->mWake, target);
  
  XSetWindowAttributes attr;
  attr.colormap = context->mColorMap;
  attr.border_pixel = 0;
  attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
  StructureNotifyMask | DestroyNotify | PointerMotionMask | FocusChangeMask;
  
  target->mWindow = XCreateWindow(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen),
                                  0, 0, GP_DEFAULT_WINDOW_WIDTH, GP_DEFAULT_WINDOW_HEIGHT,
                                  0, context->mVisualInfo->depth, InputOutput, context->mVisualInfo->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &attr);
  
  target->mContext = glXCreateContext(context->mDisplay, context->mVisualInfo, context->mShare, True);
  XStoreName(context->mDisplay, target->mWindow, "GraphicsPipeline");
  XFlush(context->mDisplay);
  glXMakeCurrent(context->mDisplay, target->mWindow, target->mContext);
  XFlush(context->mDisplay);
  XMapRaised(context->mDisplay, target->mWindow);
  XFlush(context->mDisplay);
  
  XSetWMProtocols(context->mDisplay, target->mWindow, &context->mParent->mDeleteMessage, 1);
  
  return target;
}

void gp_target_ref(gp_target* target)
{
  gp_ref_inc(&target->mRef);
}

void gp_target_unref(gp_target* target)
{
  if(gp_ref_dec(&target->mRef))
  {
    glXDestroyContext(target->mParent->mDisplay, target->mContext);
    
    gp_list_remove(&target->mParent->mParent->mTargets, (gp_list_node*)target);
    _gp_pipeline_free(target->mPipeline);
    
    gp_io_free(target->mWake);
    
    free(target);
  }
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
  size_t r = write(target->mPipe[1], "x", 1);
  
  target->mDirty = 1;
}

void _gp_target_draw(gp_target* target)
{
  glXMakeCurrent(target->mParent->mDisplay, target->mWindow, target->mContext);
  
  _gp_pipeline_execute(target->mPipeline);
  
  glXSwapBuffers(target->mParent->mDisplay, target->mWindow);
  
  target->mDirty = 0;
}


