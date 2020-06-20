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

#include <GraphicsPipeline/Types.h>

#include <cstring>

using namespace GP;

/*
 * Color
 */
Color::Color()
  : mColor{0, 0, 0, 1}
{
}

Color::Color(float r, float g, float b, float a)
  : mColor{r, g, b, a}
{
}

void Color::SetR(float r)
{
  mColor[0] = r;
}

void Color::SetG(float g)
{
  mColor[1] = g;
}

void Color::SetB(float b)
{
  mColor[2] = b;
}

void Color::SetA(float a)
{
  mColor[3] = a;
}

float Color::GetR() const
{
  return mColor[0];
}

float Color::GetG() const
{
  return mColor[1];
}

float Color::GetB() const
{
  return mColor[2];
}

float Color::GetA() const
{
  return mColor[3];
}

/*
 * ArrayData
 */
ArrayData::ArrayData()
{
}

void ArrayData::SetData(float* data, unsigned int count)
{
  mData.resize(count);
  memcpy(&mData[0], data, sizeof(float)*count);
}

int ArrayData::GetSize() const
{
  return mData.size();
}

const float* ArrayData::GetData() const
{
  return mData.data();
}
