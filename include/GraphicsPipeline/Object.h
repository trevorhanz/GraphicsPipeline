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

//! \file Object.h

#ifndef __GP_OBJECT_H__
#define __GP_OBJECT_H__

#include "Common.h"
#include "Types.h"
#include "Logging.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Object
 * \{
 */

/*!
 * Increment the reference count on a gp_object.
 * \param object Object for which to increase the reference count.
 */
GP_EXPORT void gp_object_ref(gp_object* object);

/*!
 * Decrease the reference count on a gp_object.
 * \param object Object for which to decrease the reference count.
 */
GP_EXPORT void gp_object_unref(gp_object* object);

//! \} // Object

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_object
   */
  class Object
  {
  protected:
    //! Constructor
    inline Object(gp_object* object);
    
  public:
    //! Copy Constructor
    inline Object(const Object& other);
    
    //! Destructor
    inline ~Object();
    
    //! Equal operator
    inline const Object& operator = (const Object& other);
    
  protected:
    gp_object*                mObject;
  };
  
  //
  // Implementation
  //
  Object::Object(gp_object* object) : mObject(object) {}
  Object::Object(const Object& other)
  {
    mObject = other.mObject;
    gp_object_ref(mObject);
  }
  Object::~Object() {gp_object_unref(mObject);}
  const Object& Object::operator = (const Object& other)
  {
    gp_object_unref(mObject);
    mObject = other.mObject;
    gp_object_ref(mObject);
    return *this;
  }
  
}

#endif // __cplusplus

#endif // __GP_OBJECT_H__
