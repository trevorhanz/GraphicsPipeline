
#
# Public source
#
set(PUBLIC_HEADERS
    ../include/GraphicsPipeline/Android.h
    ../include/GraphicsPipeline/Array.h
    ../include/GraphicsPipeline/Common.h
    ../include/GraphicsPipeline/Context.h
    ../include/GraphicsPipeline/Desktop.h
    ../include/GraphicsPipeline/GP.h
    ../include/GraphicsPipeline/Logging.h
    ../include/GraphicsPipeline/MacOS.h
    ../include/GraphicsPipeline/Pipeline.h
    ../include/GraphicsPipeline/Qt5.h
    ../include/GraphicsPipeline/Shader.h
    ../include/GraphicsPipeline/System.h
    ../include/GraphicsPipeline/Target.h
    ../include/GraphicsPipeline/Types.h
    ../include/GraphicsPipeline/Web.h
    ../include/GraphicsPipeline/Windows.h
    ../include/GraphicsPipeline/X11.h
    )

set(SRC
  ${PUBLIC_HEADERS}
  )

#
# API source
#
set(GL_HEADERS
  API/GL/GL.h
  )

set(GL_SRC
  ${GL_HEADERS}
  API/GL/Pipeline.c
  API/GL/Array.c
  API/GL/Shader.c
  )

set(GLES_HEADERS
  API/GLES/GLES.h
  )

set(GLES_SRC
  ${GLES_HEADERS}
  API/GLES/Pipeline.c
  API/GLES/Array.c
  API/GLES/Shader.c
  )

#
# Source Groups
#
source_group("Header Files\\Public" FILES ${PUBLIC_HEADERS})
source_group("Header Files\\API" FILES ${GL_HEADERS} ${GLES_HEADERS})
source_group("Header Files\\Platform" REGULAR_EXPRESSION "Platforms.+\.h")

source_group("Source Files\\API" REGULAR_EXPRESSION "API")
source_group("Source Files\\Platform" REGULAR_EXPRESSION "Platforms")

set(LIB "")
set(INC "")

if(EMSCRIPTEN)
  set(GP_WEB ON)
  
  list(APPEND SRC ${GLES_SRC})
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
  
  list(APPEND SRC ${GLES_SRC})
  list(APPEND INC ${GLES2_INCLUDE_DIR} ${EGL_INCLUDE_DIR} Platforms/Android/)
  list(APPEND LIB ${GLES2_LIBRARY} ${EGL_LIBRARY} ${log-lib} android)
  
  list(APPEND SRC
    Platforms/Android/Context.c
    Platforms/Android/Logging.c
    Platforms/Android/System.c)
else(ANDROID)
  set(GP_DESKTOP ON)
  
  find_package(OpenGL REQUIRED)
  list(APPEND SRC ${GL_SRC})
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
      Platforms/X11/Event.c
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
  
  if(GP_BUILD_QT5)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(CMAKE_AUTOMOC ON)
    
    set(Qt5_SRC
      ${GL_SRC}
      Platforms/Qt5/Context.cpp
      Platforms/Qt5/Event.cpp
      Platforms/Qt5/IOCallback.cpp
      Platforms/Qt5/Logging.c
      Platforms/Qt5/System.cpp
      Platforms/Qt5/Target.cpp
      Platforms/Qt5/TimerCallback.cpp)
    
    add_library(GP_Qt5 ${Qt5_SRC})
    target_include_directories(GP_Qt5 PRIVATE ${INC} API/GL/)
    target_include_directories(GP_Qt5 PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
    target_link_libraries(GP_Qt5 Qt5::Widgets ${LIB} Interface)
    set_property(TARGET GP_Qt5 PROPERTY OUTPUT_NAME GPQt5)
    set_property(TARGET GP_Qt5 PROPERTY CXX_STANDARD 11)
    add_library(GP::Qt5 ALIAS GP_Qt5)
    
  endif(GP_BUILD_QT5)
endif(EMSCRIPTEN)

add_library(Interface INTERFACE)
target_include_directories(Interface INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  $<INSTALL_INTERFACE:include>)

add_library(Native ${SRC})
target_include_directories(Native PRIVATE ${INC} ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Native PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(Native ${LIB} Interface)
set_property(TARGET Native PROPERTY OUTPUT_NAME GPNative)
set_property(TARGET Native PROPERTY CXX_STANDARD 11)

string(TOUPPER "${CMAKE_BUILD_TYPE}" build_type )
if(build_type STREQUAL "DEBUG")
  target_compile_definitions(Native PRIVATE GP_DEBUG)
  
  if(GP_BUILD_QT5)
    target_compile_definitions(GP_Qt5 PRIVATE GP_DEBUG)
  endif(GP_BUILD_QT5)
endif()

if(NOT BUILD_SHARED_LIBS)
    set(GP_STATIC ON)
endif(NOT BUILD_SHARED_LIBS)
configure_file(Config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/Config.h)

if(EMSCRIPTEN)
  set_target_properties(Native PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  add_executable(GraphicsPipeline-main Platforms/Web/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main Native)
endif(EMSCRIPTEN)

#
# Install
#
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(../cmake/GraphicsPipelineConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GraphicsPipelineConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/)

write_basic_package_version_file(
    "GraphicsPipelineVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GraphicsPipelineConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/GraphicsPipelineVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/)
install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)
install(TARGETS Interface Native
        EXPORT GP
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime)
install(EXPORT GP NAMESPACE GP:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/)
