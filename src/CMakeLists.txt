
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
    ../include/GraphicsPipeline/Texture.h
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
  API/GL/FrameBuffer.c
  API/GL/Shader.c
  API/GL/Texture.c
  )

#
# Utils sources
#
set(UTILS_HEADERS
  Utils/List.h
  Utils/RefCounter.h
  )

set(UTILS_SRC
  ${UTILS_HEADERS}
  Utils/List.c
  Utils/RefCounter.c
  )

#
# Source Groups
#
source_group("Header Files\\Public" FILES ${PUBLIC_HEADERS})
source_group("Header Files\\API" FILES ${GL_HEADERS} ${GLES_HEADERS})
source_group("Header Files\\Platform" REGULAR_EXPRESSION "Platforms.*\.h")
source_group("Header Files\\Utils" REGULAR_EXPRESSION "Utils.*\.h")

source_group("Source Files\\API" REGULAR_EXPRESSION "API")
source_group("Source Files\\Platform" REGULAR_EXPRESSION "Platforms")
source_group("Source Files\\Utils" REGULAR_EXPRESSION "Utils")

set(LIB "")
set(INC "")

if(EMSCRIPTEN)
  set(GP_WEB ON)
  set(GP_GLES3 ON)
  
  list(APPEND SRC ${GL_SRC} ${UTILS_SRC})
  list(APPEND SRC
    Platforms/Web/System.c
    Platforms/Web/Context.c
    Platforms/Web/Event.c
    Platforms/Web/Logging.c)
elseif(ANDROID)
  set(GP_MOBILE ON)
  set(GP_ANDROID ON)
  set(GP_GLES3 ON)
  
  find_path(GLES3_INCLUDE_DIR GLES3/gl3.h
    HINTS ${ANDROID_NDK})
  find_library(GLES3_LIBRARY libGLESv3.so
    HINTS ${GLES3_INCLUDE_DIR}/../lib)
  find_path(EGL_INCLUDE_DIR EGL/egl.h
    HINTS ${ANDROID_NDK})
  find_library(EGL_LIBRARY libEGL.so
    HINTS ${EGL_INCLUDE_DIR}/../lib)
  
  find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)
  
  list(APPEND SRC ${GL_SRC} ${UTILS_SRC})
  list(APPEND INC ${GLES3_INCLUDE_DIR} ${EGL_INCLUDE_DIR} Platforms/Android/)
  list(APPEND LIB ${GLES3_LIBRARY} ${EGL_LIBRARY} ${log-lib} android)
  
  list(APPEND SRC
    Platforms/Android/Android.h
    Platforms/Android/Context.c
    Platforms/Android/Event.c
    Platforms/Android/Logging.c
    Platforms/Android/System.c)
