# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.6.0-purple.svg)
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

PSDK 3.6.0 was released on 18 Sep 2023. This version of Payload SDK mainly add some new features support and fixed some
bugs.
Please refer to the release notes for detailed changes list.

* Added support for multi-language display in HMS management.
* Added support for PSDK Non-RC flight.
* Added support for aircraft version number retrieval.
* Added support for RTCM data subscription.
* Added support for cloud-based custom small data communication on Matrice 30/30T PSDK.
* Added Support for Matrice 30/30T PSDK speaker cloud integration.
* Added support for custom widgets wayline actions on Matrice 30/30T and Mavic 3E/3T aircraft.
* Added support for grayscale image retrieval on M350 RTK.
* Added support for time synchronization on M300 RTK and Matrice 30/30T.
* Added support for third-party firmware upgrades through M350 RTK E-Port.
* Added support for SDK interconnection on Mavic 3E/3T.
* Added support for hotspot orbit function on Mavic 3E/3T.
* Added support for expanded camera and gimbal management interfaces.
* Fixed issue with the 500m altitude limit for flight control across all models.
* Fixed issue with incomplete small data transmission on specific models.
* Fixed issue with H20 I-frame retrieval on M300 RTK.
* Fixed issue with occasional custom widget display failure in DJI Pilot.
* Fixed issues with multiple live stream retrieval on Matrice 30/30T.
* Fixed issues with camera mode retrieval on Matrice 30/30T.
* Fixed issues with camera focal length value retrieval on Matrice 30/30T.
* Fixed issue with X-Port failed to self-check when both X-Port and SkyPort V2 were equipped on M300 RTK.
* Fixed issue with gimbal angle subscription data mismatching App display on M300 RTK.
* Fixed issue with flight control feature initialization failure on specific models after multiple calls.
* Fixed compatibility issues with initialization failures on some HiSilicon platforms.
* Optimized PSDK startup delays in single UART mode.
* Optimized slow media file download speeds for Mavic 3E/3T on ethernet links.

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