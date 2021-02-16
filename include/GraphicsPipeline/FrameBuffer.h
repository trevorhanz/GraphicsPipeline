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

//! \file FrameBuffer.h

#ifndef __GP_FRAMEBUFFER_H__
#define __GP_FRAMEBUFFER_H__

#include "Common.h"
#include "Types.h"
#include "Pipeline.h"
#include "Texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \defgroup FrameBuffer
 * \{
 */

/*!
 * Create a new gp_frame_buffer object tied to a context.
 * \param context Context object used to create the frame buffer.
 * \return Newly created frame buffer.
 */
GP_EXPORT gp_frame_buffer* gp_frame_buffer_new(gp_context* context);

/*!
 * Increase frame buffer object reference count.
 * \param fb FrameBuffer object for which to increase the ref count.
 */
GP_EXPORT void gp_frame_buffer_ref(gp_frame_buffer* fb);

/*!
 * Decrease frame buffer object reference count.
 * \param fb FrameBuffer object for which to decrease the ref count.
 */
GP_EXPORT void gp_frame_buffer_unref(gp_frame_buffer* fb);

/*!
 * Get the gp_pipeline tied to a gp_frame_buffer.
 * \param fb Pointer to frame buffer object.
 * \return Pointer to pipeline object.
 */
GP_EXPORT gp_pipeline* gp_frame_buffer_get_pipeline(gp_frame_buffer* fb);

/*!
 * Schedule the frame buffer to be redrawn.
 * \param fb Pointer to frame buffer object.
 */
GP_EXPORT void gp_frame_buffer_redraw(gp_frame_buffer* fb);

/*!
 * Attach a gp_texture to output of frame buffer.
 * \param fb Pointer to frame buffer object.
 * \param texture Texture object to be attached.
 */
GP_EXPORT void gp_frame_buffer_attach(gp_frame_buffer* fb, gp_texture* texture);

/*!
 * Set the size of the frame buffer in pixels.
 * \param fb Pointer to frame buffer object.
 * \param width Number of pixels wide.
 * \param height Number of pixels high.
 */
GP_EXPORT void gp_frame_buffer_set_size(gp_frame_buffer* fb, int width, int height);

//! \} // FrameBuffer

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_frame_buffer
   */
  class FrameBuffer
  {
  public:
    //! Constructor
    inline FrameBuffer(const Context& context);
    
    //! Copy Constructor
    inline FrameBuffer(const FrameBuffer& other);
    
    //! Destructor
    inline ~FrameBuffer();
    
    /*!
     * Get the gp_pipeline tied to a gp_frame_buffer.
     * \return GP::Pipeline used by this frame buffer.
     */
    inline Pipeline GetPipeline();
    
    /*!
     * Schedule the frame buffer to be redrawn.
     */
    inline void Redraw();
    
    /*!
     * Attach a GP::Texture to output of frame buffer.
     * \param texture Texture object to be attached.
     */
    inline void Attach(const Texture& texture);
    
    /*!
     * Set the size of the frame buffer in pixels.
     * \param width Number of pixels wide.
     * \param height Number of pixels high.
     */
    inline void SetSize(int width, int height);
    
    //! Equal operator
    inline const FrameBuffer& operator = (const FrameBuffer& other);
    
  private:
    gp_frame_buffer*                mFrameBuffer;
  };
  
  //
  // Implementation
  //
  FrameBuffer::FrameBuffer(const Context& context) : mFrameBuffer(gp_frame_buffer_new(context.mContext)) {}
  FrameBuffer::FrameBuffer(const FrameBuffer& other)
  {
    mFrameBuffer = other.mFrameBuffer;
    gp_frame_buffer_ref(mFrameBuffer);
  }
  FrameBuffer::~FrameBuffer() {gp_frame_buffer_unref(mFrameBuffer);}
  Pipeline FrameBuffer::GetPipeline() {return Pipeline(gp_frame_buffer_get_pipeline(mFrameBuffer));}
  void FrameBuffer::Redraw() {gp_frame_buffer_redraw(mFrameBuffer);}
  void FrameBuffer::Attach(const Texture& texture) {gp_frame_buffer_attach(mFrameBuffer, texture.mTexture);}
  void FrameBuffer::SetSize(int width, int height) {gp_frame_buffer_set_size(mFrameBuffer, width, height);}
}

#endif // __cplusplus

#endif // __GP_FRAMEBUFFER_H__
