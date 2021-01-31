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
#include "Context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Array
 * \{
 */

/*!
 * Create a new gp_array_data object.
 * \return Newly created array data object.
 */
GP_EXPORT gp_array_data* gp_array_data_new();

/*!
 * Increase array data object reference count.
 * \param data Array data object for which to increase the ref count.
 */
GP_EXPORT void gp_array_data_ref(gp_array_data* data);

/*!
 * Decrease array data object reference count.
 * \param data Array data object for which to decrease the ref count.
 */
GP_EXPORT void gp_array_data_unref(gp_array_data* data);

/*!
 * Store float array in array data object.
 * \param ad Array data object to be used.
 * \param data Pointer to array of floats.
 * \param count Number of elements in data array.
 */
GP_EXPORT void gp_array_data_set(gp_array_data* ad, float* data, unsigned int count);

/*!
 * Create a new gp_array object tied to a context.
 * \param context Context object used to create array.
 * \return Newly created array.
 */
GP_EXPORT gp_array* gp_array_new(gp_context* context);

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
 * \param data Pointer to array data object to be uploaded.
 */
GP_EXPORT void gp_array_set_data(gp_array* array, gp_array_data* data);

//! \} // Array

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_array_data
   */
  class ArrayData
  {
  public:
    //! Constructor
    inline ArrayData();
    
    //! Copy Constructor
    inline ArrayData(const ArrayData& other);
    
    //! Destructor
    inline ~ArrayData();
    
    /*!
     * Store float array in array data object.
     * \param data Pointer to array of floats.
     * \param count Number of elements in data array.
     */
    inline void Set(float* data, unsigned int count);
    
    //! Equal operator
    inline const ArrayData& operator = (const ArrayData& other);
    
  private:
    gp_array_data*      mData;  //!< Internal array data object
    
    friend class Array;
  };
  
  /*!
   * \brief Wrapper class for ::gp_array.
   */
  class Array
  {
  public:
    //! Constructor
    inline Array(const Context& context);
    
    //! Copy Constructor
    inline Array(const Array& other);
    
    //! Destructor
    inline ~Array();
    
    /*!
     * Uploads data to %Array object.
     * \param data %ArrayData to be uploaded.
     */
    inline void SetData(const ArrayData& ad);
    
    //! Equal operator
    inline const Array& operator = (const Array& other);
    
  private:
    gp_array*           mArray;     //!< Internal array object
    
    friend class Pipeline;
    friend class DrawOperation;
  };
  
  //
  // Implementation
  //
  ArrayData::ArrayData() : mData(gp_array_data_new()) {}
  ArrayData::ArrayData(const ArrayData& other)
  {
    mData = other.mData;
    gp_array_data_ref(mData);
  }
  ArrayData::~ArrayData() {gp_array_data_unref(mData);}
  void ArrayData::Set(float* data, unsigned int count) {gp_array_data_set(mData, data, count);}
  const ArrayData& ArrayData::operator = (const ArrayData& other)
  {
    gp_array_data_unref(mData);
    mData = other.mData;
    gp_array_data_ref(mData);
    return *this;
  }
  
  Array::Array(const Context& context) : mArray(gp_array_new(context.mContext)) {}
  Array::Array(const Array& other)
  {
    mArray = other.mArray;
    gp_array_ref(mArray);
  }
  Array::~Array() {gp_array_unref(mArray);}
  void Array::SetData(const ArrayData& ad) {gp_array_set_data(mArray, ad.mData);}
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
