{
  description = "Towers of Hanoi";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f system);
    in {
      packages = forAllSystems (system:
        let pkgs = nixpkgs.legacyPackages.${system};
        in {
          hanoi = pkgs.stdenv.mkDerivation {
            pname   = "hanoi";
            version = "0.1.0";

            src = ./.;

            buildPhase = ''
              gcc main.c -o hanoi
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp hanoi $out/bin/hanoi
            '';
          };

          default = self.packages.${system}.hanoi;
        });
    };
}
