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
#include <string.h>

gp_array_data* gp_array_data_new()
{
  gp_array_data* data = malloc(sizeof(gp_array_data));
  
  data->mData = NULL;
  data->mCount = 0;
  gp_ref_init(&data->mRef);
  
  return data;
}

void gp_array_data_ref(gp_array_data* data)
{
  gp_ref_inc(&data->mRef);
}

void gp_array_data_unref(gp_array_data* data)
{
  if(gp_ref_dec(&data->mRef))
  {
    if(data->mData) free(data->mData);
    free(data);
  }
}

void gp_array_data_set(gp_array_data* ad, float* data, unsigned int count)
{
  const size_t size = sizeof(float)*count;
  
  if(ad->mData == NULL) ad->mData = malloc(size);
  
  memcpy(ad->mData, data, size);
  ad->mCount = count;
}

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

void gp_array_set_data(gp_array* array, gp_array_data* data)
{
  glBindBuffer(GL_ARRAY_BUFFER, array->mVBO);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*data->mCount, data->mData, GL_STATIC_DRAW);
}
