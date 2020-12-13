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

#include <GraphicsPipeline/Qt5.h>

#include "Qt.h"

extern "C" gp_system* gp_system_new()
{
  static int argc = 1;
  static const char* argv[] = {"default"};
  
  return gp_qt_system_new(&argc, const_cast<char**>(argv));
}

extern "C" gp_system* gp_qt_system_new(int* argc, char** argv)
{
  gp_system* system = new gp_system();
  system->mApp = new QApplication(*argc, argv);
  
  return system;
}

extern "C" void gp_system_free(gp_system* system)
{
  delete system->mApp;
  delete system;
}

extern "C" gp_context* gp_system_context_new()
{
  return gp_qt_context_new();
}

extern "C" void gp_system_run(gp_system* system)
{
  auto ret = system->mApp->exec();
}

extern "C" gp_timer* gp_system_timer_new(gp_system* system)
{
  gp_timer* timer = new gp_timer();
  timer->mTimer = new QTimer();
  timer->mTimerCallback = new TimerCallback(timer);
  
  QObject::connect(timer->mTimer, SIGNAL(timeout(void)), timer->mTimerCallback, SLOT(Callback(void)));
  
  return timer;
}

extern "C" gp_io* gp_system_io_read_new(gp_system* system, int fd)
{
  gp_io* io = new gp_io();
  io->mSocketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
  io->mIOCallback = new IOCallback(io);
  
  QObject::connect(io->mSocketNotifier, SIGNAL(timeout(void)), io->mIOCallback, SLOT(Callback(void)));
  
  return io;
}

extern "C" gp_io* gp_system_io_write_new(gp_system* system, int fd)
{
  gp_io* io = new gp_io();
  io->mSocketNotifier = new QSocketNotifier(fd, QSocketNotifier::Write);
  io->mIOCallback = new IOCallback(io);
  
  QObject::connect(io->mSocketNotifier, SIGNAL(timeout(void)), io->mIOCallback, SLOT(Callback(void)));
  
  return io;
}
