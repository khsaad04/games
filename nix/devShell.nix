{
  systems = [ "x86_64-linux" ];
  perSystem =
    { pkgs, ... }:
    {
      devShells.default = import ../shell.nix { inherit pkgs; };
    };
}
