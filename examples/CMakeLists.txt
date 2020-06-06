
add_subdirectory(Simple)

if(EMSCRIPTEN)
  add_subdirectory(Web)
endif(EMSCRIPTEN)
