{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };
  outputs = { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          clang-tools
          nil
          gdb
          astyle
          raylib
        ];
      };
      packages.${system} = {
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
      formatter.${system} = pkgs.nixpkgs-fmt;
    };
}
