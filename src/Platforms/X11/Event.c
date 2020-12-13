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

#include <assert.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef max
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

typedef struct __gp_event_link _gp_event_link; 
struct __gp_event_link
{
  _gp_event_link*         mNext;
  void*                   mData;
};

struct __gp_event
{
  fd_set                  mReadFDs;
  fd_set                  mWriteFDs;
  int                     mReadFDMax;
  int                     mWriteFDMax;
  unsigned int            mRunning;
  
  _gp_event_link*         mIORead;
  _gp_event_link*         mIOWrite;
  _gp_event_link*         mTimer;
};

struct _gp_timer
{
  int                     mFD;
  gp_timer_callback       mCallback;
  void*                   mUserData;
};

struct _gp_io
{
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
  
  event->mIORead = NULL;
  event->mIOWrite = NULL;
  event->mTimer = NULL;
  
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
      _gp_event_link* link = event->mIORead;
      while(link != NULL)
      {
        gp_io* io = link->mData;
        if(FD_ISSET(io->mFD, &readfds))
        {
          io->mCallback(io);
        }
        link = link->mNext;
      }
      
      link = event->mIOWrite;
      while(link != NULL)
      {
        gp_io* io = link->mData;
        if(FD_ISSET(io->mFD, &writefds))
        {
          io->mCallback(io);
        }
        link = link->mNext;
      }
      
      link = event->mTimer;
      while(link != NULL)
      {
        gp_timer* timer = link->mData;
        if(FD_ISSET(timer->mFD, &readfds))
        {
          uint64_t buff;
          read(timer->mFD, &buff, sizeof(uint64_t));
          timer->mCallback(timer);
        }
        link = link->mNext;
      }
    }
  }
}

gp_timer* _gp_event_timer_new(_gp_event* event)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  timer->mFD = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
  timer->mCallback = NULL;
  
  _gp_event_fd_set(timer->mFD, &event->mReadFDs, &event->mReadFDMax);
  
  _gp_event_link* link = malloc(sizeof(_gp_event_link));
  link->mNext = event->mTimer;
  link->mData = timer;
  event->mTimer = link;
  
  return timer;
}

void gp_timer_free(gp_timer* timer)
{
  free(timer);
}

void gp_timer_set_callback(gp_timer* timer, gp_timer_callback callback)
{
  timer->mCallback = callback;
}

void gp_timer_set_userdatak(gp_timer* timer, void* userdata)
{
  timer->mUserData = userdata;
}

void* gp_timer_get_userdatak(gp_timer* timer)
{
  return timer->mUserData;
}

void gp_timer_arm(gp_timer* timer, double timeout)
{
  struct itimerspec it;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_nsec = 0;
  it.it_value.tv_sec = timeout;
  it.it_value.tv_nsec = (timeout-((int)timeout))*100000000;
  
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
  io->mFD = fd;
  
  _gp_event_fd_set(fd, &event->mReadFDs, &event->mReadFDMax);
  
  _gp_event_link* link = malloc(sizeof(_gp_event_link));
  link->mNext = event->mIORead;
  link->mData = io;
  event->mIORead = link;
  
  return io;
}

gp_io* _gp_event_io_write_new(_gp_event* event, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  io->mFD = fd;
  
  _gp_event_fd_set(fd, &event->mWriteFDs, &event->mWriteFDMax);
  
  _gp_event_link* link = malloc(sizeof(_gp_event_link));
  link->mNext = event->mIOWrite;
  link->mData = io;
  event->mIOWrite = link;
  
  return io;
}

void gp_io_free(gp_io* io)
{
  // TODO - clear fd
  
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
