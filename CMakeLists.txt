CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(matrices)

SET(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(Threads REQUIRED)

ADD_EXECUTABLE(mult matrix.cpp main.cpp)
TARGET_LINK_LIBRARIES(mult Threads::Threads)

ADD_EXECUTABLE(tests test.cpp matrix.cpp)
TARGET_LINK_LIBRARIES(tests Threads::Threads)
