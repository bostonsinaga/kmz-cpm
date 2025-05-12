#include <ctime>
#include "wrappers/call-briefer.h"

int main(int argc, char *argv[]) {

  double startTime = std::time(0);

  // convert 'argv' to vector start from index 1
  std::vector<std::string> inputStrings;
  for (int i = 1; i < argc; i++) {
    inputStrings.push_back(argv[i]);
  }

  ////////////////////////
  //// MENU SELECTION ////
  ////////////////////////

  Menu menu = Menu(&inputStrings);

  // make row space under the command in CLI
  std::cout << std::endl;

  int SELECTED_FLAG = menu.select();

  /////////////////////
  // MAIN MENU FLAGS //
  /////////////////////

  if (SELECTED_FLAG == MAIN_MENU_FLAG ||
    SELECTED_FLAG == HELP_MENU_FLAG
  ) {
    menu.displayAvailableCommands();
  }
  else if (SELECTED_FLAG == VERSION_NUMBER_FLAG) {
    menu.displayVersionNumber();
  }

  ////////////////////////
  // CONVERT TXT TO KML //
  ////////////////////////
  
  else if (SELECTED_FLAG == CONVERT_TXT_KML_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("txt-in"),
      fileOut_paramStr = menu.getParameterString("kml-out"),
      type_paramStr = menu.getParameterString("type");

    bool sort_toggleBool = menu.isToggleExist("sort");

    // from south west - north east
    std::vector<std::string> selPtVec = {
      "90°0'0.0S 180°0'0.0W", "90°0'0.0N 180°0'0.0E"
    };

    xml::Node *kmlNode = call_briefer::selectFunctionByType(
      type_paramStr,
      {"pin", "path"},
      {
        // parse pins
        [&]()->xml::Node* {
          xml::Node *kmlNode_baby = txt::Scanner().parsePins(
            fileIn_paramStr, fileOut_paramStr
          );

          // optional
          if (sort_toggleBool) {
            call_briefer::sortPlacemarksFunc(
              kmlNode_baby,
              selPtVec,
              "pin",
              true,
              false
            );
          }

          return kmlNode_baby;
        },
        // parse paths
        [&]()->xml::Node* {

          xml::Node *kmlNode_baby = txt::Scanner().parsePins(
            fileIn_paramStr, fileOut_paramStr
          );

          if (kmlNode_baby) {
            std::vector<xml::Node*> selectedPinNodes;

            if (sort_toggleBool) {
              selectedPinNodes = call_briefer::sortPlacemarksFunc(
                kmlNode_baby,
                selPtVec,
                "pin",
                false,
                false
              );
            }
            else {
              selectedPinNodes = call_briefer::cropPlacemarkFunc(
                kmlNode_baby,
                selPtVec,
                "pin",
                false,
                false
              );
            }

            if (selectedPinNodes.size() > 0) {
              kml::Placemark::pinsPathSegments(
                kmlNode_baby,
                selectedPinNodes,
                true
              );
            }
          }

          return kmlNode_baby;
        }
      }
    );

    call_briefer::writeFileFunc(kmlNode, fileOut_paramStr);
  }

  ////////////////////////
  // CONVERT KML TO CSV //
  ////////////////////////

  else if (SELECTED_FLAG == CONVERT_KML_CSV_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("kml-in"),
      fileOut_paramStr = menu.getParameterString("csv-out");

    xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

    if (kmlNode) {
      std::string separatorSign = "|";

      menu.setNotification(
        std::string("\nCSV-> Consider to set column/folder name as 'PATH', 'PATHS' or 'JALUR'\n") +
        std::string("      to generate path column format\n")
      );

      if (menu.setAlert(
        "CSV-> Do you want to change columns separator? (default is '|')\n",
        false
      )) {
        csv::Util::singleCharacterInputNotify(menu, false);
        std::string additionalInput = menu.setAdditionalInput(
          "CSV-> Set columns separator sign:  "
        );
        separatorSign = additionalInput;
      }

      xml::Node *mainFolderNode = kml::General::searchMainFolder(kmlNode);

      if (mainFolderNode) {
        csv::Builder csvBuilder;
        if (csvBuilder.compose(fileOut_paramStr, mainFolderNode, separatorSign)) {
          std::cout << "\n**SUCCEEDED**\n";
        }
      }
      else std::cerr << "\n**FAILED**\n";

      delete kmlNode;
    }
    else std::cerr
      << "KML-TOWN-> Error. '"
      << fileIn_paramStr
      << "' not found or empty\n"
      << "\n**FAILED**\n";
  }

  //////////////
  // KML CROP //
  //////////////
  
  else if (SELECTED_FLAG == KML_CROP_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("crop"),
      fileOut_paramStr = menu.getParameterString("out"),
      startPoint_paramStr = menu.getParameterString("start-point"),
      endPoint_paramStr = menu.getParameterString("end-point"),
      type_paramStr = menu.getParameterString("type");

    bool includeFolders_toggleBool = menu.isToggleExist("include-folders");

    kml::Cropper().printNotification(menu);

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );

    if (fileDir_check != "") {

      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);
      std::vector<std::string> selPtVec = {startPoint_paramStr, endPoint_paramStr};

      if (call_briefer::cropPlacemarkFunc(
        kmlNode,
        selPtVec,
        type_paramStr,
        true,
        includeFolders_toggleBool
      ).size() > 0) {
        // expected size is one (container node)
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  //////////////
  // KML SORT //
  //////////////

  else if (SELECTED_FLAG == KML_SORT_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("sort"),
      fileOut_paramStr = menu.getParameterString("out"),
      startPoint_paramStr = menu.getParameterString("start-point"),
      endPoint_paramStr = menu.getParameterString("end-point"),
      type_paramStr = menu.getParameterString("type");

    bool includeFolders_toggleBool = menu.isToggleExist("include-folders");

    kml::Cropper().printNotification(menu);
    kml::Sorter().printNotification(menu);

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {

      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);
      std::vector<std::string> selPtVec = {startPoint_paramStr, endPoint_paramStr};

      if (call_briefer::sortPlacemarksFunc(
        kmlNode,
        selPtVec,
        type_paramStr,
        true,
        includeFolders_toggleBool
      ).size() > 0) {
        // expected size is one (working folder node)
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  //////////////////////
  // KML PINS TO PATH //
  //////////////////////

  else if (SELECTED_FLAG == KML_PINS_PATH_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("pins-path"),
      fileOut_paramStr = menu.getParameterString("out"),
      startPoint_paramStr = menu.getParameterString("start-point"),
      endPoint_paramStr = menu.getParameterString("end-point");
    
    kml::Cropper().printNotification(menu);
    kml::Sorter().printNotification(menu);

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {

      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);
      std::vector<std::string> selPtVec = {startPoint_paramStr, endPoint_paramStr};

      std::vector<xml::Node*> sortedPinNodes = call_briefer::sortPlacemarksFunc(
        kmlNode,
        selPtVec,
        "pin",
        false,
        false
      );

      if (sortedPinNodes.size() > 0) {
        kml::Placemark::pinsPath(
          kmlNode,
          sortedPinNodes
        );
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
    }
  }

  /////////////////////
  // KML TWINS CHECK //
  /////////////////////

  else if (SELECTED_FLAG == KML_TWINS_CHECK_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("twins-check"),
      fileOut_paramStr = menu.getParameterString("out"),
      type_paramStr = menu.getParameterString("type"),
      radius_paramStr = menu.getParameterString("radius");

    bool includeFolders_toggleBool = menu.isToggleExist("include-folders"),
      onlySimilarStyle_toggleBool = menu.isToggleExist("only-similar-style"),
      pathTextPrioritizeFirst_toggleBool = menu.isToggleExist("path-text-prioritize-first");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );

    if (fileDir_check != "") {

      xml::Node
        *kmlNode = xml::Reader::fileParse(fileIn_paramStr),
        *twinsCheckedFolder;

      if (kmlNode) {
        twinsCheckedFolder = call_briefer::selectFunctionByType(
          type_paramStr,
          {"pin", "path", "all"},
          {
            [=]()->xml::Node* {
              return kml::TwinsChecker().findPins(
                kmlNode,
                radius_paramStr,
                false,
                includeFolders_toggleBool,
                onlySimilarStyle_toggleBool
              );
            },
            [=]()->xml::Node* {
              return kml::TwinsChecker().findPaths(
                kmlNode,
                radius_paramStr,
                false,
                includeFolders_toggleBool,
                onlySimilarStyle_toggleBool,
                pathTextPrioritizeFirst_toggleBool
              );
            },
            [=]()->xml::Node* {
              return kml::TwinsChecker().findAll(
                kmlNode,
                radius_paramStr,
                includeFolders_toggleBool,
                onlySimilarStyle_toggleBool,
                pathTextPrioritizeFirst_toggleBool
              );
            }
          }
        );

        if (twinsCheckedFolder) {
          call_briefer::writeFileFunc(kmlNode, fileDir_check);
        }
        else {
          delete kmlNode;
          std::cerr << "\n**FAILED**\n";
        }
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  ////////////////////////
  // KML TIDY UP STYLES //
  ////////////////////////

  else if (SELECTED_FLAG == KML_TIDY_UP_STYLES) {

    std::string
      fileIn_paramStr = menu.getParameterString("tidy-up-styles"),
      fileOut_paramStr = menu.getParameterString("out");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

      if (kmlNode) {
        if (kml::TwinsChecker().tidyUpStyles(kmlNode)) {
          call_briefer::writeFileFunc(kmlNode, fileDir_check);
        }
        else {
          delete kmlNode;
          std::cerr << "\n**FAILED**\n";
        }
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  ///////////////////////
  // KML MEASURE PATHS //
  ///////////////////////

  else if (SELECTED_FLAG == KML_MEASURE_PATHS_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("measure-paths"),
      fileOut_paramStr = menu.getParameterString("out");
    
    bool info_toggleBool = menu.isToggleExist("info");

    std::string fileDir_check = "";

    if (!info_toggleBool) {
      fileDir_check = call_briefer::checkOverwrite(
        menu,
        fileIn_paramStr,
        fileOut_paramStr
      );
    }
    
    if (fileDir_check != "" || info_toggleBool) {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

      if (kmlNode) {
        if (kml::Placemark::setPathDistance(
          kmlNode,
          info_toggleBool
        )) {
          if (!info_toggleBool) {
            call_briefer::writeFileFunc(kmlNode, fileDir_check);
          }
          else {
            delete kmlNode;
            std::cout << "\n**SUCCEEDED**\n";
          }
        }
        else {
          delete kmlNode;
          std::cerr << "\n**FAILED**\n";
        }
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  ////////////////////
  // KML CLASSIFIER //
  ////////////////////

  else if (SELECTED_FLAG == KML_CLASSIFY_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("classify"),
      fileOut_paramStr = menu.getParameterString("out");

    bool cleanFolders_toggleBool = menu.isToggleExist("clean-folders"),
      includeFolders_toggleBool = menu.isToggleExist("include-folders");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

      bool isProceed = false;

      if ((cleanFolders_toggleBool &&
        menu.setAlert(
          std::string("KML-> Classifying warning. This will remove empty or previous folders.\n") +
          std::string("      Keep proceeding?\n")
        )) ||
        !cleanFolders_toggleBool
      ) {
        isProceed = true;
      }
      
      if (isProceed && kmlNode) {
        kml::Classifier().groupStyles(kmlNode, cleanFolders_toggleBool, includeFolders_toggleBool);
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else if (
        (!cleanFolders_toggleBool && !kmlNode) ||
        (isProceed && !kmlNode)
      ) {
        std::cerr << "\n**FAILED**\n";
      }
    }
  }

  ////////////////////////
  // KML FOLDER BY DATE //
  ////////////////////////

  else if (SELECTED_FLAG == KML_FOLDER_BY_DATE_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("folder-by-date"),
      fileOut_paramStr = menu.getParameterString("out");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

      if (kml::DateFolder().packNumeral(kmlNode)) {
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  ////////////////////////
  // KML DATE BY FOLDER //
  ////////////////////////

  else if (SELECTED_FLAG == KML_DATE_BY_FOLDER_FLAG) {
    std::string
      fileIn_paramStr = menu.getParameterString("date-by-folder"),
      fileOut_paramStr = menu.getParameterString("out");

    bool overrideDated_toggleBool = menu.isToggleExist("override-dated");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);
      
      if (kml::DateFolder().spreadNumeral(kmlNode, overrideDated_toggleBool)) {
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  ///////////////////////
  // KML FILTER STRING //
  ///////////////////////

  else if (SELECTED_FLAG == KML_FILTER_STRING_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("filter-string"),
      fileOut_paramStr = menu.getParameterString("out"),
      text_paramStr = menu.getParameterString("text");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);
      
      if (kml::Classifier().filterString(kmlNode, text_paramStr)) {
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  //////////////////////
  // KML REMOVE PATHS //
  //////////////////////

  else if (SELECTED_FLAG == KML_REMOVE_PATHS_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("remove-paths"),
      fileOut_paramStr = menu.getParameterString("out"),
      distance_paramStr;

    // expected integer are '1' or '2'
    int isOver_oppositeCode = menu.checkOppositeParameterString(
      "under-distance", "over-distance", &distance_paramStr
    );

    // directly end
    if (isOver_oppositeCode == 0) {
      std::cerr << "\n**FAILED**\n";
      return 0;
    }

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      xml::Node *kmlNode = xml::Reader::fileParse(fileIn_paramStr);

      bool isPathRemoved = kml::Placemark::removePathsByDistance(
        kmlNode,
        mini_tool::filterStringDecimal(distance_paramStr),
        isOver_oppositeCode - 1
      );

      if (isPathRemoved) {
        call_briefer::writeFileFunc(kmlNode, fileDir_check);
      }
      else std::cerr << "\n**FAILED**\n";
    }
  }

  //////////////////////////
  // CSV CHANGE SEPARATOR //
  //////////////////////////

  else if (SELECTED_FLAG == CSV_CHANGE_SEPARATOR_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("change-separator"),
      fileOut_paramStr = menu.getParameterString("out"),
      oldSign_paramStr = menu.getParameterString("old-sign"),
      newSign_paramStr = menu.getParameterString("new-sign");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      csv::Util::singleCharacterInputNotify(menu, true);

      csv::Util::changeCSVSeparator(
        fileIn_paramStr,
        fileDir_check,
        oldSign_paramStr,
        newSign_paramStr
      );
    }
  }

  ///////////////////////
  // TXT COMPLETE DATE //
  ///////////////////////

  else if (SELECTED_FLAG == TXT_COMPLETE_DATE_FLAG) {

    std::string
      fileIn_paramStr = menu.getParameterString("complete-date"),
      fileOut_paramStr = menu.getParameterString("out");

    std::string fileDir_check = call_briefer::checkOverwrite(
      menu,
      fileIn_paramStr,
      fileOut_paramStr
    );
    
    if (fileDir_check != "") {
      txt::CoordinateDating().complete(fileIn_paramStr, fileDir_check);
    }
  }

  std::cout << "\nProcessing time: " << std::time(0) - startTime << std::endl;
  
  return 0;
}

