# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.8.0-cyan.svg)
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

The latest release version of PSDK is 3.8.0. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

* Added support for the M3D series models
* Fixed memory leak in the HMS Sample
* Resolved compilation errors of Sample header files in certain environments
* Fixed unexpected crashes in C++ Sample when attempting to use unsupported features
* Enhanced prompts for camera Sample file downloads
* Optimized some improper uses of CMake in samples
* Change firmware version numbers in the PSDK Sample now align across DJI Assistant 2 and Pilot 2.
> Note: DJI Assistant 2 prevents firmware downgrading during payload upgrades. Ensure your upgrade package's name (the version number) is higher than the payload's reported firmware version.
> To comply with DJI Assistant 2 verification: You can simply rename your payload firmware upgrade package to make it valid, or maintain the previous practice of independently setting a version number to be checked during upgrades.
* In the STM32 FreeRTOS Sample, the heap size has been increased to 90,000 bytes.
> This may cause startup or build failures on some devices. You can adjust the stack size according to the business requirements of your hardware.

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
