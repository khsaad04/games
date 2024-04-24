{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };
  outputs =
    { flake-parts, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" ];
      perSystem =
        { pkgs, ... }:
        {
          devShells.default = pkgs.mkShell {
            packages = [
              pkgs.clang-tools
              pkgs.clang
              pkgs.gdb
              pkgs.raylib
            ];
          };
          packages =
            let
              buildPackage =
                name:
                pkgs.stdenv.mkDerivation {
                  name = "${name}";
                  src = ./.;
                  nativeBuildInputs = [ pkgs.clang ];
                  buildInputs = [
                    pkgs.raylib
                    pkgs.pkg-config
                  ];
                  buildPhase = ''
                    mkdir -p $out/bin
                    clang -O3 -Wall -Wextra -o ${name} ${name}.c -lraylib
                    chmod +x ${name}
                    cp ${name} $out/bin
                  '';
                };
            in
            rec {
              default = breakout;
              breakout = buildPackage "breakout";
              flappy_bird = buildPackage "flappy_bird";
            };
        };
    };
}
