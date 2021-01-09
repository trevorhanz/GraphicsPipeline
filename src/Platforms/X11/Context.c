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

#include "../../API/GL/GL.h"

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

void gp_context_free(gp_context* context)
{
  XFreeColormap(context->mDisplay, context->mColorMap);
  free(context);
}

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(struct _gp_target));
  target->mParent = context;
  target->mPipeline = malloc(sizeof(struct _gp_pipeline));
  target->mPipeline->mOperations = NULL;
  target->mDirty = 1;
  context->mParent->mTarget = target;
  _gp_event_pipe_new(context->mParent->mEvent, target->mPipe);
  
  target->mWake = gp_system_io_read_new(context->mParent, target->mPipe[0]);
  gp_io_set_callback(target->mWake, _gp_target_wake_callback);
  gp_io_set_userdata(target->mWake, target);
  
  XSetWindowAttributes attr;
  attr.colormap = context->mColorMap;
  attr.border_pixel = 0;
  attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
  StructureNotifyMask | DestroyNotify | PointerMotionMask | FocusChangeMask;
  
  target->mWindow = XCreateWindow(context->mDisplay, RootWindow(context->mDisplay, context->mVisualInfo->screen),
                                  0, 0, 1024, 768, 0, context->mVisualInfo->depth, InputOutput, context->mVisualInfo->visual,
                                  CWBorderPixel | CWColormap | CWEventMask, &attr);
  
  target->mContext = glXCreateContext(context->mDisplay, context->mVisualInfo, context->mShare, True);
  XStoreName(context->mDisplay, target->mWindow, "GraphicsPipeline");
  XFlush(context->mDisplay);
  glXMakeCurrent(context->mDisplay, target->mWindow, target->mContext);
  XFlush(context->mDisplay);
  XMapRaised(context->mDisplay, target->mWindow);
  XFlush(context->mDisplay);
  
  return target;
}

gp_array* gp_context_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(struct _gp_array));
  
  glGenBuffers(1, &array->mVBO);
  
  return array;
}

gp_shader* gp_context_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(struct _gp_shader));
  
  return shader;
}

gp_pipeline* gp_target_get_pipeline(gp_target* target)
{
  return target->mPipeline;
}

void gp_target_redraw(gp_target* target)
{
  write(target->mPipe[1], "x", 1);
  
  target->mDirty = 1;
}

void _gp_target_draw(gp_target* target)
{
  glXMakeCurrent(target->mParent->mDisplay, target->mWindow, target->mContext);
  
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  _gp_pipeline_execute(target->mPipeline);
  
  glXSwapBuffers(target->mParent->mDisplay, target->mWindow);
  
  target->mDirty = 0;
}

