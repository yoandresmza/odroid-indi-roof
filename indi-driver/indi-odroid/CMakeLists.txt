cmake_minimum_required(VERSION 2.4.7)
PROJECT(odroid_indi_roof CXX C)

set(CMAKE_CXX_FLAGS "-o -lwiringPi -lpthread -std=c++0x ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
set (VERSION_MAJOR 0)
set (VERSION_MINOR 2)

find_package(INDI REQUIRED)
find_package(Nova REQUIRED)
# Locate libraries and headers
find_package(WiringPi REQUIRED)
find_package(Threads REQUIRED)

# Include headers
include_directories(${WIRINGPI_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${INDI_INCLUDE_DIR})
include_directories(${NOVA_INCLUDE_DIR})

################ Roof ################
set(odroidrolloff_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/odroidroof.cpp
   )

cmake_policy(SET CMP0003 OLD)
add_executable(indi_roof_odroid ${odroidrolloff_SRCS})
target_link_libraries(indi_roof_odroid ${INDI_DRIVER_LIBRARIES})
target_link_libraries(indi_roof_odroid ${WIRINGPI_LIBRARIES})
target_link_libraries(indi_roof_odroid ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS indi_roof_odroid RUNTIME DESTINATION bin )
install(FILES indi_roof_odroid.xml DESTINATION ${INDI_DATA_DIR})

