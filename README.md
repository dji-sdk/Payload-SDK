# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.5.0-yellow.svg)
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

PSDK 3.5.0 was released on 18 May 2023. This version of Payload SDK mainly add support for M350 RTK, also add some new features support and fixed some bugs. 
Please refer to the release notes for detailed changes list.

* Added support for new drone model Matrice 350 RTK
* Matrice 300 RTK added support for DJI RC Plus
* Added support for downloading media files in pieces on all drone models
* Added support for Osal get random number callback interface on all drone model
* Fixed issue with the black screen problem when uses third-party camera to playback video on M300 RTK and DJI smart controller screen combination
* Fixed issue with failure of subscribing L1 camera code stream on M300 RTK
* Fixed issue with failure of subscribing main camera code stream in dual control mode on M300 RTK
* Fixed issue with local upgrade function of RTOS platform on Mavic 3E/3T
* Fixed issue with the initialization failure of using PSDK Payload device and millimeter wave radar on M300 RTK
* Fixed issue with custom HMS function on M300 RTK and X-Port combination
* Fixed issue with invalid TTS option of speaker widget on M300 RTK
* Fixed issue with downloading file list with high probability of failure when the camera photos are over 400 on M300 RTK
* Fixed some of the memory leaks

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