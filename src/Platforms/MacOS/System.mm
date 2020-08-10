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

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/MacOS.h>
#include "Context.h"
#include "MacOS.h"

#include <iostream>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

using namespace GP;
using namespace std;

class System::Data
{
public:
  NSApplication*                        mApp;
  static std::function<void()>          mExposeCallback;
  
  static void Callback();
};

std::function<void()> System::Data::mExposeCallback = NULL;

System::System()
  : mData(new Data())
{
  
}

System::~System()
{
  delete mData;
}

GP::WindowPtr System::CreateWindow(const std::string& title, int width, int height)
{
  return std::make_shared<MacOS::Window>();
}

ContextPtr System::CreateContext()
{
  return std::make_shared<GL::Context>(&System::Data::Callback);
}

void System::Poll()
{
  [NSApplication sharedApplication];
  [NSApp run];
}

void System::Run()
{
  while(true)
  {
    Poll();
  }
}

void System::SetExposeCallback(std::function<void()> callback)
{
  mData->mExposeCallback = callback;
}

void System::Data::Callback()
{
  printf("Callback\n");
  static int count = 0;
  if(++count == 1) return;
  mExposeCallback();
}
