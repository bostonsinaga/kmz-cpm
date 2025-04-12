#ifndef __MENU_CPP__
#define __MENU_CPP__

#include "menu.h"

Menu::Menu(std::vector<std::string> *inputStrings_ptrIn) {
  inputStrings_ptr = inputStrings_ptrIn;

  /////////////////////
  // INPUT MANDATORY //
  /////////////////////

  // >>> TOGGLES //

  toggleStringsVec[0].at(MAIN_MENU_FLAG) = {"menu"};
  toggleStringsVec[0].at(HELP_MENU_FLAG) = {"help"};
  toggleStringsVec[0].at(VERSION_NUMBER_FLAG) = {"version"};
  toggleStringsVec[0].at(CONVERT_TXT_KML_FLAG) = {"convert"};
  toggleStringsVec[0].at(CONVERT_KML_CSV_FLAG) = {"convert"};
  toggleStringsVec[0].at(KML_CROP_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_SORT_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_PINS_PATH_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_TWINS_CHECK_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_TIDY_UP_STYLES) = {"kml"};
  toggleStringsVec[0].at(KML_MEASURE_PATHS_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_CLASSIFY_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_FOLDER_BY_DATE_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_DATE_BY_FOLDER_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_FILTER_STRING_FLAG) = {"kml"};
  toggleStringsVec[0].at(KML_REMOVE_PATHS_FLAG) = {"kml"};
  toggleStringsVec[0].at(CSV_CHANGE_SEPARATOR_FLAG) = {"csv"};
  toggleStringsVec[0].at(TXT_COMPLETE_DATE_FLAG) = {"txt"};

  // >>> PARAMETERS //

  parameterStringsVec[0].at(CONVERT_TXT_KML_FLAG) = {"txt-in", "kml-out", "type"};
  parameterStringsVec[0].at(CONVERT_KML_CSV_FLAG) = {"kml-in", "csv-out"};
  parameterStringsVec[0].at(KML_CROP_FLAG) = {"crop", "start-point", "end-point", "type"};
  parameterStringsVec[0].at(KML_SORT_FLAG) = {"sort", "start-point", "end-point", "type"};
  parameterStringsVec[0].at(KML_PINS_PATH_FLAG) = {"pins-path", "start-point", "end-point"};
  parameterStringsVec[0].at(KML_TWINS_CHECK_FLAG) = {"twins-check", "type", "radius"};
  parameterStringsVec[0].at(KML_TIDY_UP_STYLES) = {"tidy-up-styles"};
  parameterStringsVec[0].at(KML_MEASURE_PATHS_FLAG) = {"measure-paths"};
  parameterStringsVec[0].at(KML_CLASSIFY_FLAG) = {"classify"};
  parameterStringsVec[0].at(KML_FOLDER_BY_DATE_FLAG) = {"folder-by-date"};
  parameterStringsVec[0].at(KML_DATE_BY_FOLDER_FLAG) = {"date-by-folder"};
  parameterStringsVec[0].at(KML_FILTER_STRING_FLAG) = {"filter-string", "text"};
  parameterStringsVec[0].at(KML_REMOVE_PATHS_FLAG) = {"remove-paths"};
  parameterStringsVec[0].at(CSV_CHANGE_SEPARATOR_FLAG) = {"change-separator", "old-sign", "new-sign"};
  parameterStringsVec[0].at(TXT_COMPLETE_DATE_FLAG) = {"complete-date"};

  ////////////////////
  // INPUT OPTIONAL //
  ////////////////////

  // >>> TOGGLES //

  toggleStringsVec[1].at(CONVERT_TXT_KML_FLAG) = {"sort"};
  toggleStringsVec[1].at(KML_CROP_FLAG) = {"include-folders"};
  toggleStringsVec[1].at(KML_SORT_FLAG) = {"include-folders"};
  toggleStringsVec[1].at(KML_MEASURE_PATHS_FLAG) = {"info"};
  toggleStringsVec[1].at(KML_TWINS_CHECK_FLAG) = {"include-folders", "only-similar-style", "path-text-prioritize-first"};
  toggleStringsVec[1].at(KML_CLASSIFY_FLAG) = {"clean-folders", "include-folders"};
  toggleStringsVec[1].at(KML_DATE_BY_FOLDER_FLAG) = {"override-dated"};

  // >>> PARAMETERS //

  parameterStringsVec[1].at(KML_CROP_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_SORT_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_PINS_PATH_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_TWINS_CHECK_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_TIDY_UP_STYLES) = {"out"};
  parameterStringsVec[1].at(KML_MEASURE_PATHS_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_CLASSIFY_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_FOLDER_BY_DATE_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_DATE_BY_FOLDER_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_FILTER_STRING_FLAG) = {"out"};
  parameterStringsVec[1].at(KML_REMOVE_PATHS_FLAG) = {"out", "over-distance", "under-distance"};
  parameterStringsVec[1].at(CSV_CHANGE_SEPARATOR_FLAG) = {"out"};
  parameterStringsVec[1].at(TXT_COMPLETE_DATE_FLAG) = {"out"};
}

