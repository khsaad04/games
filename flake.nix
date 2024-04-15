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
          packages = with pkgs; [
            clang-tools
            nil
            gdb
            raylib
          ];
        };
        packages = {
          breakout = pkgs.stdenv.mkDerivation {
            name = "breakout";
            src = ./.;
            buildInputs = [ pkgs.raylib ];
            buildPhase = ''
              mkdir -p $out/bin
              cc -Wall -Wextra -o breakout breakout.c -lraylib
              chmod +x breakout
              cp breakout $out/bin
            '';
          };
          flappy-bird = pkgs.stdenv.mkDerivation {
            name = "flappy_bird";
            src = ./.;
            buildInputs = [ pkgs.raylib ];
            buildPhase = ''
              mkdir -p $out/bin
              cc -Wall -Wextra -o flappy_bird flappy_bird.c -lraylib
              chmod +x flappy_bird
              cp flappy_bird $out/bin
            '';
          };
        };
        formatter = pkgs.nixpkgs-fmt;
      };
    };
}
