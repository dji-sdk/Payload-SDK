# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.4.0-pink.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-green.svg)
![](https://img.shields.io/badge/license-MIT-blue.svg)

## What is the DJI Payload SDK?

The DJI Payload SDK(PSDK), is a development kit provided by DJI to support developers to develop payload that can be
mounted on DJI drones. Combined with the X-Port, SkyPort or extension port adapter, developers can obtain the
information or other resource from the drone. According to the software logic and algorithm framework designed by the
developer, users could develop payload that can be mounted on DJI Drone, to perform actions they need, such as Automated
Flight Controller, Payload Controller, Video Image Analysis Platform, Mapping Camera, Megaphone And Searchlight, etc.

## Documentation

For full documentation, please visit
the [DJI Developer Documentation](https://developer.dji.com/doc/payload-sdk-tutorial/en/). Documentation
regarding the code can be found in the [PSDK API Reference](https://developer.dji.com/doc/payload-sdk-api-reference/en/)
section of the developer's website. Please visit
the [Latest Version Information](https://developer.dji.com/doc/payload-sdk-tutorial/en/)
to get the latest version information.

## Latest Release

PSDK 3.4.0 was released on 18 April 2023. This version of Payload SDK mainly add some new features support and fixed
some bugs. Please refer to the release notes for detailed changes list.

* Added support for SDK interconnection on M30/M30T
* Added support for downloading camera media files on M3E/M3T
* Added support for infrared temperature in camera management on M30T/M3T
* Added support for waypoint action status push on M30/M30T and M3E/M3T
* Added support for custom HMS function on all models
* Added support for custom device version number and serial number on all models
* Added support for subscribing to video stream request I-frame on all models
* Added support for obtaining H20N laser distance measurement data camera management on M300 RTK
* Added support for controlling H20N infrared zoom in camera management on M300 RTK
* Added support for subscribing perception grayscale image by Ethernet on M30/M30T and M3E/M3T
* Added support for downloading camera media files by Ethernet on M30/M30T and M3E/M3T
* Added support for Nvidia Jeston series development boards sample
* Added support for H20/H20T model files
* Fixed issue with unsuccessful initialization in USB single Bulk mode on M30/M30T and M3E/M3T
* Fixed issue with unstable data transmission OSDK expansion port to PSDK port for passing through small data on M300 RTK
* Fixed issue with third-party camera function causing payload disconnection due to deleting all media files in certain platforms on M300 RTK
* Fixed issue with gimbal mode abnormal setting in gimbal management on M300 RTK
* Fixed issue with deleting L1 camera media files in camera management on M300 RTK
* Fixed issue with abnormal triggering of text input box widget function in RTOS platform on M300 RTK
* Fixed issue with abnormal payload power supply during hot plugging in some scenarios in RTOS platform on M3E/M3T
* Fixed issue with CPU loading abnormal usage due to multiple video stream subscriptions on M30/M30T
* Fixed issue with occasional payload negotiation failure on M30/M30T and M3E/M3T
* Fixed issue with obtaining camera work mode on M30/M30T and M3E/M3T
* Fixed issue with obtaining camera zoom parameters on M30/M30T and M3E/M3T
* Fixed issue with altitude and distance limitations when using joystick function in some scenarios on all models
* Fixed some memory leak issues
* Optimized camera management C++ Sample support
* Optimized gimbal management C++ Sample support

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

- Post questions on [**Stackoverflow**](http://stackoverflow.com) using [**
  dji-sdk**](http://stackoverflow.com/questions/tagged/dji-sdk) tag