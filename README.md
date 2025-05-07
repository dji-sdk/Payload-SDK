# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.11.1-yellow.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-pink.svg)
![](https://img.shields.io/badge/license-MIT-red.svg)

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

The latest release version of PSDK is 3.11.1. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* Fixed the issue where the Matrice 4D or Matrice 4TD E-Port Lite interface was not recognized on FlightHub 2.
* Fixed the crash issue when using the DjiCore_Delnit interface.
* Fixed the occasional crash issue when transmitting video streams through the USB bulk channel on Matrice 350 RTK.

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
