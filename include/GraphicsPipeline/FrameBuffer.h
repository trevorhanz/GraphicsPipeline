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
#include "Object.h"

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

/*!
 * Retrieve the size of the frame buffer in pixels.
 * \param fb Pointer to frame buffer object.
 * \param width Pointer where the frame buffer width value will be set.  Set as NULL to ignore.
 * \param height Pointer where the frame buffer height value will be set.  Set as NULL to ignore.
 */
GP_EXPORT void gp_frame_buffer_get_size(gp_frame_buffer* fb, int* width, int* height);

/*!
 * Retrieve a pixels value from the frame buffer.
 * \param fb Pointer to frame buffer object.
 * \param x X coordinate for the pixel.
 * \param y Y coordinate for the pixel.
 * \param c Pointer to a gp_color where the pixel value will be stored.
 */
GP_EXPORT void gp_frame_buffer_get_pixel(gp_frame_buffer* fb, int x, int y, gp_color* c);

//! \} // FrameBuffer

#ifdef __cplusplus
}

namespace GP
{
  /*!
   * \brief Wrapper class for ::gp_frame_buffer
   */
  class FrameBuffer : public Object
  {
  public:
    //! Constructor
    inline FrameBuffer(gp_frame_buffer* frame_buffer);
    
    //! Constructor
    inline FrameBuffer(const Context& context);
    
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
    
    /*!
     * Retrieve the size of the frame buffer in pixels.
     * \param width Pointer where the frame buffer width value will be set.  Set as NULL to ignore.
     * \param height Pointer where the frame buffer height value will be set.  Set as NULL to ignore.
     */
    inline void GetSize(int* width, int* height);
    
    /*!
     * Retrieve a pixels value from the frame buffer.
     * \param x X coordinate for the pixel.
     * \param y Y coordinate for the pixel.
     * \param c Pointer to a gp_color where the pixel value will be stored.
     */
    inline void GetPixel(int width, int height, gp_color* c);
  };
  
  //
  // Implementation
  //
  FrameBuffer::FrameBuffer(gp_frame_buffer* frame_buffer) : Object((gp_object*)frame_buffer) {}
  FrameBuffer::FrameBuffer(const Context& context) : Object((void*)gp_frame_buffer_new((gp_context*)context.GetObject())) {}
  Pipeline FrameBuffer::GetPipeline() {return Pipeline(gp_frame_buffer_get_pipeline((gp_frame_buffer*)GetObject()));}
  void FrameBuffer::Redraw() {gp_frame_buffer_redraw((gp_frame_buffer*)GetObject());}
  void FrameBuffer::Attach(const Texture& texture) {gp_frame_buffer_attach((gp_frame_buffer*)GetObject(), (gp_texture*)texture.GetObject());}
  void FrameBuffer::SetSize(int width, int height) {gp_frame_buffer_set_size((gp_frame_buffer*)GetObject(), width, height);}
  void FrameBuffer::GetSize(int* width, int* height) {gp_frame_buffer_get_size((gp_frame_buffer*)GetObject(), width, height);}
  void FrameBuffer::GetPixel(int width, int height, gp_color* c) {gp_frame_buffer_get_pixel((gp_frame_buffer*)GetObject(), width, height, c);}
}

#endif // __cplusplus

#endif // __GP_FRAMEBUFFER_H__
