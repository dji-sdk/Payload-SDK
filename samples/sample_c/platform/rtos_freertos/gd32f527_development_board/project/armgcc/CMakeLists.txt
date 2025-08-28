set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.15)

project(dji_sdk_demo_rtos C CXX ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_definitions(ARM_MATH_CM33;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
add_compile_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)
add_link_options(-mfloat-abi=hard -mfpu=fpv4-sp-d16)

add_compile_options(-mcpu=cortex-m33 -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

add_compile_definitions(USE_USB_FS;USE_BOOTLOADER)

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

include_directories (
    ../../../../../module_sample
    ../../application
    ../../../common/osal
    ../../hal
    ../../osal
)

include_directories (
    ../../application
    ../../hal
    ../../../common/osal
    ../../middlewares/FreeRTOS-10.3.1/include
    ../../middlewares/FreeRTOS-10.3.1/portable/GCC/ARM_CM33_NTZ
    ../../middlewares/FreeRTOS-10.3.1/portable/GCC/ARM_CM33_NTZ/non_secure
    ../../../../../module_sample
    ../../../../../../../modules/aircraft_info/include
    ../../../../../../../modules/error/include
    ../../../../../../../modules/camera_manager/include
    ../../../../../../../modules/core/include
    ../../../../../../../modules/xxx_template/include
    ../../../../../../../modules/widget/include
    ../../../../../../../modules/waypoint_v3/include
    ../../../../../../../modules/waypoint_v2/include
    ../../../../../../../modules/upgrade/include
    ../../../../../../../modules/time_sync/include
    ../../../../../../../modules/power_management/include
    ../../../../../../../modules/positioning/include
    ../../../../../../../modules/platform/include
    ../../../../../../../modules/perception/include
    ../../../../../../../modules/payload_xport/include
    ../../../../../../../modules/payload_gimbal/include
    ../../../../../../../modules/payload_camera/include
    ../../../../../../../modules/payload_camera/payload_collaboration
    ../../../../../../../modules/fc_subscription/include
    ../../../../../../../modules/logger/include
    ../../../../../../../modules/data_channel/include
    ../../../../../../../modules/data_channel/high_speed_data_channel
    ../../../../../../../modules/data_channel/low_speed_data_channel
    ../../../../../../../modules/data_channel/stream_channel
    ../../../../../../../modules/data_channel/mop_channel/mop
    ../../../../../../../modules/flight_controller/include
    ../../../../../../../modules/liveview/include
    ../../../../../../../modules/gimbal_manager/include
    ../../../../../../../modules/hms/include
    ../../../../../../../modules/data_channel/mop_channel/mop/inc
    ../../../../../../../modules/battery/include
    ../../../../../../../modules/authentication/ciu98/huada_lib/inc
    ../../../../../../../modules/authentication
    ../../../../../../../modules/authentication/ciu98
    ../../drivers/CMSIS/include
    ../../drivers/CMSIS/GD/GD32F5xx/Include
    ../../drivers/BSP
    ../../drivers/GD32F5xx_standard_peripheral/Include
    ../../drivers/GD32F5xx_usb_library/device/core/Include
    ../../drivers/GD32F5xx_usb_library/driver/Include
    ../../drivers/GD32F5xx_usb_library/device/class/cdc/Include
    ../../drivers/GD32F5xx_usb_library/ustd/common
    ../../drivers/GD32F5xx_usb_library/ustd/class/cdc
)

link_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../../../../out/lib/arm-none-eabi-gcc)
link_libraries(${CMAKE_CURRENT_LIST_DIR}/../../../../../../../out/lib/arm-none-eabi-gcc/lib${PACKAGE_NAME}.a)

add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DGD32F527)

