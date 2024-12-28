{
  pkgs ? import <nixpkgs> { },
}:
pkgs.stdenv.mkDerivation {
  name = "games";
  srcs = ./.;
  nativeBuildInputs = [ pkgs.clang ];
  buildInputs = [
    pkgs.raylib
    pkgs.pkg-config
  ];
  buildPhase = ''
    mkdir -p $out/bin

    clang -O3 -o breakout ./src/breakout.c -lraylib -lm
    cp breakout $out/bin

    clang -O3 -o flappy_bird ./src/flappy_bird.c -lraylib -lm
    cp flappy_bird $out/bin

    clang -O3 -o snake ./src/snake.c -lraylib -lm
    cp snake $out/bin

    chmod +x $out/bin/* 
  '';
}
