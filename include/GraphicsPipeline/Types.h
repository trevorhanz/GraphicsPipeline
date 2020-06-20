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

//! \file Types.h

#ifndef __GP_TYPES_H__
#define __GP_TYPES_H__

#include <memory>
#include <vector>

namespace GP
{
  /*!
   * Internal data structure for use by Context and derived classes.
   */
  class UserData
  {
  public:
    virtual ~UserData() {};
  };
  //! Shared pointer to UserData object.
  typedef std::shared_ptr<UserData> UserDataPtr;
  
  /*!
   * Storage object for color information
   */
  class Color
  {
  public:
    //! Default constructor.  Stores data as (0, 0, 0, 1).
    Color();
    
    /*!
     * Constructor
     * \param r Red component.
     * \param g Green component.
     * \param b Blue component.
     * \param a Alpha component.
     */
    Color(float r, float g, float b, float a=1.0f);
    
    void SetR(float r); //!< Sets red component. \param r Red component.
    void SetG(float g); //!< Sets green component. \param g Green component.
    void SetB(float b); //!< Sets blue component. \param b Blue component.
    void SetA(float a); //!< Sets alpha component. \param a Alpha component.
    
    float GetR() const; //!< Get red component. \return Red component.
    float GetG() const; //!< Get green component. \return Green component.
    float GetB() const; //!< Get blue component. \return Blue component.
    float GetA() const; //!< Get alpha component. \return Alpha component.
    
  private:
    float                   mColor[4];
  };
  
  /*!
   * A surface on to which can be rendered
   */
  class Target
  {
  public:
    //! Constructor
    Target() {}
    //! Destructor
    virtual ~Target() {}
    
    UserDataPtr             mUserData;
  };
  //! Shared pointer to Target object.
  typedef std::shared_ptr<Target> TargetPtr;
  
  /*!
   * Storage object for numerical data
   */
  class Array
  {
  public:
    UserDataPtr                   mUserData;
  };
  //! Shared pointer to Array object.
  typedef std::shared_ptr<Array> ArrayPtr;
  
  /*!
   * Temporary storage for numberical data.
   * Data is stored here on the CPU side before it is uploaded to the GPU
   * by using a LoadArray operation. Loading data can be an asynchronous
   * operation so an ArrayData objects should not continue to be used after
   * the LoadArray operation was executed.
   */
  class ArrayData
  {
  public:
    //! Constructor
    ArrayData();
    
    /*!
     * Stores float data into the object.
     * \param data Pointer to float array to be stored.
     * \param count Number of float elements in array.
     */
    void SetData(float* data, unsigned int count);
    
    /*!
     * Returns number of elements.
     * \return Number of elements.
     */
    int GetSize() const;
    
    /*!
     * Returns pointer to float data.
     * \return Pointer to float data.
     */
    const float* GetData() const;
    
  private:
    std::vector<float>            mData;
  };
  //! Shared pointer to ArrayData object.
  typedef std::shared_ptr<ArrayData> ArrayDataPtr;
  
  /*!
   * Storage object for texture data.
   */
  class Texture
  {
  public:
    UserDataPtr                   mUserData;
  };
  //! Shared pointer to Texture object.
  typedef std::shared_ptr<Texture> TexturePtr;
  
  /*!
   * Storage object for a shader program.
   */
  class Shader
  {
  public:
    UserDataPtr                   mUserData;
  };
  //! Shared pointer to Shader object.
  typedef std::shared_ptr<Shader> ShaderPtr;
}


#endif // __GP_TYPES_H__
