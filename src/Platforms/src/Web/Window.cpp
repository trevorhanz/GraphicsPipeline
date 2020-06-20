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
#include "Context.h"
#include "Web.h"

using namespace GP;

Window::Window(UserDataPtr userData)
  : mUserData(userData)
{
}

Window::~Window()
{
}

TargetPtr Window::GetTarget()
{
  auto data = std::dynamic_pointer_cast<WindowUserData>(mUserData);
  return data->mTarget;
}

Web::Window::Window(const std::string& id)
  : GP::Window(std::make_shared<WindowUserData>())
{
  auto data = std::dynamic_pointer_cast<WindowUserData>(mUserData);
  data->mTarget = std::make_shared<Web::Target>(id);
}
