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

#ifndef __GP_REFCOUNTER_H__
#define __GP_REFCOUNTER_H__

#if !defined(__STDC_NO_ATOMICS__) && (__STDC_VERSION__ >= 201112L)
  #define GP_ATOMICS 1
#endif

// Workaround for GCC 4.8 bug
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 8)
  #undef GP_ATOMICS
#endif

#ifndef GP_ATOMICS
typedef int _gp_refcounter_type;
#else
#ifndef __cplusplus
#include <stdatomic.h>
#else
# include <atomic>
# define _Atomic(X) std::atomic< X >
#endif
typedef _Atomic(int) _gp_refcounter_type;
#endif

typedef struct
{
  _gp_refcounter_type mRefCount;
} gp_refcounter;

#ifdef __cplusplus
extern "C" {
#endif

void gp_ref_init(gp_refcounter* ref);
void gp_ref_inc(gp_refcounter* ref);
int gp_ref_dec(gp_refcounter* ref);

#ifdef __cplusplus
}
#endif

#endif // __GP_REFCOUNTER_H__
