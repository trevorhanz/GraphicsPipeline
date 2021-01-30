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
 * Increase array object reference count.
 * \param array Array object for which to increase the ref count.
 */
GP_EXPORT void gp_array_ref(gp_array* array);

/*!
 * Decrease array object reference count.
 * \param array Array object for which to decrease the ref count.
 */
GP_EXPORT void gp_array_unref(gp_array* array);
  
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
    //! Copy Constructor
    inline Array(const Array& other);
    
    //! Destructor
    inline ~Array();
    
    /*!
     * Uploads data to %Array object.
     * \param data %Array of data to be uploaded.
     * \param count Number of elements in data array;
     */
    inline void SetData(float* data, unsigned int count);
    
    //! Equal operator
    inline const Array& operator = (const Array& other);
    
  private:
    gp_array*           mArray;     //!< Internal array object
    
    friend class Context;
    friend class Pipeline;
    friend class DrawOperation;
  };
  
  //
  // Implementation
  //
  Array::Array(gp_array* array) : mArray(array) {}
  Array::Array(const Array& other)
  {
    mArray = other.mArray;
    gp_array_ref(mArray);
  }
  Array::~Array() {gp_array_unref(mArray);}
  void Array::SetData(float* data, unsigned int count) {gp_array_set_data(mArray, data, count);}
  const Array& Array::operator = (const Array& other)
  {
    gp_array_unref(mArray);
    mArray = other.mArray;
    gp_array_ref(mArray);
    return *this;
  }
}

#endif // __cplusplus

#endif // __GP_ARRAY_H__
