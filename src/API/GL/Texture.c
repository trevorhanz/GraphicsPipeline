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

#include <GraphicsPipeline/Texture.h>
#include <GraphicsPipeline/Logging.h>

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#endif // GP_GL
#include "GL.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

size_t _gp_data_type_to_size(GP_DATA_TYPE type)
{
  switch(type)
  {
    case GP_DATA_TYPE_UBYTE:
      return sizeof(uint8_t);
    case GP_DATA_TYPE_INT:
      return sizeof(int);
    case GP_DATA_TYPE_FLOAT:
      return sizeof(float);
    case GP_DATA_TYPE_DOUBLE:
      return sizeof(double);
  }
}

void _gp_texture_data_free(gp_object* object)
{
  gp_texture_data* data = (gp_texture_data*)object;
  
  if(data->mData) free(data->mData);
  free(data);
}

gp_texture_data* gp_texture_data_new()
{
  gp_texture_data* data = malloc(sizeof(gp_texture_data));
  _gp_object_init(&data->mObject, _gp_texture_data_free);
  data->mData = NULL;
  data->mFormat = GP_FORMAT_R;
  data->mType = GP_DATA_TYPE_UBYTE;
  data->mWidth = 0;
  data->mHeight = 0;
  
  return data;
}

void _gp_texture_data_set_1d(gp_texture_data* td,
                             void* data,
                             GP_FORMAT format,
                             GP_DATA_TYPE type,
                             unsigned int width)
{
  // NOTE: Only desktop GL supports 1D textures.
  // If not supported, generate 1D textures as 2D.
#ifdef GP_GL
  td->mDimensions = GL_TEXTURE_1D;
#else
  td->mDimensions = GL_TEXTURE_2D;
#endif
  td->mFormat = format;
  td->mType = type;
  
  const size_t size = _gp_data_type_to_size(type)*format*width;
  
  if(data == NULL)
  {
    if(td->mData != NULL) free(td->mData);
    td->mData = NULL;
  }
  else
  {
    if(td->mData == NULL) td->mData = malloc(size);
    
    memcpy(td->mData, data, size);
  }
  td->mWidth = width;
  td->mHeight = 1;
}

void gp_texture_data_set_1d(gp_texture_data* td,
                            void* data,
                            GP_FORMAT format,
                            GP_DATA_TYPE type,
                            unsigned int width)
{
  _gp_texture_data_set_1d(td, data, format, type, width);
  
  td->mWidthOffset = -1;
  td->mHeightOffset = -1;
}

void gp_texture_data_set_1d_chunk(gp_texture_data* td,
                                  void* data,
                                  GP_FORMAT format,
                                  GP_DATA_TYPE type,
                                  unsigned int width,
                                  unsigned int offset)
{
  _gp_texture_data_set_1d(td, data, format, type, width);
  
  td->mWidthOffset = offset;
  td->mHeightOffset = 0;
}

void _gp_texture_data_set_2d(gp_texture_data* td,
                             void* data,
                             GP_FORMAT format,
                             GP_DATA_TYPE type,
                             unsigned int width,
                             unsigned int height)
{
  td->mDimensions = GL_TEXTURE_2D;
  td->mFormat = format;
  td->mType = type;
  
  const size_t size = _gp_data_type_to_size(type)*format*width*height;
  
  if(data == NULL)
  {
    if(td->mData != NULL) free(td->mData);
    td->mData = NULL;
  }
  else
  {
    if(td->mData == NULL) td->mData = malloc(size);
    
    memcpy(td->mData, data, size);
  }
  td->mWidth = width;
  td->mHeight = height;
}

void gp_texture_data_set_2d(gp_texture_data* td,
                            void* data,
                            GP_FORMAT format,
                            GP_DATA_TYPE type,
                            unsigned int width,
                            unsigned int height)
{
  _gp_texture_data_set_2d(td, data, format, type, width, height);
  
  td->mWidthOffset = -1;
  td->mHeightOffset = -1;
}

