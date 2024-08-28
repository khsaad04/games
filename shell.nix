{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  packages = [
    pkgs.clang-tools
    pkgs.clang
    pkgs.gdb
    pkgs.raylib
    pkgs.pkg-config
  ];

}
