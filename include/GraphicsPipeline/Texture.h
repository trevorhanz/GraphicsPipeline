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

//! \file Texture.h

#ifndef __GP_TEXTURE_H__
#define __GP_TEXTURE_H__

#include "Common.h"
#include "Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup Texture
 * \{
 */

/*!
 * Create a new gp_texture object tied to a context.
 * \param context Context object used to create texture.
 * \return Newly created texture.
 */
GP_EXPORT gp_texture* gp_texture_new(gp_context* context);

/*!
 * Increase texture object reference count.
 * \param texture Texture object for which to increase the ref count.
 */
GP_EXPORT void gp_texture_ref(gp_texture* texture);

/*!
 * Decrease texture object reference count.
 * \param texture Texture object for which to decrease the ref count.
 */
GP_EXPORT void gp_texture_unref(gp_texture* texture);

/*!
 * Upload data to a texture object.
 * \param texture Pointer to texture object.
 * \param data Pointer to data to be uploaded.
 * \param width Number of elements in data width.
 * \param height Number of elements in data height.
 */
  GP_EXPORT void gp_texture_set_data(gp_texture* texture, float* data, unsigned int width, unsigned int height);

//! \} // Texture

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_texture.
   */
  class Texture
  {
  private:
    //! Constructor
    inline Texture(gp_texture* texture);
  public:
    //! Copy Constructor
    inline Texture(const Texture& other);
    
    //! Destructor
    inline ~Texture();
    
    /*!
     * Uploads texture to %Array object.
     * \param data %Array of data to be uploaded.
     * \param width Number of elements in data width.
     * \param height Number of elements in data height.
     */
    inline void SetData(float* data, unsigned int width, unsigned int height);
    
    //! Equal operator
    inline const Texture& operator = (const Texture& other);
    
  private:
    gp_texture*           mTexture;     //!< Internal array object
    
    friend class Context;
    friend class Uniform;
  };
  
  //
  // Implementation
  //
  Texture::Texture(gp_texture* texture) : mTexture(texture) {}
  Texture::Texture(const Texture& other)
  {
    mTexture = other.mTexture;
    gp_texture_ref(mTexture);
  }
  Texture::~Texture() {gp_texture_unref(mTexture);}
  void Texture::SetData(float* data, unsigned int width, unsigned int height) {gp_texture_set_data(mTexture, data, width, height);}
  const Texture& Texture::operator = (const Texture& other)
  {
    gp_texture_unref(mTexture);
    mTexture = other.mTexture;
    gp_texture_ref(mTexture);
    return *this;
  }
}

#endif // __cplusplus

#endif // __GP_TEXTURE_H__
