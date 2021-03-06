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
#include "Object.h"

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
 * Store 1D data in texture data object.  If the platform doesn't support
 * 1D textures, the data is converted to a 2D texture with a hieght of 1.
 * \param td Texture data object to be used.
 * \param data Pointer to an array of data to be stored.
 * \param format Number of values per color value.
 * \param type The data type for data values.
 * \param width Number of elements in data width.
 */
GP_EXPORT void gp_texture_data_set_1d(gp_texture_data* td,
                                      void* data,
                                      GP_FORMAT format,
                                      GP_DATA_TYPE type,
                                      unsigned int width);

/*!
 * Store 1D data in texture data object.  If the platform doesn't support
 * 1D textures, the data is converted to a 2D texture with a hieght of 1.
 * \param td Texture data object to be used.
 * \param data Pointer to an array of data to be stored.
 * \param format Number of values per color value.
 * \param type The data type for data values.
 * \param width Number of elements in data width.
 * \param offset Width offset where this chunk should be uploaded.
 */
GP_EXPORT void gp_texture_data_set_1d_chunk(gp_texture_data* td,
                                            void* data,
                                            GP_FORMAT format,
                                            GP_DATA_TYPE type,
                                            unsigned int width,
                                            unsigned int offset);

/*!
 * Store 2D data in texture data object.
 * \param td Texture data object to be used.
 * \param data Pointer to an array of data to be stored.
 * \param format Number of values per color value.
 * \param type The data type for data values.
 * \param width Number of elements in data width.
 * \param height Number of elements in data height.
 */
GP_EXPORT void gp_texture_data_set_2d(gp_texture_data* td,
                                      void* data,
                                      GP_FORMAT format,
                                      GP_DATA_TYPE type,
                                      unsigned int width,
                                      unsigned int height);

/*!
 * Store 2D data in texture data object.
 * \param td Texture data object to be used.
 * \param data Pointer to an array of data to be stored.
 * \param format Number of values per color value.
 * \param type The data type for data values.
 * \param width Number of elements in data width.
 * \param height Number of elements in data height.
 * \param w_offset Width offset where this chunk should be uploaded.
 * \param h_offset Height offset where this chunk should be uploaded.
 */
GP_EXPORT void gp_texture_data_set_2d_chunk(gp_texture_data* td,
                                            void* data,
                                            GP_FORMAT format,
                                            GP_DATA_TYPE type,
                                            unsigned int width,
                                            unsigned int height,
                                            unsigned int w_offset,
                                            unsigned int h_offfset);

/*!
 * Create a new gp_texture object tied to a context.
 * \param context Context object used to create texture.
 * \return Newly created texture.
 */
GP_EXPORT gp_texture* gp_texture_new(gp_context* context);

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

/*!
 * Set how data will be wrapped along the X axis.
 * \param texture Pointer to texture object.
 * \param wrap The new wrapping behavior.
 */
GP_EXPORT void gp_texture_set_wrap_x(gp_texture* texture, GP_WRAP wrap);

/*!
 * Set how data will be wrapped along the Y axis.
 * \param texture Pointer to texture object.
 * \param wrap The new wrapping behavior.
 */
GP_EXPORT void gp_texture_set_wrap_y(gp_texture* texture, GP_WRAP wrap);

