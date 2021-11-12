# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.0.0-orange.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-green.svg)
![](https://img.shields.io/badge/license-MIT-blue.svg)
[![Join the chat at https://gitter.im/dji-sdk/Payload-SDK](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/dji-sdk/Payload-SDK?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

## What is the DJI Payload SDK?

The DJI Payload SDK(PSDK), is a development kit provided by DJI to support developers to develop payload that can be mounted 
on DJI drones. Combined with the X-Port, SkyPort or extension port adapter, developers can obtain the information or other 
resource from the drone. According to the software logic and algorithm framework designed by the developer, users could 
develop payload that can be mounted on DJI Drone, to perform actions they need, such as Automated Flight Controller, 
Payload Controller, Video Image Analysis Platform, Mapping Camera, Megaphone And Searchlight, etc.

## Documentation

For full documentation, please visit the [DJI Developer Documentation](https://developer.dji.com/payload-sdk/documentation/). 
Documentation regarding the code can be found in the [PSDK API Reference](https://developer.dji.com/payload-api-reference/introduction/index.html) section of the developer's website.
Please visit the [Latest Version Information](https://developer.dji.com/payload-sdk/documentation/appendix/firmware.html) to get the latest version information.

## Latest Release
PSDK 3.0.0 was released on 20 October 2021. This version inherit the PSDK 2.2.1 existing functions (use X-Port or Skyport-V2 
to develop payload), and add extension port existing functions(based on OSDK 4.1.0) support, add the DJI offline certificate 
support, add the serial port baud rate adaptive support, add the arrest-flying feature support in flight-controller module, 
add the custom widget feature support on extend port, add the remote controller custom button mapping function support, 
add the DJI L1/P1 camera support in camera-manager module. Also enhance the camera media file download speed and fix some 
bugs on the last version. At the same time, add more api sample support and more toolchain support.

## License

Payload SDK codebase is MIT-licensed. Please refer to the LICENSE file for detailed information.

## Contributing

Please read the following guidelines before starting work on a pull request.

#### Summary of the guidelines:

* One pull request per issue;
* Choose the right base branch;
* Clean up "oops" commits before submitting;
* Follow the coding style by "doc/dji_sdk_code_style"

## Support

You can get official support from DJI and the community with the following methods:

- Post questions on Developer Forum
  * [DJI SDK Developer Forum(Cn)](https://djisdksupport.zendesk.com/hc/zh-cn/community/topics)
  * [DJI SDK Developer Forum(En)](https://djisdksupport.zendesk.com/hc/en-us/community/topics)
- Submit a request describing your problem on Developer Support
  * [DJI SDK Developer Support(Cn)](https://djisdksupport.zendesk.com/hc/zh-cn/requests/new)
  * [DJI SDK Developer Support(En)](https://djisdksupport.zendesk.com/hc/en-us/requests/new)
  
You can also communicate with other developers by the following methods:

- Github Issues or [gitter.im](https://gitter.im/dji-sdk/Payload-SDK)
- Post questions on [**Stackoverflow**](http://stackoverflow.com) using [**dji-sdk**](http://stackoverflow.com/questions/tagged/dji-sdk) tag