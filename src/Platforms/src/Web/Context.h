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

#ifndef __GP_WEB_ASSEMBLY_H__
#define __GP_WEB_ASSEMBLY_H__

#include <GraphicsPipeline.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <emscripten.h>
#include <emscripten/html5.h>

#include <list>

namespace GP
{
  namespace Web
  {
    class Target : public GP::Target
    {
    public:
      Target(std::string selector);
      
    private:
      EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mContext;
      GLuint                                vbo;
      GLuint                                shaderProgram;
      GLint                                 posAttrib;
      
      friend class Pipeline;
    };
    typedef std::shared_ptr<Target> TargetPtr;
    
    class Context : public GP::Context
    {
    public:
      Context();
      GP::PipelinePtr CreatePipeline() override;
      
      void Bind(GP::TargetPtr target) override;
      
    private:
      EMSCRIPTEN_WEBGL_CONTEXT_HANDLE       mShare;
    };
    typedef std::shared_ptr<Context> ContextPtr;
  }
}


#endif // __GP_WEB_ASSEMBLY_H__
