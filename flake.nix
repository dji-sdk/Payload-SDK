{
  description = "Build DJI Payload SDK From Nix-Shell(Suggect to use docker image nixpkgs/nix-flakes:latest for a clean environment, otherwise may failed)";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1.*.tar.gz";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override {
          # Override stdenv in order to change compiler:
          # stdenv = pkgs.clangStdenv;
        }
        {
          packages = with pkgs; [
            cmake
            libusb1
            libopus
            gcc
            ffmpeg_4-full
            opencv3
          ];
        };
      });
    };
}