/**
 * Returns command flags (invalid returns -1).
 * This only check for mandatory input to get intended flag.
 */
int Menu::select(std::vector<std::string> *inputStrings_ptrIn) {

  if (inputStrings_ptrIn) {
    inputStrings_ptr = inputStrings_ptrIn;
  }
  
  bool isFailForced = false,
    isParameterListeningString = false,
    isUsingSelectionArea = false,
    isParameterNotFollowedByString = false;

  int mainCtr = 0,
    INPUTORDERFLAG = STRING_INPUTORDERFLAG;

  // used in 'amount check'
  int mandatoriesCount = 0;

  // this function main loop
  for (auto &inputStr : *inputStrings_ptr) {

    // determine input string type
    if (inputStr.length() > 2 &&
      inputStr.at(0) == '-' &&
      inputStr.at(1) == '-'
    ) {
      INPUTORDERFLAG = TOGGLE_INPUTORDERFLAG;
    }
    else if (
      inputStr.length() > 1 &&
      inputStr.at(0) == '-'
    ) {
      INPUTORDERFLAG = PARAMETER_INPUTORDERFLAG;
    }
    else INPUTORDERFLAG = STRING_INPUTORDERFLAG;

    int strCheckingStartDex = 0;

    // [mandatory, optional]
    std::vector<std::vector<std::string>> *ptrStringsVec[2];

    // select the appropriate module/tool
    switch (INPUTORDERFLAG) {
      case TOGGLE_INPUTORDERFLAG: {

        if (isParameterListeningString) {
          isFailForced = true;
          break;
        }

        strCheckingStartDex = 2;
        ptrStringsVec[0] = &toggleStringsVec[0];
        ptrStringsVec[1] = &toggleStringsVec[1];
      break;}
      case PARAMETER_INPUTORDERFLAG: {

        if (isParameterListeningString) {
          isFailForced = true;
          break;
        }

        // error when no string following (parameter at back)
        if (mainCtr == inputStrings_ptr->size() - 1) {
          isParameterNotFollowedByString = true;
        }
        
        strCheckingStartDex = 1;
        ptrStringsVec[0] = &parameterStringsVec[0];
        ptrStringsVec[1] = &parameterStringsVec[1];
        isParameterListeningString = true;
      break;}
      case STRING_INPUTORDERFLAG: {
        
        if (!isParameterListeningString) {
          isFailForced = true;
          break;
        }

        expectingFlagQueueVec.push_back(expectingFlagQueueVec.back());
        isParameterListeningString = false;
      break;}
    }

    if (isFailForced) break;

    // 'false' means parameter string is currently as input
    if (strCheckingStartDex) {

      expectingFlagQueueVec.push_back({});
      std::string inputStr_subStr;

      int priorityDex = 0,
        limit_i = ptrStringsVec[priorityDex]->size();

      bool isUseOptional = false,
        isMandatoryChecked = false;

      for (int i = 0; i < limit_i; i++) {
        for (int j = 0; j < ptrStringsVec[priorityDex]->at(i).size(); j++) {

          std::string str = ptrStringsVec[priorityDex]->at(i).at(j);
          inputStr_subStr = inputStr.substr(strCheckingStartDex);

          if (inputStr_subStr == str) {
            expectingFlagQueueVec.back().push_back(i);

            // count mandatory input
            if (!isMandatoryChecked && priorityDex == 0) {
              isMandatoryChecked = true;
              mandatoriesCount++;

              // include parameter's string
              if (strCheckingStartDex == 1 &&
                mainCtr < inputStrings_ptr->size() - 1
              ) { mandatoriesCount++; }
            }
          }

          // whether using DMS error in 'getErrorString' function
          if (!isUsingSelectionArea &&
            (inputStr_subStr == "start-point" ||
            inputStr_subStr == "end-point")
          ) {
            isUsingSelectionArea = true;
          }
        }

        // 'expectingFlagQueueVec' empty at end of 'i' loop
        if (i == limit_i - 1 &&
          expectingFlagQueueVec.back().size() == 0
        ) {
          // check for optional list
          if (!isUseOptional) {
            isUseOptional = true;
            priorityDex = 1;
            i = 0;
          }
          // not detected
          else isFailForced = true;
        }
      }

      // not detected
      if (isFailForced) break;
    }

    mainCtr++;
  }

  if (isFailForced || isParameterNotFollowedByString) {
    if (isParameterNotFollowedByString) {
      std::cerr << "KML-TOWN-> Incomplete parameter\n";
    }

    std::cerr << getErrorString(isUsingSelectionArea);
    return -1;
  }

  /** Check expecting flags consistency (determine intended flag) */

  bool isConsistent = true;
  int intendedFlag = -1; // the below product (command flag)

  if (expectingFlagQueueVec.size() > 0) {
    std::vector<int> sustainFlags = expectingFlagQueueVec.at(0);

    for (int i = 1; i < expectingFlagQueueVec.size(); i++) {
      /**
       * Get 2D intersected current 'expectingFlagQueueVec' indexes
       * (return empty if all not found).
       */
      std::vector<std::vector<int>>
        intersectIndexesVec = (     // pass 1D vector only
          mini_tool::vectorIntersectsVector<int>(sustainFlags, expectingFlagQueueVec.at(i))
        );

      if (intersectIndexesVec.size() > 0) {

        // set to 'intersectIndexesVec' 1D vector
        std::vector<int> intersectIndexes = (
          mini_tool::collapse2DTo1DVector<int>(intersectIndexesVec)
        );

        sustainFlags = {};

        for (auto &index : intersectIndexes) {
          sustainFlags.push_back(expectingFlagQueueVec.at(i).at(index));
        }
      }
      else { // not consistent
        isConsistent = false;
        break;
      }
    }

    if (isConsistent) {
      intendedFlag = sustainFlags.front();
    }
  }
  else isConsistent = false;

  /**
   * AMOUNT CHECK
   * The 'inputStrings_ptr' count must more than or equals
   * to mandatory toggles and parameters count.
   */
  bool isInputsCountEnough = true;
  int expectedInputsCount = intendedFlag >= 0 ? (
    toggleStringsVec[0].at(intendedFlag).size() +
    parameterStringsVec[0].at(intendedFlag).size() * 2
  ) : 1;

  if (mandatoriesCount < expectedInputsCount) {
    isInputsCountEnough = false;
  }

  if (isConsistent && isInputsCountEnough) {
    return intendedFlag;
  }
  else {
    if (!isInputsCountEnough) {
      std::cerr << "KML-TOWN-> Too few mandatory inputs\n";
    }

    std::cerr << getErrorString(isUsingSelectionArea);
    return -1;
  }
}

