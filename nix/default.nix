{
  systems = [ "x86_64-linux" ];
  perSystem =
    { pkgs, ... }:
    {
      packages =
        let
          buildPackage =
            name:
            pkgs.stdenv.mkDerivation {
              name = "${name}";
              src = ../.;
              nativeBuildInputs = [ pkgs.clang ];
              buildInputs = [
                pkgs.raylib
                pkgs.pkg-config
              ];
              buildPhase = ''
                mkdir -p $out/bin
                clang -O3 -o ${name} ${../src/${name}.c} -lraylib
                chmod +x ${name}
                cp ${name} $out/bin
              '';
            };
        in
        {
          breakout = buildPackage "breakout";
          flappy_bird = buildPackage "flappy_bird";
          snake = buildPackage "snake";
        };
    };
}
