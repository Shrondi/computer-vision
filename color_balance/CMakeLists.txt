CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(color_balance)
ENABLE_LANGUAGE(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -Wall")

FIND_PACKAGE(OpenCV REQUIRED )
LINK_LIBRARIES(${OpenCV_LIBS})
include_directories ("${OpenCV_INCLUDE_DIRS}")

add_executable(color_balance color_balance.cpp common_code.cpp common_code.hpp)
add_executable(color_balance_test_common_code test_common_code.cpp common_code.cpp
    common_code.hpp)
set_target_properties(color_balance_test_common_code PROPERTIES OUTPUT_NAME "test_common_code")

 
