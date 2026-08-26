#include "cli-menu.hpp"
#include "kmz-cpm.hpp"

int main(int argc, char *argv[]) {

  cm::Program<cm::Boolean> *cli_kmzcpm = cm::Program<cm::Boolean>::create(
    "kmzcpm",
    cm::ProgramAbout(
      "Google Earth assisting software to manipulate KML/KMZ files",
      "Boston Sinaga",
      "https://github.com/bostonsinaga/kmz-cpm",
      cm::ProgramVersion(2, 0, 0)
    )
  );

  cli_kmzcpm->run(argc, argv);
  return 0;
}
