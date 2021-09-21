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

//! \file Common.h

#ifndef __GP_COMMON_H__
#define __GP_COMMON_H__

#ifdef _WIN64
	#ifndef GP_STATIC
		#ifdef GraphicsPipeline_EXPORTS
			#define GP_EXPORT __declspec(dllexport)
		#else
			#define GP_EXPORT __declspec(dllimport)
		#endif // GraphicsPipeline_EXPORTS
	#else
		#define GP_EXPORT
	#endif // GP_STATIC
#else
	#define GP_EXPORT
#endif // _WIN32



#endif // __GP_COMMON_H__