void gp_texture_data_set_2d_chunk(gp_texture_data* td,
                                  void* data,
                                  GP_FORMAT format,
                                  GP_DATA_TYPE type,
                                  unsigned int width,
                                  unsigned int height,
                                  unsigned int w_offset,
                                  unsigned int h_offfset)
{
  _gp_texture_data_set_2d(td, data, format, type, width, height);
  
  td->mWidthOffset = w_offset;
  td->mHeightOffset = h_offfset;
}


void _gp_texture_free(gp_object* object)
{
  gp_texture* texture = (gp_texture*)object;
  
  glDeleteTextures(1, &texture->mTexture);
#ifndef GP_WEB
  glDeleteBuffers(1, &texture->mPBO);
#endif
  free(texture);
}

gp_texture* gp_texture_new(gp_context* context)
{
  gp_texture* texture = malloc(sizeof(gp_texture));
  _gp_object_init(&texture->mObject, _gp_texture_free);
  glGenTextures(1, &texture->mTexture);
  texture->mDimensions = GL_TEXTURE_2D;
  texture->mWrapX = GL_CLAMP_TO_EDGE;
  texture->mWrapY = GL_CLAMP_TO_EDGE;
  
#ifndef GP_WEB
  glGenBuffers(1, &texture->mPBO);
#endif
  
  return texture;
}

