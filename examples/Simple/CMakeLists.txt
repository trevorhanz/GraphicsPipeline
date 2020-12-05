
if(EMSCRIPTEN)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

  configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)
endif(EMSCRIPTEN)

add_executable(Simple main.c)
target_link_libraries(Simple Native)

add_executable(Simple++ main.cpp)
target_link_libraries(Simple++ Native)
