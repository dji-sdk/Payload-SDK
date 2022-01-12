Payload SDK package directory structure introduction:

doc/psdk_other_docs/psdk_code_style: Template files described code style of Payload SDK.
doc/simple_model: Some structural models of Skyport 1/Skyport 2/X-Port and aircraft. The structure models provide convenience for users to design payload structure.
psdk_lib/api_headers: Header files of Payload SDK, describing all API of Payload SDK. Users can understand features provided by Payload SDK and its usages relying on the header files.
psdk_lib/lib: Library files of Payload SDK. Users select appropriate library file to use based on target platform.
sample/api_sample: Sample code used to demonstrate usage of Payload SDK features.
sample/platform: Sample code related to specific platform, including Linux and FreeRTOS platforms.
sample/platform/linux/common/osal: OSAL layer implementation of Linux operation system.
sample/platform/linux/manifold2: Sample project used on Manifold 2.
sample/platform/rtos_freertos/common/osal: OSAL layer implementation of FreeRTOS operation system.
sample/platform/rtos_freertos/psdk_development_board_1.0: Sample project used on Payload SDK development board 1.0.
sample/platform/rtos_freertos/stm32f4_eval: Sample project used on STM32F4XX-EVAL development board.
tools: Some tools used with Payload SDK.
