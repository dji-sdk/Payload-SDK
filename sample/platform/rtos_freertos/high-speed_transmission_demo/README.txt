Payload SDK High-Speed Transmission Function Sample.

Test Environment:
Hardware:               Manifold 2-C/G
Serial:                 USB-to-Serial Controller CP210x
C Compiler:             GCC version 5.4.0 20160609
CMake Version:          Cmake version 3.5.1

Interaction Serial Configuration:
Baud Rate:              115200

Application Introduction:

This sample applies to multiprocessor or multi-SoC complex system, such as Payload SDK demo run on a RTOS development
board and the video stream need be sent by another device's (Manifold 2-C/G) network port. Because the RTOS is difficult
to deal with video stream. To get the local and remote network info about aircraft, manifold should communicate with
RTOS development board by usb-to-serial controller. Then manifold can send video stream via network to aircraft and DJI
Pilot APP/MSDK APP.

====== The steps to send test video stream and data stream ======
Step 1 : modify related device information, such as device name of network or uart, uart baud rate.

Step 2 : build source file, use following cmd:
cd source_code_dir/project
mkdir build
cd build
cmake ..
make

Step 3 : Manifold should connect to Skyport 2.0/XPort via network cable and connect to RTOS development board
via usb-to-serial controller, make sure the serial pins are connected properly. And please refer to README.txt file of
RTOS sample (just like, sample/platform/rtos_freertos/psdk_development_board_1.0/README.txt or
sample/platform/rtos_freertos/stm32f4_eval/README.txt) for interaction UART pins in RTOS system used to connect to
Manifold

Step 4 : make sure the RTOS environment is ok and enable the camera function to run the RTOS sample

Step 5 : use the root permission to execute this program, use following cmd:
sudo ./high-speed_transmission_demo

Step 6 : use DJI Pilot APP/MSDK APP to see video stream and receive test data