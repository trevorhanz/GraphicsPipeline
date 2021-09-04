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

#ifndef __GP_GL_H__
#define __GP_GL_H__

#include <GraphicsPipeline/Types.h>
#include <GraphicsPipeline/Common.h>

#include "../../Utils/List.h"
#include "../../Utils/Object.h"
#include "../../Utils/RefCounter.h"

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/gl.h>
#else // __APPLE__
// OpenGL is deprecated since macOS 10.14
// Apple recommends porting to Metal
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif // __APPLE__
#else // GP_GL
#include <GLES3/gl3.h>
#endif // GP_GL

#define CHECK_GL_ERROR() \
{\
  GLenum err;\
  while((err = glGetError()) != GL_NO_ERROR) {\
    gp_log_error("GL ERROR (%d) at: %s:%d", err, __FILE__, __LINE__);\
  }\
}

struct _gp_texture_cache_list
{
  gp_list_node            mNode;
  gp_texture*             mTexture;
  int                     mIndex;
};
typedef struct _gp_texture_cache_list gp_texture_cache_list;

struct __gp_draw_context
{
  gp_shader*              mShader;          // Current shader in use
  gp_list                 mTextureCache;    // Current bound textures
};
typedef struct __gp_draw_context _gp_draw_context;

struct _gp_frame_buffer
{
  gp_object               mObject;
  GLuint                  mFBO;
  GLuint                  mRBO;
  gp_context*             mContext;
  gp_texture*             mTexture;
  gp_pipeline*            mPipeline;
};

struct _gp_array_data
{
  gp_object               mObject;
  void*                   mData;
  unsigned int            mSize;
  int                     mOffset;
};

struct _gp_array
{
  gp_object               mObject;
  GLuint                  mVBO;
};

struct _gp_texture_data
{
  gp_object               mObject;
  void*                   mData;
  GLuint                  mDimensions;
  GP_FORMAT               mFormat;
  GP_DATA_TYPE            mType;
  unsigned int            mWidth;
  unsigned int            mHeight;
};

struct _gp_texture
{
  gp_object               mObject;
  GLuint                  mDimensions;
  GLuint                  mTexture;
  GLuint                  mPBO;
  GLuint                  mWrapX;
  GLuint                  mWrapY;
};

struct _gp_shader_source
{
  gp_object               mObject;
  gp_list                 mSource;
};

struct _gp_shader
{
  gp_object               mObject;
  GLuint                  mProgram;
  GLuint                  mAttribute;
  gp_refcounter           mRef;
};

typedef void(*LoadUniform)(gp_uniform* uniform, _gp_draw_context* context);

struct _gp_uniform
{
  gp_object               mObject;
  GLuint                  mLocation;
  LoadUniform             mOperation;
  void*                   mData;
};

typedef void(*_gp_operation_function)(gp_operation* self, _gp_draw_context* context);
typedef void(*_gp_notification)(gp_operation* self);

struct _gp_operation
{
  gp_object               mObject;
  gp_list_node            mNode;
  _gp_operation_function  mFunc;
  gp_pipeline*            mPipeline;
  _gp_notification        mAdded;
  _gp_notification        mRemoved;
};

struct _gp_pipeline
{
  gp_list                 mOperations;
};

#ifdef __cplusplus
extern "C" {
#endif

gp_pipeline* _gp_pipeline_new();

void _gp_pipeline_free(gp_pipeline* pipeline);

void _gp_pipeline_execute(gp_pipeline* pipeline);

void _gp_pipeline_execute_with_context(gp_pipeline* pipeline, _gp_draw_context* context);

void _gp_api_init();

void _gp_api_init_context();

void _gp_api_work(void(*work)(void*), void(*join)(void*), void* data);

void _gp_api_prepare_window(unsigned int width, unsigned int height);

void _gp_api_context_make_current(gp_context* context);

#ifdef __cplusplus
}
#endif

#endif // __GP_GL_H__
