
add_subdirectory(Animation)
add_subdirectory(IO)
add_subdirectory(PostProcessing)
add_subdirectory(Simple)
add_subdirectory(Timer)
add_subdirectory(Windows)

if(EMSCRIPTEN)
  add_subdirectory(Web)
endif(EMSCRIPTEN)

if(GP_BUILD_QT5)
  add_subdirectory(Qt5)
endif(GP_BUILD_QT5)
