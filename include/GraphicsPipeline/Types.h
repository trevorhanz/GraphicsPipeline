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

//! \file Types.h

#ifndef __GP_TYPES_H__
#define __GP_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \typedef gp_system
 * \brief \ref System object.
 * Controls the main event loop and creates system specific rendering contexts.
 * 
 * \typedef gp_context
 * \brief \ref Context object.
 * Contains system specific rendering information.  Generates and stores graphics primatives.  All data generated by this context is shared among other primatives generated by this context.
 * 
 * \typedef gp_target
 * \brief \ref Target object.
 * Graphics primative that provides a surface to which can be rendered.
 * 
 * \typedef gp_frame_buffer
 * \brief \ref FrameBuffer object.
 * Graphics primative  that provides an offscreen surface to which can be rendered.
 * 
 * \typedef gp_array
 * \brief \ref Array object.
 * Graphics primative that stores an array of numerical data.
 * 
 * \typedef gp_array_data
 * \brief \ref ArrayData object.
 * Storage class array objects.
 * 
 * \typedef gp_texture
 * \brief \ref Texture object.
 * Graphics primative that stores multi-dimensional texture data.
 * 
 * \typedef gp_shader
 * \brief \ref Shader object.
 * Graphics primative that contains a shader program.
 * 
 * \typedef gp_shader_source
 * \brief \ref ShaderSource object
 * Storage class for shader source code yet to be compiled.
 * 
 * \typedef gp_uniform
 * \brief \ref Uniform object.
 * Graphics primative that stores data for a shader object's uniform variable.
 * 
 * \typedef gp_pipeline
 * \brief \ref Pipeline object.
 * Manages a list of rendering commands.
 * 
 * \typedef gp_operation
 * \brief A single rendering operation.
 * 
 * \typedef gp_timer
 * \brief Event timer object.
 * 
 * \typedef gp_io
 * \brief Event io object.
 */
typedef struct _gp_system gp_system;
typedef struct _gp_context gp_context;
typedef struct _gp_target gp_target;
typedef struct _gp_frame_buffer gp_frame_buffer;
typedef struct _gp_array gp_array;
typedef struct _gp_array_data gp_array_data;
typedef struct _gp_texture gp_texture;
typedef struct _gp_shader gp_shader;
typedef struct _gp_shader_source gp_shader_source;
typedef struct _gp_uniform gp_uniform;
typedef struct _gp_pipeline gp_pipeline;
typedef struct _gp_operation gp_operation;
typedef struct _gp_timer gp_timer;
typedef struct _gp_io gp_io;

typedef enum
{
  GP_SHADER_SOURCE_VERTEX,
  GP_SHADER_SOURCE_FRAGMENT
} GP_SHADER_SOURCE_TYPE;

/*!
 * \brief Callback function to be used by gp_timer objects.
 * \param timer Pointer to gp_timer object that timed out.
 */
typedef void(*gp_timer_callback)(gp_timer* timer);

/*!
 * \brief Callback function to be used by gp_io objects.
 * \param io Pointer to gp_io object with fd that is ready.
 */
typedef void(*gp_io_callback)(gp_io* io);


#ifdef __cplusplus
}

namespace GP
{
  class System;
  class Context;
  class Target;
  class Array;
  class Shader;
  class Pipeline;
  class Operation;
}

#endif // __cplusplus

#endif // __GP_TYPES_H__
