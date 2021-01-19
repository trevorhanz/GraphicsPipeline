
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main','_add_canvas'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)

add_custom_target(WebExample ALL SOURCES index.html)
add_dependencies(WebExample GraphicsPipeline-main)

add_custom_command(
        TARGET WebExample POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_BINARY_DIR}/src/GP.js
                ${CMAKE_CURRENT_BINARY_DIR}/
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_BINARY_DIR}/src/GP.wasm
                ${CMAKE_CURRENT_BINARY_DIR}/)
