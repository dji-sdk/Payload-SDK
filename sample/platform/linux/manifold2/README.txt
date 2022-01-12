Payload SDK Linux Sample.

Test Environment:
Hardware:               Manifold2-C/G
Serial:                 USB-to-Serial Controller CP210x
C Compiler:             GCC version 5.4.0 20160609
CMake Version:          Cmake version 3.5.1
IDE:                    CLion 2020.3.1

Communication Serial Configuration:
Baud Rate:              460800

Network Port Configuration:
IP Address:             Auto Config (Need root permission)
Network Port:           Auto Config
NetMask:                Auto Config
Gateway:                Auto Config

====== The steps to run the Payload SDK linux sample  ======
Step 1 : modify related device information, such as device name of network or uart, uart baud rate.

Step 2 : fill in your app info to file with path 'sample/platform/linux/manifold2/application/app_info.h'.
This info can be obtained by logging in to the developer website https://developer.dji.com/payload-sdk.

Step 3 : build source file, use following cmd:
cd source_code_dir/project
mkdir build
cd build
cmake ..
make

Step 4 : Manifold should connect to Skyport 2.0/XPort via usb-to-serial controller and network cable, make sure the
serial pins are connected properly.

Step 5 : use the root permission to execute this program, use following cmd:
sudo ./psdk_demo

Step 6: open DJI Pilot APP/MSDK APP to use the payload demo functions.