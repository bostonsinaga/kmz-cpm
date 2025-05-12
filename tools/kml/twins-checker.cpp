#ifndef __KML_TWINS_CHECKER_CPP__
#define __KML_TWINS_CHECKER_CPP__

#include "twins-checker.h"

namespace kml {

// return working folder node
xml::Node *TwinsChecker::findPins(
    xml::Node *kmlNode,
    std::string meterRadiusRateString,
    bool isParentFolderNamedAType,
    bool isIncludeFolders,
    bool isOnlySimilarStyle,
    bool isPrioritizePrintAboutMessage
) {
    double meterRadius = getLimitedMeterRadius(meterRadiusRateString);

    std::vector<xml::Node*> nodes;
    std::vector<Point> pointVec;

    // FILTERING PINS //

    for (auto &node : kmlNode->getDescendantsByName("Placemark", true)) {
        xml::Node *pointNode = node->getFirstDescendantByName("Point");
        
        if (pointNode) {
            nodes.push_back(node);

            // get points as degree of decimal
            pointVec.push_back(Point(
                pointNode->getFirstDescendantByName("coordinates")->getInnerText()
            ));

            pointVec.back() = Placemark::convertDegreeToMeterPoint(pointVec.back());
        }
    }

    // CHECK WHETHER INSIDE 'pointB' //

    int ctrA = 0,
        ctrB = 0,
        ctrC; // 'C' is prioritize

    std::vector<int> matchedIndexes[2]; // {originals, duplicates}
    bool isFirstPlcItr = true;

    std::vector<std::string>
        testStr_A_vec,
        testStr_B_vec,
        *testStr_C_vec; // 'C' is prioritize

    Point *pointC;
    const Point farPoint_const = Point(std::pow(10, 9), std::pow(10, 9));

    // special for pin
    auto prioritizeUpdate = [&](
        int &ctr_in,
        std::vector<std::string> &testStr_in_vec,
        Point &point_in
    ) {
        ctrC = ctr_in;
        Prioritize::testTextObtain(nodes.at(ctr_in), testStr_in_vec);
        testStr_C_vec = &testStr_in_vec;
        pointC = &point_in;
    };

    if (isPrioritizePrintAboutMessage) {
        Prioritize::printAboutMessage();
    }

    for (auto &pointA : pointVec) {
        prioritizeUpdate(ctrA, testStr_A_vec, pointA);
        
        for (auto &pointB : pointVec) {

            if (ctrC != ctrB &&
                !pointB.isEqualTo(farPoint_const) &&
                !pointC->isEqualTo(farPoint_const) &&
                pointC->x >= pointB.x - meterRadius &&
                pointC->x <= pointB.x + meterRadius &&
                pointC->y >= pointB.y - meterRadius &&
                pointC->y <= pointB.y + meterRadius &&
                (!isOnlySimilarStyle || (
                    isOnlySimilarStyle &&
                    checkSimilarStyle(nodes.at(ctrC), nodes.at(ctrB), isFirstPlcItr)
                ))
            ) {
                /* using name/description completeness for priority */

                Prioritize::testTextObtain(nodes.at(ctrB), testStr_B_vec);

                std::vector<std::string> otherIgnoredStrings = {
                    "Pin",
                    "Penanda letak"
                };

                if (Prioritize::isChanged(
                    *testStr_C_vec, testStr_B_vec, otherIgnoredStrings
                )) {
                    if (matchedIndexes[0].size() > 0 &&
                        ctrC == matchedIndexes[0].back()
                    ) {
                        matchedIndexes[0].pop_back();
                    }

                    matchedIndexes[1].push_back(ctrC);
                    *pointC = farPoint_const;

                    matchedIndexes[0].push_back(ctrB);
                    prioritizeUpdate(ctrB, testStr_B_vec, pointB);
                }
                else {
                    if (matchedIndexes[0].size() == 0 ||
                        (matchedIndexes[0].size() > 0 &&
                        ctrC != matchedIndexes[0].back())
                    ) {
                        matchedIndexes[0].push_back(ctrC);
                    }
                    
                    matchedIndexes[1].push_back(ctrB);
                    pointB = farPoint_const;
                }
            }

            ctrB++;
        }

        *pointC = farPoint_const;
        ctrA++;
        ctrB = 0;
    }
    
    return insertFoundPlacemarks(
        kmlNode,
        matchedIndexes,
        nodes,
        "pin",
        isParentFolderNamedAType,
        isIncludeFolders
    );
}

xml::Node *TwinsChecker::findPaths(
    xml::Node *kmlNode,
    std::string meterRadiusRateString,
    bool isParentFolderNamedAType,
    bool isIncludeFolders,
    bool isOnlySimilarStyle,
    bool isPathTextPrioritizeFirst,
    bool isPrioritizePrintAboutMessage
) {
    std::cerr << "KML-> Twins checking attention. This will sanitize/remove\n"
              << "      zero path (single coordinate) or empty path\n";

    double meterRadius = getLimitedMeterRadius(meterRadiusRateString);
    std::vector<xml::Node*> nodes;

    // may contains empty point vector
    std::vector<std::vector<Point>> pointVecVec;

    // FILTERING PATHS //

    for (auto &placemark : kmlNode->getDescendantsByName("Placemark", true)) {
        
        xml::Node *lineString = placemark->getFirstDescendantByName("LineString");
        
        if (lineString) {

            nodes.push_back(placemark);
            xml::Node *coorNode = lineString->getFirstDescendantByName("coordinates");

            // get vector points as degree of decimal
            pointVecVec.push_back(Point::getPathPointsFromString(
                coorNode ? coorNode->getInnerText() : ""
            ));

            /*  ATTENTION SANITIZE!
            *   this will remove zero length path
            *   (single coordinate) or empty path
            */
            if (pointVecVec.back().size() <= 1) {
                placemark->removeFromParent(true);
                nodes.pop_back();
                pointVecVec.pop_back();
                continue;
            }

            std::vector<Point> pointVec_buffer;

            for (auto &point : pointVecVec.back()) {
                pointVec_buffer.push_back(
                    Placemark::convertDegreeToMeterPoint(point)
                );
            }

            pointVecVec.back() = pointVec_buffer;
        }
    }

    // CHECK WHETHER INSIDE VECTOR OF 'pointB' //

    int ctrA = 0,
        ctrB = 0,
        ctrC; // 'C' is prioritize

    std::vector<int> matchedIndexes[2]; // {originals, duplicates}

    std::vector<std::string>
        testStr_A_vec,
        testStr_B_vec,
        *testStr_C_vec; // 'C' is prioritize

    std::vector<Point> *pointVec_C; // 'C' is prioritize
    bool isFirstPlcItr = true;

    // special for path
    auto prioritizeUpdate = [&](
        int &ctr_in,
        std::vector<std::string> &testStr_in_vec,
        std::vector<Point> &pointVec_in
    ) {
        Prioritize::testTextObtain(nodes.at(ctr_in), testStr_in_vec);
        pointVec_C = &pointVec_in;
        testStr_C_vec = &testStr_in_vec;
        ctrC = ctr_in;
    };

    // this will prioritize the 'B'
    auto setPathPrioritizeByText = [&](
        int &ctrB_in,
        std::vector<std::string> &testStr_B_vec_in,
        std::vector<Point> &pointVec_B_in
    ) {
        Prioritize::testTextObtain(nodes.at(ctrB_in), testStr_B_vec_in);

        std::vector<std::string> otherIgnoredStrings = {
            "Path",
            "Jalur",
            "Path Measure",
            "Pengukuran Jalur",
            "Line Measure",
            "Pengukuran Garis"
        };

        if (Prioritize::isChanged(
            *testStr_C_vec,
            testStr_B_vec_in,
            otherIgnoredStrings
        )) {
            if (matchedIndexes[0].size() > 0 &&
                ctrC == matchedIndexes[0].back()
            ) {
                matchedIndexes[0].pop_back();
            }

            matchedIndexes[1].push_back(ctrC);
            pointVec_C->clear();

            matchedIndexes[0].push_back(ctrB_in);
            prioritizeUpdate(ctrB_in, testStr_B_vec_in, pointVec_B_in);
        }
        else {
            if (matchedIndexes[0].size() == 0 ||
                (matchedIndexes[0].size() > 0 &&
                ctrC != matchedIndexes[0].back())
            ) {
                matchedIndexes[0].push_back(ctrC);
            }
            
            matchedIndexes[1].push_back(ctrB_in);
            pointVec_B_in.clear();
        }
    };

    if (isPrioritizePrintAboutMessage) {
        Prioritize::printAboutMessage();
    }

    std::cout << "KML-> Twins checking for paths is more prioritizing\n"
              << "      the length over the text data by default\n";
    
    for (auto &pointVec_A : pointVecVec) {

        bool isStyleSimilar = false;
        prioritizeUpdate(ctrA, testStr_A_vec, pointVec_A);

        for (auto &pointVec_B : pointVecVec) {
            if (ctrC != ctrB && pointVec_B.size() > 0) {

                if (isOnlySimilarStyle &&
                    !checkSimilarStyle(nodes.at(ctrC), nodes.at(ctrB), isFirstPlcItr)
                ) {
                    break;
                }

                std::vector<Point> *longestPointVec = (
                    pointVec_C->size() >= pointVec_B.size() ?
                    pointVec_C : &pointVec_B
                );
                
                int equalCount = 0;

                for (int i = 0; i < longestPointVec->size(); i++) {

                    if (i == pointVec_C->size() - 1 ||
                        i == pointVec_B.size() - 1
                    ) {
                        if (equalCount == pointVec_C->size() - 1 ||
                            equalCount == pointVec_B.size() - 1
                        ) {
                            /* using name/description completeness for priority */
                            if (isPathTextPrioritizeFirst) {
                                setPathPrioritizeByText(ctrB, testStr_B_vec, pointVec_B);
                            }
                            /* using path length for priority test */
                            else if (pointVec_C->size() < pointVec_B.size()) {

                                if (matchedIndexes[0].size() > 0 &&
                                    ctrC == matchedIndexes[0].back()
                                ) {
                                    matchedIndexes[0].pop_back();
                                }

                                matchedIndexes[1].push_back(ctrC);
                                pointVec_C->clear();

                                matchedIndexes[0].push_back(ctrB);
                                prioritizeUpdate(ctrB, testStr_B_vec, pointVec_B);
                            }
                            /* using path length for priority test */
                            else if (pointVec_C->size() > pointVec_B.size()) { // normal

                                if (matchedIndexes[0].size() == 0 ||
                                    (matchedIndexes[0].size() > 0 &&
                                    ctrC != matchedIndexes[0].back())
                                ) {
                                    matchedIndexes[0].push_back(ctrC);
                                }

                                matchedIndexes[1].push_back(ctrB);
                                pointVec_B.clear();
                            }
                            /*
                                the lengths are equal and
                                using name/description completeness for priority
                            */
                            else setPathPrioritizeByText(ctrB, testStr_B_vec, pointVec_B);
                        }

                        break;
                    }

                    if (pointVec_C->size() > 0 && pointVec_B.size() > 0 &&
                        pointVec_C->at(i).x >= pointVec_B.at(i).x - meterRadius &&
                        pointVec_C->at(i).x <= pointVec_B.at(i).x + meterRadius &&
                        pointVec_C->at(i).y >= pointVec_B.at(i).y - meterRadius &&
                        pointVec_C->at(i).y <= pointVec_B.at(i).y + meterRadius
                    ) {
                        equalCount++;
                    }
                }
            }

            ctrB++;
        }

        pointVec_C->clear();
        ctrA++;
        ctrB = 0;
    }

    return insertFoundPlacemarks(
        kmlNode,
        matchedIndexes,
        nodes,
        "path",
        isParentFolderNamedAType,
        isIncludeFolders
    );
}

xml::Node *TwinsChecker::findAll(
    xml::Node *kmlNode,
    std::string meterRadiusRateString,
    bool isIncludeFolders,
    bool isOnlySimilarStyle,
    bool isPathTextPrioritizeFirst
) {
    Prioritize::printAboutMessage();

    xml::Node *pinsFolder = findPins(
        kmlNode,
        meterRadiusRateString,
        true,
        isIncludeFolders,
        isOnlySimilarStyle,
        false
    );

    xml::Node *pathsFolder = findPaths(
        kmlNode,
        meterRadiusRateString,
        true,
        isIncludeFolders,
        isOnlySimilarStyle,
        isPathTextPrioritizeFirst,
        false
    );

    xml::Node *workingFolder = Builder::createFolder(
        definitions::TWINS_CHECK_COMMAND_WORKING_FOLDER
    );

    if (pinsFolder) workingFolder->addChild(pinsFolder);
    if (pathsFolder) workingFolder->addChild(pathsFolder);

    // connect to 'kmlNode'
    if (pinsFolder || pathsFolder) {
        General::putOnTopFolder(
            General::getRootDocument(kmlNode),
            workingFolder
        );
        return workingFolder;
    }
    else {
        delete workingFolder;
        return nullptr;
    }
}

xml::Node *TwinsChecker::insertFoundPlacemarks(
    xml::Node *kmlNode,
    std::vector<int> *matchedIndexes, // int vector array (has 2 members)
    std::vector<xml::Node*> &placemarkNodes,
    std::string placemarksType,
    bool isParentFolderNamedAType,
    bool isIncludeFolders
) {
    std::string kmlNameString = General::getRootDocumentName(kmlNode);

    if (matchedIndexes[0].size() > 0) {

        xml::Node *returnFolder = Builder::createFolder(
            isParentFolderNamedAType ?
            mini_tool::changeStringCase(placemarksType, mini_tool::UPPER_CASE_FLAG) :
            definitions::TWINS_CHECK_COMMAND_WORKING_FOLDER
        );

        xml::Node *folderOri = Builder::createFolder("ORIGINALS");
        returnFolder->addChild(folderOri);

        xml::Node *folderDupl = Builder::createFolder("DUPLICATES");
        returnFolder->addChild(folderDupl);

        int matchedCtr = 0;

        for (int i = 0; i < 2; i++) {
            for (auto &index : matchedIndexes[i]) {

                // remove from parent
                if (!isIncludeFolders) {
                    placemarkNodes.at(index)->removeFromParent();
                }

                xml::Node *twinsDivFolder;

                // originals
                if (i == 0) {
                    twinsDivFolder = folderOri;
                    matchedCtr++;
                }
                // duplicates
                else twinsDivFolder = folderDupl;

                if (isIncludeFolders) {
                    Placemark::includeFolder(
                        placemarkNodes.at(index),
                        twinsDivFolder
                    );
                }
                else twinsDivFolder->addChild(placemarkNodes.at(index));
            }
        }

        std::cout
            << "----------------\n"
            << "Total matched:  " << matchedCtr
            << (isParentFolderNamedAType ? std::string("  (") + placemarksType + std::string(")") : "")
            << std::endl
            << "----------------\n"
            << "KML-> Twins check for '" << placemarksType
            << "' inside '"<< kmlNameString << "' document completed!\n";

        if (isParentFolderNamedAType) return returnFolder;
        else {
            // connect to 'kmlNode'
            xml::Node *rootDoc = General::getRootDocument(kmlNode);
            General::putOnTopFolder(rootDoc, returnFolder);
            return returnFolder;
        }
    }
    else {
        std::cerr << "KML-> Twins checking warning. No '"
                  << placemarksType << "' twins inside '"
                  << kmlNameString << "' document\n";
        return nullptr;
    }
}

/* limited from 0 to 100 meters */
double TwinsChecker::getLimitedMeterRadius(std::string meterRadiusRateString) {

    double meterRadius = mini_tool::filterStringDecimal(meterRadiusRateString);
    meterRadius = std::abs(meterRadius);

    if (meterRadius > 100.0) {
        std::cerr << "KML-> Twins checking warning. The given radius exceeds 100 meters.\n"
                  << "      Radius is set to limit.\n";
        meterRadius = 100.0;
    }

    return meterRadius;
}

// used when 'find' function's 'isOnlySimilarStyle' is true
bool TwinsChecker::checkSimilarStyle(
    xml::Node *placemark_A,
    xml::Node *placemark_B,
    bool &isFirstPlcItr
) {
    std::string styleDataStr_A, styleDataStr_B;

    if (isFirstPlcItr) {
        styleDataStr_A = StyleStrings::getPlacemarkStyleData(placemark_A, true);
        styleDataStr_B = StyleStrings::getPlacemarkStyleData(placemark_B, false);
        isFirstPlcItr = false;
        return styleDataStr_A == styleDataStr_B;
    }
    else {
        styleDataStr_A = StyleStrings::getPlacemarkStyleData(placemark_A, false);
        styleDataStr_B = StyleStrings::getPlacemarkStyleData(placemark_B, false);
        return styleDataStr_A == styleDataStr_B;
    }
}

//////////////////////
// PRIORITIZE STUFF //
//////////////////////

void TwinsChecker::Prioritize::testTextObtain(
    xml::Node *placemark,
    std::vector<std::string> &testStr_vec_in
) {
    testStr_vec_in.clear();
    testStr_vec_in.push_back(Placemark::getDataText(placemark, "name"));
    testStr_vec_in.push_back(Placemark::getDataText(placemark, "description"));
}

bool TwinsChecker::Prioritize::isChanged(
    std::vector<std::string> &testStr_A_vec_in,   // 'A' should be lose
    std::vector<std::string> &testStr_B_vec_in,   // 'B' should be win
    std::vector<std::string> &otherIgnoredStrings_in
) {
    enum {name_flg, desc_flg};

    // description
    if (testStr_A_vec_in.at(desc_flg).size() < testStr_B_vec_in.at(desc_flg).size()) {
        return true;
    }
    // name
    else if (
        mini_tool::isStringEquals(testStr_A_vec_in.at(desc_flg), testStr_B_vec_in.at(desc_flg), true) &&
        isNoname(testStr_A_vec_in.at(name_flg), otherIgnoredStrings_in) &&
        !isNoname(testStr_B_vec_in.at(name_flg), otherIgnoredStrings_in)
    ) {
        return true;
    }
    // 'A' wins (even the descriptions are equal in size but different in definition)
    else return false;
}

void TwinsChecker::Prioritize::printAboutMessage() {
    std::cout << "KML-> Twins checking is prioritizing the more complete text data\n"
              << "      for be the original (description is superior to name)\n";
}

bool TwinsChecker::Prioritize::isNoname(
    std::string &testStr,
    std::vector<std::string> &otherIgnoredStrings_in
) {
    std::vector<std::string> ignoredStrings = {
        "Untitled",
        "Tanpa Nama",
        "Noname"
    };

    if (testStr == "") return true;

    for (auto &str : ignoredStrings) {
        if (mini_tool::isStringContains(testStr, str, true)) {
            return true;
        }
    }

    for (auto &str : otherIgnoredStrings_in) {
        if (mini_tool::isStringEquals(testStr, str, true)) {
            return true;
        }
    }

    return false;
}

/*
    -clean copies of icon urls or color codes that contained inside separate 'StyleMap'
     to reduce size of 'kml' file and also rename the 'styleUrl' to templated names
     (note that this operation will rewrite previous tidied styles)

    -this won't keep the previous pins or paths size but will use default size

    -command: 'tidy-up-styles'
*/
bool TwinsChecker::tidyUpStyles(xml::Node *kmlNode) {
    if (kmlNode) {

        /* Array:
            index '0' is pins
            index '1' is paths
        */

        bool isFirstPlcItr = true;
        std::vector<std::string> styleUrlVec[2];
        std::vector<xml::Node*> placemarkNodesVec[2];

        placemarkNodesVec[0] = kmlNode->getDescendantsByName("Point", true);
        placemarkNodesVec[1] = kmlNode->getDescendantsByName("LineString", true);

        if (placemarkNodesVec[0].size() == 0 &&
            placemarkNodesVec[1].size() == 0
        ) {
            std::cerr << "KML-> Tidy up styles error. Placemark not found\n";
            return false;
        }

        bool styleExistanceHook, isStyleExist = false;

        for (int i = 0; i < 2; i++) {
            for (auto &placemarkNode : placemarkNodesVec[i]) {
                placemarkNode = placemarkNode->getParent();

                styleUrlVec[i].push_back(
                    StyleStrings::getPlacemarkStyleData(
                        placemarkNode, isFirstPlcItr, &styleExistanceHook
                    )
                );

                if (!isStyleExist && styleExistanceHook) {
                    isStyleExist = true;
                }

                if (isFirstPlcItr) isFirstPlcItr = false;
            }
        }

        if (!isStyleExist) {
            std::cerr << "KML-> Tidy up styles error. Style data not found\n";
            return false;
        }

        // delete existing 'StyleMap' and 'Style' nodes //

        std::vector<std::string> deletedNames {"StyleMap", "Style"};

        for (auto &name : deletedNames) {
            for (auto &node : kmlNode->getDescendantsByName(name, true)) {
                node->removeFromParent();
                delete node;
            }
        }

        // get clean 'styleUrlVec' from duplications //

        std::vector<std::string> cleanedStyleUrlVec[2];

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < styleUrlVec[i].size(); j++) {
                int cleanedLatestDex;

                int oriDex = mini_tool::isPrimitiveInsideVector<std::string>(
                    cleanedStyleUrlVec[i], styleUrlVec[i].at(j)
                );

                if ((cleanedStyleUrlVec[i].size() > 0 && oriDex == -1) ||
                    cleanedStyleUrlVec[i].size() == 0
                ) {
                    cleanedStyleUrlVec[i].push_back(styleUrlVec[i].at(j));
                    cleanedLatestDex = cleanedStyleUrlVec[i].size() - 1;
                }
                else cleanedLatestDex = oriDex;

                xml::Node *styleUrlNode = (
                    placemarkNodesVec[i].at(j)->getFirstChildByName("styleUrl")
                );

                if (styleUrlNode) {
                    styleUrlNode->setInnerText("#msn_" + cleanedStyleUrlVec[i].at(cleanedLatestDex));
                }
                // no 'styleUrl' node
                else {
                    styleUrlNode = new xml::Node("styleUrl");

                    General::putOnTopFolder(
                        placemarkNodesVec[i].at(j),
                        styleUrlNode,
                        {"description", "LookAt"}
                    );

                    styleUrlNode->setInnerText("#msn_" + styleUrlVec[i].at(j));
                }
            }
        }

