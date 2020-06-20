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

#ifndef __GP_CONTEXT_X11_H__
#define __GP_CONTEXT_X11_H__

#include <GraphicsPipeline/GP.h>
#include <API/GL/Pipeline.h>

#include <GL/glew.h>
#include <GL/glx.h>

namespace GP
{
  namespace GL
  {
    class Context : public ContextBase
    {
    public:
      Context(Display* display, XVisualInfo* vi, ::Window window);
      virtual ~Context();
      
      virtual PipelinePtr CreatePipeline();
      
      TargetUserDataPtr CreateTarget() override;
      
      void Bind(GP::TargetPtr target) override;
      
    private:
      Display*                mDisplay;
      XVisualInfo*            mVisualInfo;
      ::Window                mWindow;
      Colormap                mColorMap;
      GLXContext              mShare;
    };
  }
};


#endif // __GP_CONTEXT_X11_H__
