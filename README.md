# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.8.1-purple.svg)
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
the [DJI Developer Documentation](https://developer.dji.com/doc/payload-sdk-tutorial/en/). Documentation regarding the
code can be found in the [PSDK API Reference](https://developer.dji.com/doc/payload-sdk-api-reference/en/)
section of the developer's website. Please visit
the [Latest Version Information](https://developer.dji.com/doc/payload-sdk-tutorial/en/)
to get the latest version information.

## Latest Release

The latest release version of PSDK is 3.8.1. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* Removed the camera management module interface DjiCameraManager_GetPhotoBurstCount.
* Removed the AEB photography function in the camera management module.
* Fixed the issue where subscribing to real-time point cloud data was not byte-aligned on some platforms.
* Fixed the discrepancy between flight speed units and annotations in the motion planning WP 2.0 feature.
* Fixed issues with obtaining the camera shooting mode and zoom magnification on the Mavic 3 series drones.
* Optimized the startup time for PSDK initialization.
* Complemented the flight control topic CONTROL_DEVICE data structure definition and added related enumeration ranges.
* Fixed the incorrect zoom multiplier retrieval for the H20N camera.
* Fixed the issue where custom media file suffix names were not taking effect.
* Fixed the occasional subscription errors for single battery data in the M300 RTK and M350 RTK.
* Fixed anomalous Pilot widget values for the M30 series, M300 RTK, and M350 RTK remote controllers.
* Note: Streamlined the toolchain for compiling PSDK, removing low-usage toolchain libraries.
> We have stopped offering some toolchains on GitHub. If you have trouble building your project, reach out to the SDK Support Team for help.

## License

Payload SDK codebase is MIT-licensed. Please refer to the LICENSE file for detailed information.

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

## About Pull Request
As always, the DJI Dev Team is committed to improving your developer experience, and we also welcome your contribution,
but the code review of any pull request maybe not timely, when you have any questionplease send an email to dev@dji.com.
