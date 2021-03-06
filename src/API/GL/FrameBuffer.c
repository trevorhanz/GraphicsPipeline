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

#include <GraphicsPipeline/FrameBuffer.h>
#include <GraphicsPipeline/Texture.h>
#include <GraphicsPipeline/Logging.h>

#include "Config.h"

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#endif // GP_GL
#include "GL.h"

#include <stdlib.h>
#include <stdint.h>

void _gp_check_frame_buffer()
{
  int ret = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(ret != GL_FRAMEBUFFER_COMPLETE)
  {
    gp_log_debug("Ret: %x", ret);
    switch(ret)
    {
      case GL_FRAMEBUFFER_UNDEFINED:
        gp_log_warn("FrameBuffer is undefined");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        gp_log_warn("FrameBuffer has incomplete attachment");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        gp_log_warn("FrameBuffer has incomplete missing attachment");
        break;
      case GL_FRAMEBUFFER_UNSUPPORTED:
        gp_log_warn("FrameBuffer is not supported");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        gp_log_warn("FrameBuffer has incomplete multisample setup");
        break;
#ifdef GP_GL
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        gp_log_warn("FrameBuffer has incomplete draw buffer");
        break;
      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        gp_log_warn("FrameBuffer has incomplete layer targets");
        break;
#else
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        gp_log_warn("FrameBuffer has incomplete dimensions");
        break;
#endif
      default:
        gp_log_warn("FrameBuffer is not complete");
    }
  }
}

void _gp_frame_buffer_free(gp_object* object)
{
  gp_frame_buffer* fb = (gp_frame_buffer*)object;
  
  if(fb->mTexture)
    gp_object_unref((gp_object*)fb->mTexture);
  _gp_pipeline_free(fb->mPipeline);
  free(fb);
}

gp_frame_buffer* gp_frame_buffer_new(gp_context* context)
{
  gp_frame_buffer* fb = malloc(sizeof(gp_frame_buffer));
  _gp_object_init(&fb->mObject, _gp_frame_buffer_free);
  fb->mContext = context;
  
  _gp_api_context_make_current(fb->mContext);
  
  glGenFramebuffers(1, &fb->mFBO);
  glGenRenderbuffers(1, &fb->mRBO);
  fb->mTexture = 0;
  fb->mPipeline = _gp_pipeline_new();
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 768);  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->mRBO);
  
  _gp_check_frame_buffer();
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
  return fb;
}

gp_pipeline* gp_frame_buffer_get_pipeline(gp_frame_buffer* fb)
{
  return fb->mPipeline;
}

void gp_frame_buffer_redraw(gp_frame_buffer* fb)
{
  _gp_api_context_make_current(fb->mContext);
  
  int width, height;
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  
  _gp_api_prepare_window(width, height);
  
  _gp_pipeline_execute(fb->mPipeline);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
}

void gp_frame_buffer_attach(gp_frame_buffer* fb, gp_texture* texture)
{
  _gp_api_context_make_current(fb->mContext);
  
  if(fb->mTexture)
    gp_object_unref((gp_object*)fb->mTexture);
  fb->mTexture = texture;
  gp_object_ref((gp_object*)texture);
  
  int width, height;
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  gp_texture_data* data = gp_texture_data_new();
  gp_texture_data_set_2d(data, 0, GP_FORMAT_RGBA, GP_DATA_TYPE_FLOAT, width, height);
  gp_texture_set_data(fb->mTexture, data);
  gp_object_unref((gp_object*)data);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->mTexture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
}

void gp_frame_buffer_set_size(gp_frame_buffer* fb, int width, int height)
{
  _gp_api_context_make_current(fb->mContext);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  
  if(fb->mTexture)
  {
    gp_texture_data* data = gp_texture_data_new();
    gp_texture_data_set_2d(data, 0, GP_FORMAT_RGBA, GP_DATA_TYPE_FLOAT, width, height);
    gp_texture_set_data(fb->mTexture, data);
    gp_object_unref((gp_object*)data);
  }
  CHECK_GL_ERROR()
}

void gp_frame_buffer_get_size(gp_frame_buffer* fb, int* width, int* height)
{
  _gp_api_context_make_current(fb->mContext);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO); 
  
  if(width)
  {
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, width);
  }
  if(height)
  {
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, height);
  }
  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  CHECK_GL_ERROR()
}

void gp_frame_buffer_get_pixel(gp_frame_buffer* fb, int width, int height, gp_color* c)
{
  _gp_api_context_make_current(fb->mContext);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO);
  
  glReadPixels(width, height, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (uint8_t*)c);
  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  CHECK_GL_ERROR()
}


