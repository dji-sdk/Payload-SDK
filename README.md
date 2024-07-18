# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.9.1-red.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-blue.svg)
![](https://img.shields.io/badge/license-MIT-purple.svg)

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

The latest release version of PSDK is 3.9.1. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* GEO Caging feature supported for the M3D series models.
* Flight control function and waypoint mission supported for the FlyCart 30 model.
* Fixed an issue with subscribing to the flight control topics for M300 RTK and M350 RTK models.
* Resolved a problem with DJI Smart Controller Enterprise not recognizing PSDK payload devices on M300 RTK.
* Fixed an issue with acquiring the camera stream without I-frames on M3D series models.
* Added the MOP 49154 channel as an alternative to the 49152 channel for subscribing to the L2 camera's real-time 3D point cloud. This enhancement avoids conflicts when subscribing to point cloud data concurrently with Pilot2 and improves the quality of the point cloud data.
> Note: When using M300 RTK with the DJI Smart Controller Enterprise to activate payloads via the OSDK interface, the remote controller will not recognize the payload device if the virtual serial port is not utilized.

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
