/************************************************************************
* Copyright (C) 2021 Trevor Hanz
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

#include <GraphicsPipeline/GP.h>
#include <GraphicsPipeline/Qt5.h>

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>

#include "../Common.h"

using namespace GP;

const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_Position = vec4(position.xyz, 1.0);
  });
const char* fragmentSource = GLSL(
  out highp vec4 fragColor;
  void main()
  {
    fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
  });

float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  
  QWidget window;
  
  window.resize(250, 150);
  window.setWindowTitle("Simple example");
  
  GP::Qt::Context context;
  
  GP::Window view(context);
  Array array(context);
  Shader shader(context);
  
  ArrayData ad;
  ad.Set(vertexData, 6*sizeof(float));
  array.SetData(ad);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  Pipeline pipeline = view.GetPipeline();
  
  DrawOperation draw;
  draw.SetShader(shader);
  draw.AddArrayByIndex(array, 0, 2);
  draw.SetVerticies(3);
  pipeline.AddOperation(draw);
  
  QWidget* widget = static_cast<GP::Qt::Window&>(view).GetQWidget();
  QHBoxLayout* layout = new QHBoxLayout();
  layout->addWidget(widget);
  
  window.setLayout(layout);
  window.show();
  
  return app.exec();
}
