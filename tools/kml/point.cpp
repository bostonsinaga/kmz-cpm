#ifndef __KML_POINT_CPP__
#define __KML_POINT_CPP__

#include "point.h"

namespace kml {

Point::Point(LD x_in, LD y_in) {
    x = x_in;
    y = y_in;
}

// from string coordinate
/* ('lng-lat' decimal 'coorString' will be swapped) */
Point::Point(std::string coorString) {

    // overload space removement (only as separator allowed) //

    std::vector<int> spaceIndexes;
    bool isHasCommaSeparator = false;
    int CTR = 0;

    for (auto &ch : coorString) {
        if (ch == ' ') {
            spaceIndexes.push_back(CTR);
        }
        else if (ch == ',') {
            isHasCommaSeparator = true;
        }
        CTR++;
    }

    int nearestSeparatorSpaceIndex = 0;
    if (!isHasCommaSeparator) {
        for (auto &index : spaceIndexes) {
            if (std::abs(int(index - coorString.size() / 2))
                < std::abs(int(nearestSeparatorSpaceIndex - coorString.size() / 2))
            ) {
                nearestSeparatorSpaceIndex = index;
            }
        }
    }

    int spaceIndexReduceRate = 0;
    for (auto &index : spaceIndexes) {

        if (index - spaceIndexReduceRate
            != nearestSeparatorSpaceIndex - spaceIndexReduceRate
            ||
            isHasCommaSeparator
        ) {
            coorString.erase(
                coorString.begin() + index - spaceIndexReduceRate,
                coorString.begin() + index - spaceIndexReduceRate + 1
            );
            spaceIndexReduceRate++;
        }
    }

    // axis handle //

    bool isInvalidAxis = false;
    int coorStringCommaCount = mini_tool::getInStringCharCount(coorString, ',');
    int coorStringSpaceCount = mini_tool::getInStringCharCount(coorString, ' ');

    // cut more then 2 axis (>= 3D)
    if (coorStringCommaCount >= 2 || coorStringSpaceCount >= 2) {
        char chFind = ',';
        if (coorStringSpaceCount >= 2) chFind = ' ';

        // search the last separator and cut the rest
        size_t separatorDex = coorString.find(chFind);
        for (auto &ch : coorString) {
            size_t testDex = coorString.find(chFind, separatorDex + 1);
            if (testDex != std::string::npos) {
                separatorDex = testDex;
            }
            else break;
        }

        coorString.erase(
            coorString.begin() + separatorDex,
            coorString.end()
        );
    }
    // one axis error (1D)
    else if (coorStringCommaCount == 0 && coorStringSpaceCount == 0) {
        std::cerr << "KML-> Point warning. Coordinate has only one axis. Default set to zero point\n";
        isInvalidAxis = true;
    }

    // convert degree into decimal coordinate //

    if (!isInvalidAxis) {
        Converter converter;

        // separate strings coordinate into 'Point'
        std::vector<std::string> coorStringVec = (
            converter.separateCoordinate(coorString)
        );

        for (auto &coorStr : coorStringVec) {
            if (coorStr.front() == char(DEG_INT_CHAR_PT1) ||
                (!mini_tool::isANumber(coorStr.front()) &&
                coorStr.front() != '-')
            ) {
                coorStr.at(0) = '-';
            }
        }
        
        // compass rate for swapping purpose
        int compassRate = General::checkCompass(coorStringVec.at(X));

        if (std::abs(compassRate) == 1) { // horizontal
            coorStringVec = converter.convertCoor_degreeDecimal(
                coorStringVec,
                converter.LNG_LAT_SEPARATE_FLAG_IN,
                converter.LNG_LAT_SEPARATE_FLAG_OUT
            );
        }
        else if (std::abs(compassRate) == 2) { // vertical
            coorStringVec = converter.convertCoor_degreeDecimal(
                coorStringVec,
                converter.LAT_LNG_SEPARATE_FLAG_IN,
                converter.LNG_LAT_SEPARATE_FLAG_OUT
            );
        }
        /*
        *   '0' 'compassRate' considered by default as 'latitude' and 'longitude'
        *   (this should be a decimal coordinate)
        *   for 'cropper' input, there was notice message
        *   for inputting as format above
        */
        else {
            /* will be swapped back in node parameter constructor */
            converter.swapCoorStringVector(
                &coorStringVec,
                converter.LAT_LNG_SEPARATE_FLAG_IN,
                converter.LNG_LAT_SEPARATE_FLAG_OUT
            );
        }
        
        x = mini_tool::safe_stod(coorStringVec.at(X));
        y = mini_tool::safe_stod(coorStringVec.at(Y));
    }
}

// the node must has coordinate string (for single coordinate)
Point::Point(xml::Node *coorNode) {
    if (coorNode->getName() == "coordinates") {
        Point pt = Point(coorNode->getInnerText());
        /*
        *   invert the 'pt' because default 'latitude' and 'longitude' input format consideration
        *   (see 'compassRate' in string parameter constructor)
        */
        x = pt.y;
        y = pt.x;
    }
}

Point Point::operator+(Point addPt) {
    return Point(x + addPt.x, y + addPt.y);
}

Point Point::operator-(Point subtPt) {
    return Point(x - subtPt.x, y - subtPt.y);
}

Point Point::operator*(Point multPt) {
    return Point(x * multPt.x, y * multPt.y);
}

Point Point::operator/(Point divPt) {
    if (divPt.x == 0) {
        divPt = Point(0.000001, divPt.y);
    }

    if (divPt.y == 0) {
        divPt = Point(divPt.x, 0.000001);
    }

    return Point(x / divPt.x, y / divPt.y);
}

Point Point::operator+(LD val) {
    return Point(x + val, y + val);
}

Point Point::operator-(LD val) {
    return Point(x - val, y - val);
}

Point Point::operator*(LD val) {
    return Point(x * val, y * val);
}

Point Point::operator/(LD val) {
    if (val == 0.0) {
        val = 0.000001;
    }
    return Point(x / val, y / val);
}

bool Point::isEqualTo(Point ptIn) {
    if (x == ptIn.x && y == ptIn.y) {
        return true;
    }
    return false;
}

// expected input: '180,-90,0 -180,90,0'
std::vector<Point> Point::getPathPointsFromString(std::string coorString) {
    
    std::vector<Point> retPoints;
    std::string coorStringBuffer = "";
    int commaCount = 0;

    int ctr = 0;
    for (auto &ch : coorString) {
        if (ch == ',') {
            commaCount++;
            coorStringBuffer += ch;
        }
        else if (ch == ' ' && commaCount >= 2) {
            retPoints.push_back(Point(coorStringBuffer));
            commaCount = 0;
            coorStringBuffer = "";
        }
        else if (ctr == coorString.size() - 1 && commaCount >= 2) {
            retPoints.push_back(Point(coorStringBuffer));
        }
        else if (mini_tool::isANumber(ch) || ch == '-' || ch == '.') coorStringBuffer += ch;
        ctr++;
    }

    return retPoints;
}

std::vector<std::string> Point::stringifyVector(
    std::vector<Point> points,
    bool isAddZeroAltitude
) {
    std::vector<std::string> retPointStrVec;

    for (auto &pt : points) {
        retPointStrVec.push_back(pt.stringify(isAddZeroAltitude));
    }

    return retPointStrVec;
}

std::string Point::stringify(bool isAddZeroAltitude) {
    return (
        to_string_with_precision(x) + "," +
        to_string_with_precision(y) +
        (isAddZeroAltitude ? ",0" : "")
    );
}

bool Point::isBetween(
    Point &testPt,
    Point &startPt,
    Point &endPt
) {
    if ((testPt.x >= startPt.x && testPt.x <= endPt.x && // BL - TR
        testPt.y >= startPt.y && testPt.y <= endPt.y)
        ||
        (testPt.x >= startPt.x && testPt.x <= endPt.x && // TL - BR
        testPt.y >= endPt.y && testPt.y <= startPt.y)
        ||
        (testPt.x >= endPt.x && testPt.x <= startPt.x && // TR - BL
        testPt.y >= endPt.y && testPt.y <= startPt.y)
        ||
        (testPt.x >= endPt.x && testPt.x <= startPt.x && // BR - TL
        testPt.y >= startPt.y && testPt.y <= endPt.y)
    ) {
        return true;
    }
    
    return false;
}

std::string Point::to_string_with_precision(LD &axis) {
    std::ostringstream out;
    
    // maximum digits is 16 (from Google Earth generated '.kml')
    int axisDigitCounts = std::log10(std::abs(axis));
    out.precision(15 - axisDigitCounts);
    
    out << std::fixed << axis;
    return std::move(out).str();
}
}

#endif // __KML_POINT_CPP__