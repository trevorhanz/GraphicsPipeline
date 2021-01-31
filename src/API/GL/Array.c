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

#include <GraphicsPipeline/Array.h>
#include <GraphicsPipeline/Logging.h>

#ifdef GP_GL
#ifndef __APPLE__
#include <GL/glew.h>
#endif // __APPLE__
#endif // GP_GL
#include "GL.h"

#include <stdlib.h>

gp_array* gp_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(gp_array));
  
  glGenBuffers(1, &array->mVBO);
  gp_ref_init(&array->mRef);
  
  return array;
}

void gp_array_ref(gp_array* array)
{
  gp_ref_inc(&array->mRef);
}

void gp_array_unref(gp_array* array)
{
  if(gp_ref_dec(&array->mRef))
  {
    glDeleteBuffers(1, &array->mVBO);
    free(array);
  }
}

void gp_array_set_data(gp_array* array, float* data, unsigned int count)
{
  glBindBuffer(GL_ARRAY_BUFFER, array->mVBO);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*count, data, GL_STATIC_DRAW);
}
