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
#include "../../Utils/List.h"
#include "../../Utils/Object.h"

#include <assert.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef max
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

#define GP_IO_READ      0x01
#define GP_IO_WRITE     0x02

struct __gp_event
{
  fd_set                  mReadFDs;
  fd_set                  mWriteFDs;
  int                     mReadFDMax;
  int                     mWriteFDMax;
  unsigned int            mRunning;
  int                     mPipe[2];
  
  gp_list                 mIORead;
  gp_list                 mIOWrite;
  gp_list                 mTimer;
};

typedef struct
{
  gp_list_node            mNode;
  _gp_event*              mEvent;
  int                     mFD;
  gp_timer_callback       mCallback;
  gp_pointer*             mUserData;
} _gp_timer_data;

struct _gp_timer
{
  gp_object               mObject;
  _gp_timer_data          mTimer;
};

struct _gp_io
{
  gp_object               mObject;
  gp_list_node            mNode;
  _gp_event*              mEvent;
  int                     mFD;
  gp_io_callback          mCallback;
  gp_pointer*             mUserData;
  uint8_t                 mType;
};

void _gp_event_fd_set(int fd, fd_set* fds, int* max_fd) {
  assert(max_fd != NULL);

  FD_SET(fd, fds);
  if (fd > *max_fd) {
    *max_fd = fd;
  }
}

/* clear fd from fds, update max fd if needed */
void _gp_event_fd_clr(int fd, fd_set* fds, int* max_fd) {
  assert(max_fd != NULL);

  FD_CLR(fd, fds);
  if (fd == *max_fd) {
    (*max_fd)--;
  }
}

_gp_event* _gp_event_new()
{
  _gp_event* event = malloc(sizeof(_gp_event));
  FD_ZERO(&event->mReadFDs);
  FD_ZERO(&event->mWriteFDs);
  event->mReadFDMax = -1;
  event->mWriteFDMax = -1;
  event->mRunning = 0;
  
  gp_list_init(&event->mIORead);
  gp_list_init(&event->mIOWrite);
  gp_list_init(&event->mTimer);
  
  _gp_event_pipe_new(event, &event->mPipe[0]);
  _gp_event_fd_set(event->mPipe[0], &event->mReadFDs, &event->mReadFDMax);
  
  return event;
}

void _gp_event_free(_gp_event* event)
{
  free(event);
}

void _gp_event_run(_gp_event* event)
{
  event->mRunning = 1;
  
  while(event->mRunning)
  {
    fd_set readfds = event->mReadFDs;
    fd_set writefds = event->mWriteFDs;
    
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    
    if(select(max(event->mReadFDMax, event->mWriteFDMax)+1, &readfds, &writefds, NULL, &tv))
    {
      if(FD_ISSET(event->mPipe[0], &readfds))
      {
        for(;;)
        {
          char ch;
          if(read(event->mPipe[0], &ch, 1) == -1)
            break;
        }
      }
      
      gp_list_node* node = gp_list_front(&event->mIORead);
      while(node != gp_list_end(&event->mIORead))
      {
        gp_io* io = (gp_io*)GP_OBJECT_FROM_LIST_NODE(node);
        if(FD_ISSET(io->mFD, &readfds))
        {
          io->mCallback(io, io->mUserData);
        }
        node = gp_list_node_next(node);
      }
      
      node = gp_list_front(&event->mIOWrite);
      while(node != gp_list_end(&event->mIOWrite))
      {
        gp_io* io = (gp_io*)GP_OBJECT_FROM_LIST_NODE(node);
        if(FD_ISSET(io->mFD, &writefds))
        {
          io->mCallback(io, io->mUserData);
        }
        node = gp_list_node_next(node);
      }
      
      node = gp_list_front(&event->mTimer);
      while(node != gp_list_end(&event->mTimer))
      {
        _gp_timer_data* timer = (_gp_timer_data*)node;
        if(FD_ISSET(timer->mFD, &readfds))
        {
          uint64_t buff;
          size_t r = read(timer->mFD, &buff, sizeof(uint64_t));
          timer->mCallback((gp_timer*)GP_OBJECT_FROM_LIST_NODE(node), timer->mUserData);
        }
        node = gp_list_node_next(node);
      }
    }
  }
}

void _gp_event_stop(_gp_event* event)
{
  event->mRunning = 0;
  
  _gp_event_wake(event);
}

void _gp_event_wake(_gp_event* event)
{
  if(write(event->mPipe[1], "x", 1) == -1)
  {
    gp_log_error("Unable to wake event pipe.");
  }
}