// returns input string vector (follows parameter name)
std::vector<std::string> Menu::getParameterStrings(
  const std::string &testStr,
  bool isOnlyGetOne,
  std::vector<std::string> *inputStrings_ptrIn
) {
  if (inputStrings_ptrIn) {
    inputStrings_ptr = inputStrings_ptrIn;
  }

  int ctr = 0;
  std::vector<std::string> retStrVec;

  // this function main loop
  for (auto &inputStr : *inputStrings_ptr) {

    if (inputStr.length() > 1 &&
      inputStr.at(0) == '-'
    ) {
      if (inputStr.substr(1) == testStr) {
        if (inputStrings_ptr->size() > ctr + 1) {
          // expected input string is '+1' index
          retStrVec.push_back(inputStrings_ptr->at(ctr + 1));
          if (isOnlyGetOne) break;
        }
        // unexpected input string (may followed by toggle or parameter)
        else return std::vector<std::string>{};
      }
    }

    ctr++;
  }

  return retStrVec;
}

// returns input single string (using 'getParameterStrings' method)
std::string Menu::getParameterString(
  const std::string &testStr,
  bool isUsingVectorBackIndex, // 'getParameterStrings' vector product back or front
  std::vector<std::string> *inputStrings_ptrIn
) {
  std::string retStr = "";
  std::vector<std::string> strVec;

  if (inputStrings_ptrIn) {
    strVec = getParameterStrings(testStr, true, inputStrings_ptrIn);
  }
  else strVec = getParameterStrings(testStr, true);

  if (strVec.size() > 0) {
    if (isUsingVectorBackIndex) retStr = strVec.back();
    else retStr = strVec.front();
  }

  return retStr;
}

