
find_package(GTest REQUIRED)
find_package(GraphicsPipeline REQUIRED COMPONENTS Interface Native)

include(GoogleTest)
add_executable(TestC TestC.cpp)
target_link_libraries(TestC GTest::GTest GP::Interface GP::Native)
gtest_discover_tests(TestC XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_executable(TestCPP TestCPP.cpp)
target_link_libraries(TestCPP GTest::GTest GP::Interface GP::Native)
gtest_discover_tests(TestCPP XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

message(GP::Native)

add_executable(TestUtils Utils.cpp)
target_link_libraries(TestUtils GTest::GTest GP::Interface GP::Native)
gtest_discover_tests(TestUtils XML_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
