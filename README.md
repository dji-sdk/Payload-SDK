# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.9.0-pink.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-cyan.svg)
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

The latest release version of PSDK is 3.9.0. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* Supports the payload camera Zenmuse H30 series.
* FlyCart 30 aircraft supports the time synchronization function.
* FlyCart 30 aircraft supports HMS functions.
* FlyCart 30 aircraft supports the data subscription function.
* Fixed the issue with inaccurate zoom multiplier retrieval for the Mavic 3 Enterprise series cameras.
* Fixed data errors on the receiving end of small data transfers from MSDK to PSDK.
* Fixed the inability of devices at payload ports 2 and 3 of the M300 RTK aircraft to perform small data transmission to devices connected to the OSDK port.
* Fixed the issue where MSDK V5 and PSDK devices could not perform small data transmission when using M300 RTK and M350 RTK aircraft.
* Fixed occasional payload negotiation failures between PSDK and the M350 RTK aircraft.
* Fixed screen flickering and glitching issues with the H20T payload camera's video stream.
* Fixed issues with retrieving the visible light camera video stream from the M3TD camera.
* Optimized the C++ camera management sample.
* Optimized memory usage strategy for STM32 platform sample by introducing CCMRAM.
> Note: For M300 RTK and M350 RTK, the camera management module now checks the payload port device's status at startup. This may occasionally result in log errors. Normal function is not affected, and this will be optimized in a future release.

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