file(GLOB_RECURSE APP_SRC ../../application/*.c)

message("-- Use onboard sdk package")
file(GLOB_RECURSE MODULE_SAMPLE_SRC
        ../../../../../module_sample/camera_manager/*.c
        ../../../../../module_sample/payload_collaboration/*.c
        ../../../../../module_sample/camera_emu/test_payload_cam_emu_base.c
        ../../../../../module_sample/xport/*.c
        ../../../../../module_sample/gimbal_emu/*.c
        ../../../../../module_sample/data_transmission/*.c
        ../../../../../module_sample/fc_subscription/*.c
        ../../../../../module_sample/flight_control/*.c
        ../../../../../module_sample/gimbal_manager/*.c
        ../../../../../module_sample/waypoint_v2/*.c
        ../../../../../module_sample/hms/*.c
        ../../../../../module_sample/hotpoint/*.c
        ../../../../../module_sample/liveview/*.c
        ../../../../../module_sample/mop_channel/*.c
        ../../../../../module_sample/perception/*.c
        ../../../../../module_sample/waypoint_v2/*.c
        ../../../../../module_sample/upgrade/*.c
        ../../../../../module_sample/widget/*.c
        ../../../../../module_sample/utils/*.c
        ../../../../../module_sample/positioning/*.c
        ../../../../../module_sample/time_sync/*.c
        ../../../../../module_sample/waypoint_v3/*.c
        ../../../../../module_sample/power_management/*.c
        ../../../../../module_sample/tethered_battery/*.c
        )

# 添加第三方代码
file(GLOB_RECURSE SOURCES
    ../../../common/osal/osal.c
    ../../drivers/BSP/startup_gd32f5xx_armgcc.s
    ../../drivers/BSP/*.c
    ../../drivers/GD32F5xx_standard_peripheral/Source/*.c
    ../../drivers/CMSIS/GD/GD32F5xx/Source/syscalls.c
    ../../drivers/CMSIS/GD/GD32F5xx/Source/system_gd32f5xx.c
    ../../drivers/GD32F5xx_usb_library/driver/Source/drv_usb_core.c
    ../../drivers/GD32F5xx_usb_library/driver/Source/drv_usb_dev.c
    ../../drivers/GD32F5xx_usb_library/driver/Source/drv_usbd_int.c
    ../../drivers/GD32F5xx_usb_library/device/core/Source/usbd_core.c
    ../../drivers/GD32F5xx_usb_library/device/core/Source/usbd_enum.c
    ../../drivers/GD32F5xx_usb_library/device/core/Source/usbd_transc.c
    ../../drivers/GD32F5xx_usb_library/device/class/cdc/Source/cdc_acm_core.c
    ../../middlewares/FreeRTOS-10.3.1/portable/GCC/ARM_CM33_NTZ/non_secure/port.c
    ../../middlewares/FreeRTOS-10.3.1/portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c
    ../../middlewares/FreeRTOS-10.3.1/portable/MemMang/heap_4.c
    ../../middlewares/FreeRTOS-10.3.1/croutine.c
    ../../middlewares/FreeRTOS-10.3.1/event_groups.c
    ../../middlewares/FreeRTOS-10.3.1/list.c
    ../../middlewares/FreeRTOS-10.3.1/queue.c
    ../../middlewares/FreeRTOS-10.3.1/tasks.c
    ../../middlewares/FreeRTOS-10.3.1/stream_buffer.c
    ../../middlewares/FreeRTOS-10.3.1/timers.c
    ../../hal/hal_uart.c
    ../../hal/hal_i2c.c
)



#链接脚本
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/gd32f5xx_flash.ld)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
add_link_options(-mcpu=cortex-m33 -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${MODULE_SAMPLE_SRC} ${APP_SRC} ${LINKER_SCRIPT})
add_dependencies(${PROJECT_NAME}.elf djisdk)

set(DIS_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.dis)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE_SECTION -j.vectors -j.text -j.rodata -j.ARM.extab -j.ARM -j.preinit_array -j.init_array -j.fini_array -j.data)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf
        POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${PROJECT_NAME}.elf> > ${DIS_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${BIN_FILE_SECTION} $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${DIS_FILE}
Building ${HEX_FILE}
Building ${BIN_FILE}")

add_custom_command(TARGET ${PROJECT_NAME}.elf
        POST_BUILD
        COMMAND cp -rf  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.* ${PROJECT_BINARY_DIR}/../../../../../../../
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
