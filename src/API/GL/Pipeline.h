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
 
#ifndef __GP_PIPELINE_GL_H__
#define __GP_PIPELINE_GL_H__

#include <GraphicsPipeline/Context.h>
#include <GraphicsPipeline/Pipeline.h>

#include <list>

namespace GP
{
  namespace GL
  {
    class TargetUserData : public GP::UserData
    {
    public:
      virtual void MakeCurrent() = 0;
      virtual void Present() = 0;
    };
    typedef std::shared_ptr<TargetUserData> TargetUserDataPtr;
    
    class ContextBase : public GP::Context
    {
    public:
      virtual TargetUserDataPtr CreateTarget() = 0;
    };
    
    class Pipeline : public GP::Pipeline
    {
    public:
      class Context;
      
      Pipeline(ContextBase* context);
      ~Pipeline() override;
      
      void AddOperation(OperationPtr operation) override;
      void RemoveOperation(OperationPtr operation) override;
      void ClearPipeline() override;
      
      void Execute() override;
      
    protected:
      virtual Context* CreateContext();
      
    private:
      ContextBase*                          mContext;
      std::list<GP::OperationPtr>           mOperations;
    };
    
    class Pipeline::Context : public GP::Pipeline::Context
    {
    public:
      Context(Pipeline* pipeline);
      ~Context() override;
      void SetTarget(TargetPtr target) override;
      void ClearColor() override;
      void ClearDepth() override;
      void LoadArray(ArrayPtr array, ArrayDataPtr data) override;
      void LoadShader(ShaderPtr shader, const char* vertex, const char* fragment) override;
      void SetShader(ShaderPtr shader) override;
      void AttachArray(ArrayPtr array, const std::string& name) override;
      void Draw() override;
      
    private:
      int CheckShaderStatus(unsigned int shader, const char* type);
      
      Pipeline*                     mPipeline;
      
      GP::TargetPtr                 mCurrentTarget;
      GP::ShaderPtr                 mCurrentShader;
    };
  }
}

#endif // __GP_PIPELINE_GL_H__
