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
#include <GraphicsPipeline/Windows.h>
#include "Context.h"
#include "Windows.h"

#include <GL/wglew.h>

#include <iostream>

using namespace GP;
using namespace std;

class System::Data
{
public:
  static LRESULT CALLBACK WndProc(HWND    hWnd,                   // Handle For This Window
    UINT    uMsg,                   // Message For This Window
    WPARAM  wParam,                 // Additional Message Information
    LPARAM  lParam)                 // Additional Message Information
  {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
};

System::System()
  : mData(new Data())
{
  WNDCLASS wc;                                          // Windows Class Structure
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;        // Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc = (WNDPROC)&System::Data::WndProc;  // WndProc Handles Messages
  wc.cbClsExtra = 0;                                    // No Extra Window Data
  wc.cbWndExtra = 0;                                    // No Extra Window Data
  wc.hInstance = GetModuleHandle(NULL);                 // Set The Instance
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);               // Load The Default Icon
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);             // Load The Arrow Pointer
  wc.hbrBackground = NULL;                              // No Background Required For GL
  wc.lpszMenuName = NULL;                               // We Don't Want A Menu
  wc.lpszClassName = "GraphicsPipeline";                // Set The Class Name

  if(!RegisterClass(&wc))                               // Attempt To Register The Window Class
  {
    MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }
}

System::~System()
{
  delete mData;
}
#undef CreateWindow
WindowPtr System::CreateWindow(const std::string& title, int width, int height)
{
  auto data = std::make_shared<WindowUserData>();
  return std::make_shared<Windows::Window>();
}

ContextPtr System::CreateContext()
{
  return std::make_shared<GL::Context>();
}

void System::Poll()
{
  MSG msg;                                              // Windows Message Structure
  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))           // Is There A Message Waiting?
  {
    TranslateMessage(&msg);                             // Translate The Message
    DispatchMessage(&msg);                              // Dispatch The Message
  }
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

}
