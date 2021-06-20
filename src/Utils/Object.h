/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#ifndef __GP_UTILS_OBJECT_H__
#define __GP_UTILS_OBJECT_H__

#include <GraphicsPipeline/Object.h>
#include "RefCounter.h"

typedef void(*gp_object_free)(gp_object*);

struct _gp_object
{
  gp_refcounter           mRef;
  gp_object_free          mFree;
};

struct _gp_pointer
{
  gp_object               mObject;
  void*                   mPointer;
  gp_free                 mFree;
};

#ifdef __cplusplus
extern "C" {
#endif

void _gp_object_init(gp_object* object, gp_object_free func);

#ifdef __cplusplus
}
#endif

#endif // __GP_UTILS_OBJECT_H__
