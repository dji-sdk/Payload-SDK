cmake_minimum_required(VERSION 2.8)
project(psdk_demo C)

### Set toolchain flags
set(CMAKE_C_FLAGS "-D_REENTRANT -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "-pthread -lpthread")

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(PSDK_ARCH_SYS linux)

include_directories(..)
include_directories(../../common)
include_directories(../../../../api_sample)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/api_headers)

## Srcs
file(GLOB_RECURSE APP_SRC ../../manifold2/application/*.c
        ../../manifold2/hal/*.c)
file(GLOB_RECURSE COMMON_SRC ../../common/*.c)
file(GLOB_RECURSE SAMPLE_SRC ../../../../api_sample/*.c)

## "uname -m" to auto distinguish Manifold2-G or Manifold2-C
execute_process(COMMAND uname -m
        OUTPUT_VARIABLE DEVICE_SYSTEM_ID)

if (NOT PSDK_ARCH_SYS)
    message(FATAL_ERROR
            "to use psdk inc src cmake ,
            you must set <PSDK_ARCH_SYS> value to indicate your system arch!
            This value can be linux, baremetal or rtos_xxxx (xxxx is the specific name of rtos."
            )
endif ()

string(TOUPPER ${PSDK_ARCH_SYS} PSDK_ARCH_SYS_DEF)
add_definitions(-DPSDK_ARCH_SYS_${PSDK_ARCH_SYS_DEF}=1)
add_definitions(-D_GNU_SOURCE)

## Include directories and library
if (DEVICE_SYSTEM_ID MATCHES x86_64)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/x86_64-linux-gnu-gcc)
    link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/x86_64-linux-gnu-gcc/libpayloadsdk.a)
elseif (DEVICE_SYSTEM_ID MATCHES aarch64)
    link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/aarch64-linux-gnu-gcc)
    link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/aarch64-linux-gnu-gcc/libpayloadsdk.a)
endif ()

## Outputs
add_executable(${CMAKE_PROJECT_NAME}
        ${APP_SRC}
        ${COMMON_SRC}
        ${SAMPLE_SRC}
        )

target_link_libraries(${CMAKE_PROJECT_NAME} m)


