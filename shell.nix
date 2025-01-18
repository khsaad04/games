{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  packages = [
    pkgs.clang-tools
    pkgs.raylib
  ];
}
