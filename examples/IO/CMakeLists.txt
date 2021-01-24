if(EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

find_package(GraphicsPipeline REQUIRED COMPONENTS Interface OPTIONAL_COMPONENTS Native Qt5)

if(TARGET GP::Native)
  add_executable(IO main.c)
  target_link_libraries(IO GP::Interface GP::Native)

  add_executable(IO++ main.cpp)
  target_link_libraries(IO++ GP::Interface GP::Native)
endif()

if(TARGET GP::Qt5)
  add_executable(IO_qt5 main.c)
  target_link_libraries(IO_qt5 GP::Interface GP::Qt5)
  
  add_executable(IO++_qt5 main.cpp)
  target_link_libraries(IO++_qt5 GP::Interface GP::Qt5)
endif(TARGET GP::Qt5)

