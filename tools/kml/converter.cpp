#ifndef __KML_CONVERTER_CPP__
#define __KML_CONVERTER_CPP__

#include "converter.h"

/**
 * Notes:
 *
 * This class is not a '.txt' to '.kml' converter.
 * Please use 'txt/scanner' and 'kml/builder' instead.
 * This class just for coordinate converter from 'degree'
 * to 'decimal' and vice versa.
 *
 * The tool functions take
 * only two stringified axis.
 */

namespace kml {

std::vector<std::string> Converter::separateCoordinate(
    std::string coorStr,
    int separateFlagIn, // '-1' means no position swap
    int separateFlagOut
) {
    bool isComma = false;
    std::string latStr = "", lngStr = "";

    for (auto &ch : coorStr) {
        if (ch == ',' || ch == ' ') isComma = true;
        else {
            if (separateFlagIn == LAT_LNG_SEPARATE_FLAG_IN ||
                separateFlagIn == -1
            ) {
                if (isComma) {
                    lngStr += ch;
                }
                else latStr += ch;
            }
            else if (separateFlagIn == LNG_LAT_SEPARATE_FLAG_IN) {
                if (isComma) {
                    latStr += ch;
                }
                else lngStr += ch;
            }
        }
    }

    if (separateFlagOut == LAT_LNG_SEPARATE_FLAG_OUT ||
        separateFlagOut == -1
    ) {
        return std::vector<std::string>{latStr, lngStr};
    }
    else if (separateFlagOut == LNG_LAT_SEPARATE_FLAG_OUT) {
        return std::vector<std::string>{lngStr, latStr};
    }

    return std::vector<std::string>{"", ""};
}

// return value e.g: '{1°10'100"S,100°10'100"E}' (no minus sign '-')
std::vector<std::string> Converter::convertCoor_decimalDegree(
    std::vector<std::string> coorStrCouple,
    int separateFlagIn,
    int separateFlagOut
) {
    int CTR = 0;
    double coor, minNum;
    std::string deg, min, sec;

    for (auto &str : coorStrCouple) {

        coor = mini_tool::safe_stod(str);
        deg = std::to_string(std::abs(int(coor))) + mini_tool::DEGREE_SIGN;
        minNum = (coor - int(coor)) * 60.0;
        sec = std::to_string(std::abs(minNum - int(minNum)) * 60.0) + "\"";
        min = std::to_string(std::abs(int(minNum))) + "'";
        str = deg + min + sec;

        if (separateFlagIn == LAT_LNG_SEPARATE_FLAG_IN) {
            if (CTR == 0) { // latitude
                if (coor < 0) str += "S";
                else str += "N";
            }
            else { // longitude
                if (coor < 0) str += "W";
                else str += "E";
            }
        }
        else if (separateFlagIn == LNG_LAT_SEPARATE_FLAG_IN) {
            if (CTR == 0) { // longitude
                if (coor < 0) str += "W";
                else str += "E";
            }
            else { // latitude
                if (coor < 0) str += "S";
                else str += "N";
            }
        }
        
        CTR++;
    }

    swapCoorStringVector(
        &coorStrCouple,
        separateFlagIn,
        separateFlagOut
    );

    return coorStrCouple;
}

std::vector<std::string> Converter::convertCoor_degreeDecimal(
    std::vector<std::string> coorStrCouple, // can be partly empty
    int separateFlagIn,
    int separateFlagOut
) {
    int CTR = 0;
    double coor = 0;

    std::string formErrorMessage = (
        "KML-> Convert error. Uncorrect form of degree coordinate\n"
    );

    for (auto &STR : coorStrCouple) {
        if (STR != "") {

            size_t deg_startCt = STR.find(mini_tool::DEGREE_SIGN),
                min_startCt = STR.find("'"),
                sec_startCt = STR.find("\"");

            bool isWithDecimalSign = false;

            // with decimal sign axis (not letter sign)
            if (deg_startCt != std::string::npos &&
                min_startCt != std::string::npos &&
                sec_startCt == std::string::npos
            ) {
                isWithDecimalSign = true;
                sec_startCt = STR.size() - 1;
            }
            // error message
            else if (
                deg_startCt == std::string::npos ||
                min_startCt == std::string::npos ||
                sec_startCt == std::string::npos
            ) {
                std::cerr << formErrorMessage;
                break;
            }

            ////////////////////
            // TAKE THE VALUE //
            ////////////////////

            std::string degStr = "", minStr = "", secStr = "";

            for (int i = deg_startCt - 1; i >= 0; i--) {
                degStr += STR.at(i);
            }
            degStr = mini_tool::reverseString(degStr);

            for (int i = min_startCt - 1; i > deg_startCt + 1; i--) {
                minStr += STR.at(i);
            }
            minStr = mini_tool::reverseString(minStr);

            for (int i = sec_startCt - 1; i > min_startCt; i--) {
                secStr += STR.at(i);
            }
            secStr = mini_tool::reverseString(secStr);

            // must be at 'int' converted 'char' of numbers interval
            if ((mini_tool::isANumber(degStr.front()) &&
                mini_tool::isANumber(minStr.front()) &&
                mini_tool::isANumber(secStr.front())) ||
                degStr.front() == '-'
            ) {
                int deg = std::stoi(degStr),
                    min = std::stoi(minStr);
                
                double sec = mini_tool::safe_stod(secStr);

                // PRODUCT
                coor = (sec / 60.0) + min;
                coor = (coor / 60.0) + std::abs(deg);
            }
            else {
                std::cerr << formErrorMessage;
                break;
            }

            //////////////////////
            // START CONVERTING //
            //////////////////////

            // with decimal sign
            if (isWithDecimalSign) {
                if (STR.front() == '-') {
                    coor *= -1;
                }
                STR = std::to_string(coor);
            }

            /* NOTE:
            *  consider the 'seperate flag'!!
            *  the letter sign might be ignored
            *  if not adjusted with the flag
            */
            else { // with letter sign

                if (separateFlagIn == LAT_LNG_SEPARATE_FLAG_IN) {
                    if (CTR == X) { // latitude
                        if (STR.back() == 'S' || STR.back() == 's') {
                            coor *= -1;
                        }
                    }
                    else if (CTR == Y) { // longitude
                        if (STR.back() == 'W' || STR.back() == 'B' ||
                            STR.back() == 'w' || STR.back() == 'b'
                        ) {
                            coor *= -1;
                        }
                    }
                    STR = std::to_string(coor);
                }
                else if (separateFlagIn == LNG_LAT_SEPARATE_FLAG_IN) {
                    if (CTR == X) { // latitude
                        if (STR.back() == 'W' || STR.back() == 'B' ||
                            STR.back() == 'w' || STR.back() == 'b'
                        ) {
                            coor *= -1;
                        }
                    }
                    else if (CTR == Y) { // longitude
                        if (STR.back() == 'S' || STR.back() == 's') {
                            coor *= -1;
                        }
                    }
                    STR = std::to_string(coor);
                }
            }
        }
        CTR++;
    }

    swapCoorStringVector(
        &coorStrCouple,
        separateFlagIn,
        separateFlagOut
    );

    return coorStrCouple;
}

void Converter::swapCoorStringVector(
    std::vector<std::string> *coorStrCouple_ptr,
    int separateFlagIn,
    int separateFlagOut
) {
    if ((separateFlagOut == LAT_LNG_SEPARATE_FLAG_OUT &&
        separateFlagIn == LNG_LAT_SEPARATE_FLAG_IN) ||
        (separateFlagOut == LNG_LAT_SEPARATE_FLAG_OUT &&
        separateFlagIn == LAT_LNG_SEPARATE_FLAG_IN)
    ) {
        std::string buffer = coorStrCouple_ptr->at(0);
        coorStrCouple_ptr->at(0) = coorStrCouple_ptr->at(1);
        coorStrCouple_ptr->at(1) = buffer;
    }
}
}

#endif // __KML_CONVERTER_CPP__