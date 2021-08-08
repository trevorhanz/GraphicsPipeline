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
#include "Object.h"

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
 * Upload data to an array object.
 * \param array Pointer to array object.
 * \param data Pointer to array data object to be uploaded.
 */
GP_EXPORT void gp_array_set_data(gp_array* array, gp_array_data* data);

/*!
 * Upload data to an array object asynchronously.
 * \param array Pointer to array object.
 * \param data Pointer to array data object to be uploaded.
 */
GP_EXPORT void gp_array_set_data_async(gp_array* array, gp_array_data* data, void (*callback)(void*), void* userdata);

//! \} // Array

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_array_data
   */
  class ArrayData : public Object
  {
  public:
    //! Constructor
    inline ArrayData(gp_array_data* data);
    
    //! Constructor
    inline ArrayData();
    
    /*!
     * Store float array in array data object.
     * \param data Pointer to array of floats.
     * \param count Number of elements in data array.
     */
    inline void Set(float* data, unsigned int count);
  };
  
  /*!
   * \brief Wrapper class for ::gp_array.
   */
  class Array : public Object
  {
  public:
    //! Constructor
    inline Array(gp_array* array);
    
    //! Constructor
    inline Array(const Context& context);
    
    /*!
     * Uploads data to %Array object.
     * \param data %ArrayData to be uploaded.
     */
    inline void SetData(const ArrayData& ad);
    
    /*!
     * Uploads data to %Array object asynchronously.
     * \param data %ArrayData to be uploaded.
     */
    inline void SetDataAsync(const ArrayData& ad, std::function<void(Array*)> callback);
    
  private:
    struct AsyncData
    {
      Array* mArray;
      std::function<void(Array*)> mCallback;
    };
    inline static void AsyncCallback(void* data);
  };
  
  //
  // Implementation
  //
  ArrayData::ArrayData(gp_array_data* data) : Object((gp_object*)data) {}
  ArrayData::ArrayData() : Object((void*)gp_array_data_new()) {}
  void ArrayData::Set(float* data, unsigned int count) {gp_array_data_set((gp_array_data*)GetObject(), data, count);}
  
  Array::Array(gp_array* array) : Object((void*)array) {}
  Array::Array(const Context& context) : Object((gp_object*)gp_array_new((gp_context*)context.GetObject())) {}
  void Array::SetData(const ArrayData& ad) {gp_array_set_data((gp_array*)GetObject(), (gp_array_data*)ad.GetObject());}
  void Array::SetDataAsync(const ArrayData& ad, std::function<void(Array*)> callback)
  {
    AsyncData* async = new AsyncData();
    async->mArray = this;
    async->mCallback = callback;
    gp_array_set_data_async((gp_array*)GetObject(), (gp_array_data*)ad.GetObject(), &Array::AsyncCallback, async);
  }
  void Array::AsyncCallback(void* data)
  {
    AsyncData* async = (AsyncData*)data;
    async->mCallback(async->mArray);
    delete async;
  }
}

#endif // __cplusplus

#endif // __GP_ARRAY_H__
