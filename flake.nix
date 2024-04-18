{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };
  outputs = { flake-parts, ... } @ inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" ];
      perSystem = { pkgs, ... }: {
        devShells.default = pkgs.mkShell {
          packages = [
            pkgs.clang-tools
            pkgs.clang
            pkgs.nil
            pkgs.gdb
            pkgs.raylib
          ];
        };
        packages = {
          breakout = pkgs.stdenv.mkDerivation {
            name = "breakout";
            src = ./.;
            buildInputs = [ pkgs.raylib pkgs.clang ];
            buildPhase = ''
              mkdir -p $out/bin
              clang -O3 -Wall -Wextra -o breakout breakout.c -lraylib
              chmod +x breakout
              cp breakout $out/bin
            '';
          };
          flappy_bird = pkgs.stdenv.mkDerivation {
            name = "flappy_bird";
            src = ./.;
            buildInputs = [ pkgs.raylib pkgs.clang ];
            buildPhase = ''
              mkdir -p $out/bin
              clang -O3 -Wall -Wextra -o flappy_bird flappy_bird.c -lraylib
              chmod +x flappy_bird
              cp flappy_bird $out/bin
            '';
          };
        };
        formatter = pkgs.nixpkgs-fmt;
      };
    };
}
