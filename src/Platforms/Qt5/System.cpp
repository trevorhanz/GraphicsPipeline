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

void _gp_system_free(gp_object* object)
{
  gp_system* system = (gp_system*)object;
  
  if(system->mManageApp)
    delete system->mApp;
  delete system;
}

extern "C" gp_system* gp_system_new()
{
  static int argc = 1;
  static const char* argv[] = {"default"};
  
  return gp_qt_system_new(&argc, const_cast<char**>(argv));
}

extern "C" gp_system* gp_qt_system_new(int* argc, char** argv)
{
  gp_system* system = new gp_system();
  _gp_object_init(&system->mObject, _gp_system_free);
  auto app = QCoreApplication::instance();
  if(app)
  {
    system->mApp = app;
    system->mManageApp = false;
  }
  else
  {
    system->mApp = new QApplication(*argc, argv);
    system->mManageApp = true;
  }
  
  return system;
}

gp_monitor_list* gp_monitor_list_new(gp_system* system)
{
  gp_monitor_list* monitors = new gp_monitor_list();
  _gp_object_init(&monitors->mObject, (gp_object_free)free);
  
  return monitors;
}

int gp_monitor_list_get_count(gp_monitor_list* monitors)
{
  return qApp->screens().size();
}

void _gp_monitor_free(gp_object* object)
{
  delete object;
}

gp_monitor* gp_monitor_list_get_by_index(gp_monitor_list* monitors, int index)
{
  gp_monitor* monitor = new gp_monitor();
  _gp_object_init(&monitor->mObject, _gp_monitor_free);
  
  monitor->mScreen = qApp->screens()[index];
  
  return monitor;
}

gp_monitor* gp_monitor_list_get_at_position(gp_monitor_list* monitors, int x, int y)
{
  gp_monitor* monitor = new gp_monitor();
  _gp_object_init(&monitor->mObject, _gp_monitor_free);
  
#if QT_VERSION >= 0x051000
  monitor->mScreen = qApp->screenAt(QPoint(x, y));
#else
  QList<QScreen*> screens = qApp->screens();
  foreach(QScreen* screen, screens)
  {
    if(screen->geometry().contains(x, y))
    {
      monitor->mScreen = screen;
      break;
    }
  }
#endif
  
  return monitor;
}

gp_monitor* gp_monitor_list_get_primary(gp_monitor_list* monitors)
{
  return NULL;
}

gp_point gp_monitor_get_position(gp_monitor* monitor)
{
  QRect geometry = monitor->mScreen->geometry();
  gp_point point;
  point.x = geometry.left();
  point.y = geometry.top();
  return point;
}

gp_size gp_monitor_get_size(gp_monitor* monitor)
{
  gp_size size;
  size.width = monitor->mScreen->size().width();
  size.height = monitor->mScreen->size().height();
  return size;
}

gp_rect gp_monitor_get_rect(gp_monitor* monitor)
{
  QRect geometry = monitor->mScreen->geometry();
  gp_rect rect;
  rect.size.width = geometry.width();
  rect.size.height = geometry.height();
  rect.point.x = geometry.left();
  rect.point.y = geometry.top();
  return rect;
}

extern "C" void gp_system_run(gp_system* system)
{
  auto ret = system->mApp->exec();
}

extern "C" void gp_system_stop(gp_system* system)
{
  system->mApp->quit();
}

void _gp_timer_free(gp_object* object)
{
  gp_timer* timer = (gp_timer*)object;
  
  if(timer->mUserData)
  {
    gp_object_unref((gp_object*)timer->mUserData);
  }
  
  delete timer->mTimer;
  delete timer->mTimerCallback;
  delete timer;
}

extern "C" gp_timer* gp_timer_new(gp_system* system)
{
  gp_timer* timer = new gp_timer();
  _gp_object_init(&timer->mObject, _gp_timer_free);
  timer->mTimer = new QTimer();
  timer->mTimerCallback = new TimerCallback(timer);
  timer->mUserData = NULL;
  timer->mTimer->setSingleShot(true);
  
  QObject::connect(timer->mTimer, SIGNAL(timeout(void)), timer->mTimerCallback, SLOT(Callback(void)));
  
  return timer;
}

void _gp_io_free(gp_object* object)
{
  gp_io* io = (gp_io*)object;
  
  if(io->mUserData)
  {
    gp_object_unref((gp_object*)io->mUserData);
  }
  
  delete io->mSocketNotifier;
  delete io->mIOCallback;
  delete io;
}

extern "C" gp_io* gp_io_read_new(gp_system* system, int fd)
{
  gp_io* io = new gp_io();
  _gp_object_init(&io->mObject, _gp_io_free);
  io->mSocketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read);
  io->mIOCallback = new IOCallback(io);
  io->mUserData = NULL;
  
  QObject::connect(io->mSocketNotifier, SIGNAL(activated(int)), io->mIOCallback, SLOT(Callback(void)));
  
  return io;
}

extern "C" gp_io* gp_io_write_new(gp_system* system, int fd)
{
  gp_io* io = new gp_io();
  _gp_object_init(&io->mObject, _gp_io_free);
  io->mSocketNotifier = new QSocketNotifier(fd, QSocketNotifier::Write);
  io->mIOCallback = new IOCallback(io);
  io->mUserData = NULL;
  
  QObject::connect(io->mSocketNotifier, SIGNAL(timeout(void)), io->mIOCallback, SLOT(Callback(void)));
  
  return io;
}
