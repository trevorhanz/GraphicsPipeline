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

//! \file Android.h

#ifndef __GP_ANDROID_H__
#define __GP_ANDROID_H__

#include <android/native_window.h>

#include "Types.h"

/*!
 * \defgroup Android
 * \{
 */

/*!
 * Creates a new window object from an Android ANativeWindow.
 * \param context Pointer to context for which the window will be tied.
 * \param window ANativeWindow for which the window will be created.
 * \return Pointer to new window object.
 */
gp_window* gp_window_new_from_native(gp_context* context, ANativeWindow* window);

//! \} // Android

#endif // __GP_ANDROID_H__
