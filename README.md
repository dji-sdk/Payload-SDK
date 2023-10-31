# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.7.0-green.svg)
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

PSDK 3.7.0 was released on 31 Oct 2023. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* Added L2 new camera model support
* Added FlyCart 30 new drone model support
* Added support for L2 subscription 3D point cloud data function
* Added L2 support for downloading original point cloud files
* Fixed the issue where M300 RTK and M350 RTK occasionally failed to negotiate load
* Fixed an issue where some open source library conflicts caused flight control module registration to fail
* Fixed the issue where PSDK obtains aircraft version numbers and displays them in reverse order
* Fixed an issue where the zoom value range of some camera zoom rings was incorrect
* Fixed the problem that the Sample value not updated when repeatedly running the M30/M3 series drone data subscription
* Fixed compatibility issues on some ESP32 platforms
* Optimize the return value prompt of MOP channel closing and destruction
* Optimize the point of interest surround function Sample log prompt
* Optimize compilation warning level

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
