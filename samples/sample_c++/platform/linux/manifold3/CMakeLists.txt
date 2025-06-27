cmake_minimum_required(VERSION 3.5)
project(dji_sdk_demo_on_manifold3_cxx LANGUAGES CXX C)

set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc" CACHE STRING "C compiler")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++" CACHE STRING "C++ compiler")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -std=gnu99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_compile_definitions(
    _GNU_SOURCE
    PLATFORM_ARCH_AARCH64=1
    SYSTEM_ARCH_LINUX=1
)

include_directories(
    ../../../module_sample
    ../../../../sample_c/module_sample
    ../common
    ../manifold3/application
    include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/include)
)

file(GLOB_RECURSE MODULE_SAMPLE_SRC
    ../../../module_sample/liveview/*.c*
    ../../../module_sample/camera_manager/*.c*
    ../../../module_sample/hms_manager/*.c*
    ../../../module_sample/perception/*.c*
    ../../../module_sample/gimbal/*.c*
    ../../../module_sample/flight_controller/*.c*
    ../../../module_sample/hms_manager/*.c*
    ../../../module_sample/widget_manager/*.c*
    ../../../../sample_c/module_sample/*.c
    ../../../../sample_c/module_sample/widget_interaction_test/*.c
)
file(GLOB_RECURSE MODULE_COMMON_SRC ../common/*.c*)
file(GLOB_RECURSE MODULE_HAL_SRC hal/*.c*)
file(GLOB_RECURSE MODULE_APP_SRC application/*.c*)

set(TOOLCHAIN_NAME aarch64-linux-gnu-gcc)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME})

add_executable(${PROJECT_NAME}
    ${MODULE_APP_SRC}
    ${MODULE_SAMPLE_SRC}
    ${MODULE_COMMON_SRC}
    ${MODULE_HAL_SRC}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME}/libpayloadsdk.a
        stdc++
        m
        dl
)

# OpenCV
find_package(OpenCV QUIET)
if (OpenCV_FOUND)
    message("\n${PROJECT_NAME}...")
    message(STATUS "Found OpenCV installed in the system, will use it to display image in AdvancedSensing APIs")
    message(STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS " - Libraries: ${OpenCV_LIBRARIES}")
    add_definitions(-DOPEN_CV_INSTALLED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBRARIES})
else ()
    message(STATUS "Did not find OpenCV in the system, image data is inside RecvContainer as raw data")
endif ()

# FFMPEG
find_package(PkgConfig)
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
if (FFMPEG_FOUND)
    message(STATUS "Found FFMPEG installed in the system")
    message(STATUS " - Includes: ${FFMPEG_INCLUDE_DIRS}")
    message(STATUS " - Libraries: ${FFMPEG_LIBRARIES}")

    target_link_libraries(${PROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_compile_definitions(${PROJECT_NAME} PRIVATE FFMPEG_INSTALLED)
else()
    message(STATUS "Cannot Find FFMPEG")
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)