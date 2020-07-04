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

#include "Context.h"
#include "Windows.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <winuser.h>
#include <GL/wglew.h>


using namespace GP::GL;
using namespace std;

Context::Context()
{
  // Create The Window
  HWND hWnd;
  if (!(hWnd = CreateWindowEx(0,                                          // Extended Style For The Window
    "GraphicsPipeline",                                   // Class Name
    "",                                         // Window Title
    0,                            // Required Window Style,                                          // Required Window Style
    0, 0,                                       // Window Position
    1,                                          // Calculate Window Width
    4180,                                          // Calculate Window Height
    NULL,                                       // No Parent Window
    NULL,                                       // No Menu
    GetModuleHandle(NULL),                      // Instance
    NULL)))                                     // Pass a pointer to this window
  {
    MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  HDC hDC = GetDC(hWnd);

  GLuint                PixelFormat;                                    // Holds The Results After Searching For A Match
  static      PIXELFORMATDESCRIPTOR pfd =                                // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),                                      // Size Of This Pixel Format Descriptor
    1,                                                                  // Version Number
    PFD_DRAW_TO_WINDOW |                                                // Format Must Support Window
    PFD_SUPPORT_OPENGL |                                                // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,                                                   // Must Support Double Buffering
    PFD_TYPE_RGBA,                                                      // Request An RGBA Format
    (BYTE)0,                                                           // Select Our Color Depth
    0, 0, 0, 0, 0, 0,                                                   // Color Bits Ignored
    0,                                                                  // No Alpha Buffer
    0,                                                                  // Shift Bit Ignored
    0,                                                                  // No Accumulation Buffer
    0, 0, 0, 0,                                                         // Accumulation Bits Ignored
    16,                                                                 // 16Bit Z-Buffer (Depth Buffer)
    0,                                                                  // No Stencil Buffer
    0,                                                                  // No Auxiliary Buffer
    PFD_MAIN_PLANE,                                                     // Main Drawing Layer
    0,                                                                  // Reserved
    0, 0, 0                                                             // Layer Masks Ignored
  };

  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))                       // Did Windows Find A Matching Pixel Format?
  {
    MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  if (!SetPixelFormat(hDC, PixelFormat, &pfd))                            // Are We Able To Set The Pixel Format?
  {
    MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  HGLRC hRC;
  if (!(hRC = wglCreateContext(hDC)))                             // Are We Able To Get A Rendering Context?
  {
    MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }
  if (!wglMakeCurrent(hDC, hRC))                                 // Try To Activate The Rendering Context
  {
    MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  if (wglChoosePixelFormatARB)
  {
    int                 pixelFormat;
    int                 valid;
    UINT                numFormats;
    float               fAttributes[] = { 0,0 };

    // These Attributes Are The Bits We Want To Test For In Our Sample
    // Everything Is Pretty Standard, The Only One We Want To 
    // Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
    // These Two Are Going To Do The Main Testing For Whether Or Not
    // We Support Multisampling On This Hardware.
    int iAttributes[] =
    {
      WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
      WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
      WGL_COLOR_BITS_ARB,24,
      WGL_ALPHA_BITS_ARB,8,
      WGL_DEPTH_BITS_ARB,16,
      WGL_STENCIL_BITS_ARB,0,
      WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
      WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
      WGL_SAMPLES_ARB,8,
      0,0
    };

    // First We Check To See If We Can Get A Pixel Format For 8 Samples
    valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

    // If We Returned True, And Our Format Count Is Greater Than 1
    if (!valid || numFormats < 1)
    {
      // Our Pixel Format With 8 Samples Failed, Test For 4 Samples
      iAttributes[19] = 4;
      valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
    }

    if (valid) mPixelFormat = pixelFormat;

    // Recreate the window with the new pixel format
    DestroyWindow(hWnd);
    {
      DWORD                 dwExStyle;                                      // Window Extended Style
      DWORD                 dwStyle;                                        // Window Style
      RECT                  WindowRect;                                     // Grabs Rectangle Upper Left / Lower Right Values
      WindowRect.left = (long)0;                                        // Set Left Value To 0
      WindowRect.right = (long)720;                                    // Set Right Value To Requested Width
      WindowRect.top = (long)0;                                        // Set Top Value To 0
      WindowRect.bottom = (long)480;                                   // Set Bottom Value To Requested Height
      HWND                  hWnd;

      static      PIXELFORMATDESCRIPTOR pfd =                                // pfd Tells Windows How We Want Things To Be
      {
        sizeof(PIXELFORMATDESCRIPTOR),                                      // Size Of This Pixel Format Descriptor
        1,                                                                  // Version Number
        PFD_DRAW_TO_WINDOW |                                                // Format Must Support Window
        PFD_SUPPORT_OPENGL |                                                // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                                                   // Must Support Double Buffering
        PFD_TYPE_RGBA,                                                      // Request An RGBA Format
        (BYTE)0,                                                           // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                                                   // Color Bits Ignored
        0,                                                                  // No Alpha Buffer
        0,                                                                  // Shift Bit Ignored
        0,                                                                  // No Accumulation Buffer
        0, 0, 0, 0,                                                         // Accumulation Bits Ignored
        16,                                                                 // 16Bit Z-Buffer (Depth Buffer)
        0,                                                                  // No Stencil Buffer
        0,                                                                  // No Auxiliary Buffer
        PFD_MAIN_PLANE,                                                     // Main Drawing Layer
        0,                                                                  // Reserved
        0, 0, 0                                                             // Layer Masks Ignored
      };

      if (false) // fullscreen                                                       // Are We Still In Fullscreen Mode?
      {
        dwExStyle = WS_EX_APPWINDOW;                                          // Window Extended Style
        dwStyle = WS_POPUP;                                                   // Windows Style
      }
      else
      {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;                       // Window Extended Style
        dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
      }

      AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);           // Adjust Window To True Requested Size

      // Create The Window
      if (!(hWnd = CreateWindowEx(dwExStyle,                                  // Extended Style For The Window
        "GraphicsPipeline",                                   // Class Name
        "Title",                                      // Window Title
        dwStyle |                                   // Defined Window Style
        WS_CLIPSIBLINGS |                           // Required Window Style
        WS_CLIPCHILDREN,                            // Required Window Style
        0, 0,                                       // Window Position
        WindowRect.right - WindowRect.left,           // Calculate Window Width
        WindowRect.bottom - WindowRect.top,           // Calculate Window Height
        NULL,                                       // No Parent Window
        NULL,                                       // No Menu
        GetModuleHandle(NULL),                      // Instance
        NULL)))                                     // Pass a pointer to this window
      {
        MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
        return;
      }

      if (mPixelFormat > 0)
        if (!SetPixelFormat(GetDC(hWnd), mPixelFormat, &pfd))                   // Are We Able To Set The Pixel Format?
        {
          MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
          return;
        }
      mWindow = hWnd;
    }

    HDC hDC = GetDC(mWindow);
    if (!(mShare = wglCreateContext(hDC)))                             // Are We Able To Get A Rendering Context?
    {
      MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
      return;
    }

    if (!wglMakeCurrent(hDC, mShare))                                 // Try To Activate The Rendering Context
    {
      MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
      return;
    }
  }
  else
  {
    MessageBox(NULL, "Can't Find Pixel Format.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  glewInit();

  OSVERSIONINFOEX version;
  version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  GetVersionEx((LPOSVERSIONINFO)&version);

  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);

  // Try to identify the Windows version
  string OS = "";
  if (version.dwMajorVersion == 10)
  {
    if (version.wProductType == VER_NT_WORKSTATION) OS = "Windows 10";
    else OS = "Windows Server 2016";
  }
  else if (version.dwMajorVersion == 6)
  {
    if (version.dwMinorVersion == 3)
    {
      if (version.wProductType == VER_NT_WORKSTATION) OS = "Windows 8.1";
      else OS = "Windows Server 2012 R2";
    }
    else if (version.dwMinorVersion == 2)
    {
      if (version.wProductType == VER_NT_WORKSTATION) OS = "Windows 8";
      else OS = "Windows Server 2012";
    }
    else if (version.dwMinorVersion == 1)
    {
      if (version.wProductType == VER_NT_WORKSTATION) OS = "Windows 7";
      else OS = "Windows Server 2008 R2";
    }
    else if (version.dwMinorVersion == 0)
    {
      if (version.wProductType == VER_NT_WORKSTATION) OS = "Windows Vista";
      else OS = "Windows Server 2008";
    }
  }
  else if (version.dwMajorVersion == 5)
  {
    if (version.dwMinorVersion == 2)
    {
      if (version.wSuiteMask & VER_SUITE_WH_SERVER) OS = "Windows Home Server";
      else if ((version.wProductType == VER_NT_WORKSTATION) && (sys_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)) OS = "Windows XP Pro";
      else if (GetSystemMetrics(SM_SERVERR2) == 0) OS = "Windows Server 2003";
      else OS = "Windows Server 2003 R2";
    }
    else if (version.dwMinorVersion == 1) OS = "Windows XP";
    else if (version.dwMinorVersion == 0) OS = "Windows 2000";
  }
  if (OS == "")
  {
    stringstream ss;
    ss << "Windows " << version.dwMajorVersion << "." << version.dwMinorVersion;
    OS = ss.str();
  }

  cout << "OS: " << OS << endl;

  int major, minor;
  glGetIntegerv(GL_MAJOR_VERSION, &major);
  glGetIntegerv(GL_MINOR_VERSION, &minor);
  cout << "OpenGL Version: " << major << "." << minor << endl << endl;
}

Context::~Context()
{
}

GP::PipelinePtr Context::CreatePipeline()
{
  return std::make_shared<Pipeline>(this);
}

TargetUserDataPtr Context::CreateTarget()
{
  DWORD                 dwExStyle;                                      // Window Extended Style
  DWORD                 dwStyle;                                        // Window Style
  RECT                  WindowRect;                                     // Grabs Rectangle Upper Left / Lower Right Values
  WindowRect.left = (long)0;                                        // Set Left Value To 0
  WindowRect.right = (long)720;                                    // Set Right Value To Requested Width
  WindowRect.top = (long)0;                                        // Set Top Value To 0
  WindowRect.bottom = (long)480;                                   // Set Bottom Value To Requested Height
  HWND                  hWnd;

  static      PIXELFORMATDESCRIPTOR pfd =                                // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),                                      // Size Of This Pixel Format Descriptor
    1,                                                                  // Version Number
    PFD_DRAW_TO_WINDOW |                                                // Format Must Support Window
    PFD_SUPPORT_OPENGL |                                                // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,                                                   // Must Support Double Buffering
    PFD_TYPE_RGBA,                                                      // Request An RGBA Format
    (BYTE)0,                                                           // Select Our Color Depth
    0, 0, 0, 0, 0, 0,                                                   // Color Bits Ignored
    0,                                                                  // No Alpha Buffer
    0,                                                                  // Shift Bit Ignored
    0,                                                                  // No Accumulation Buffer
    0, 0, 0, 0,                                                         // Accumulation Bits Ignored
    16,                                                                 // 16Bit Z-Buffer (Depth Buffer)
    0,                                                                  // No Stencil Buffer
    0,                                                                  // No Auxiliary Buffer
    PFD_MAIN_PLANE,                                                     // Main Drawing Layer
    0,                                                                  // Reserved
    0, 0, 0                                                             // Layer Masks Ignored
  };

  if (false)//fullscreen                                                       // Are We Still In Fullscreen Mode?
  {
    dwExStyle = WS_EX_APPWINDOW;                                          // Window Extended Style
    dwStyle = WS_POPUP;                                                   // Windows Style
  }
  else
  {
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;                       // Window Extended Style
    dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
  }

  AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);           // Adjust Window To True Requested Size

  // Create The Window
  if (!(hWnd = CreateWindowEx(dwExStyle,                                  // Extended Style For The Window
    "GraphicsPipeline",                                   // Class Name
    "WindowTitle",                                      // Window Title
    dwStyle |                                   // Defined Window Style
    WS_CLIPSIBLINGS |                           // Required Window Style
    WS_CLIPCHILDREN,                            // Required Window Style
    0, 0,                                       // Window Position
    WindowRect.right - WindowRect.left,           // Calculate Window Width
    WindowRect.bottom - WindowRect.top,           // Calculate Window Height
    NULL,                                       // No Parent Window
    NULL,                                       // No Menu
    GetModuleHandle(NULL),                      // Instance
    NULL)))                                     // Pass a pointer to this window
  {
    MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }
  
  HDC hDC = GetDC(hWnd);

  if (mPixelFormat > 0)
    if (!SetPixelFormat(hDC, mPixelFormat, &pfd))                   // Are We Able To Set The Pixel Format?
    {
      MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
      return NULL;
    }

  HGLRC hRC;
  if (!(hRC = wglCreateContext(hDC)))                             // Are We Able To Get A Rendering Context?
  {
    MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }
  if (!wglMakeCurrent(hDC, hRC))                                 // Try To Activate The Rendering Context
  {
    MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    return NULL;
  }

  wglShareLists(mShare, hRC);

  GP::TargetUserDataPtr ptr = std::make_shared<GP::TargetUserData>();
  ptr->mWindow = hWnd;
  ptr->mContext = hRC;

  ::ShowWindow(hWnd, SW_SHOW);

  return ptr;
}

void Context::Bind(GP::TargetPtr target)
{
  
}


