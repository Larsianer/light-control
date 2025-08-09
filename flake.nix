{
    description = "Driver for LEDs in my room";

    inputs = {
        nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    };

    outputs = { self, nixpkgs, ...}: let
        system = "x86_64-linux";
    in {
        devShells."${system}".default = let
            pkgs = import nixpkgs {
                inherit system;
            };
        in pkgs.mkShell {
            packages = with pkgs; [
                platformio-core
                gnumake
                inetutils
            ];

            shellHook = ''
                exec zsh
                '';
        };
    };
}
