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

void _gp_object_init(gp_object* object, gp_object_free func)
{
  gp_ref_init(&object->mRef);
  object->mFree = func;
}


