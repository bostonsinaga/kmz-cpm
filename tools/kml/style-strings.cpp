#ifndef __KML_STYLE_STRINGS_CPP__
#define __KML_STYLE_STRINGS_CPP__

#include "style-strings.h"

namespace kml {

  StyleStrings::StyleStrings(xml::Node *root) {
    if (root) {
      xml::Node
        *iconStyleNode,
        *iconNode,
        *hrefNode,
        *lineStyleNode,
        *colorNode;

      iconMaps.clear();
      lineMaps.clear();
      styleMaps.clear();
      classification.clear();

      if (!root->isRoot()) {
        root = root->getRoot();
      }

      for (xml::Node *node : root->getDescendantsByName("Style")) {

        iconStyleNode = node->getFirstChildByName("IconStyle");
        lineStyleNode = node->getFirstChildByName("LineStyle");

        if (iconStyleNode) {
          iconNode = iconStyleNode->getFirstChildByName("Icon");

          if (iconNode) {
            hrefNode = iconNode->getFirstChildByName("href");

            if (hrefNode) {
              std::string hrefText = hrefNode->getInnerText();
              size_t urlCutIndex = 0;

              if (hrefText.find("https") != std::string::npos) {
                urlCutIndex = 37;
              }
              else if (hrefText.find("http") != std::string::npos) {
                urlCutIndex = 36;
              }

              iconMaps["#" + node->attributes["id"]] = std::string(
                hrefText.begin() + urlCutIndex,
                hrefText.end()
              );
            }
          }
        }

        if (lineStyleNode) {
          colorNode = lineStyleNode->getFirstChildByName("color");

          if (colorNode) {
            lineMaps["#" + node->attributes["id"]] = std::string(
              colorNode->getInnerText()
            );
          }
        }
      }

      for (xml::Node *node : root->getDescendantsByName("StyleMap")) {
        styleMaps["#" + node->attributes["id"]]
          = node->getFirstDescendantByName("styleUrl")->getInnerText();
      }
    }
  }

  void StyleStrings::classify(xml::Node *node, const bool &isIcon) {

    xml::Node *styleUrl = node->getFirstChildByName("styleUrl");
    std::string styleUrlText;

    if (styleUrl) {
      styleUrlText = styleUrl->getInnerText();
    }

    if (styleMaps.find(styleUrlText) != styleMaps.end()) {
      classification[isIcon?
        iconMaps[styleMaps[styleUrlText]] :
        lineMaps[styleMaps[styleUrlText]]
      ].push_back(node);
    }
  }
}

/** TEMPORARY */

namespace kml {

  int StyleStrings::getPinIconNamedIndex(std::string &pinIconNamed) {
    for (int i = 0;
      i < sizeof(pinIconUrlArray) / sizeof(std::string);
      i++
    ) {
      if (pinIconUrlArray[i].find(pinIconNamed) != std::string::npos) {
        return i;
      }
    }
    return -1;
  }

  std::string StyleStrings::getPinIconNamedUrl(std::string pinIconNamed) {

    if (pinIconNamed.find(".png") == std::string::npos) {
      pinIconNamed += ".png";
    }

    int index = getPinIconNamedIndex(pinIconNamed);

    if (index != -1) {
      return pinIconUrlArray[index];
    }

    return "";
  }

  int StyleStrings::getPinTyleFlag(std::string &pinIconNamed) {
    std::string url = getPinIconNamedUrl(pinIconNamed);

    if (url != "") {

      if (url.find("/pushpin/") != std::string::npos) {
        return PUSHPIN_PIN_TYPE_FLAG;
      }
      else if (url.find("/paddle/") != std::string::npos) {
        return PADDLE_PIN_TYPE_FLAG;
      }
      else if (url.find("/shapes/") != std::string::npos) {
        return SHAPES_PIN_TYPE_FLAG;
      }
    }

    return -1;
  }

  // identify if an icon
  bool StyleStrings::isAnIconUrl(std::string testStr) {

    if (testStr.find(".png") == std::string::npos) {
      testStr += ".png";
    }

    for (int i = 0; i < StyleStrings::pinIconUrlArray_count; i++) {
      if (mini_tool::cutFileDirName(pinIconUrlArray[i]) == testStr) {
        return true;
      }
    }

    return false;
  }

