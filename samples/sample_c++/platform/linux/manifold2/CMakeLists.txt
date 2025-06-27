cmake_minimum_required(VERSION 3.5)
project(dji_sdk_demo_linux_cxx CXX)

set(CMAKE_C_FLAGS "-pthread -std=gnu99")
set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
add_definitions(-D_GNU_SOURCE)

if (MEMORY_LEAK_CHECK_ON MATCHES TRUE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=leak")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan")
endif ()

if (BUILD_TEST_CASES_ON MATCHES TRUE)
    set(COMMON_CXX_FLAGS "-std=c++11 -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -Wno-deprecated-declarations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
endif ()

include_directories(../../../module_sample)
include_directories(../../../../sample_c/module_sample)
include_directories(../common)
include_directories(../manifold2/application)

file(GLOB_RECURSE MODULE_SAMPLE_SRC
        ../../../module_sample/liveview/*.c*
        ../../../module_sample/camera_manager/*.c*
        ../../../module_sample/perception/*.c*
        ../../../module_sample/gimbal/*.c*
        ../../../module_sample/flight_controller/*.c*
        ../../../module_sample/hms_manager/*.c*
        ../../../../sample_c/module_sample/*.c
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
    add_definitions(-DPLATFORM_ARCH_AARCH64=1)
else ()
    message(FATAL_ERROR "FATAL: Please confirm your platform.")
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/3rdparty)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME})
link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME}/libpayloadsdk.a -lstdc++)

add_executable(${PROJECT_NAME}
        ${MODULE_APP_SRC}
        ${MODULE_SAMPLE_SRC}
        ${MODULE_COMMON_SRC}
        ${MODULE_HAL_SRC})

# Try to see if user has OpenCV installed
# if yes, default callback will display the image
find_package(OpenCV QUIET)
if (OpenCV_FOUND)
    message("\n${PROJECT_NAME}...")
    message(STATUS "Found OpenCV installed in the system, will use it to display image in AdvancedSensing APIs")
    message(STATUS " - Includes: ${OpenCV_INCLUDE_DIRS}")
    message(STATUS " - Libraries: ${OpenCV_LIBRARIES}")
    add_definitions(-DOPEN_CV_INSTALLED)
else ()
    message(STATUS "Did not find OpenCV in the system, image data is inside RecvContainer as raw data")
endif ()

find_package(FFMPEG REQUIRED)
if (FFMPEG_FOUND)
    message(STATUS "Found FFMPEG installed in the system")
    message(STATUS " - Includes: ${FFMPEG_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${FFMPEG_LIBRARIES}")

    EXECUTE_PROCESS(COMMAND ffmpeg -version
            OUTPUT_VARIABLE ffmpeg_version_output
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    string(REGEX MATCH "version.*Copyright" ffmpeg_version_line ${ffmpeg_version_output})
    string(REGEX MATCH " .* " ffmpeg_version ${ffmpeg_version_line})
    string(REGEX MATCH "^ 5.*$" ffmpeg_major_version ${ffmpeg_version})

    if (HEAD${ffmpeg_major_version} STREQUAL "HEAD")
        message(STATUS " - Version: ${ffmpeg_version}")
    else ()
        message(FATAL_ERROR " - Not support FFMPEG version: ${ffmpeg_major_version}, please install 4.x.x instead.")
    endif ()

    target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})
    include_directories(${FFMPEG_INCLUDE_DIR})
    add_definitions(-DFFMPEG_INSTALLED)
else ()
    message(STATUS "Cannot Find FFMPEG")
endif (FFMPEG_FOUND)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/include)

find_package(OPUS REQUIRED)
if (OPUS_FOUND)
    message(STATUS "Found OPUS installed in the system")
    message(STATUS " - Includes: ${OPUS_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${OPUS_LIBRARY}")

    add_definitions(-DOPUS_INSTALLED)
    target_link_libraries(${PROJECT_NAME} /usr/local/lib/libopus.a)
else ()
    message(STATUS "Cannot Find OPUS")
endif (OPUS_FOUND)

find_package(LIBUSB REQUIRED)
if (LIBUSB_FOUND)
    message(STATUS "Found LIBUSB installed in the system")
    message(STATUS " - Includes: ${LIBUSB_INCLUDE_DIR}")
    message(STATUS " - Libraries: ${LIBUSB_LIBRARY}")

    add_definitions(-DLIBUSB_INSTALLED)
    target_link_libraries(${PROJECT_NAME} usb-1.0)
else ()
    message(STATUS "Cannot Find LIBUSB")
endif (LIBUSB_FOUND)

if (NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif ()

target_link_libraries(${PROJECT_NAME} m)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
if (OpenCV_FOUND)
    target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif ()

target_link_libraries(${PROJECT_NAME} dl)

