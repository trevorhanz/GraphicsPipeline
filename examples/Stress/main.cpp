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

class App
{
public:
  App()
    : mSystem(),
    mContext(mSystem),
    mWindow(mContext),
    mArrays{Array(mContext), Array(mContext)},
    mTextures{Texture(mContext), Texture(mContext)},
    mThroughput(0),
    mCurrentArray(0),
    mCurrentTexture(0), 
    mFrame(0)
  {
    Shader shader(mContext);
    Shader shaderQuad(mContext);
    Array arrayQuad(mContext);
    
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
      ads[a].Set(&data[0], data.size());
    }
#endif

#if BUILD_ARRAY
    mArrays[0].SetDataAsync(mADs[0], std::bind(&App::ReloadArray, this));
#elif RENDER_ARRAY
    mArrays[0].SetData(mADs[0]);
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
      mTDs[t].Set(&data[0], TEX_WIDTH, TEX_HEIGHT);
    }
#endif
  
#if BUILD_TEXTURE
    mTextures[0].SetDataAsync(mTDs[0], std::bind(&App::ReloadTexture, this));
#elif RENDER_TEXTURE
    mTextures[0]->SetData(&mTDs[0]);
#endif
    
    Pipeline pipeline = mWindow.GetPipeline();
    
    ClearOperation clear;
    pipeline.AddOperation(clear);
    
#if RENDER_ARRAY
    mOperation.SetShader(shader);
    mOperation.AddArrayByIndex(*sArray[0], 0, 2);
    mOperation.SetVerticies(100);
    mOperation.SetMode(GP_MODE_TRIANGLE_STRIP);
    pipeline.AddOperation(mOperation);
#endif
  
#if RENDER_TEXTURE
    mTex = new UniformTexture(shaderQuad, "Texture");
    mTex->Set(mTextures[0]);
    
    DrawOperation operation;
    operation.SetShader(shaderQuad);
    operation.AddArrayByIndex(arrayQuad, 0, 2);
    operation.SetUniform(*mTex);
    operation.SetVerticies(4);
    operation.SetMode(GP_MODE_TRIANGLE_STRIP);
    pipeline.AddOperation(operation);
#endif
  }
  
  ~App()
  {
    delete mTex;
  }
  
  void Run()
  {
    gettimeofday(&mBeginFrame, NULL);
    
    Timer timer(mSystem);
    timer.SetCallback(std::function<void(Timer*)>([&](Timer* timer){
      // Redraw Window
      mWindow.Redraw();
      
      // Calculate FPS at 1Hz
      struct timeval endFrame;
      gettimeofday(&endFrame, NULL);
      double delta = (endFrame.tv_sec-mBeginFrame.tv_sec) * 1000000 + (endFrame.tv_usec-mBeginFrame.tv_usec);
      if(delta > 1000000)
      {
        gp_log("FPS: %.2f - Throughput %.2fMB/s", mFrame/(delta/1000000.0), mThroughput/(delta/1000000.0));
        mBeginFrame = endFrame;
        mFrame = 0;
        mThroughput = 0;
      }
      
      ++mFrame;
      timer->Arm(.01);
    }));
    timer.Arm(.01);
    
    mSystem.Run();
  }
  
private:
  // Async array callback
  void ReloadArray()
  {
    mThroughput += ((double)(ARRAY_SIZE*sizeof(float)))/(1024.0*1024.0);
    
#if RENDER_ARRAY
    mOperation.AddArrayByIndex(*sArray[currentArray], 0, 2);
#endif
    
    mCurrentArray += 1;
    if(mCurrentArray>1) mCurrentArray = 0;
    
    mArrays[mCurrentArray].SetDataAsync(mADs[mCurrentArray], std::bind(&App::ReloadArray, this));
  }
  
  // Async texture callback
  void ReloadTexture()
  {
    mThroughput += ((double)(TEX_WIDTH*TEX_HEIGHT*sizeof(float)*4))/(1024.0*1024.0);
  
#if RENDER_TEXTURE
    mTex->Set(mTextures[mCurrentTexture]);
#endif
  
    mCurrentTexture += 1;
    if(mCurrentTexture>1) mCurrentTexture = 0;
    
    mTextures[mCurrentTexture].SetDataAsync(mTDs[mCurrentTexture], std::bind(&App::ReloadTexture, this));
  }
  
  System                        mSystem;
  Context                       mContext;
  Window                        mWindow;
  Array                         mArrays[2];
  Texture                       mTextures[2];
  ArrayData                     mADs[2];
  TextureData                   mTDs[2];
  DrawOperation                 mOperation;
  UniformTexture*               mTex;
  
  double                        mThroughput;
  int                           mCurrentArray;
  int                           mCurrentTexture;

  struct timeval                mBeginFrame;
  int                           mFrame;
};

int main(int argc, char* argv[])
{
  App app;
  app.Run();
}

