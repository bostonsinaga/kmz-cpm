#ifndef __KML_BUILDER_H__
#define __KML_BUILDER_H__

#include "point.h"

namespace kml {

class Builder {
    public:
        static xml::Node *createFolder(
            std::string name = "",
            const std::string &defaultName = "Folder",
            const bool &isOpen = false
        );

        // return one style map set of pins
        static xml::Node *createPinStyleMap(
            std::string *styleMapId_hook = nullptr,
            std::string pinIconNamed = ""
        );

        // return one style map set of path
        static xml::Node *createPathStyleMap(
            std::string *styleMapId_hook = nullptr,
            std::string pathColorNamed = "",
            std::string pathThicknessStr = "2.0"
        );

        enum {
            COORSTR_AUTO_ADD_ALTITUDE,
            COORSTR_ZERO_ADD_ALTITUDE,
            COORSTR_NO_ADD_ALTITUDE
        };

        static xml::Node *createPin(
            int altitudeAdditionFlag,
            std::string styleMapId,
            std::string decimalCoorStr,
            std::string name = "",
            std::string description = ""
        );

        static xml::Node *createPath(
            int altitudeAdditionFlag,
            std::string styleMapId,
            std::vector<std::string> coorStrVec,
            std::string name = "",
            std::string description = ""
        );

        static xml::Node *createSkeleton(std::string &docName);
        static void setTitle(xml::Node *kmlNode, std::string &docName);
        static void insertStyleMap(xml::Node *kmlNode, xml::Node *styleSetNode);

    private:
        static std::string getAltitudeAddition(
            int altitudeAdditionFlag,
            std::string coorStrAuto = ""
        );

        static std::vector<std::string>
            pushpinHotspotPosVec,
            paddleHotspotPosVec,
            shapesHotspotPosVec;

        static std::string
            pushpinScaleNormal, pushpinScaleHighlight,
            paddleScaleNormal, paddleScaleHighlight,
            shapesScaleNormal, shapesScaleHighlight;
};
}

#endif // __KML_BUILDER_H__