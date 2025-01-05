# CMakeList.txt : CMake project for low-latency-trading-system, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project(LowLatencyTradingSystem VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

enable_testing()

add_subdirectory(googletest)

set(Headers
    ./Order_Book/Book.hpp
    ./Order_Book/Limit.hpp
    ./Order_Book/Order.hpp
)
set(Sources
    ./Order_Book/Book.cpp
    ./Order_Book/Limit.cpp
    ./Order_Book/Order.cpp
)

add_library(${PROJECT_NAME}_lib STATIC ${Sources} ${Headers})

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)