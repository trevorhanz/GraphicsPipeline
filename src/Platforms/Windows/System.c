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

#pragma comment(lib, "wsock32.lib")

#include <GL/glew.h>
#include <GL/wglew.h>

#include <GraphicsPipeline/System.h>
#include <GraphicsPipeline/Windows.h>
#include <GraphicsPipeline/Logging.h>
#include "API/GL/GL.h"
#include "Windows.h"
#include <winsock2.h>

#include <stdlib.h>
#include <stdio.h>

static LRESULT CALLBACK _gp_WndProc(HWND    hWnd,                   // Handle For This Window
                                    UINT    uMsg,                   // Message For This Window
                                    WPARAM  wParam,                 // Additional Message Information
                                    LPARAM  lParam)                 // Additional Message Information
{
  switch (uMsg)
  {
  case WM_NCCREATE:
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
    SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);

  case WM_CREATE:
    return 0;

  case WM_SIZE:
    return 0;

  case WM_DESTROY:
    return 0;
  
  case WM_PAINT:
    {
      gp_window* window = (gp_window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

      PAINTSTRUCT ps;
      HDC hDC = BeginPaint(hWnd, &ps);
      wglMakeCurrent(hDC, window->mContext);

      _gp_pipeline_execute(window->mPipeline);

      SwapBuffers(GetDC(window->mWindow));
      EndPaint(hWnd, &ps);
    }
    return 0;

  case WM_SOCKET:
    return 0;

  case WM_WORK_DONE:
    _gp_work_done(lParam);
    return 0;

  default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

void _gp_system_free(gp_object* object)
{
  gp_system* system = (gp_object*)object;

  free(system);
}

gp_system* gp_system_new()
{
  gp_system* system = malloc(sizeof(gp_system));
  _gp_object_init(&system->mObject, _gp_system_free);

  WNDCLASS wc;                                          // Windows Class Structure
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;        // Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc = (WNDPROC)&_gp_WndProc;               // WndProc Handles Messages
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
    free(system);
    return NULL;
  }

  system->mInternalWindow = CreateWindowEx(
    0,                              // Optional window styles.
    "GraphicsPipeline",             // Window class
    "",                             // Window text
    WS_OVERLAPPEDWINDOW,            // Window style

    // Size and position
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

    NULL,                           // Parent window    
    NULL,                           // Menu
    GetModuleHandle(NULL),          // Instance handle
    NULL                            // Additional application data
  );
  if (!system->mInternalWindow)
  {
    MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    free(system);
    return NULL;
  }

  return system;
}

void gp_system_run(gp_system* system)
{
  MSG msg;                                                  // Windows Message Structure
  while(1)
  {
    if (WaitMessage())
    {
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))         // Is There A Message Waiting?
      {
        TranslateMessage(&msg);                               // Translate The Message
        DispatchMessage(&msg);                                // Dispatch The Message
      }
    }
  }
}

void gp_system_stop(gp_system* system)
{

}

void _gp_timer_free(gp_object* object)
{
  gp_timer* timer = (gp_timer*)object;

  free(timer);
}

gp_timer* gp_timer_new(gp_system* system)
{
  gp_timer* timer = malloc(sizeof(gp_timer));
  _gp_object_init(&timer->mObject, _gp_timer_free);
  timer->mSystem = system;
  timer->mCallback = NULL;
  timer->mUserData = NULL;

  return timer;
}

void _gp_io_free(gp_object* object)
{
  gp_io* io = (gp_io*)object;

  free(io);
}

gp_io* gp_io_read_new(gp_system* system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  _gp_object_init(&io->mObject, _gp_io_free);

  WSAAsyncSelect(fd, system->mInternalWindow, WM_SOCKET, FD_READ);

  return io;
}

gp_io* gp_io_write_new(gp_system* system, int fd)
{
  gp_io* io = malloc(sizeof(gp_io));
  _gp_object_init(&io->mObject, _gp_io_free);
  
  WSAAsyncSelect(fd, system->mInternalWindow, WM_SOCKET, FD_WRITE);

  return io;
}
