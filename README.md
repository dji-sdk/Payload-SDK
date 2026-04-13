# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.16.0-orange.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-green.svg)
![](https://img.shields.io/badge/license-MIT-pink.svg)

## What is the DJI Payload SDK?

The DJI Payload SDK(PSDK), is a development kit provided by DJI to support developers to develop payload that can be
mounted on DJI drones. Combined with the X-Port, SkyPort or extension port adapter, developers can obtain the
information or other resource from the drone. According to the software logic and algorithm framework designed by the
developer, users could develop payload that can be mounted on DJI Drone, to perform actions they need, such as Automated
Flight Controller, Payload Controller, Video Image Analysis Platform, Mapping Camera, Megaphone And Searchlight, etc.

## Documentation

For full documentation, please visit
the [DJI Developer Documentation](https://developer.dji.com/doc/payload-sdk-tutorial/en/) the [DJI Developer Documentation Chinese Version](https://developer.dji.com/cn/payload-sdk/).  Documentation regarding the
code can be found in the [PSDK API Reference](https://developer.dji.com/doc/payload-sdk-api-reference/en/)
section of the developer's website. Please visit
the [Latest Version Information](https://developer.dji.com/doc/payload-sdk-tutorial/en/)
to get the latest version information.

## Latest Release

The latest release version of PSDK is 3.16.0. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

### New Features

* Added support for installing software dependencies on Manifold 3 without root privileges through the install.py script.
* Added support for external network adapters and network configuration on Manifold 3. RTL8852BU and RTL88X2BU USB Wi‑Fi adapters are currently supported.
* Added support for Attitude Mode control on Matrice 4T/4E.
* Added support for retrieving the aircraft remaining flight time and the low-battery RTH threshold on Matrice 400 and Matrice 4T/4E.
* Added support for camera status push on Matrice 4T/4E, including camera mode, zoom ratio, and other information.
* Added support for enabling Transport Control Mode on Matrice 400.

### Fixes and Optimizations

* Fixed an issue where DjiPowerManagement_OutputHighPower could not request high voltage on Manifold 3. Resolved after updating the Manifold 3 firmware.
* Fixed an issue on Matrice 400 where DjiTimeSync_TransferToAircraftTime failed to convert time. Resolved after updating the PSDK version.
* Fixed an issue where the payload camera on Port 2 of Matrice 400 could not be controlled properly. Resolved after updating the PSDK version.
* Optimized the default PSDK build directory configuration. The default target has been changed from Manifold 2 to Manifold 3.
* Fixed an issue where the PSDK C++ Sample failed to start due to a missing data/logs directory when run directly.
* Optimized exception logging and removed redundant warning messages in the PSDK C / C++ Samples.
* Added mechanism description and usage documentation for the Manifold 3 adaptive power feature.

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
