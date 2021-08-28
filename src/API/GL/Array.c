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

void _gp_array_data_free(gp_object* object)
{
  gp_array_data* data = (gp_array_data*)object;
  
  if(data->mData) free(data->mData);
  free(data);
}

gp_array_data* gp_array_data_new()
{
  gp_array_data* data = malloc(sizeof(gp_array_data));
  _gp_object_init(&data->mObject, _gp_array_data_free);
  data->mData = NULL;
  data->mSize = 0;
  
  return data;
}

gp_array_data* gp_array_data_new_with_size(unsigned int size)
{
  gp_array_data* data = malloc(sizeof(gp_array_data));
  _gp_object_init(&data->mObject, _gp_array_data_free);
  data->mData = malloc(size);
  data->mSize = size;
  
  return data;
}

void gp_array_data_allocate(gp_array_data* ad, unsigned int size)
{
  if(ad->mData == NULL) ad->mData = malloc(size);
  
  ad->mSize = size;
}

void gp_array_data_set(gp_array_data* ad, void* data, unsigned int size)
{
  if(ad->mData == NULL) ad->mData = malloc(size);
  
  memcpy(ad->mData, data, size);
  ad->mSize = size;
}

void* gp_array_data_get_data(gp_array_data* ad)
{
  return ad->mData;
}

unsigned int gp_array_data_get_size(gp_array_data* ad)
{
  return ad->mSize;
}

void _gp_array_free(gp_object* object)
{
  gp_array* array = (gp_array*)object;
  
  glDeleteBuffers(1, &array->mVBO);
  free(array);
}

gp_array* gp_array_new(gp_context* context)
{
  gp_array* array = malloc(sizeof(gp_array));
  _gp_object_init(&array->mObject, _gp_array_free);
  glGenBuffers(1, &array->mVBO);
  
  return array;
}

void gp_array_set_data(gp_array* array, gp_array_data* data)
{
  glBindBuffer(GL_ARRAY_BUFFER, array->mVBO);
  
  glBufferData(GL_ARRAY_BUFFER, data->mSize, data->mData, GL_STATIC_DRAW);
}

typedef struct
{
  gp_array*       mArray;
  gp_array_data*  mData;
  void(*mCallback)(void*);
  void*           mUserData;
} _gp_array_async;

void _gp_array_async_func(void* userdata)
{
  _gp_array_async* async = (_gp_array_async*)userdata;
  
  glBindBuffer(GL_ARRAY_BUFFER, async->mArray->mVBO);
  
  glBufferData(GL_ARRAY_BUFFER, async->mData->mSize, async->mData->mData, GL_STATIC_DRAW);
}

void _gp_array_join_func(void* userdata)
{
  _gp_array_async* async = (_gp_array_async*)userdata;
  
  if(async->mCallback)
  {
    async->mCallback(async->mUserData);
  }
  
  free(async);
}

void gp_array_set_data_async(gp_array* array, gp_array_data* data, void (*callback)(void*), void* userdata)
{
  _gp_array_async* async = malloc(sizeof(_gp_array_async));
  async->mArray = array;
  async->mData = data;
  async->mCallback = callback;
  async->mUserData = userdata;
  
  _gp_api_work(_gp_array_async_func, _gp_array_join_func, async);
}
