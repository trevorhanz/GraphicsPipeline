
set(SRC
  Operations.cpp
  Types.cpp)

set(LIB "")
set(INC "")

if(EMSCRIPTEN)
  list(APPEND SRC API/GLES/Pipeline.cpp)
  list(APPEND SRC
    Platforms/Web/System.cpp
    Platforms/Web/Context.cpp
    Platforms/Web/Window.cpp)
elseif(ANDROID)
  find_path(GLES2_INCLUDE_DIR GLES2/gl2.h
    HINTS ${ANDROID_NDK})
  find_library(GLES2_LIBRARY libGLESv2.so
    HINTS ${GLES2_INCLUDE_DIR}/../lib)
  find_path(EGL_INCLUDE_DIR EGL/egl.h
    HINTS ${ANDROID_NDK})
  find_library(EGL_LIBRARY libEGL.so
    HINTS ${EGL_INCLUDE_DIR}/../lib)
  
  find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)
  
  list(APPEND SRC API/GLES/Pipeline.cpp)
  list(APPEND INC ${GLES2_INCLUDE_DIR} ${EGL_INCLUDE_DIR} Platforms/Android/)
  list(APPEND LIB ${GLES2_LIBRARY} ${EGL_LIBRARY} ${log-lib} android)
  
  list(APPEND SRC
    Platforms/Android/Context.cpp
    Platforms/Android/System.cpp
    Platforms/Android/Window.cpp)
else(ANDROID)
  find_package(OpenGL REQUIRED)
  list(APPEND SRC API/GL/Pipeline.cpp)
  list(APPEND INC ${OPENGL_INCLUDE_DIRS})
  list(APPEND LIB ${OPENGL_LIBRARIES})
  
  if(WIN32)
    
  elseif(UNIX)
    list(APPEND SRC
      Platforms/X11/Context.cpp
      Platforms/X11/System.cpp
      Platforms/X11/Window.cpp)
    
    find_package(X11 REQUIRED)
    list(APPEND LIB ${X11_LIBRARIES})
  else()
    
  endif()
  
  if(NOT APPLE)
    find_package(GLEW REQUIRED)
    list(APPEND INC ${GLEW_INCLUDE_DIRS})
    list(APPEND LIB ${GLEW_LIBRARIES})
  endif(NOT APPLE)
endif(EMSCRIPTEN)

add_library(GraphicsPipeline ${SRC})
target_include_directories(GraphicsPipeline PUBLIC ${INC} ../include)
target_include_directories(GraphicsPipeline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GraphicsPipeline ${LIB})

if(EMSCRIPTEN)
  set_target_properties(GraphicsPipeline PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  add_executable(GraphicsPipeline-main Platforms/Web/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main GraphicsPipeline)
endif(EMSCRIPTEN)
