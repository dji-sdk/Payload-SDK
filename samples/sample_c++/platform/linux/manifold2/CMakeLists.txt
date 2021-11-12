cmake_minimum_required(VERSION 3.5)
project(dji_sdk_demo_linux_cxx CXX)

set(CMAKE_C_FLAGS "-pthread -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
add_definitions(-D_GNU_SOURCE)

# Try to see if user has OpenCV installed
# if yes, default callback will display the image
find_package( OpenCV QUIET )
if (OpenCV_FOUND)
    message( "\n${PROJECT_NAME}...")
    message( STATUS "Found OpenCV installed in the system, will use it to display image in AdvancedSensing APIs")
    message( STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
    message( STATUS " - Libraries: ${OpenCV_LIBRARIES}")
    add_definitions(-DOPEN_CV_INSTALLED)
else()
    message( STATUS "Did not find OpenCV in the system, image data is inside RecvContainer as raw data")
endif ()

set(COMMON_CXX_FLAGS "-std=c++11 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_CXX_FLAGS} -fprofile-arcs -ftest-coverage -Wno-deprecated-declarations")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")

include_directories(../../../module_sample)
include_directories(../common)

file(GLOB_RECURSE MODULE_SAMPLE_SRC
        ../../../module_sample/liveview/*.c*
        ../../../module_sample/perception/*.c*
        )
file(GLOB_RECURSE MODULE_COMMON_SRC ../common/*.c*)
file(GLOB_RECURSE MODULE_HAL_SRC hal/*.c*)
file(GLOB_RECURSE MODULE_APP_SRC application/*.c*)

execute_process(COMMAND uname -m
        OUTPUT_VARIABLE DEVICE_SYSTEM_ID)
if (DEVICE_SYSTEM_ID MATCHES x86_64)
    set(TOOLCHAIN_NAME x86_64-linux-gnu-gcc)
    add_definitions(-DPLATFORM_ARCH_x86_64=1)
elseif (DEVICE_SYSTEM_ID MATCHES aarch64)
    set(TOOLCHAIN_NAME aarch64-linux-gnu-gcc)
    add_definitions(-DPLATFORM_ARCH_aarch64=1)
else ()
    message(FATAL_ERROR "FATAL: Please confirm your platform.")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(FFMPEG REQUIRED)

if(FFMPEG_FOUND)
    message("Found FFMPEG FFMPEG_INCLUDE_DIR = ${FFMPEG_INCLUDE_DIR}")
    message("Found FFMPEG FFMPEG_LIBRARIES   = ${FFMPEG_LIBRARIES}")
else()
    message("Cannot Find FFMPEG")
endif(FFMPEG_FOUND)
include_directories(${FFMPEG_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/include)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME})
link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME}/libpayloadsdk.a -lstdc++)

if (NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif ()

add_executable(${PROJECT_NAME}
        ${MODULE_APP_SRC}
        ${MODULE_SAMPLE_SRC}
        ${MODULE_COMMON_SRC}
        ${MODULE_HAL_SRC})
target_link_libraries(${PROJECT_NAME} m usb-1.0 ${FFMPEG_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK COMMAND cmake ..
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
if (OpenCV_FOUND)
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif ()