/**
 * Returns '1' or '2' if one of strings exist / not both (consecutive with 'param_' order).
 * Returns '0' if none exist or both exist.
 * 
 * Note:
 * This will make optional parameter become mandatory
 * (one of parameter in arguments will be selected based on existence in input strings).
*/
int Menu::checkOppositeParameterString(
  const std::string &param_a,
  const std::string &param_b,
  std::string *strValPtr,
  bool isUsingVectorBackIndex, // 'getParameterStrings' function vector back or front
  std::vector<std::string> *inputStrings_ptrIn
) {
  int retInt = 0;

  std::string
    paramStr_a = getParameterString(param_a),
    paramStr_b = getParameterString(param_b);

  if (paramStr_a != "" && paramStr_b == "") {
    *strValPtr = paramStr_a;
    retInt = 1;
  }
  else if (paramStr_a == "" && paramStr_b != "") {
    *strValPtr = paramStr_b;
    retInt = 2;
  }
  // none exist or both exist
  else if (paramStr_a != "" && paramStr_b != "") {
    std::cerr << "KML-TOWN-> Error. Please choose only one of '"
      << param_a << "' or '" << param_b << "'\n";
  }
  else {
    std::cerr << "KML-TOWN-> Error. Please specify '"
      << param_a << "' or '" << param_b << "'\n";
  }

  return retInt;
}

/** Useful for checking optional toggle */
bool Menu::isToggleExist(
  const std::string &testStr,
  std::vector<std::string> *inputStrings_ptrIn
) {
  if (inputStrings_ptrIn) {
    inputStrings_ptr = inputStrings_ptrIn;
  }

  // this function main loop
  for (auto &inputStr : *inputStrings_ptr) {

    if (inputStr.length() > 2 &&
      inputStr.at(0) == '-' &&
      inputStr.at(1) == '-'
    ) {
      if (inputStr.substr(2) == testStr) return true;
    }
  }

  return false;
}

void Menu::setNotification(std::string message, bool isNeedEnterKey) {
  std::cout << message;
  if (isNeedEnterKey) {
    std::cout << "\n**PRESS ENTER TO CONTINUE**\n";
    std::cin.get();
  }
  else std::cout << std::endl; // line separator
}

bool Menu::setAlert(std::string message, bool isUseFailedMessage) {
  std::cout << message << "\n**PRESS Y[yes] / N[no]**\n";

  std::string answer;
  std::cin >> answer;
  std::cout << std::endl;

  if (answer == "Y" || answer == "y") {
    return true;
  }
  else {
    if (isUseFailedMessage) {
      std::cerr << "**CANCELED**\n";
    }
    return false;
  }
}

// input is first CLI strings
std::string Menu::setAdditionalInput(std::string message) {
  std::cout << message;
  std::string input;
  std::cin >> input;
  std::cout << std::endl;
  return input;
}

std::string Menu::getErrorString(bool isUseDMSErrorMessage) {
  std::stringstream retStr_stream;

  if (isUseDMSErrorMessage) {
    retStr_stream
      << getYellowWarning() << " You may have to remove double quote signs (\") of degree (DMS) coordinate"
      << "to prevent unexpected or overload command parameters!\n";
  }

  retStr_stream
    << getRedError() << " Type '--help' for available commands.\n"
    << "\n**FAILED**\n";

  return retStr_stream.str();
}

