cmake_minimum_required(VERSION 2.4.7)
PROJECT(odroid_indi_roof CXX C)

set(CMAKE_CXX_FLAGS "-o -lwiringPi -lpthread -std=c++0x ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 2)

find_package(INDI REQUIRED)
find_package(Nova REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INDI_INCLUDE_DIR})
include_directories(${NOVA_INCLUDE_DIR})

################ Roll Off ################
set(odroidrolloff_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/odroidroof.cpp
   )

add_executable(odroid_indi_roof ${odroidrolloff_SRCS})
target_link_libraries(odroid_indi_roof ${INDI_DRIVER_LIBRARIES})
install(TARGETS odroid_indi_roof RUNTIME DESTINATION bin )
install(FILES odroid_indi_roof.xml DESTINATION ${INDI_DATA_DIR})

