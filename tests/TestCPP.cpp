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

#include "gtest/gtest.h"

using namespace GP;

#ifdef __APPLE__
#define GLSL(src) "#version 400\n" #src
#else
#define GLSL(src) "#version 300 es\n" #src
#endif

// Shader sources
const char* vertexSource = GLSL(
  in highp vec4 position;
  void main()
  {
    gl_Position = vec4(position.xyz, 1.0);
  });
const char* fragmentSource = GLSL(
  uniform highp float Position;
  uniform sampler2D Texture;
  out highp vec4 fragColor;
  void main()
  {
    fragColor = texture(Texture, vec2(Position, Position));
  });
float vertexData[] = {0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f};

gp_context* _sContext = NULL;

TEST(CPP, Init)
{
  System system;
  
  Context context(system);
  _sContext = (gp_context*)context.GetObject();
}

TEST(CPP, Memory)
{
  float* data = new float[32];
  int* data2 = new int[32];
  
  // Constructor
  Pointer pointer;
  ASSERT_EQ(pointer, false);
  ASSERT_EQ(!pointer, true);
  Pointer pointer2(data);
  ASSERT_EQ(pointer2, true);
  ASSERT_EQ(!pointer2, false);
  
  // Copy constructor
  pointer = Pointer(data2);
  ASSERT_EQ(pointer, true);
  ASSERT_EQ(!pointer, false);
  pointer = Pointer();
  ASSERT_EQ(pointer, false);
  ASSERT_EQ(!pointer, true);
  
  // Equality
  Pointer pointer3;
  pointer3 = pointer;
  ASSERT_EQ(pointer3, false);
  ASSERT_EQ(!pointer3, true);
  pointer3 = pointer2;
  ASSERT_EQ(pointer3, true);
  ASSERT_EQ(!pointer3, false);
  
  if(_sContext)
  {
    Context context(_sContext);
    
    // Array
    Array array;
    ASSERT_EQ(array, false);
    array = Array(context);
    ASSERT_EQ(array.GetCount(), 1);
    
    // Texture
    Texture texture;
    ASSERT_EQ(texture, false);
    texture = Texture(context);
    ASSERT_EQ(texture.GetCount(), 1);
    
    // Shader
    Shader shader;
    ASSERT_EQ(shader, false);
    shader = Shader(context);
    ASSERT_EQ(shader.GetCount(), 1);
    
    // Compile shader so uniforms will work.
    ShaderSource source;
    source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
    source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
    shader.Compile(source);
    
    // UniformFloat
    // NOTE: All uniforms except UniformTexture are generated using 
    // the same code so UniformFloat's behavior should be representative
    // of other uniforms.
    UniformFloat uFloat;
    ASSERT_EQ(uFloat, false);
    uFloat = UniformFloat(shader, "Position");
    ASSERT_EQ(uFloat.GetCount(), 1);
    
    // UniformTexture
    UniformTexture uTexture;
    ASSERT_EQ(uTexture, false);
    uTexture = UniformTexture(shader, "Texture");
    ASSERT_EQ(uTexture.GetCount(), 1);
  }
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
