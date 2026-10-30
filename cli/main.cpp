#include "cli-menu.hpp"
#include "kmz-cpm.hpp"

void displayXMLErrorMessage() {
  if (kmzcpm::xml::Parser::isError()) {
    cm::Console::logUnderlineString(
      "STATUS\n",
      cm::COLORS_SET[cm::RED]
    );

    cm::Console::logItalicString(
      kmzcpm::xml::Parser::getStatusMessage(),
      cm::COLORS_SET[cm::RED]
    );
  }
}

int main(int argc, char *argv[]) {

  cm::Program<cm::Word> *cli_kmzcpm = cm::Program<cm::Word>::create(
    "kmzcpm",
    cm::ProgramAbout(
      "Google Earth assisting software to manipulate KML/KMZ files",
      "Boston Sinaga",
      "https://github.com/bostonsinaga/kmz-cpm",
      cm::ProgramVersion(2, 0, 0)
    ),
    [](cm::Command *command)->cm::COMMAND_CALLBACK_CODE {

      kmzcpm::xml::Node *root = kmzcpm::xml::Parser::build(cm::Data::getWord(command));
      size_t num = 0;

      if (root) {
        root->traverse(kmzcpm::xml::Node::RIGHT, [&](kmzcpm::xml::Node *current)->bool {
          num++;
          return true;
        });

        displayXMLErrorMessage();
        root->annihilate();
      }

      std::cout << "NUMBER OF NODES: " << num << std::endl;
      return cm::COMMAND_CALLBACK_DONE;
    }
  );

  cli_kmzcpm->run(argc, argv);
  return 0;
}
