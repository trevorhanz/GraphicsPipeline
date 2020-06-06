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

#include <System.h>

#include "X11.h"

#include <GL/glew.h>
#include <GL/glx.h>

using namespace GP;

class GP::Window::Data
{
public:
  Display*          mDisplay;
  TargetPtr         mTarget;
};

GP::Window::Window(Init* init)
  : mData(new Data())
{
  mData->mDisplay = init->mDisplay;
  mData->mTarget = std::make_shared<Target>();
}

GP::Window::~Window()
{
  delete mData;
}

TargetPtr GP::Window::GetTarget()
{
  return mData->mTarget;
}