//! \} // Texture

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_texture_data
   */
  class TextureData : public Object
  {
  public:
    //! Constructor
    inline TextureData(gp_texture_data* data);
    
    //! Constructor
    inline TextureData();
    
    /*!
     * Store 1D data in texture data object.  If the platform doesn't support
     * 1D textures, the data is converted to a 2D texture with a hieght of 1.
     * \param data Pointer to an array to be stored.
     * \param format Number of values per color value.
     * \param type The data type for data values.
     * \param width Number of elements in data width.
     */
    inline void Set1D(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width);
    
    /*!
     * Store 1D data in texture data object.  If the platform doesn't support
     * 1D textures, the data is converted to a 2D texture with a hieght of 1.
     * \param data Pointer to an array to be stored.
     * \param format Number of values per color value.
     * \param type The data type for data values.
     * \param width Number of elements in data width.
     * \param offset Width offset where this chunk should be uploaded.
     */
    inline void Set1DChunk(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width, unsigned int offset);
    
    /*!
     * Store 2D data in texture data object.
     * \param data Pointer to an array of data to be stored.
     * \param format Number of values per color value.
     * \param type The data type for data values.
     * \param width Number of elements in data width.
     * \param height Number of elements in data height.
     */
    inline void Set2D(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width, unsigned int height);
    
    /*!
     * Store 2D data in texture data object.
     * \param data Pointer to an array of data to be stored.
     * \param format Number of values per color value.
     * \param type The data type for data values.
     * \param width Number of elements in data width.
     * \param height Number of elements in data height.
     * \param w_offset Width offset where this chunk should be uploaded.
     * \param h_offset Height offset where this chunk should be uploaded.
     */
    inline void Set2DChunk(void* data,
                           GP_FORMAT format,
                           GP_DATA_TYPE type,
                           unsigned int width,
                           unsigned int height,
                           unsigned int w_offset,
                           unsigned int h_offset);
  };
  
  /*!
   * \brief Wrapper class for ::gp_texture.
   */
  class Texture : public Object
  {
  public:
    inline Texture();
    
    //! Constructor
    inline Texture(gp_texture* texture);
    
    //! Constructor
    inline Texture(const Context& context);
    
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
    
    /*!
     * Set how data will be wrapped along the X axis.
     * \param wrap The new wrapping behavior.
     */
    inline void SetWrapX(GP_WRAP wrap);
    
    /*!
     * Set how data will be wrapped along the Y axis.
     * \param wrap The new wrapping behavior.
     */
    inline void SetWrapY(GP_WRAP wrap);
    
  private:
    struct AsyncData
    {
      Texture* mTexture;
      std::function<void(Texture*)> mCallback;
    };
    inline static void AsyncCallback(void* data);
  };
  
  //
  // Implementation
  //
  TextureData::TextureData(gp_texture_data* data) : Object((gp_object*)data) {}
  TextureData::TextureData() : Object((void*)gp_texture_data_new()) {}
  void TextureData::Set1D(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width)
  {
    gp_texture_data_set_1d((gp_texture_data*)GetObject(*this), data, format, type, width);
  }
  void TextureData::Set1DChunk(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width, unsigned int offset)
  {
    gp_texture_data_set_1d_chunk((gp_texture_data*)GetObject(*this), data, format, type, width, offset);
  }
  void TextureData::Set2D(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width, unsigned int height)
  {
    gp_texture_data_set_2d((gp_texture_data*)GetObject(*this), data, format, type, width, height);
  }
  void TextureData::Set2DChunk(void* data, GP_FORMAT format, GP_DATA_TYPE type, unsigned int width, unsigned int height, unsigned int w_offset, unsigned int h_offset)
  {
    gp_texture_data_set_2d_chunk((gp_texture_data*)GetObject(*this), data, format, type, width, height, w_offset, h_offset);
  }
  
  Texture::Texture() : Object((void*)0) {}
  Texture::Texture(gp_texture* texture) : Object((gp_object*)texture) {}
  Texture::Texture(const Context& context) : Object((void*)gp_texture_new((gp_context*)GetObject(*this))) {}
  void Texture::SetData(const TextureData& data) {gp_texture_set_data((gp_texture*)GetObject(*this), (gp_texture_data*)GetObject(data));}
  void Texture::SetDataAsync(const TextureData& data, std::function<void(Texture*)> callback)
  {
    AsyncData* async = new AsyncData();
    async->mTexture = this;
    async->mCallback = callback;
    gp_texture_set_data_async((gp_texture*)GetObject(*this), (gp_texture_data*)GetObject(data), &Texture::AsyncCallback, async);
  }
  void Texture::AsyncCallback(void* data)
  {
    AsyncData* async = (AsyncData*)data;
    async->mCallback(async->mTexture);
    delete async;
  }
  void Texture::SetWrapX(GP_WRAP wrap) {gp_texture_set_wrap_x((gp_texture*)GetObject(*this), wrap);}
  void Texture::SetWrapY(GP_WRAP wrap) {gp_texture_set_wrap_y((gp_texture*)GetObject(*this), wrap);}
}

#endif // __cplusplus

#endif // __GP_TEXTURE_H__
