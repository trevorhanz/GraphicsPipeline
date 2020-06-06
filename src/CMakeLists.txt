
set(SRC
  Operations.cpp
  Types.cpp)

set(LIB "")
set(INC "")

if(EMSCRIPTEN)
  list(APPEND SRC
    Platforms/src/Web/src/System.cpp
    Platforms/src/Web/src/WebAssembly.cpp
    Platforms/src/Web/src/Window.cpp)
  list(APPEND INC
    Platforms/src/Web/include)
else(EMSCRIPTEN)
  find_package(OpenGL REQUIRED)
  list(APPEND SRC API/GL/Pipeline.cpp)
  list(APPEND INC ${OPENGL_INCLUDE_DIRS})
  list(APPEND LIB ${OPENGL_LIBRARIES})
  
  if(WIN32)
    
  elseif(UNIX)
    list(APPEND SRC
      Platforms/src/X11/Context.cpp
      Platforms/src/X11/System.cpp
      Platforms/src/X11/Window.cpp)
    
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
target_include_directories(GraphicsPipeline PUBLIC ${INC} ../include Platforms/include/)
target_include_directories(GraphicsPipeline PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GraphicsPipeline ${LIB})

if(EMSCRIPTEN)
  set_target_properties(GraphicsPipeline PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  add_executable(GraphicsPipeline-main Platforms/src/Web/src/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main GraphicsPipeline)
endif(EMSCRIPTEN)
