cmake_minimum_required (VERSION 3.8)

set(This OrderBookTests)

set(Sources
    OrderBookTests.cpp
    OrdersTests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    gtest_main
    LowLatencyTradingSystem_lib
)

add_test(
    NAME ${This}
    COMMAND ${This}
)