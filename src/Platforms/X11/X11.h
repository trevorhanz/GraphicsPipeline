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

#ifndef __GP_X11_COMMON_H__
#define __GP_X11_COMMON_H__

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Logging.h>

#include "../../Utils/List.h"

#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glx.h>

typedef struct __gp_event _gp_event;

struct _gp_system
{
  Display*                mDisplay;
  gp_list                 mTargets;
  _gp_event*              mEvent;
  Atom                    mDeleteMessage;
};

struct _gp_context
{
  gp_system*              mParent;
  Display*                mDisplay;
  XVisualInfo*            mVisualInfo;
  Window                  mWindow;
  Colormap                mColorMap;
  GLXContext              mShare;
};

struct _gp_target
{
  gp_list_node            mNode;
  gp_context*             mParent;
  gp_pipeline*            mPipeline;
  Window                  mWindow;
  GLXContext              mContext;
  uint8_t                 mDirty;
  int                     mPipe[2];
  gp_io*                  mWake;
};

void _gp_target_draw(gp_target* target);

_gp_event* _gp_event_new();

void _gp_event_free(_gp_event* event);

void _gp_event_run(_gp_event* event);

void _gp_event_stop(_gp_event* event);

void _gp_event_wake(_gp_event* event);

gp_timer* _gp_event_timer_new(_gp_event* event);

gp_io* _gp_event_io_read_new(_gp_event* event, int fd);

gp_io* _gp_event_io_write_new(_gp_event* event, int fd);

void _gp_event_pipe_new(_gp_event* event, int* fds);

#endif // __GP_X11_COMMON_H__
