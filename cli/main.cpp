#include "cli-menu.hpp"
#include "kmz-cpm.hpp"
#include "txt-menu.hpp"
#include "kml-menu.hpp"
#include "csv-menu.hpp"

int main(int argc, char *argv[]) {

  // root menu
  cm::Program<cm::Boolean> *cm_kmzcpm = cm::Program<cm::Boolean>::create(
    "kmzcpm",
    cm::ProgramAbout(
      "KMZ Copy Paste Machine.\nEdit and analyze KML/KMZ geospatial data.",
      "Boston Sinaga",
      "https://github.com/bostonsinaga/kmz-cpm",
      cm::ProgramVersion(2, 0, 0)
    )
  );

  // children menu initialization
  initTXTMenu(cm_kmzcpm);
  initKMLMenu(cm_kmzcpm);
  initCSVMenu(cm_kmzcpm);

  // theme
  cm::Console::outlineStyle = false;

  // run and destroy
  cm_kmzcpm->run(argc, argv);
  cm_kmzcpm->destroy();

  return 0;
}
