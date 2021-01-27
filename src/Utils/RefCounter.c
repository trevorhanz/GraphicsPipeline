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

#include "RefCounter.h"

void gp_ref_init(gp_refcounter* ref)
{
#ifdef __STDC_NO_AUTOMICS__
  ref->mRefCount = 0;
#else
  atomic_init(&ref->mRefCount, 0);
#endif
}

void gp_ref_inc(gp_refcounter* ref)
{
#ifdef __STDC_NO_AUTOMICS__
  ++ref->mRefCount;
#else
  atomic_fetch_add(&ref->mRefCount, 1);
#endif
}

int gp_ref_dec(gp_refcounter* ref)
{
  #ifdef __STDC_NO_AUTOMICS__
    return (--ref->mRefCount) == 0;
  #else
    return atomic_fetch_sub(&ref->mRefCount, 1) == 1;
  #endif
}