std::string Menu::getGeotonFullPath() {

  const char *env = std::getenv("PATH");
  std::string dirname = "geoton/build";

#ifdef _WIN32
  dirname = "geoton\\build";
#endif

  const int envct = std::strlen(env),
    dirnamect = dirname.size();

  if (envct < dirnamect) return "";

  int i, match = 0;
  std::string path;

  // firstly search only the first 'dirname' character
  for (i = 0; i < envct - envct % dirnamect; i++) {

    if (env[i] == dirname[0]) {
      match++;

      // then match to all remaining 'dirname' characters
      for (int j = 1; j < dirnamect; j++) {
        if (env[i + j] == dirname[j]) {
          match++;
        }
        else break;
      }

      if (match == dirnamect) break;
      match = 0;
    }
  }

  // left
  for (int j = i; j >= 0; j--) {

    if (env[j] != ';') path += env[j];
    else break;
  }

  std::reverse(path.begin(), path.end());

  // right
  for (int j = i + 1; j < envct; j++) {

    if (env[j] != ';') path += env[j];
    else break;
  }

  return path;
}

/** Main Menu (Help) */
void Menu::displayAvailableCommands() {

  std::ifstream readFile;
  if (!testReadmeFile(&readFile, getGeotonFullPath())) return;

  std::string text, stringBuffer;
  bool isDetected = false;

  while (std::getline(readFile, stringBuffer)) {

    // clean '<br>' (breakline mark)
    size_t foundBr = stringBuffer.find("<br>");
    if (foundBr != std::string::npos) {
      stringBuffer = stringBuffer.substr(0, foundBr) + stringBuffer.substr(foundBr + 4);
    }

    // end
    if (isDetected && mini_tool::isStringContains(stringBuffer, "# SCREENSHOTS")) {
      break;
    }

    // inside
    if (isDetected) {
      text += stringBuffer + "\n";
    }

    // start
    if (!isDetected && mini_tool::isStringContains(stringBuffer, "# COMMANDS")) {
      isDetected = true;
    }    
  }

  // clean single new line at edge //

  int newLineCtr = 0;
  std::string strBuff = text;

  // front new line clean
  for (int i = 0; i < text.length(); i++) {
    if (text.at(i) == '\n') {
      if (i < text.length() - 1) {
        strBuff = strBuff.substr(i + 1 - newLineCtr);
        newLineCtr++;
      }
    }
    else {
      text = strBuff;
      break;
    }
  }
  
  // back new line clean
  for (int i = text.length() - 1; i >= 0; i--) {
    if (text.at(i) == '\n') {
      strBuff = strBuff.substr(0, i);
    }
    else {
      text = strBuff;
      break;
    }
  }

  readFile.close();

  // display main menu
  std::cout
    << "******************\n"
    << "**    GEOTON    **\n"
    << "** command list **\n"
    << "******************\n"
    << "\n"
    << "By Boston Sinaga\n"
    << "https://github.com/bostonsinaga\n"
    << "\n" << text << "\n";
}

/** From 'README.md' */
void Menu::displayVersionNumber() {

  std::ifstream readFile;
  if (!testReadmeFile(&readFile, getGeotonFullPath())) return;

  std::string text;

  while (std::getline(readFile, text)) {
    if (mini_tool::isStringContains(text, "# KML-TOWN v")) break;
  }

  readFile.close();

  // remove '# ' in 'text'
  for (int i = 0; i < text.length(); i++) {
    if ((text.at(i) == '#' ||
      text.at(i) == ' ') &&
      i < text.length() - 1
    ) {
      text = text.substr(i + 1);
      i--;
    }
    else break;
  }

  // display version number
  std::cout << text << std::endl;
}

bool Menu::testReadmeFile(
  std::ifstream *readFile_ptr,
  std::string testDirStr
) {
  *readFile_ptr = std::ifstream(testDirStr + "\\README.md");

  // same directory
  if (readFile_ptr->is_open()) {
    return true;
  }
  // directory 1 level above
  else {
    *readFile_ptr = std::ifstream(testDirStr + "\\..\\README.md");
    if (readFile_ptr->is_open()) return true;
  }

  // file not found
  std::cout
    << getRedError() << " Cannot display command list because 'README.md' file is not found.\n"
    << "Please make sure that the file is inside '..\\KML-TOWN\\bin\\'\n"
    << "or '..\\KML-TOWN\\platform\\bin\\' and 'Environment Variables -> PATH'\n"
    << "follows the given format.\n";

  return false;
}

#endif // __MENU_CPP__