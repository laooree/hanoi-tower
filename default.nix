let
  pkgs = import <nixpkgs> {};
in

  pkgs.stdenv.mkDerivation {
    pname   = "hanoi";
    version = "0.0.1";

    src = ./.;

    buildPhase = ''
      gcc main.c
    '';

    installPhase = ''
      mkdir -p $out/bin
      cp a.out $out/bin/hanoi
    '';
  }
