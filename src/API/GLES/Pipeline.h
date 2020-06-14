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

#include <Context.h>
#include <Pipeline.h>

#include <list>

#include <GLES2/gl2.h>

namespace GP
{
  namespace GLES
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
    
    class PipelineBase : public GP::Pipeline
    {
    public:
      class Context;
      
      PipelineBase(GP::Context* context);
      ~PipelineBase() override;
      
      void AddOperation(OperationPtr operation) override;
      void RemoveOperation(OperationPtr operation) override;
      void ClearPipeline() override;
      
      void Execute() override;
      
    protected:
      virtual Context* CreateContext();
      
    private:
      GP::Context*                          mContext;
      std::list<GP::OperationPtr>           mOperations;
    };
    
    class PipelineBase::Context : public GP::Pipeline::Context
    {
    public:
      Context(PipelineBase* pipeline);
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
      PipelineBase*                     mPipeline;
      
      GP::TargetPtr                 mCurrentTarget;
      GP::ShaderPtr                 mCurrentShader;
    };
  }
}

#endif // __GP_PIPELINE_GL_H__

