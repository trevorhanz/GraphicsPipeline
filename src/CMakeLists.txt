set(HEADERS
    ../include/GraphicsPipeline/Android.h
    ../include/GraphicsPipeline/Array.h
    ../include/GraphicsPipeline/Common.h
    ../include/GraphicsPipeline/Context.h
    ../include/GraphicsPipeline/Desktop.h
    ../include/GraphicsPipeline/GP.h
    ../include/GraphicsPipeline/Logging.h
    ../include/GraphicsPipeline/MacOS.h
    ../include/GraphicsPipeline/Pipeline.h
    ../include/GraphicsPipeline/Shader.h
    ../include/GraphicsPipeline/System.h
    ../include/GraphicsPipeline/Target.h
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
  set(GP_WEB ON)
  
  list(APPEND SRC
    API/GLES/Array.c
    API/GLES/Pipeline.c
    API/GLES/Shader.c)
  list(APPEND SRC
    Platforms/Web/System.c
    Platforms/Web/Context.c
    Platforms/Web/Logging.c)
elseif(ANDROID)
  set(GP_MOBILE ON)
  set(GP_ANDROID ON)
  
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
  set(GP_DESKTOP ON)
  
  find_package(OpenGL REQUIRED)
  list(APPEND SRC API/GL/Pipeline.c API/GL/Array.c API/GL/Shader.c)
  list(APPEND INC ${OPENGL_INCLUDE_DIRS})
  list(APPEND LIB ${OPENGL_LIBRARIES})
  
  if(WIN32)
    set(GP_WINDOWS ON)
    
    list(APPEND SRC
      Platforms/Windows/Context.c
      Platforms/Windows/Logging.c
      Platforms/Windows/System.c
      Platforms/Windows/Windows.h)
  elseif(APPLE)
    set(GP_MACOS ON)
    
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND LIB ${COCOA_LIBRARY})
    list(APPEND SRC
      Platforms/MacOS/Context.m
      Platforms/MacOS/Logging.c
      Platforms/MacOS/MacOS.h
      Platforms/MacOS/View.m
      Platforms/MacOS/View.h
      Platforms/MacOS/System.m)
  elseif(UNIX)
    set(GP_LINUX ON)
    
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
target_include_directories(GraphicsPipeline PUBLIC ${INC} ../include ${CMAKE_CURRENT_BINARY_DIR}/include/)
target_include_directories(GraphicsPipeline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GraphicsPipeline ${LIB})
set_property(TARGET GraphicsPipeline PROPERTY CXX_STANDARD 11)

if(NOT BUILD_SHARED_LIBS)
    set(GP_STATIC ON)
endif(NOT BUILD_SHARED_LIBS)
configure_file(Config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/Config.h)

if(EMSCRIPTEN)
  set_target_properties(GraphicsPipeline PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  add_executable(GraphicsPipeline-main Platforms/Web/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main GraphicsPipeline)
endif(EMSCRIPTEN)