void _gp_timer_free(gp_object* object)
{
  gp_timer* timer = (gp_timer*)object;
  
  _gp_event_fd_clr(timer->mTimer.mFD, &timer->mTimer.mEvent->mReadFDs, &timer->mTimer.mEvent->mReadFDMax);
  
  gp_list_remove(&timer->mTimer.mEvent->mTimer, (gp_list_node*)&timer->mTimer);
  
  if(timer->mTimer.mUserData)
  {
    gp_object_unref((gp_object*)timer->mTimer.mUserData);
  }
  
  free(timer);
}

gp_timer* _gp_event_timer_new(_gp_event* event)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  _gp_object_init(&timer->mObject, _gp_timer_free);
  timer->mTimer.mEvent = event;
  timer->mTimer.mFD = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  timer->mTimer.mCallback = NULL;
  timer->mTimer.mUserData = NULL;
  
  _gp_event_fd_set(timer->mTimer.mFD, &event->mReadFDs, &event->mReadFDMax);
  
  gp_list_push_back(&event->mTimer, (gp_list_node*)&timer->mTimer);
  
  return timer;
}

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback, gp_pointer* userdata)
{
  timer->mTimer.mCallback = callback;
  
  if(timer->mTimer.mUserData)
  {
    gp_object_unref((gp_object*)timer->mTimer.mUserData);
  }
  
  timer->mTimer.mUserData = userdata;
  
  if(timer->mTimer.mUserData)
  {
    gp_object_ref((gp_object*)timer->mTimer.mUserData);
  }
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
  // Set timeout above 0 to prevent disarming the timer.
  if(timeout <= 0) timeout = .0000001;
  
  struct itimerspec it;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;
  it.it_value.tv_sec = timeout;
  it.it_value.tv_nsec = (timeout-((int)timeout))*1000000000;
  
  timerfd_settime(timer->mTimer.mFD, 0, &it, NULL);
}

void gp_timer_disarm(gp_timer* timer)
{
  struct itimerspec it;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;
  it.it_value.tv_sec = 0;
  it.it_value.tv_nsec = 0;
  
  timerfd_settime(timer->mTimer.mFD, 0, &it, NULL);
}

void _gp_io_free(gp_object* object)
{
  gp_io* io = (gp_io*)object;
  
  _gp_event_fd_clr(io->mFD, &io->mEvent->mReadFDs, &io->mEvent->mReadFDMax);
  _gp_event_fd_clr(io->mFD, &io->mEvent->mWriteFDs, &io->mEvent->mWriteFDMax);
  
  if(io->mType == GP_IO_READ)
    gp_list_remove(&io->mEvent->mIORead, &io->mNode);
  else
    gp_list_remove(&io->mEvent->mIOWrite, &io->mNode);
  
  if(io->mUserData)
  {
    gp_object_unref((gp_object*)io->mUserData);
  }
  
  free(io);
}

gp_io* _gp_event_io_read_new(_gp_event* event, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  _gp_object_init(&io->mObject, _gp_io_free);
  io->mEvent = event;
  io->mFD = fd;
  io->mCallback = NULL;
  io->mUserData = 0;
  io->mType = GP_IO_READ;
  
  _gp_event_fd_set(fd, &event->mReadFDs, &event->mReadFDMax);
  
  gp_list_push_back(&event->mIORead, &io->mNode);
  
  return io;
}

gp_io* _gp_event_io_write_new(_gp_event* event, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  _gp_object_init(&io->mObject, _gp_io_free);
  io->mEvent = event;
  io->mFD = fd;
  io->mCallback = NULL;
  io->mUserData = 0;
  io->mType = GP_IO_WRITE;
  
  _gp_event_fd_set(fd, &event->mWriteFDs, &event->mWriteFDMax);
  
  gp_list_push_back(&event->mIOWrite, &io->mNode);
  
  return io;
}

void gp_io_set_callback(gp_io* io, gp_io_callback callback, gp_pointer* userdata)
{
  io->mCallback = callback;
  
  if(io->mUserData)
  {
    gp_object_unref((gp_object*)io->mUserData);
  }
  
  io->mUserData = userdata;
  
  if(io->mUserData)
  {
    gp_object_ref((gp_object*)io->mUserData);
  }
}

void _gp_event_pipe_new(_gp_event* event, int* fds)
{
  if(pipe(fds) == -1)
  {
    gp_log_error("Unable to create event pipe.");
    return;
  }
  int flags = fcntl(fds[0], F_GETFL);
  flags |= O_NONBLOCK;
  if(fcntl(fds[0], F_SETFL, flags) == -1)
  {
    gp_log_error("Unable to create non-blocking event pipe.");
    return;
  }
  
  flags = fcntl(fds[1], F_GETFL);
  flags |= O_NONBLOCK;
  if(fcntl(fds[1], F_SETFL, flags) == -1)
  {
    gp_log_error("Unable to create non-blocking event pipe.");
    return;
  }
}
