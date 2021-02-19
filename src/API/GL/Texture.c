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

gp_texture* gp_texture_new(gp_context* context)
{
  gp_texture* texture = malloc(sizeof(gp_texture));
  
  glGenTextures(1, &texture->mTexture);
  gp_ref_init(&texture->mRef);
  
#ifndef GP_WEB
  glGenBuffers(1, &texture->mPBO);
#endif
  
  return texture;
}

void gp_texture_ref(gp_texture* texture)
{
  gp_ref_inc(&texture->mRef);
}

void gp_texture_unref(gp_texture* texture)
{
  if(gp_ref_dec(&texture->mRef))
  {
    glDeleteTextures(1, &texture->mTexture);
#ifndef GP_WEB
    glDeleteBuffers(1, &texture->mPBO);
#endif
    free(texture);
  }
}

void gp_texture_set_data(gp_texture* texture, float* data, unsigned int width, unsigned int height)
{
  glBindTexture(GL_TEXTURE_2D, texture->mTexture);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  GLvoid* d = data;
#ifndef GP_WEB
  d = 0;
  
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, texture->mPBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, width*height*4*sizeof(float), 0, GL_STREAM_DRAW);
  
  if(data != 0)
  {
    GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, width*height*4*sizeof(float), GL_MAP_WRITE_BIT);
    if(ptr)
    {
        // update data directly on the mapped buffer
        memcpy(ptr, data, sizeof(float)*width*height*4);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
    }
  }
#endif
  
  glTexImage2D(GL_TEXTURE_2D,
    0,                            // Level of detail (mip-level) (0 is base image)
    GL_RGBA32F,                   // Internal format
    width,                        // Width
    height,                       // Height
    0,                            // Border
    GL_RGBA,                      // Image format
    GL_FLOAT,                     // Image type
    (GLvoid*)d                    // data
  );
  
#ifndef GP_WEB
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif
  glBindTexture(GL_TEXTURE_2D, texture->mTexture);
}

typedef struct
{
  gp_texture*     mTexture;
  float*          mData;
  unsigned int    mWidth;
  unsigned int    mHeight;
  void(*mCallback)(void*);
  void*           mUserData;
} _gp_texture_async;

void _gp_texture_async_func(void* data)
{
  _gp_texture_async* async = (_gp_texture_async*)data;
  
  glBindTexture(GL_TEXTURE_2D, async->mTexture->mTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  GLvoid* d = async->mData;
#ifndef GP_WEB
  d = 0;
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, async->mTexture->mPBO);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, async->mWidth*async->mHeight*4*sizeof(float), 0, GL_STREAM_DRAW);
  GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, async->mWidth*async->mHeight*4*sizeof(float), GL_MAP_WRITE_BIT);
  if(ptr)
  {
    // update data directly on the mapped buffer
    memcpy(ptr, async->mData, async->mWidth*async->mHeight*4*sizeof(float));
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
  }
#endif
  
  glTexImage2D(GL_TEXTURE_2D,
    0,                            // Level of detail (mip-level) (0 is base image)
    GL_RGBA32F,                   // Internal format
    async->mWidth,                // Width
    async->mHeight,               // Height
    0,                            // Border
    GL_RGBA,                      // Image format
    GL_FLOAT,                     // Image type
    (GLvoid*)d                    // data
  );
  
#ifndef GP_WEB
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
#endif
  glBindTexture(GL_TEXTURE_2D, 0);
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

void gp_texture_set_data_async(gp_texture* texture, float* data, unsigned int width, unsigned int height, void (*callback)(void*), void* userdata)
{
  _gp_texture_async* async = malloc(sizeof(_gp_texture_async));
  async->mTexture = texture;
  async->mData = data;
  async->mWidth = width;
  async->mHeight = height;
  async->mCallback = callback;
  async->mUserData = userdata;
  
  _gp_api_work(_gp_texture_async_func, _gp_texture_join_func, (void*)async);
}
