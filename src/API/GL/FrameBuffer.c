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

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#endif // GP_GL
#include "GL.h"

#include <stdlib.h>

#define CHECK_GL_ERROR() \
{\
  GLenum err;\
  while((err = glGetError()) != GL_NO_ERROR) {\
    gp_log_error("GL ERROR (%d) at: %s:%d", err, __FILE__, __LINE__);\
  }\
}

gp_frame_buffer* gp_frame_buffer_new(gp_context* context)
{
  gp_frame_buffer* fb = malloc(sizeof(gp_frame_buffer));
  
  glGenFramebuffers(1, &fb->mFBO);
  glGenRenderbuffers(1, &fb->mRBO);
  fb->mTexture = 0;
  fb->mPipeline = _gp_pipeline_new();
  gp_ref_init(&fb->mRef);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glBindRenderbuffer(GL_RENDERBUFFER, fb->mRBO); 
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->mRBO);
  
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    gp_log_warn("FrameBuffer is not complete");
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
  return fb;
}

void gp_frame_buffer_ref(gp_frame_buffer* fb)
{
  gp_ref_inc(&fb->mRef);
}

void gp_frame_buffer_unref(gp_frame_buffer* fb)
{
  if(gp_ref_dec(&fb->mRef))
  {
    if(fb->mTexture)
      gp_texture_unref(fb->mTexture);
    _gp_pipeline_free(fb->mPipeline);
    free(fb);
  }
}

gp_pipeline* gp_frame_buffer_get_pipeline(gp_frame_buffer* fb)
{
  return fb->mPipeline;
}

void gp_frame_buffer_redraw(gp_frame_buffer* fb)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  _gp_pipeline_execute(fb->mPipeline);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
}

void gp_frame_buffer_attach(gp_frame_buffer* fb, gp_texture* texture)
{
  if(fb->mTexture)
    gp_texture_unref(fb->mTexture);
  fb->mTexture = texture;
  gp_texture_ref(texture);
  glBindFramebuffer(GL_FRAMEBUFFER, fb->mFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->mTexture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  CHECK_GL_ERROR()
}
