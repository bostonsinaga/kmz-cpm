#ifndef __KML_TWINS_CHECKER_H__
#define __KML_TWINS_CHECKER_H__

#include "placemark.h"

namespace kml {
  
  class TwinsChecker {
  private:
    class Prioritize {
      public:
      static void testTextObtain(
        xml::Node *placemark,
        std::vector<std::string> &testStr_vec_in
      );
      
      static bool isChanged(
        std::vector<std::string> &testStr_A_vec_in,   // 'A' should be lose
        std::vector<std::string> &testStr_B_vec_in,   // 'B' should be win
        std::vector<std::string> &otherIgnoredStrings_in
      );

      static void printAboutMessage();

      private:
      static bool isNoname(
        std::string &testStr,
        std::vector<std::string> &otherIgnoredStrings_in
      );
    };

  public:
    TwinsChecker() {}

    xml::Node *findPins(
      xml::Node *kmlNode,
      std::string meterRadiusRateString,
      bool isParentFolderNamedAType,
      bool isIncludeFolders,
      bool isOnlySimilarStyle,
      bool isPrioritizePrintAboutMessage = true
    );
    
    xml::Node *findPaths(
      xml::Node *kmlNode,
      std::string meterRadiusRateString,
      bool isParentFolderNamedAType,
      bool isIncludeFolders,
      bool isOnlySimilarStyle,
      bool isPathTextPrioritizeFirst,
      bool isPrioritizePrintAboutMessage = true
    );
    
    xml::Node *findAll(
      xml::Node *kmlNode,
      std::string meterRadiusRateString,
      bool isIncludeFolders,
      bool isOnlySimilarStyle,
      bool isPathTextPrioritizeFirst
    );
    
    xml::Node *insertFoundPlacemarks(
      xml::Node *kmlNode,
      std::vector<int> *matchedIndexes, // int vector array (has 2 members)
      std::vector<xml::Node*> &placemarkNodes,
      std::string placemarksType,
      bool isParentFolderNamedAType,
      bool isIncludeFolders
    );

    /**
     * Clean copies of icon urls or color codes that contained inside separate 'StyleMap'
     * to reduce size of 'kml' file and also rename the 'styleUrl' to templated names
     * (note that this operation will rewrite previous tidied styles).
     * 
     * This won't keep the previous pins or paths size but will use default size
     * 
     * Command: 'tidy-up-styles'
     */
    bool tidyUpStyles(xml::Node *kmlNode);
    
    private:
    // limited from 0 to 100 meters
    double getLimitedMeterRadius(std::string meterRadiusRateString);

    // used when 'find' function's 'isOnlySimilarStyle' is true
    bool checkSimilarStyle(
      xml::Node *placemark_A,
      xml::Node *placemark_B,
      bool &isFirstPlcItr
    );
  };
}

#endif // __KML_TWINS_CHECKER_H__