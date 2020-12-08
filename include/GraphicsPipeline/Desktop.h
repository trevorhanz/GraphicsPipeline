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

//! \file Desktop.h

#ifndef __GP_DESKTOP_H__
#define __GP_DESKTOP_H__

#include "System.h"

#ifdef GP_WINDOWS
#include "Windows.h"
#endif

#ifdef GP_LINUX
#include "X11.h"
#endif

#ifdef GP_MACOS
#include "MacOS.h"
#endif

#endif // __GP_DESKTOP_H__
