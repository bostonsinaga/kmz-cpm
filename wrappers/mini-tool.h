#ifndef __MINI_TOOL__H__
#define __MINI_TOOL__H__

#define NM_BEGIN_CODE 48
#define NM_END_CODE 57

#define CH_A_UPPER_CODE 65
#define CH_Z_UPPER_CODE 90

#define CH_A_LOWER_CODE 97
#define CH_Z_LOWER_CODE 122

/**
 * Degree sign input in literal (°)
 * reformatted with 2 characters.
 */
#define DEG_INT_CHAR_PT1 -62 // normal degree sign
#define DEG_INT_CHAR_PT2 -80 // ...

/**
 * Degree sign anomaly may appear when
 * receiving degree coordinate parameter from '.bat' or '.sh' file.
 */
#define ANM_DEG_INT_CHAR_PT1 45 // anomaly degree sign
#define ANM_DEG_INT_CHAR_PT2 166 // ...

typedef long double LD;

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <cmath>
#include <functional>
#include <algorithm>

#include "definitions.h"

class mini_tool {
public:
  enum {
    LOWER_CASE_FLAG,
    UPPER_CASE_FLAG
  };

  static const std::string DEGREE_SIGN;

  static std::string cutFileDirName(const std::string &fileDir);
  static bool isANumber(char ch);
  static bool isALetter(char ch);
  static std::string reverseString(std::string &str_in);
  static int getInStringCharCount(std::string str, char ch);
  static double filterStringDecimal(std::string strIn);

  static bool isStringContains(
    std::string &containerStr,
    std::string testStr,
    bool isIgnoreCaseSensitive = false
  );

  static bool isStringEquals(
    std::string &containerStr,
    std::string testStr,
    bool isIgnoreCaseSensitive = false
  );

  static void changeStringCase(
    std::string *string_ptr,
    int caseFlag,
    int onlyCharDex = -1 // index of single character change
  );

  static std::string changeStringCase(
    std::string string_in,
    int caseFlag,
    int onlyCharDex = -1 // index of single character change
  );

  static void completeDegreeCoordinateSecondsSign(
    std::string &coorStr
  );

  // return index (-1 not found)
  template<typename T>
  static int isPrimitiveInsideVector(
    std::vector<T> &dataVec,
    T dataTest
  );

  /**
   * Returns intersect indexes of 'testVec'
   * in 2 kind of vector (duplicate and single[second dimension is 1 member]).
   * 
   * If not found at 'containerVec' index will set second dimension empty.
   * If all not found will return emty vector.
   * 
   * Empty first dimension will be deleted.
   */
  template<typename T>
  static std::vector<std::vector<int>> vectorIntersectsVector( // pass 1D vector only
    std::vector<T> &containerVec,
    std::vector<T> &testVec,
    bool isTestDuplicate = false
  );

  /**
   * Set the first member of second dimension
   * to be a member of new 1D vector.
   */
  template<typename T>
  static std::vector<T> collapse2DTo1DVector(
    std::vector<std::vector<T>> &vec_in
  );
};

#include "mini-tool.tpp"
#endif // __MINI_TOOL__H__