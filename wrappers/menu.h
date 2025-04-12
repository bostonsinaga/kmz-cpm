#ifndef __MENU_H__
#define __MENU_H__

#define TOGGLE_INPUTORDERFLAG 0     // single
#define PARAMETER_INPUTORDERFLAG 1  // followed by string
#define STRING_INPUTORDERFLAG 2     // follows parameter

#define TOTAL_COMMANDS_COUNT 18

// COMMAND FLAGS
#define MAIN_MENU_FLAG 0
#define HELP_MENU_FLAG 1
#define VERSION_NUMBER_FLAG 2
#define CONVERT_TXT_KML_FLAG 3
#define CONVERT_KML_CSV_FLAG 4
#define KML_CROP_FLAG 5
#define KML_SORT_FLAG 6
#define KML_PINS_PATH_FLAG 7
#define KML_TWINS_CHECK_FLAG 8
#define KML_TIDY_UP_STYLES 9
#define KML_MEASURE_PATHS_FLAG 10
#define KML_CLASSIFY_FLAG 11
#define KML_FOLDER_BY_DATE_FLAG 12
#define KML_DATE_BY_FOLDER_FLAG 13
#define KML_FILTER_STRING_FLAG 14
#define KML_REMOVE_PATHS_FLAG 15
#define CSV_CHANGE_SEPARATOR_FLAG 16
#define TXT_COMPLETE_DATE_FLAG 17

#include "mini-tool.h"

class Menu {
public:
  /**
   * Call methods with string vector pointer
   * to refresh 'inputStrings_ptr'.
   */
  Menu(std::vector<std::string> *inputStrings_ptrIn);

  /**
   * Returns command flags (invalid returns -1).
   * This only check for mandatory input to get intended flag.
   */
  int select(std::vector<std::string> *inputStrings_ptrIn = nullptr);

  /** Returns input string vector (follows parameter name) */
  std::vector<std::string> getParameterStrings(
    const std::string &testStr,
    bool isOnlyGetOne = false,
    std::vector<std::string> *inputStrings_ptrIn = nullptr
  );

  /** Returns input single string (using 'getParameterStrings' method) */
  std::string getParameterString(
    const std::string &testStr,
    bool isUsingVectorBackIndex = false, // 'getParameterStrings' vector product back or front
    std::vector<std::string> *inputStrings_ptrIn = nullptr
  );

  /**
   * Returns '1' or '2' if one of strings exist / not both (consecutive with 'param_' order).
   * Returns '0' if none exist or both exist.
   * 
   * Note:
   *  This will make optional parameter become mandatory
   *  (one of parameter in arguments will be selected based on existence in input strings).
   */
  int checkOppositeParameterString(
    const std::string &testStr_a,
    const std::string &testStr_b,
    std::string *strValPtr,
    bool isUsingVectorBackIndex = false, // 'getParameterStrings' function vector back or front
    std::vector<std::string> *inputStrings_ptrIn = nullptr
  );

  /** Useful for checking optional toggle */
  bool isToggleExist(
    const std::string &testStr,
    std::vector<std::string> *inputStrings_ptrIn = nullptr
  );

  /** Pop up dialog */
  void setNotification(std::string message, bool isNeedEnterKey = true);
  bool setAlert(std::string message, bool isUseFailedMessage = true);
  std::string setAdditionalInput(std::string message); // input is first CLI strings

  /** Main menu */
  void displayAvailableCommands();
  void displayVersionNumber();

  /** Colored Words */

  std::string getYellowWarning() { return "\033[33mWARNING.\033[0m"; }
  std::string getRedError() { return "\033[31mERROR.\033[0m"; }

  void printYellowWarning() { std::cout << "\033[33mWARNING.\033[0m"; }
  void printRedError() { std::cout << "\033[31mERROR.\033[0m"; }

private:
	std::string getErrorString(bool isUseDMSErrorMessage);
  std::string getGeotonFullPath();
  bool testReadmeFile(std::ifstream *readFile_ptr, std::string testDirStr);

  std::vector<std::string> *inputStrings_ptr = nullptr;
  std::vector<std::vector<int>> expectingFlagQueueVec;

  /**
   * Note for 'toggleStringsVec' and 'parameterStringsVec':
   *
   * Array:
   *  -1st array is mandatory set
   *  -2nd array is optional set
   *
   * Vector:
   *  -1D (outer) is command set
   *  -2D (inner) is toggle or parameter set
   */

  // single string (eg. '--str')
  std::vector<std::vector<std::string>> toggleStringsVec[2] = {
    std::vector<std::vector<std::string>>(
      TOTAL_COMMANDS_COUNT, std::vector<std::string>{}
    ),
    std::vector<std::vector<std::string>>(
      TOTAL_COMMANDS_COUNT, std::vector<std::string>{}
    )
  };

  // need string input (eg. '-str')
  std::vector<std::vector<std::string>> parameterStringsVec[2] = {
    std::vector<std::vector<std::string>>(
      TOTAL_COMMANDS_COUNT, std::vector<std::string>{}
    ),
    std::vector<std::vector<std::string>>(
      TOTAL_COMMANDS_COUNT, std::vector<std::string>{}
    )
  };
};

#endif // __MENU_H__