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

#include "Object.h"

#include <stdlib.h>

void gp_object_ref(gp_object* object)
{
  gp_ref_inc(&object->mRef);
}

void gp_object_unref(gp_object* object)
{
  if(gp_ref_dec(&object->mRef))
  {
    object->mFree(object);
  }
}

unsigned gp_object_get_count(gp_object* object)
{
  return gp_ref_get_count(&object->mRef);
}

void _gp_object_init(gp_object* object, gp_object_free func)
{
  gp_ref_init(&object->mRef);
  object->mFree = func;
}

void _gp_pointer_free(gp_object* object)
{
  if(((gp_pointer*)object)->mFree)
  {
    ((gp_pointer*)object)->mFree(((gp_pointer*)object)->mPointer);
  }
  free(object);
}

gp_pointer* gp_pointer_new(void* pointer, gp_free free_func)
{
  gp_pointer* result = (gp_pointer*)malloc(sizeof(gp_pointer));
  _gp_object_init(&result->mObject, _gp_pointer_free);
  result->mPointer = pointer;
  result->mFree = free_func;
  
  return result;
}

void* gp_pointer_get_pointer(gp_pointer* pointer)
{
  return pointer->mPointer;
}
