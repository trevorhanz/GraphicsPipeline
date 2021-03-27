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
 * Create a new gp_texture_data object.
 * \return Newly created texture data object
 */
GP_EXPORT gp_texture_data* gp_texture_data_new();

/*!
 * Increase texture data object reference count.
 * \param data Texture data object for which to increase the ref count.
 */
GP_EXPORT void gp_texture_data_ref(gp_texture_data* data);

/*!
 * Decrease texture data object reference count.
 * \param data Texture data object for which to decrease the ref count.
 */
GP_EXPORT void gp_texture_data_unref(gp_texture_data* data);

/*!
 * Store float data in texture data object.
 * \param td Texture data object to be used.
 * \param data Pointer to an array of floats.
 * \param width Number of elements in data width.
 * \param height Number of elements in data height.
 */
GP_EXPORT void gp_texture_data_set(gp_texture_data* td, float* data, unsigned int width, unsigned int height);

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
 * \param data Pointer to texture data object to be uploaded.
 */
GP_EXPORT void gp_texture_set_data(gp_texture* texture, gp_texture_data* data);

/*!
 * Upload data to a texture object asynchronously.
 * \param texture Pointer to texture object.
 * \param data Pointer to texture data object to be uploaded.
 * \param callback Callback function to be called upon completion.  Set to NULL to igore.
 */
GP_EXPORT void gp_texture_set_data_async(gp_texture* texture, gp_texture_data* data, void (*callback)(void*), void* userdata);

//! \} // Texture

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_texture_data
   */
  class TextureData
  {
  public:
    //! Constructor
    inline TextureData();
    
    //! Copy Constructor
    inline TextureData(const TextureData& other);
    
    //! Destructor
    inline ~TextureData();
    
    /*!
     * Store float data in texture data object.
     * \param data Pointer to an array of floats.
     * \param width Number of elements in data width.
     * \param height Number of elements in data height.
     */
    inline void Set(float* data, unsigned int width, unsigned int height);
    
    
    //! Equal operator
    inline const TextureData& operator = (const TextureData& other);
    
  private:
    gp_texture_data*      mData;      //!< Internal texture data object
    
    friend class Texture;
  };
  
  /*!
   * \brief Wrapper class for ::gp_texture.
   */
  class Texture
  {
  public:
    //! Constructor
    inline Texture(const Context& context);
    
    //! Copy Constructor
    inline Texture(const Texture& other);
    
    //! Destructor
    inline ~Texture();
    
    /*!
     * Uploads texture to %Texture object.
     * \param data %TextureData to be uploaded.
     */
    inline void SetData(const TextureData& data);
    
    /*!
     * Uploads texture to %Texture object asynchronously.
     * \param data %TextureData to be uploaded.
     * \param callback Callback function to be called when upload is complete.
     */
    inline void SetDataAsync(const TextureData& data, std::function<void(Texture*)> callback);
    
    //! Equal operator
    inline const Texture& operator = (const Texture& other);
    
  private:
    struct AsyncData
    {
      Texture* mTexture;
      std::function<void(Texture*)> mCallback;
    };
    inline static void AsyncCallback(void* data);
    
    gp_texture*           mTexture;     //!< Internal texture object
    
    friend class Context;
    friend class UniformTexture;
    friend class FrameBuffer;
  };
  
  //
  // Implementation
  //
  TextureData::TextureData() : mData(gp_texture_data_new()) {}
  TextureData::TextureData(const TextureData& other)
  {
    mData = other.mData;
    gp_texture_data_ref(mData);
  }
  TextureData::~TextureData() {gp_texture_data_unref(mData);}
  void TextureData::Set(float* data, unsigned int width, unsigned int height) {gp_texture_data_set(mData, data, width, height);}
  const TextureData& TextureData::operator = (const TextureData& other)
  {
    gp_texture_data_unref(mData);
    mData = other.mData;
    gp_texture_data_ref(mData);
    return *this;
  }
  
  Texture::Texture(const Context& context) : mTexture(gp_texture_new(context.mContext)) {}
  Texture::Texture(const Texture& other)
  {
    mTexture = other.mTexture;
    gp_texture_ref(mTexture);
  }
  Texture::~Texture() {gp_texture_unref(mTexture);}
  void Texture::SetData(const TextureData& data) {gp_texture_set_data(mTexture, data.mData);}
  void Texture::SetDataAsync(const TextureData& data, std::function<void(Texture*)> callback)
  {
    AsyncData* async = new AsyncData();
    async->mTexture = this;
    async->mCallback = callback;
    gp_texture_set_data_async(mTexture, data.mData, &Texture::AsyncCallback, async);
  }
  const Texture& Texture::operator = (const Texture& other)
  {
    gp_texture_unref(mTexture);
    mTexture = other.mTexture;
    gp_texture_ref(mTexture);
    return *this;
  }
  void Texture::AsyncCallback(void* data)
  {
    AsyncData* async = (AsyncData*)data;
    async->mCallback(async->mTexture);
    delete async;
  }
}

#endif // __cplusplus

#endif // __GP_TEXTURE_H__