void gp_texture_set_data(gp_texture* texture, gp_texture_data* data)
{
  glBindTexture(data->mDimensions, texture->mTexture);
  
  glTexParameteri(data->mDimensions, GL_TEXTURE_WRAP_S, texture->mWrapX);
  glTexParameteri(data->mDimensions, GL_TEXTURE_WRAP_T, texture->mWrapY);
  
  glTexParameteri(data->mDimensions, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(data->mDimensions, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  GLvoid* d = data->mData;
#ifndef GP_WEB
  d = 0;
  
  const size_t size = _gp_data_type_to_size(data->mType)*data->mFormat*data->mWidth*data->mHeight;
  
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->mPBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
  
  if(data->mData != 0)
  {
    GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, size, GL_MAP_WRITE_BIT);
    if(ptr)
    {
        // update data directly on the mapped buffer
        memcpy(ptr, data->mData, size);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
        CHECK_GL_ERROR()
    }
  }
#endif
  
  GLuint internalFormat = 0;
  GLuint format = 0;
  GLuint type = 0;
  
  static const GLuint formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
  static const GLuint internalFormats[3][4] = {
    {GL_R8, GL_RG8, GL_RGB8, GL_RGBA8}, 
    {GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I},
    {GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F}
  };
  
  format = formats[data->mFormat-1];
  switch(data->mType)
  {
    case GP_DATA_TYPE_UBYTE:
      type = GL_UNSIGNED_BYTE;
      internalFormat = internalFormats[0][data->mFormat-1];
      break;
    case GP_DATA_TYPE_INT:
      type = GL_INT;
      internalFormat = internalFormats[1][data->mFormat-1];
      break;
    case GP_DATA_TYPE_FLOAT:
      type = GL_FLOAT;
      internalFormat = internalFormats[2][data->mFormat-1];
      break;
    case GP_DATA_TYPE_DOUBLE:
      // NOTE: Only desktop GL supports GL_DOUBLE.
      // If not supported, convert to GL_FLOAT.
#ifdef GP_GL
      type = GL_DOUBLE;
#else
      type = GL_FLOAT;
#endif
      internalFormat = internalFormats[2][data->mFormat-1];
      break;
  }
  
  switch(data->mDimensions)
  {
#ifdef GP_GL
    case GL_TEXTURE_1D:
      if(data->mWidthOffset < 0)
      {
        glTexImage1D(data->mDimensions,
          0,                            // Level of detail (mip-level) (0 is base image)
          internalFormat,               // Internal format
          data->mWidth,                 // Width
          0,                            // Border
          format,                       // Image format
          type,                         // Image type
          (GLvoid*)d                    // data
        );
      }
      else
      {
        glTexSubImage1D(data->mDimensions,
          0,
          data->mWidthOffset,
          data->mWidth,
          format,
          type,
          (GLvoid*)d
        );
      }
      break;
#endif
    case GL_TEXTURE_2D:
      if(data->mWidthOffset < 0)
      {
        glTexImage2D(data->mDimensions,
          0,                            // Level of detail (mip-level) (0 is base image)
          internalFormat,               // Internal format
          data->mWidth,                 // Width
          data->mHeight,                // Height
          0,                            // Border
          format,                       // Image format
          type,                         // Image type
          (GLvoid*)d                    // data
        );
      }
      else
      {
        glTexSubImage2D(data->mDimensions,
          0,
          data->mWidthOffset,
          data->mHeightOffset,
          data->mWidth,
          data->mHeight,
          format,
          type,
          (GLvoid*)d
        );
      }
      break;
  }
  
#ifndef GP_WEB
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif
  glBindTexture(data->mDimensions, texture->mTexture);
  
  texture->mDimensions = data->mDimensions;
  
  CHECK_GL_ERROR()
}

GLuint _gp_wrap_to_gl(GP_WRAP wrap)
{
  switch(wrap)
  {
    case GP_WRAP_EDGE:
      return GL_CLAMP_TO_EDGE;
    case GP_WRAP_BORDER:
#ifdef GP_GL
      return GL_CLAMP_TO_BORDER;
#else
      // NOTE: GL_CLAMP_TO_BORDER is not supported in GLES
      return GL_CLAMP_TO_EDGE;
#endif
    case GP_WRAP_REPEAT:
      return GL_REPEAT;
    case GP_WRAP_MIRROR:
      return GL_MIRRORED_REPEAT;
  }
}

void gp_texture_set_wrap_x(gp_texture* texture, GP_WRAP wrap)
{
  GLuint w = _gp_wrap_to_gl(wrap);
  if(w == texture->mWrapX) return;
  
  texture->mWrapX = w;
  
  glBindTexture(texture->mDimensions, texture->mTexture);
  glTexParameteri(texture->mDimensions, GL_TEXTURE_WRAP_S, texture->mWrapX);
  glBindTexture(texture->mDimensions, 0);
}

void gp_texture_set_wrap_y(gp_texture* texture, GP_WRAP wrap)
{
  GLuint w = _gp_wrap_to_gl(wrap);
  if(w == texture->mWrapY) return;
  
  texture->mWrapY = w;
  
  glBindTexture(texture->mDimensions, texture->mTexture);
  glTexParameteri(texture->mDimensions, GL_TEXTURE_WRAP_T, texture->mWrapY);
  glBindTexture(texture->mDimensions, 0);
}

typedef struct
{
  gp_texture*         mTexture;
  gp_texture_data*    mData;
  void(*mCallback)(void*);
  void*               mUserData;
} _gp_texture_async;

void _gp_texture_async_func(void* data)
{
  _gp_texture_async* async = (_gp_texture_async*)data;
  
  gp_texture_set_data(async->mTexture, async->mData);
}

void _gp_texture_join_func(void* data)
{
  _gp_texture_async* async = (_gp_texture_async*)data;
  
  if(async->mCallback)
  {
    async->mCallback(async->mUserData);
  }
  
  free(async);
}

void gp_texture_set_data_async(gp_texture* texture, gp_texture_data* data, void (*callback)(void*), void* userdata)
{
  _gp_texture_async* async = malloc(sizeof(_gp_texture_async));
  async->mTexture = texture;
  async->mData = data;
  async->mCallback = callback;
  async->mUserData = userdata;
  
  _gp_api_work(_gp_texture_async_func, _gp_texture_join_func, (void*)async);
}
