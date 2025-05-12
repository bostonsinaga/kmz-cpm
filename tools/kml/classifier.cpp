#ifndef __CLASSIFIER_CPP__
#define __CLASSIFIER_CPP__

#include "classifier.h"

namespace kml {
  
  void Classifier::groupStyles(
    xml::Node *kmlNode,
    bool isCleanFolders,
    bool isIncludeFolders
  ) {
    if (kmlNode) {
      std::cout << "KML-> Classifying attention. Action may have to be repeated\n"
        << "      if there are duplicate folders in '--include-folders' mode\n";

      StyleStrings kmlStyleStrings(kmlNode);
      enum {pins_e, paths_e, others_e};

      std::vector<xml::Node*> placemarks[3],
        classifiedFolders = {
          Builder::createFolder("PINS"),
          Builder::createFolder("PATHS"),
          Builder::createFolder("OTHERS")
        };

      for (xml::Node* plc : kmlNode->getDescendantsByName("Placemark", true)) {

        if (plc->getFirstChildByName("Point")) {
          placemarks[pins_e].push_back(plc);
        }
        else if (plc->getFirstChildByName("LineString")) {
          placemarks[paths_e].push_back(plc);
        }
        else placemarks[others_e].push_back(plc);
      }

      for (int i = 0; i < 3; i++) {

        for (xml::Node* plc : placemarks[i]) {
          kmlStyleStrings.classify(plc, i == pins_e);
        }

        for (auto &pair : kmlStyleStrings.classification) {
          if (!pair.second.empty()) {

            xml::Node *styleFolder = Builder::createFolder(pair.first, "unknown");
            styleFolder->addChildren(pair.second);
            classifiedFolders[i]->addChild(styleFolder);
          }
        }

        kmlStyleStrings.classification.clear();
      }

      /** Add the folders to a working folder */

      xml::Node
        *mainFolderNode = General::searchMainFolder(kmlNode),
        *workingFolderNode = Builder::createFolder(definitions::CLASSIFY_COMMAND_WORKING_FOLDER);

      if (isCleanFolders) {
        General::cleanFolders(
          mainFolderNode,
          workingFolderNode,
          classifiedFolders
        );
      }
      else {
        General::insertEditedPlacemarksIntoFolder(
          mainFolderNode,
          workingFolderNode,
          classifiedFolders,
          {"Classifying placemarks", "Classify placemarks"},
          ""
        );
      }
    }
  }

  bool Classifier::filterString(xml::Node *kmlNode, std::string searchStr) {
    if (kmlNode) {
      std::vector<xml::Node*> placemarks;
      
      for (auto &placemarkNode : kmlNode->getDescendantsByName("Placemark", true)) {
        
        xml::Node *plmk_stringNode;
        bool isAble = false;
        
        plmk_stringNode = placemarkNode->getFirstDescendantByName("name");
        if (plmk_stringNode) {
          if (plmk_stringNode->getInnerText().find(searchStr) != std::string::npos) {
            placemarkNode->removeFromParent();
            placemarks.push_back(placemarkNode);
            isAble = true;
          }
        }
        
        if (!isAble) {
          plmk_stringNode = placemarkNode->getFirstDescendantByName("description");
          if (plmk_stringNode) {
            if (plmk_stringNode->getInnerText().find(searchStr) != std::string::npos) {
              placemarkNode->removeFromParent();
              placemarks.push_back(placemarkNode);
            }
          }
        }
      }
      
      std::string frontWord = "";
      
      for (auto &searchCh : searchStr) {
        if (searchCh == ' ' && frontWord != "") {
          std::cerr << "KML-> Filter string warning. The search string has more than one word.\n"
          << "      Folder name will use '" << frontWord << "' from the first word\n";
          break;
        }
        else if (searchCh != ' ') {
          frontWord += searchCh;
        }
      }
      
      if (placemarks.size() > 0) {
        std::string folderName = (
          definitions::FILTER_STRING_COMMAND_WORKING_FOLDER
          + std::string("  ") + frontWord
        );
        
        xml::Node *mainFolderNode = General::searchMainFolder(kmlNode);
        std::cout << "KML-> Found: " << placemarks.size() << "\n"
        << "      Placed to folder named '" << folderName << "'\n"
        << "      Filter string completed!\n";
        
        General::insertEditedPlacemarksIntoFolder(
          mainFolderNode,
          Builder::createFolder(folderName),
          placemarks,
          {"", ""},
          ""
        );
        return true;
      }
      else {
        std::cerr << "KML-> Filter string error. "
        << "No placemark's name or description contains '" << frontWord << "'\n";
        return false;
      }
    }
    
    return false;
  }
}

#endif // __CLASSIFIER_CPP__