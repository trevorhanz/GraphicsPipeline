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

System::System()
{
}

System::~System()
{
}

WindowPtr System::CreateWindow(const std::string& title, int width, int height)
{
  return std::make_shared<Web::Window>(title);
}

ContextPtr System::CreateContext()
{
  return std::make_shared<Web::Context>();
}

void System::Poll()
{
}

void System::Run()
{
}

