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

//! \file Array.h

#ifndef __GP_ARRAY_H__
#define __GP_ARRAY_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Array
 * \{
 */

/*!
 * Free array object.
 * \param array Pointer to array to be freed.
 */
  GP_EXPORT void gp_array_free(gp_array* array);
  
/*!
 * Upload data to an array object.
 * \param array Pointer to array object.
 * \param data Pointer to data to be uploaded.
 * \param count Number of element in data array.
 */
  GP_EXPORT void gp_array_set_data(gp_array* array, float* data, unsigned int count);

//! \} // Array

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_array.
   */
  class Array
  {
  private:
    //! Constructor
    inline Array(gp_array* array);
  public:
    //! Destructor
    inline ~Array();
    
    /*!
     * Uploads data to %Array object.
     * \param data %Array of data to be uploaded.
     * \param count Number of elements in data array;
     */
    inline void SetData(float* data, unsigned int count);
    
  private:
    gp_array*           mArray;     //!< Internal array object
    
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
