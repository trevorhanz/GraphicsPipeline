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
#include "GL.h"

void gp_array_set_data(gp_array* array, float* data, unsigned int count)
{
  glBindBuffer(GL_ARRAY_BUFFER, array->mVBO);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*count, data, GL_STATIC_DRAW);
}
