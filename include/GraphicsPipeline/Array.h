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
 * Create a new gp_array_data object with an initial size.
 * \param size Initial size of array data object.
 * \return Newly created array data object.
 */
GP_EXPORT gp_array_data* gp_array_data_new_with_size(unsigned int size);

/*!
 * Allocate enough storage space to store a given number of bytes.
 * \param ad Array data object to be used.
 * \param size Number of bytes to be allocated.
 */
GP_EXPORT void gp_array_data_allocate(gp_array_data* ad, unsigned int size);

/*!
 * Store array of data in array data object.
 * \param ad Array data object to be used.
 * \param data Pointer to array of data to be stored.
 * \param size Size of the data to be stored in bytes.
 */
GP_EXPORT void gp_array_data_set(gp_array_data* ad, void* data, unsigned int size);

/*!
 * Retrieve the array of data stored in the array data object.
 * \param ad Array data object to be used.
 * \return Pointer to array of data to be retrieved.
 */
GP_EXPORT void* gp_array_data_get_data(gp_array_data* ad);

/*!
 * Retrieve the size of the data stored in the array data object.
 * \param ad Array data object to be used.
 * \return Size of the data stored in bytes.
 */
GP_EXPORT unsigned int gp_array_data_get_size(gp_array_data* ad);

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
    
    inline ArrayData();
    
    //! Constructor
    inline ArrayData(unsigned int size);
    
    /*!
     * Allocate enough storage space to store a given number of bytes.
     * \param size Number of bytes to be allocated.
     */
    inline void Allocate(unsigned int size);
    
    /*!
     * Store array of data in array data object.
     * \param data Pointer to array of data to be stored.
     * \param size Size of the data to be stored in bytes.
     */
    inline void Set(void* data, unsigned int size);
    
    /*!
     * Retrieve the array of data stored in the array data object.
     * \return Pointer to array of data to be retrieved.
     */
    inline void* GetData();
    
    /*!
     * Retrieve the size of the data stored in the array data object.
     * \return Size of the data stored in bytes.
     */
    inline unsigned int GetSize();
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
  ArrayData::ArrayData(unsigned int size) : Object((void*)gp_array_data_new_with_size(size)) {}
  void ArrayData::Allocate(unsigned int size) {gp_array_data_allocate((gp_array_data*)GetObject(), size);}
  void ArrayData::Set(void* data, unsigned int size) {gp_array_data_set((gp_array_data*)GetObject(), data, size);}
  void* ArrayData::GetData() {return gp_array_data_get_data((gp_array_data*)GetObject());}
  unsigned int ArrayData::GetSize() {return gp_array_data_get_size((gp_array_data*)GetObject());}
  
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
