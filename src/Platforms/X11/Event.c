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

struct _gp_timer
{
  gp_list_node            mNode;
  _gp_event*              mEvent;
  int                     mFD;
  gp_timer_callback       mCallback;
  void*                   mUserData;
};

struct _gp_io
{
  gp_list_node            mNode;
  _gp_event*              mEvent;
  int                     mFD;
  gp_io_callback          mCallback;
  void*                   mUserData;
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
      while(node != NULL)
      {
        gp_io* io = (gp_io*)node;
        if(FD_ISSET(io->mFD, &readfds))
        {
          io->mCallback(io);
        }
        node = gp_list_node_next(node);
      }
      
      node = gp_list_front(&event->mIOWrite);
      while(node != NULL)
      {
        gp_io* io = (gp_io*)node;
        if(FD_ISSET(io->mFD, &writefds))
        {
          io->mCallback(io);
        }
        node = gp_list_node_next(node);
      }
      
      node = gp_list_front(&event->mTimer);
      while(node != NULL)
      {
        gp_timer* timer = (gp_timer*)node;
        if(FD_ISSET(timer->mFD, &readfds))
        {
          uint64_t buff;
          read(timer->mFD, &buff, sizeof(uint64_t));
          timer->mCallback(timer);
        }
        node = gp_list_node_next(node);
      }
    }
  }
}

void _gp_event_wake(_gp_event* event)
{
  if(write(event->mPipe[1], "x", 1) == -1)
  {
    gp_log_error("Unable to wake event pipe.");
  }
}

gp_timer* _gp_event_timer_new(_gp_event* event)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  timer->mEvent = event;
  timer->mFD = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  timer->mCallback = NULL;
  
  _gp_event_fd_set(timer->mFD, &event->mReadFDs, &event->mReadFDMax);
  
  gp_list_push_back(&event->mTimer, (gp_list_node*)timer);
  
  return timer;
}

void gp_timer_free(gp_timer* timer)
{
  _gp_event_fd_clr(timer->mFD, &timer->mEvent->mReadFDs, &timer->mEvent->mReadFDMax);
  
  gp_list_remove(&timer->mEvent->mTimer, (gp_list_node*)timer);
  
  free(timer);
}

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback)
{
  timer->mCallback = callback;
}

void gp_timer_set_userdata(gp_timer* timer, void* userdata)
{
  timer->mUserData = userdata;
}

void* gp_timer_get_userdata(gp_timer* timer)
{
  return timer->mUserData;
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
  struct itimerspec it;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;
  it.it_value.tv_sec = timeout;
  it.it_value.tv_nsec = (timeout-((int)timeout))*1000000000;
  
  timerfd_settime(timer->mFD, 0, &it, NULL);
}

void gp_timer_disarm(gp_timer* timer)
{
  struct itimerspec it;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;
  it.it_value.tv_sec = 0;
  it.it_value.tv_nsec = 0;
  
  timerfd_settime(timer->mFD, 0, &it, NULL);
}

gp_io* _gp_event_io_read_new(_gp_event* event, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mEvent = event;
  io->mFD = fd;
  
  _gp_event_fd_set(fd, &event->mReadFDs, &event->mReadFDMax);
  
  gp_list_push_back(&event->mIORead, (gp_list_node*)io);
  
  return io;
}

gp_io* _gp_event_io_write_new(_gp_event* event, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mEvent = event;
  io->mFD = fd;
  
  _gp_event_fd_set(fd, &event->mWriteFDs, &event->mWriteFDMax);
  
  gp_list_push_back(&event->mIOWrite, (gp_list_node*)io);
  
  return io;
}

void gp_io_free(gp_io* io)
{
  _gp_event_fd_clr(io->mFD, &io->mEvent->mReadFDs, &io->mEvent->mReadFDMax);
  _gp_event_fd_clr(io->mFD, &io->mEvent->mWriteFDs, &io->mEvent->mWriteFDMax);
  
  gp_list_remove(&io->mEvent->mIORead, (gp_list_node*)io);
  gp_list_remove(&io->mEvent->mIOWrite, (gp_list_node*)io);
  
  free(io);
}

void gp_io_set_callback(gp_io* io, gp_io_callback callback)
{
  io->mCallback = callback;
}

void gp_io_set_userdata(gp_io* io, void* userdata)
{
  io->mUserData = userdata;
}

void* gp_io_get_userdata(gp_io* io)
{
  return io->mUserData;
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
