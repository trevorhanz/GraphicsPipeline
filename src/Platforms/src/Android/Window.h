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

#ifndef __GP_ANDROID_WINDOW_H__
#define __GP_ANDROID_WINDOW_H__

#include <android/native_window.h>

#include "System.h"

namespace GP
{
  namespace Android
  {
    class Window : public GP::Window
    {
    public:
      Window(ANativeWindow* window);
    };
  };
};

#endif // __GP_ANDROID_WINDOW_H__
