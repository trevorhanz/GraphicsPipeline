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

#include <GraphicsPipeline/Operations.h>

using namespace GP;

/*
 * Operation
 */
Operation::Operation()
{
}

Operation::~Operation()
{
}

/*
 * LoadArray
 */
LoadArray::LoadArray()
{
}

void LoadArray::SetArray(ArrayPtr array)
{
  mArray = array;
}

void LoadArray::SetData(ArrayDataPtr data)
{
  mData = data;
}

void LoadArray::Execute(Pipeline::Context* context)
{
  context->LoadArray(mArray, mData);
}

/*
 * LoadShader
 */
LoadShader::LoadShader()
{
}

void LoadShader::SetShader(ShaderPtr shader)
{
  mShader = shader;
}

void LoadShader::SetVertexCode(const std::string& code)
{
  mVertex = code;
}

void LoadShader::SetFragmentCode(const std::string& code)
{
  mFragment = code;
}

void LoadShader::Execute(Pipeline::Context* context)
{
  context->LoadShader(mShader, mVertex.c_str(), mFragment.c_str());
}

/*
 * Draw
 */
Draw::Draw()
{
}

void Draw::SetShader(ShaderPtr shader)
{
  mShader = shader;
}

void Draw::SetArray(ArrayPtr array)
{
  mArray = array;
}

void Draw::Execute(Pipeline::Context* context)
{
  context->SetShader(mShader);
  context->AttachArray(mArray, "");
  context->Draw();
}

/*
 * SetTarget
 */
void TargetFocus::SetTarget(TargetPtr target)
{
  mTarget = target;
};

void TargetFocus::Execute(Pipeline::Context* context)
{
  context->SetTarget(mTarget);
}

/*
 * Clear
 */
void Clear::Execute(Pipeline::Context* context)
{
  context->ClearColor();
  context->ClearDepth();
}


