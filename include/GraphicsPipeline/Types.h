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

typedef struct _gp_system gp_system;
typedef struct _gp_context gp_context;
typedef struct _gp_target gp_target;
typedef struct _gp_array gp_array;
typedef struct _gp_shader gp_shader;
typedef struct _gp_pipeline gp_pipeline;
typedef struct _gp_operation gp_operation;

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
