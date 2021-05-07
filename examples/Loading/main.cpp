/************************************************************************
* Copyright (C) 2022 Trevor Hanz
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
#include <GraphicsPipeline/Desktop.h>

#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <vector>

#include "../Common.h"

using namespace GP;

// Shader sources
const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_PointSize = 64.0f;
    gl_Position = vec4(position.xyz, 1.0);
  });
const char* fragmentSource = GLSL(
  uniform highp vec4 Color;
  out highp vec4 fragColor;
  void main()
  {
    fragColor = Color;
  });
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

int Load(const char* path, Array array)
{
  std::ifstream in(path);
  if(!in)
  {
    gp_log("Error: Unable to open %s", path);
    return 0;
  }
  
  std::vector<float> data;
  
  while(in)
  {
    std::string s;
    if(!getline(in, s, ',')) break;
    data.push_back(std::atof(s.c_str()));
  }
  
  in.close();
  
  ArrayData ad;
  ad.Set(data.data(), data.size()*sizeof(float));
  array.SetData(ad);
  
  return data.size()/2;
}

int main(int argc, char* argv[])
{
  System system;
  
  Context context(system);
  
  Window window(context);
  Array array(context);
  Shader shader(context);
  
  int numPoints = Load("data/triangle.txt", array);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  float c[] = {1.0f, 0.0f, 0.0f, 1.0f};
  UniformVec4 color1(shader, "Color");
  color1.Set(c);
  
  c[1] = 1.0f;
  UniformVec4 color2(shader, "Color");
  color2.Set(c);
  
  Pipeline pipeline = window.GetPipeline();
  
  ClearOperation clear;
  pipeline.AddOperation(clear);
  
  DrawOperation operation1;
  operation1.SetShader(shader);
  operation1.SetUniform(color1);
  operation1.AddArrayByIndex(array, 0, 2);
  operation1.SetVerticies(numPoints);
  operation1.SetMode(GP_MODE_TRIANGLE_STRIP);
  pipeline.AddOperation(operation1);
  
  DrawOperation operation2;
  operation2.SetShader(shader);
  operation2.SetUniform(color2);
  operation2.AddArrayByIndex(array, 0, 2);
  operation2.SetVerticies(numPoints);
  operation2.SetMode(GP_MODE_POINTS);
  pipeline.AddOperation(operation2);
  
  bool view = false;
  
  Timer timer(system);
  timer.SetCallback([&](Timer& timer)
  {
    view = !view;
    
    int numPoints = 0;
    if(view) numPoints = Load("data/square.txt", array);
    else numPoints = Load("data/triangle.txt", array);
    operation1.SetVerticies(numPoints);
    operation2.SetVerticies(numPoints);
    
    window.Redraw();
    
    timer.Arm(1.0);
  });
  timer.Arm(1.0);
  
  window.Show();
  
  system.Run();
  
  return EXIT_SUCCESS;
}