        // set new 'StyleMap' and 'Style' nodes //

        std::vector<xml::Node*> styleSetNodes[2];

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < cleanedStyleUrlVec[i].size(); j++) {

                styleSetNodes[i].push_back(
                    i == 0 ?
                    Builder::createPinStyleMap(nullptr, cleanedStyleUrlVec[i].at(j)) :
                    Builder::createPathStyleMap(nullptr, cleanedStyleUrlVec[i].at(j))
                );
            }
        }

        // remove 'StyleSet' node as parent of 'StyleMap' and 'Style' nodes to reveal them //

        std::vector<xml::Node*> revealedStyleSetNodes;

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < styleSetNodes[i].size(); j++) {

                for (int k = 0; k < 3; k++) {
                    revealedStyleSetNodes.push_back(styleSetNodes[i].at(j)->getChildren()->at(k));
                }

                styleSetNodes[i].at(j)->releaseChildren();
                delete styleSetNodes[i].at(j);
            }
        }

        xml::Node *docNode = General::getRootDocument(kmlNode);
        std::string kmlNameString = General::getRootDocumentName(kmlNode);
        General::putOnTopFolder(docNode, revealedStyleSetNodes);

        std::cout << "KML-> Tidy up styles for placemarks inside '"
                  << kmlNameString << "' document completed!\n";

        return true;
    }

    return false;
}
}

#endif // __KML_TWINS_CHECKER_CPP__