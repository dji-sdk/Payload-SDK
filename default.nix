{
  pkgs ? import <nixpkgs> { },
}:
pkgs.stdenv.mkDerivation rec {
  pname = "dji-payload-sdk";
  version = "1.0";

  src = ./.;

  enableParallelBuilding = true;
  # 此选项禁用了对 CMake 软件包的一些自动修正
  # dontFixCmake = true;

  nativeBuildInputs = with pkgs; [
    cmake
    libusb1
    libopus
    gcc
    ffmpeg_4-full
    opencv3
  ];

  # 传给 CMake 的配置参数，控制 liboqs 的功能
  cmakeFlags = [ ];

  postInstall = ''
    mkdir -p $out/bin
    install -m755 bin/* $out/bin
  '';
}
