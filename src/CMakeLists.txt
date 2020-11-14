set(HEADERS
    ../include/GraphicsPipeline/Android.h
    ../include/GraphicsPipeline/Context.h
    ../include/GraphicsPipeline/Desktop.h
    ../include/GraphicsPipeline/GP.h
    ../include/GraphicsPipeline/Logging.h
    ../include/GraphicsPipeline/MacOS.h
    ../include/GraphicsPipeline/Pipeline.h
    ../include/GraphicsPipeline/System.h
    ../include/GraphicsPipeline/Types.h
    ../include/GraphicsPipeline/Web.h
    ../include/GraphicsPipeline/Windows.h
    ../include/GraphicsPipeline/X11.h
    )

set(SRC
  ${HEADERS}
  )

source_group("Header Files\\include" FILES ${HEADERS})

set(LIB "")
set(INC "")

if(EMSCRIPTEN)
  list(APPEND SRC
    API/GLES/Array.c
    API/GLES/Pipeline.c
    API/GLES/Shader.c)
  list(APPEND SRC
    Platforms/Web/System.c
    Platforms/Web/Context.c
    Platforms/Web/Logging.c)
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
  
  list(APPEND SRC API/GLES/Pipeline.c API/GLES/Array.c API/GLES/Shader.c)
  list(APPEND INC ${GLES2_INCLUDE_DIR} ${EGL_INCLUDE_DIR} Platforms/Android/)
  list(APPEND LIB ${GLES2_LIBRARY} ${EGL_LIBRARY} ${log-lib} android)
  
  list(APPEND SRC
    Platforms/Android/Context.c
    Platforms/Android/Logging.c
    Platforms/Android/System.c)
else(ANDROID)
  find_package(OpenGL REQUIRED)
  list(APPEND SRC API/GL/Pipeline.c API/GL/Array.c API/GL/Shader.c)
  list(APPEND INC ${OPENGL_INCLUDE_DIRS})
  list(APPEND LIB ${OPENGL_LIBRARIES})
  
  if(WIN32)
    list(APPEND SRC
      Platforms/Windows/Context.cpp
      Platforms/Windows/Logging.cpp
      Platforms/Windows/System.cpp
      Platforms/Windows/Window.cpp)
  elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND LIB ${COCOA_LIBRARY})
    list(APPEND SRC
      Platforms/MacOS/Context.mm
      Platforms/MacOS/Context.h
      Platforms/MacOS/Logging.cpp
      Platforms/MacOS/MacOS.h
      Platforms/MacOS/View.m
      Platforms/MacOS/View.h
      Platforms/MacOS/System.mm
      Platforms/MacOS/Window.mm)
  elseif(UNIX)
    list(APPEND SRC
      Platforms/X11/Context.c
      Platforms/X11/Logging.c
      Platforms/X11/System.c)
    
    find_package(X11 REQUIRED)
    list(APPEND LIB ${X11_LIBRARIES})
  else()
    message("Configured for unknown OS")
  endif()
  
  # GLEW not needed on macOS
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
set_property(TARGET GraphicsPipeline PROPERTY CXX_STANDARD 11)

if(EMSCRIPTEN)
  set_target_properties(GraphicsPipeline PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  add_executable(GraphicsPipeline-main Platforms/Web/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main GraphicsPipeline)
endif(EMSCRIPTEN)
