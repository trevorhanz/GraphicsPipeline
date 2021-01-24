
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind -std=c++11 -O3 -s EXPORTED_FUNCTIONS=['_main'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall']")

configure_file(index.html ${CMAKE_CURRENT_BINARY_DIR}/index.html COPYONLY)

find_package(GraphicsPipeline REQUIRED COMPONENTS Interface Native)

add_executable(CPP main.cpp)
target_link_libraries(CPP GP::Interface GP::Native)