else(ANDROID)
  set(GP_DESKTOP ON)
  set(GP_GL ON)
  
  find_package(OpenGL REQUIRED)
  list(APPEND SRC ${GL_SRC} ${UTILS_SRC})
  list(APPEND INC ${OPENGL_INCLUDE_DIRS})
  list(APPEND LIB ${OPENGL_LIBRARIES})
  
  if(WIN32)
    set(GP_WINDOWS ON)
    
    list(APPEND SRC
      Platforms/Windows/Context.c
      Platforms/Windows/Logging.c
      Platforms/Windows/Event.c
      Platforms/Windows/System.c
      Platforms/Windows/Windows.h)
    list(APPEND LIB wsock32)
  elseif(APPLE)
    set(GP_MACOS ON)
    
    find_package(Threads REQUIRED)
    
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND LIB ${COCOA_LIBRARY} Threads::Threads)
    list(APPEND SRC
      Platforms/MacOS/Context.m
      Platforms/MacOS/Event.m
      Platforms/MacOS/Logging.c
      Platforms/MacOS/MacOS.h
      Platforms/MacOS/View.m
      Platforms/MacOS/View.h
      Platforms/MacOS/Timer.m
      Platforms/MacOS/Timer.h
      Platforms/MacOS/System.m)
  elseif(UNIX)
    set(GP_LINUX ON)
    
    list(APPEND SRC
      Platforms/X11/Context.c
      Platforms/X11/Event.c
      Platforms/X11/Logging.c
      Platforms/X11/System.c)
    
    find_package(Threads REQUIRED)
    
    find_package(X11 REQUIRED)
    list(APPEND LIB ${X11_LIBRARIES} Threads::Threads)
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
      ${UTILS_SRC}
      Platforms/Qt5/Context.cpp
      Platforms/Qt5/Event.cpp
      Platforms/Qt5/IOCallback.cpp
      Platforms/Qt5/Logging.c
      Platforms/Qt5/System.cpp
      Platforms/Qt5/Target.cpp
      Platforms/Qt5/TimerCallback.cpp
      Platforms/Qt5/WorkQueue.cpp)
    
    add_library(GP_Qt5 ${Qt5_SRC})
    target_include_directories(GP_Qt5 PRIVATE ${INC} ${CMAKE_CURRENT_SOURCE_DIR} API/GL/)
    target_include_directories(GP_Qt5 PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
    target_link_libraries(GP_Qt5 PRIVATE Qt5::Widgets ${LIB} GP_Interface)
    set_property(TARGET GP_Qt5 PROPERTY OUTPUT_NAME GPQt5)
    set_property(TARGET GP_Qt5 PROPERTY EXPORT_NAME Qt5)
    set_property(TARGET GP_Qt5 PROPERTY CXX_STANDARD 11)
    
  endif(GP_BUILD_QT5)
endif(EMSCRIPTEN)

add_library(GP_Interface INTERFACE)
target_include_directories(GP_Interface INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  $<INSTALL_INTERFACE:include>)
set_property(TARGET GP_Interface PROPERTY EXPORT_NAME Interface)

add_library(GP_Native ${SRC})
target_include_directories(GP_Native PRIVATE ${INC} ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(GP_Native PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(GP_Native PRIVATE ${LIB} GP_Interface)
set_property(TARGET GP_Native PROPERTY OUTPUT_NAME GPNative)
set_property(TARGET GP_Native PROPERTY EXPORT_NAME Native)
set_property(TARGET GP_Native PROPERTY C_STANDARD 11)

string(TOUPPER "${CMAKE_BUILD_TYPE}" build_type )
if(build_type STREQUAL "DEBUG")
  target_compile_definitions(GP_Native PRIVATE GP_DEBUG)
  
  if(GP_BUILD_QT5)
    target_compile_definitions(GP_Qt5 PRIVATE GP_DEBUG)
  endif(GP_BUILD_QT5)
endif()

if(NOT BUILD_SHARED_LIBS)
    set(GP_STATIC ON)
endif(NOT BUILD_SHARED_LIBS)
configure_file(Config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/Config.h)

if(EMSCRIPTEN)
  set_target_properties(GP_Native PROPERTIES OUTPUT_NAME GP PREFIX "" SUFFIX .bc)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")
  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.13")
    target_link_options(GP_Native PUBLIC "SHELL:-s MAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1")
  endif()
  add_executable(GraphicsPipeline-main Platforms/Web/main.cpp)
  set_target_properties(GraphicsPipeline-main PROPERTIES OUTPUT_NAME GP PREFIX "")
  target_link_libraries(GraphicsPipeline-main GP_Interface GP_Native)
endif(EMSCRIPTEN)

#
# Install
#
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(../cmake/GraphicsPipelineConfig.cmake.in
  ${PROJECT_BINARY_DIR}/cmake/GraphicsPipelineConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/)

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/cmake/GraphicsPipelineVersion.cmake
    VERSION 0.0.0
    COMPATIBILITY SameMajorVersion
)

install(FILES ${PROJECT_BINARY_DIR}/cmake/GraphicsPipelineConfig.cmake
              ${PROJECT_BINARY_DIR}/cmake/GraphicsPipelineVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ COMPONENT devel)
install(DIRECTORY ../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT devel)
install(TARGETS GP_Interface
        EXPORT GraphicsPipelineInterface
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime)
install(EXPORT GraphicsPipelineInterface NAMESPACE GP:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ COMPONENT devel)
install(TARGETS GP_Native
        EXPORT GraphicsPipelineNative
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime)
install(EXPORT GraphicsPipelineNative NAMESPACE GP:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ COMPONENT devel)
if(GP_BUILD_QT5)
  install(TARGETS GP_Qt5
          EXPORT GraphicsPipelineQt5
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime)
  install(EXPORT GraphicsPipelineQt5 NAMESPACE GP:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ COMPONENT devel)
endif(GP_BUILD_QT5)

set(GraphicsPipeline_DIR ${PROJECT_BINARY_DIR}/cmake/ CACHE INTERNAL "" FORCE)
set(GraphicsPipeline_IN_TREE ON CACHE INTERNAL "" FORCE)
