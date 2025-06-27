# DJI Payload SDK (PSDK)

![](https://img.shields.io/badge/version-V3.12.0-green.svg)
![](https://img.shields.io/badge/platform-linux_|_rtos-blue.svg)
![](https://img.shields.io/badge/license-MIT-orange.svg)

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

The latest release version of PSDK is 3.12.0. This version of Payload SDK mainly add some new features support and fixed some
bugs. Please refer to the release notes for detailed changes list.

### 1. Matrice 400 Feature Support
- **Added point cloud data subscription for LiDAR/millimeter-wave radar on aircraft.**
- **Added custom widget management.**
- **Added Expanded payload device support.**
   Supports up to 7 payload devices simultaneously, compatible with:
   - Skyport-V2
   - XPort

### 2. Manifold 3 Now Supports:
- **Added AR image drawing function.**
- **Added Enhanced warning functionality.**
- **Added H30 camera image processing capabilities.**
  - Together with Matrice 400, Manifold 3 can:
  - Obtain decoded images from the H30 camera.
  - Encode using a hardware encoder.
  - Display AI recognition results in real time.
- **Added PSDK application development**
  - Supports developing and packaging applications based on PSDK.
  - Supports installing and running applications on Manifold 3.

### 3. Matrice 4TD/4D Series Feature Optimization
- **Added cloud API firmware upgrade**

### 4. E-PORT V2 Hardware Port Support
- **Adapter compatibility:**
  - E-Port V2 coaxial cable
  - Skyport V3
- **Power management:**
  - Supports voltage requests for 13.6V, 17V, and 24V.
- **Link models:**
  - Added: ONLY_USB_BULK, ONLY_NETWORK.
  - Not supported: UART_AND_NETWORK, UART_AND_USB_BULK.
- **Development support:**
  - Released DJI E-Port V2 development kit.
  - Documentation: Refer to the Quick Start > E-Port Quick Start section.

###  5. Upgrade and Compatibility
#### SkyPort V2 and X-Port Software Upgrade
The software versions of Skyport V2 and X-Port have been upgraded. Devices need to be mounted on the Matrice 300 or Matrice 350 to be upgraded to the latest software version before they can be used on the Matrice 400 model. Subsequent software upgrades can be performed on the Matrice 400.

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