  bool StyleStrings::isAColorCode(std::string &testStr) {

    for (int i = 0; i < colorCodeArray_count; i++) {
      if (testStr == colorCodeArray[i]) {
        return true;
      }
    }

    return false;
  }

  // path's color codes from name
  std::string StyleStrings::getPathColorCode(std::string pathColorNamed) {

    std::string colorNames[] = {
      "red",
      "yellow",
      "magenta",
      "chocolate",
      "green",
      "orange",
      "purple",
      "white",
      "blue",
      "cyan",
      "black",
      "gray"
    };

    for (int i = 0; i < colorCodeArray_count; i++) {
      if (colorNames[i] == pathColorNamed) {
        return colorCodeArray[i];
      }
    }

    return colorCodeArray[7];
  }

  // get style string data of pin 'href' or path 'color-code'
  std::string StyleStrings::getPlacemarkStyleData(
    xml::Node *placemark,
    bool isRefreshStaticData,
    bool *styleExistanceHook
  ) {
    /**
     * Make sure in the first call to set 'isRefreshStaticData' true
     * for prevent unexpected no 'kmlNode' error.
     */
    static xml::Node *kmlNode = nullptr;
    static std::vector<xml::Node*> styleMapNodeVec, styleNodeVec;

    if (isRefreshStaticData) {
      if (placemark) {
        kmlNode = placemark->getRoot();

        if (kmlNode) {
          styleMapNodeVec = kmlNode->getDescendantsByName("StyleMap", true);
          styleNodeVec = kmlNode->getDescendantsByName("Style", true);
        }
      }
      else kmlNode = nullptr;
    }

    if (!kmlNode) {
      std::cerr
        << "KML-> Style strings error. No 'kml' root node as container\n"
        << "      for 'StyleMap' and 'Style' nodes\n";
      return "";
    }

    xml::Node *styleUrlNode = nullptr;

    if (placemark) {
      styleUrlNode = placemark->getFirstDescendantByName("styleUrl");
    }

    if (styleUrlNode) {
      std::string styleName = styleUrlNode->getInnerText();
      styleName = styleName.substr(1);

      for (xml::Node *styleMapNode : styleMapNodeVec) {
        for (auto &att1 : styleMapNode->attributes) {

          if (att1.first == "id" && att1.second == styleName) {

            /** Search only for normal style */

            std::string normalPairStyleName = (
              styleMapNode
                ->getFirstDescendantByName("Pair")
                ->getFirstDescendantByName("styleUrl")->getInnerText()
            );

            normalPairStyleName = normalPairStyleName.substr(1);

            for (xml::Node *styleNode : styleNodeVec) {
              for (auto &att2 : styleNode->attributes) {

                if (att2.first == "id" &&
                  att2.second == normalPairStyleName
                ) {
                  // pin
                  if (placemark->getFirstDescendantByName("Point")) {
                    xml::Node *hrefNode = styleNode->getFirstDescendantByName("href");

                    std::string pinUrlStr = (
                      hrefNode ? hrefNode->getInnerText() : pinIconUrlArray[0]
                    );

                    std::string urlStr = mini_tool::cutFileDirName(pinUrlStr);
                    size_t found = urlStr.find(".png");

                    if (found != std::string::npos) {
                      urlStr = urlStr.substr(0, found);
                    }

                    if (styleExistanceHook) *styleExistanceHook = true;
                    return urlStr;
                  }
                  //path 
                  else if (placemark->getFirstDescendantByName("LineString")) {
                    if (styleExistanceHook) *styleExistanceHook = true;

                    xml::Node *colorNode =  styleNode->getFirstDescendantByName("color");

                    if (colorNode) return colorNode->getInnerText();
                    return "ffffffff";
                  }
                }
              }
            }

            break;
          }
        }
      }
    }
    // no 'styleUrl', set to default
    else if (placemark && placemark->getFirstDescendantByName("Point")) {
      // pin
      if (styleExistanceHook) *styleExistanceHook = false;
      return "ylw-pushpin";
    }

    // path
    if (styleExistanceHook) *styleExistanceHook = false;
    return "ffffffff";
  }
}

#endif // __KML_STYLE_STRINGS_CPP__