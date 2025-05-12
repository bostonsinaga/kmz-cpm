#ifndef __KML_BUILDER_CPP__
#define __KML_BUILDER_CPP__

#include "builder.h"

namespace kml {

xml::Node *Builder::createFolder(
    std::string name,
    const std::string &defaultName,
    const bool &isOpen
) {
    if (name == "") name = defaultName;

    std::stringstream placemark_strStream;
    placemark_strStream
        << "<Folder>"
        << "<name>" << name << "</name>"
        << "<open>" << std::to_string(isOpen) << "</open>"
        << "</Folder>";

    return xml::Reader::parse(
        placemark_strStream.str(),
        "Folder [in runtime element]"
    );
}

// return one style map set of pins
xml::Node *Builder::createPinStyleMap(
    std::string *styleMapId_hook,
    std::string pinIconNamed
) {
    /* scale cannot be changed (only using default rate) */

    std::string
        normalId, highlightId,
        normalScale, highlightScale,
        styleMapId, hotspotPos[2];

    std::stringstream styleMap_strStream;

    std::string pinIconUrl;
    int pinIconTypeFlag;

    // using default 'ylw-pushpin'
    if (pinIconNamed == "" ||
        !StyleStrings::isAnIconUrl(pinIconNamed)
    ) {
        pinIconUrl = StyleStrings::pinIconUrlArray[0];
        pinIconNamed = mini_tool::cutFileDirName(pinIconUrl);
    }
    // using custom
    else {
        pinIconUrl = StyleStrings::getPinIconNamedUrl(pinIconNamed);
    }

    // remove '.png' extension string
    size_t containsPNGDex = pinIconNamed.find(".png");
    if (containsPNGDex != std::string::npos) {
        pinIconNamed = pinIconNamed.substr(0, containsPNGDex);
    }

    normalId = "sn_" + pinIconNamed;
    highlightId = "sh_" + pinIconNamed;
    styleMapId = "msn_" + pinIconNamed;

    if (styleMapId_hook) *styleMapId_hook = styleMapId;

    pinIconTypeFlag = StyleStrings::getPinTyleFlag(pinIconNamed);

    switch (pinIconTypeFlag) {
        case StyleStrings::PUSHPIN_PIN_TYPE_FLAG: {
            normalScale = pushpinScaleNormal;
            highlightScale = pushpinScaleHighlight;
            hotspotPos[0] = pushpinHotspotPosVec.at(0);
            hotspotPos[1] = pushpinHotspotPosVec.at(1);
        break;}
        case StyleStrings::PADDLE_PIN_TYPE_FLAG: {
            normalScale = paddleScaleNormal;
            highlightScale = paddleScaleHighlight;
            hotspotPos[0] = paddleHotspotPosVec.at(0);
            hotspotPos[1] = paddleHotspotPosVec.at(1);
        break;}
        case StyleStrings::SHAPES_PIN_TYPE_FLAG: {
            normalScale = shapesScaleNormal;
            highlightScale = shapesScaleHighlight;
            hotspotPos[0] = shapesHotspotPosVec.at(0);
            hotspotPos[1] = shapesHotspotPosVec.at(1);
        break;}
    }
    
    styleMap_strStream
        << "<StyleSet>"
        // normal style
        << "<Style id=\"" << normalId << "\">"
        << "<IconStyle>"
        << "<scale>" << normalScale << "</scale>"
        << "<Icon>"
        << "<href>" << pinIconUrl << "</href>"
        << "</Icon>"
        << "<hotSpot x=\""
            << hotspotPos[0] << "\" y=\""
            << hotspotPos[1] << "\" xunits=\"pixels\" yunits=\"pixels\"/>"
        << "</IconStyle>"
        << "</Style>"
        // highlight style
        << "<Style id=\"" << highlightId << "\">"
        << "<IconStyle>"
        << "<scale>" << highlightScale << "</scale>"
        << "<Icon>"
        << "<href>" << pinIconUrl << "</href>"
        << "</Icon>"
        << "<hotSpot x=\""
            << hotspotPos[0] << "\" y=\""
            << hotspotPos[1] << "\" xunits=\"pixels\" yunits=\"pixels\"/>"
        << "</IconStyle>"
        << "</Style>"
        // style map
        << "<StyleMap id=\"" << styleMapId << "\">"
        << "<Pair>"
        << "<key>normal</key>"
        << "<styleUrl>#" << normalId << "</styleUrl>"
        << "</Pair>"
        << "<Pair>"
        << "<key>highlight</key>"
        << "<styleUrl>#" << highlightId << "</styleUrl>"
        << "</Pair>"
        << "</StyleMap>"
        << "</StyleSet>";

    return xml::Reader::parse(
        styleMap_strStream.str(),
        "StyleSet [in runtime element]"
    );
}

// return one style map set of path
xml::Node *Builder::createPathStyleMap(
    std::string *styleMapId_hook,
    std::string pathColorNamed,
    std::string pathThicknessStr
) {
    std::string
        normalId,
        highlightId,
        styleMapId;
        
    std::stringstream styleMap_strStream;
    std::string pathColorCode;

    if (pathColorNamed == "") {
        pathColorNamed = "white";
        pathColorCode = StyleStrings::getPathColorCode(pathColorNamed);
        pathColorNamed += "-path";
    }
    else {
        for (int i = 0; i < StyleStrings::colorCodeArray_count; i++) {
            // 'pathColorNamed' as code
            if (StyleStrings::colorCodeArray[i] == pathColorNamed) {
                pathColorCode = pathColorNamed;
                break;
            }
            // 'pathColorNamed' as name or other
            else if (i == StyleStrings::colorCodeArray_count - 1) {
                pathColorCode = StyleStrings::getPathColorCode(pathColorNamed);
                pathColorNamed += "-path";
            }
        }
    }

    normalId = "sn_" + pathColorNamed;
    highlightId = "sh_" + pathColorNamed;
    styleMapId = "msn_" + pathColorNamed;

    if (styleMapId_hook) *styleMapId_hook = styleMapId;
    
    styleMap_strStream
        << "<StyleSet>"
        // normal style
        << "<Style id=\"" << normalId << "\">"
        << "<LineStyle>"
        << "<color>" << pathColorCode << "</color>"
        << "<width>" << pathThicknessStr << "</width>"
        << "</LineStyle>"
        << "</Style>"
        // highlight style
        << "<Style id=\"" << highlightId << "\">"
        << "<LineStyle>"
        << "<color>" << pathColorCode << "</color>"
        << "<width>" << pathThicknessStr << "</width>"
        << "</LineStyle>"
        << "</Style>"
        // style map
        << "<StyleMap id=\"" << styleMapId << "\">"
        << "<Pair>"
        << "<key>normal</key>"
        << "<styleUrl>#" << normalId << "</styleUrl>"
        << "</Pair>"
        << "<Pair>"
        << "<key>highlight</key>"
        << "<styleUrl>#" << highlightId << "</styleUrl>"
        << "</Pair>"
        << "</StyleMap>"
        << "</StyleSet>";
    
    return xml::Reader::parse(
        styleMap_strStream.str(),
        "StyleSet [in runtime element]"
    );
}

xml::Node *Builder::createPin(
    int altitudeAdditionFlag,
    std::string styleMapId,
    std::string decimalCoorStr,
    std::string name,
    std::string description
) {
    if (altitudeAdditionFlag == COORSTR_AUTO_ADD_ALTITUDE) {
        decimalCoorStr += getAltitudeAddition(
            altitudeAdditionFlag,
            decimalCoorStr
        );
    }
    else decimalCoorStr += getAltitudeAddition(altitudeAdditionFlag);

    std::stringstream placemark_strStream;
    placemark_strStream 
        << "<Placemark>"
        << "<name>" << name << "</name>"
        << "<description>" << description << "</description>"
        << "<styleUrl>#" << styleMapId << "</styleUrl>"
        << "<Point>"
        << "<coordinates>" << decimalCoorStr << "</coordinates>"
        << "</Point>"
        <<" </Placemark>";

    return xml::Reader::parse(
        placemark_strStream.str(),
        "Placemark [in runtime element]"
    );
}

xml::Node *Builder::createPath(
    int altitudeAdditionFlag,
    std::string styleMapId,
    std::vector<std::string> coorStrVec,
    std::string name,
    std::string description
) {
    std::string
        decimalCoorStrVec_joinStr = "",
        altAddStr;

    bool isAltAddStrUpdate = false;
    
    if (altitudeAdditionFlag == COORSTR_AUTO_ADD_ALTITUDE) {
        isAltAddStrUpdate = true;
    }
    else {
        altAddStr = getAltitudeAddition(altitudeAdditionFlag);
    }

    for (auto &coorStr : coorStrVec) {

        /* when unsure of comma count (coordinate form) consistency */
        if (isAltAddStrUpdate) {
            altAddStr = getAltitudeAddition(
                altitudeAdditionFlag,
                coorStr
            );
        }

        decimalCoorStrVec_joinStr += coorStr + altAddStr + " ";
    }

    // remove last space
    if (decimalCoorStrVec_joinStr != "") {
        decimalCoorStrVec_joinStr.pop_back();
    }

    std::stringstream placemark_strStream;
    placemark_strStream 
        << "<Placemark>"
        << "<name>" << name << "</name>"
        << "<description>" << description << "</description>"
        << "<styleUrl>#" << styleMapId << "</styleUrl>"
        << "<LineString>"
        << "<tessellate>1</tessellate>"
        << "<coordinates>" << decimalCoorStrVec_joinStr << "</coordinates>"
        << "</LineString>"
        <<" </Placemark>";

    return xml::Reader::parse(
        placemark_strStream.str(),
        "Placemark [in runtime element]"
    );
}

xml::Node *Builder::createSkeleton(std::string &docName) {
    if (docName == "") docName = "KML-TOWN";

    std::stringstream placemark_strStream;
    placemark_strStream
        // kml doc
        << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" "
        << "xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"
        << "<Document>"
        << "<name>" << docName << "</name>"

            // main folder
            << "<Folder>"
            << "<name>" << docName << "</name>"
            << "<open>1</open>"
            // all stuff branches from here
            << "</Folder>"

        << "</Document>"
        << "</kml>";
    
    return xml::Reader::parse(
        placemark_strStream.str(),
        "kml [in runtime element]"
    );
}

void Builder::setTitle(xml::Node *kmlNode, std::string &docName) {
    xml::Node *docNode = General::getRootDocument(kmlNode);

    if (docNode) {
        xml::Node *nameNode = docNode->getFirstChildByName("name");
        if (!nameNode) {
            nameNode = new xml::Node("name", docNode);
        }
        nameNode->setInnerText(docName);
    }

    xml::Node *mainFolderNode = General::searchMainFolder(kmlNode);

    if (mainFolderNode) {
        xml::Node *nameNode = mainFolderNode->getFirstChildByName("name");
        if (!nameNode) {
            nameNode = new xml::Node("name", mainFolderNode);
        }
        nameNode->setInnerText(docName);
    }
}

void Builder::insertStyleMap(xml::Node *kmlNode, xml::Node *styleSetNode) {

    xml::Node *docNode = General::getRootDocument(kmlNode);
    int childCtr = 0;

    for (auto &child : *docNode->getChildren()) {
        if (child->getName() == "Folder") break;
        childCtr++;
    }

    std::vector<xml::Node*> styleSetNodeVec;

    if (styleSetNode) {
        styleSetNodeVec = styleSetNode->releaseChildren();
        delete styleSetNode;

        for (auto &node : styleSetNodeVec) {
            docNode->addChild(node, childCtr);
            childCtr++; // still pointing to named 'Folder'
        }
    }
}

std::string Builder::getAltitudeAddition(
    int altitudeAdditionFlag,
    std::string coorStrAuto // must be filled if using 'COORSTR_AUTO_ADD_ALTITUDE'
) {
    switch (altitudeAdditionFlag) {

        case COORSTR_AUTO_ADD_ALTITUDE: {
            int commaCount = mini_tool::getInStringCharCount(coorStrAuto, ',');
            if (commaCount == 1) {
                return ",0";
            }
        }
        case COORSTR_ZERO_ADD_ALTITUDE: {
            return ",0";
        }
        case COORSTR_NO_ADD_ALTITUDE: {
            return "";
        }
    }

    return "";
}

std::vector<std::string>
    Builder::pushpinHotspotPosVec = {"20", "2"},
    Builder::paddleHotspotPosVec = {"32", "1"},
    Builder::shapesHotspotPosVec = {"0.5", "0"};

std::string
    Builder::pushpinScaleNormal = "1.1",
    Builder::pushpinScaleHighlight = "1.3",
    Builder::paddleScaleNormal = "1.1",
    Builder::paddleScaleHighlight = "1.3",
    Builder::shapesScaleNormal = "1.2",
    Builder::shapesScaleHighlight = "1.4";
}

#endif // __KML_BUILDER_CPP__