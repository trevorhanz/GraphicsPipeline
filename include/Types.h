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

#ifndef __GP_TYPES_H__
#define __GP_TYPES_H__

#include <memory>
#include <vector>

namespace GP
{
  /*
   * Internal data structure for use by Contexts
   */
  class UserData
  {
  public:
    virtual ~UserData() {};
  };
  typedef std::shared_ptr<UserData> UserDataPtr;
  
  /*
   * Storage object for color information
   */
  class Color
  {
  public:
    Color();
    Color(float r, float g, float b, float a=1.0f);
    
    void SetR(float r);
    void SetG(float g);
    void SetB(float b);
    void SetA(float a);
    
    float GetR() const;
    float GetG() const;
    float GetB() const;
    float GetA() const;
    
  private:
    float                   mColor[4];
  };
  
  /*
   * A surface on to which can be rendered
   */
  class Target
  {
  public:
    Target() {}
    virtual ~Target() {}
    
    UserDataPtr             mUserData;
  };
  typedef std::shared_ptr<Target> TargetPtr;
  
  /*
   * Storage object for numerical data
   */
  class Array
  {
  public:
    UserDataPtr                   mUserData;
  };
  typedef std::shared_ptr<Array> ArrayPtr;
  
  /*
   * Temporary storage for numberical data
   */
  class ArrayData
  {
  public:
    ArrayData();
    
    void SetData(float* data, unsigned int count);
    
    int GetSize() const;
    const float* GetData() const;
    
  private:
    std::vector<float>            mData;
  };
  typedef std::shared_ptr<ArrayData> ArrayDataPtr;
  
  /*
   * Storage object for texture data
   */
  class Texture
  {
  public:
    UserDataPtr                   mUserData;
  };
  typedef std::shared_ptr<Texture> TexturePtr;
  
  /*
   * Storage object for a shader program
   */
  class Shader
  {
  public:
    UserDataPtr                   mUserData;
  };
  typedef std::shared_ptr<Shader> ShaderPtr;
}


#endif // __GP_TYPES_H__
