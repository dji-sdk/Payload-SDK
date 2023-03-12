Config items of CLion project:
1. Go to https://segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack
   For x64 ubuntu:
   Download <J-Link Software and Documentation pack for Linux, DEB installer, 64-bit>, double click the downloaded file to install.
2. Go to https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
   Download toolchain, for x64 ubuntu is <gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2>
   cd /usr/local/
   sudo tar xjf ~/Downloads/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
   add "export PATH=$PATH:/usr/local/gcc-arm-none-eabi-9-2020-q2-update/bin" to /etc/profile
   reboot system
3. Create External Tool named "fw_download", Program is $CMakeCurrentBuildDir$/download.sh and Work Directory is $CMakeCurrentBuildDir$
   You can add shortcuts to this external tool to quickly download and run program.
4. Create Embedded GDB Server, setting GDB Server to <cmake build dir>/gdbserver.sh, 'target remote' args to 127.0.0.1:2331

Note: double-check the all above path, and ensure they are correct
