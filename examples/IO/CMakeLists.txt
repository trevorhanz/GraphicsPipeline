if(EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

add_executable(IO main.c)
target_link_libraries(IO Native)

if(GP_BUILD_QT5)
  add_executable(IO_qt5 main.c)
  target_link_libraries(IO_qt5 GP::Qt5)
endif(GP_BUILD_QT5)

