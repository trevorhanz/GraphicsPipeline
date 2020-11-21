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

#ifndef __GP_ARRAY_H__
#define __GP_ARRAY_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

  GP_EXPORT void gp_array_free(gp_array* array);
  GP_EXPORT void gp_array_set_data(gp_array* array, float* data, unsigned int count);

#ifdef __cplusplus
}

namespace GP
{
  class Array
  {
  private:
    inline Array(gp_array* array);
  public:
    inline ~Array();
    
    inline void SetData(float* data, unsigned int count);
    
  private:
    gp_array*           mArray;
    
    friend class Context;
    friend class Pipeline;
  };
  
  //
  // Implementation
  //
  Array::Array(gp_array* array) : mArray(array) {}
  Array::~Array() {gp_array_free(mArray);}
  void Array::SetData(float* data, unsigned int count) {gp_array_set_data(mArray, data, count);}
}

#endif // __cplusplus

#endif // __GP_ARRAY_H__
