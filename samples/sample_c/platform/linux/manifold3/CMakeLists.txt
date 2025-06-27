cmake_minimum_required(VERSION 3.5)
project(dji_sdk_demo_on_manifold3 C)

set(CMAKE_C_FLAGS "-pthread -std=gnu99")
set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
set(CMAKE_EXE_LINKER_FLAGS "-pthread")
set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")
add_definitions(-D_GNU_SOURCE)

if (NOT USE_SYSTEM_ARCH)
    add_definitions(-DSYSTEM_ARCH_LINUX)
endif ()

if (BUILD_TEST_CASES_ON MATCHES TRUE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
endif ()

set(PACKAGE_NAME payloadsdk)

set(TOOLCHAIN_NAME aarch64-linux-gnu-gcc)
add_definitions(-DPLATFORM_ARCH_aarch64=1)

file(GLOB_RECURSE MODULE_COMMON_SRC ../common/*.c)
file(GLOB_RECURSE MODULE_HAL_SRC hal/*.c)
file(GLOB_RECURSE MODULE_APP_SRC application/*.c)
file(GLOB_RECURSE MODULE_SAMPLE_SRC ../../../module_sample/*.c)

include_directories(../../../module_sample)
include_directories(../common)
include_directories(application)

include_directories(../../../../../psdk_lib/include)
link_directories(../../../../../psdk_lib/lib/${TOOLCHAIN_NAME})
link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../psdk_lib/lib/${TOOLCHAIN_NAME}/lib${PACKAGE_NAME}.a)

if (NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
endif ()

add_executable(${PROJECT_NAME}
        ${MODULE_APP_SRC}
        ${MODULE_SAMPLE_SRC}
        ${MODULE_COMMON_SRC}
        ${MODULE_HAL_SRC})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../common/3rdparty)

target_link_libraries(${PROJECT_NAME} m dl)

add_custom_command(TARGET ${PROJECT_NAME}
        PRE_LINK COMMAND cmake ..
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# set app.json and build_dpk.sh path
set(JSON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app_json/app.json)
set(SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tools/build_dpk/build_dpk.sh)

# set psdk_libput dpk path
set(OUTPUT_PACKAGE ${CMAKE_BINARY_DIR}/dpk)

# generate .dpk app package
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD # generate .dpk package after make
    COMMAND bash ${SCRIPT_FILE} -i ${JSON_FILE} -o ${OUTPUT_PACKAGE}
    DEPENDS ${JSON_FILE}
    COMMENT "Running build_dpk.sh, generating .dpk app package from app.json file"
)
