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

gp_target* gp_context_target_new(gp_context* context)
{
  gp_target* target = malloc(sizeof(struct _gp_target));
  target->mParent = context;
  target->mPipeline = _gp_pipeline_new();
  target->mDirty = 1;
  gp_list_push_back(&context->mParent->mTargets, (gp_list_node*)target);
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

gp_array* gp_context_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(struct _gp_array));
  
  _gp_generate_array(array);
  
  return array;
}

gp_texture* gp_context_texture_new(gp_context* context)
{
  gp_texture* texture = malloc(sizeof(gp_texture));
  
  _gp_generate_texture(texture);
  
  return texture;
}

gp_shader* gp_context_shader_new(gp_context* context)
{
  gp_shader* shader = malloc(sizeof(struct _gp_shader));
  
  _gp_generate_shader(shader);
  
  return shader;
}

void gp_target_free(gp_target* target)
{
  glXDestroyContext(target->mParent->mDisplay, target->mContext);
  
  gp_list_remove(&target->mParent->mParent->mTargets, (gp_list_node*)target);
  _gp_pipeline_free(target->mPipeline);
  
  gp_io_free(target->mWake);
  
  free(target);
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


