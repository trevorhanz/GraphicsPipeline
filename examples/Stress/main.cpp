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
#include <GraphicsPipeline/Desktop.h>

#define BUILD_ARRAY     0
#define BUILD_TEXTURE   1
#define RENDER_ARRAY    0
#define RENDER_TEXTURE  1

#include <vector>
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 26451 ) // Potential overflow from '*' operator
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <winsock.h>

int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
  // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
  // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
  // until 00:00:00 January 1, 1970 
  static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

  SYSTEMTIME  system_time;
  FILETIME    file_time;
  uint64_t    time;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time, &file_time);
  time = ((uint64_t)file_time.dwLowDateTime);
  time += ((uint64_t)file_time.dwHighDateTime) << 32;

  tp->tv_sec = (long)((time - EPOCH) / 10000000L);
  tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
  return 0;
}
#else
#include <sys/time.h>
#endif

using namespace GP;

#ifdef __APPLE__
#define GLSL(src) "#version 400\n" #src
#else
#define GLSL(src) "#version 300 es\n" #src
#endif

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
    fragColor = vec4(0, 1, 1, 1);
  });

const char* vertexSourceQuad = GLSL(
  in highp vec4 position;
  out vec2 UV;
  void main()
  {
    UV = position.xy;
    gl_Position = vec4(position.xyz*2.0-1.0, 1.0);
  });
const char* fragmentSourceQuad = GLSL(
  precision highp float;
  out highp vec4 fragColor;
  in vec2 UV;
  uniform sampler2D Texture;
  void main()
  {
    fragColor = texture(Texture, UV);
  });
float vertexDataQuad[] = {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f};

#define ARRAY_SIZE 20000000
#define TEX_WIDTH 4096
#define TEX_HEIGHT 4096

Target* sTarget;
Array* sArray[2];
Texture* sTexture[2];
Timer* sTimer;
UniformTexture* sTex;

DrawOperation sOperation;

ArrayData sAD[2];
TextureData sTD[2];
int currentArray = 0;
int currentTexture = 0;

struct timeval beginFrame;
int frame;

double throughput = 0;

// Async array callback
void ReloadArray(Array* array)
{
  throughput += ((double)(ARRAY_SIZE*sizeof(float)))/(1024.0*1024.0);
  
#if RENDER_ARRAY
  sOperation.AddArrayByIndex(*sArray[currentArray], 0, 2);
#endif
  
  currentArray += 1;
  if(currentArray>1) currentArray = 0;
  
  sArray[currentArray]->SetDataAsync(sAD[currentArray], ReloadArray);
}

// Async texture callback
void ReloadTexture(Texture* texture)
{
  throughput += ((double)(TEX_WIDTH*TEX_HEIGHT*sizeof(float)*4))/(1024.0*1024.0);
  
#if RENDER_TEXTURE
  sTex->Set(*sTexture[currentTexture]);
#endif
  
  currentTexture += 1;
  if(currentTexture>1) currentTexture = 0;
  
  sTexture[currentTexture]->SetDataAsync(sTD[currentTexture], ReloadTexture);
}

int main(int argc, char* argv[])
{
  System* system = new System();
  
  Context* context = new Context(*system);
  
  sTarget = new Target(*context);
  sArray[0] = new Array(*context);
  sArray[1] = new Array(*context);
  sTexture[0] = new Texture(*context);
  sTexture[1] = new Texture(*context);
  Shader shader(*context);
  Shader shaderQuad(*context);
  Array arrayQuad(*context);
  
  ArrayData ad;
  ad.Set(vertexDataQuad, 8);
  arrayQuad.SetData(ad);
  
  ShaderSource source;
  source.AddString(GP_SHADER_SOURCE_VERTEX, vertexSource);
  source.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSource);
  shader.Compile(source);
  
  ShaderSource sourceQuad;
  sourceQuad.AddString(GP_SHADER_SOURCE_VERTEX, vertexSourceQuad);
  sourceQuad.AddString(GP_SHADER_SOURCE_FRAGMENT, fragmentSourceQuad);
  shaderQuad.Compile(sourceQuad);
  
#if BUILD_ARRAY || RENDER_ARRAY
  std::vector<float> data(ARRAY_SIZE);
  for(int a=0; a<2; ++a)
  {
    for(int i=0; i<data.size(); ++i)
    {
      data[i] = (rand()%1000)/500.0-1.0;
    }
    sAD[a].Set(&data[0], data.size());
  }
#endif

#if BUILD_ARRAY
  sArray[0]->SetDataAsync(sAD[0], ReloadArray);
#elif RENDER_ARRAY
  sArray[0]->SetData(sAD[0]);
#endif
  
#if BUILD_TEXTURE || RENDER_TEXTURE
  std::vector<float> data(TEX_WIDTH*TEX_HEIGHT*4);
  for(int t=0; t<2; ++t)
  {
    for(int i=0; i<TEX_WIDTH*TEX_HEIGHT; ++i)
    {
      data[i*4+0] = (rand()%1000)/1000.0f;
      data[i*4+1] = (rand()%1000)/1000.0f;
      data[i*4+2] = (rand()%1000)/1000.0f;
      data[i*4+3] = 1.0f;
    }
    sTD[t].Set(&data[0], TEX_WIDTH, TEX_HEIGHT);
  }
#endif
  
#if BUILD_TEXTURE
  sTexture[0]->SetDataAsync(sTD[0], ReloadTexture);
#elif RENDER_TEXTURE
  sTexture[0]->SetData(&sTD[0]);
#endif
  
  Pipeline pipeline = sTarget->GetPipeline();
  
  ClearOperation clear;
  pipeline.AddOperation(clear);
  
#if RENDER_ARRAY
  sOperation.SetShader(shader);
  sOperation.AddArrayByIndex(*sArray[0], 0, 2);
  sOperation.SetVerticies(100);
  sOperation.SetMode(GP_MODE_TRIANGLE_STRIP);
  pipeline.AddOperation(sOperation);
#endif
  
#if RENDER_TEXTURE
  sTex = new UniformTexture(shaderQuad, "Texture");
  sTex->Set(*sTexture[0]);
  
  DrawOperation operation;
  operation.SetShader(shaderQuad);
  operation.AddArrayByIndex(arrayQuad, 0, 2);
  operation.SetUniform(*sTex);
  operation.SetVerticies(4);
  operation.SetMode(GP_MODE_TRIANGLE_STRIP);
  pipeline.AddOperation(operation);
#endif
  
  gettimeofday(&beginFrame, NULL);
  
  sTimer = new Timer(*system);
  sTimer->SetCallback(std::function<void(Timer*)>([](Timer* timer){
    // Redraw Target
    sTarget->Redraw();
    
    // Calculate FPS at 1Hz
    struct timeval endFrame;
    gettimeofday(&endFrame, NULL);
    double delta = (endFrame.tv_sec-beginFrame.tv_sec) * 1000000 + (endFrame.tv_usec-beginFrame.tv_usec);
    if(delta > 1000000)
    {
      gp_log("FPS: %.2f - Throughput %.2fMB/s", frame/(delta/1000000.0), throughput/(delta/1000000.0));
      beginFrame = endFrame;
      frame = 0;
      throughput = 0;
    }
    
    ++frame;
    timer->Arm(.01);
  }));
  sTimer->Arm(.01);
  
  system->Run();
}

