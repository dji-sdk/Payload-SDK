#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.15)

# project settings
project(dji_sdk_demo_rtos C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

#Uncomment for hardware floating point
add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

#Uncomment for software floating point
#add_compile_options(-mfloat-abi=soft)

add_compile_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for speed")
    add_compile_options(-Ofast)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    message(STATUS "Maximum optimization for speed, debug info included")
    add_compile_options(-Ofast -g)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
else ()
    message(STATUS "Minimal optimization, debug info included")
    #    add_compile_options(-Og -g)
endif ()

include_directories(../../../../../module_sample
        ../../application
        ../../drivers/STM32F4xx_HAL_Driver/Inc
        ../../drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        ../../drivers/CMSIS/Device/ST/STM32F4xx/Include
        ../../drivers/CMSIS/Include
        ../../drivers/USB_HOST/Target
        ../../drivers/USB_HOST/App
        ../../drivers/BSP
        ../../hal/
        ../../../common/osal/
        ../../middlewares/Third_Party/FreeRTOS/Source/include
        ../../middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS
        ../../middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
        ../../middlewares/ST/STM32_USB_Host_Library/Core/Inc
        ../../middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../../../psdk_lib/include)
link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../../../psdk_lib/lib/arm-none-eabi-gcc)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../../../psdk_lib/lib/arm-none-eabi-gcc/libpayloadsdk.a)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx)

file(GLOB_RECURSE APP_SRC ../../application/*.c)
include_directories(../../application)
message("-- Use payload sdk package")
file(GLOB_RECURSE MODULE_SAMPLE_SRC
        ../../../../../module_sample/camera_emu/test_payload_cam_emu_common.c
        ../../../../../module_sample/gimbal_emu/*.c
        ../../../../../module_sample/data_transmission/*.c
        ../../../../../module_sample/fc_subscription/*.c
        ../../../../../module_sample/mop_channel/*.c
        ../../../../../module_sample/time_sync/*.c
        ../../../../../module_sample/positioning/*.c
        ../../../../../module_sample/xport/*.c
        ../../../../../module_sample/widget/*.c
        ../../../../../module_sample/upgrade/*.c
        ../../../../../module_sample/payload_collaboration/*.c
        ../../../../../module_sample/utils/*.c
        ../../../../../module_sample/power_management/*.c
        )

file(GLOB_RECURSE SOURCES
        "../../../common/osal/*.*"
        "../../drivers/BSP/*.*"
        "../../hal/*.*"
        "../../drivers/USB_HOST/*.*"
        "../../middlewares/ST/*.*"
        "../../middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/crpsdk_libine.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/event_groups.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/list.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/queue.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/timers.c"
        "../../middlewares/Third_Party/FreeRTOS/Source/tasks.c"
        "../../drivers/STM32F4xx_HAL_Driver/*.*"
        )

set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/STM32F407VGTX_FLASH.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m4 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${MODULE_SAMPLE_SRC} ${APP_SRC} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
