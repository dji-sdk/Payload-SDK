# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.3.0-red.svg)
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

PSDK 3.3.0 was released on 18 November 2022. This version of Payload SDK mainly add support for Mavic 3E/3T, also add
some new features support and fixed some bugs. Please refer to the release notes for detailed changes list.

* Add the Mavic 3E/3T support
* Add getting the perception image support on M30/M30T and M3E/M3T
* Add downloading and deleting camera media file support on M30/M30T
* Add USB convert to Ethernet support on M30/M30T and M3E/M3T
* Add hybrid zoom function support of camera H20/H20T/H20N in Camera Manager module
* Add gimbal speed control mode in Gimbal Manager module
* Add getting aircraft connection state support in Aircraft Info module
* Fix the problem that Camera P1 can not set the parameter of interval shooting in Camera Manager module
* Fix the camera type recognition problem of H20 in Camera Manager module
* Fix the abnormal camera switching working mode of H20/H20T/H20N in Camera Manager module
* Fix the infrared zoom interface error in Camera Manager module
* Fix the occasional problem that force landing fail in Flight Controller module
* Fix the abnormal display in specific configuration of custom widget function
* Fix the occasional black screen when the M300 RTK PSDK camera is playing back
* Fix the abnormal display of custom widget icon in Pilot App because the dual PSDK payload is mounted on M300 RTK.
* Fix the occasional abnormal bandwidth occupation of Data Subscription function on M30/M30T
* Optimize the getting the camera laser measuring data function in Camera Manager module
* Optimize the Gimbal Manager module Sample
* Optimize the channel configuration method of USB-Bulk and Ethernet
* Optimize the problem that the memory occupation is too large in STM32 platform in some cases
* Optimize the import audio transmission quality of Speaker widget function

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