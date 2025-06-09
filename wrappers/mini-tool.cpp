#ifndef __MINI_TOOL_CPP__
#define __MINI_TOOL_CPP__

#include "mini-tool.h"

const std::string mini_tool::DEGREE_SIGN = std::string{
  char(DEG_INT_CHAR_PT1), 
  char(DEG_INT_CHAR_PT2)
};

// acually remove the string in front of '/' with the sign itself
std::string mini_tool::cutFileDirName(const std::string &fileDir) {
  std::string cutName = fileDir;

  for (int i = 0; i < fileDir.size(); i++) {
    size_t foundDex = cutName.find("/");
    
    if (foundDex != std::string::npos) {
      cutName = cutName.substr(foundDex + 1);
    }
    else break;
  }

  return cutName;
}

bool mini_tool::isANumber(char ch) {
  if (int(ch) >= NM_BEGIN_CODE &&
    int(ch) <= NM_END_CODE
  ) { return true; }
  return false;
}

bool mini_tool::isALetter(char ch) {
  if ((int(ch) >= CH_A_UPPER_CODE && int(ch) <= CH_Z_UPPER_CODE) ||
    (int(ch) >= CH_A_LOWER_CODE && int(ch) <= CH_Z_LOWER_CODE)
  ) { return true; }
  return false;
}

std::string mini_tool::reverseString(std::string &str_in) {

  std::string str_out = str_in;
  int out_ctr = str_out.size() - 1;

  for (int in_ctr = 0; in_ctr < str_in.size(); in_ctr++) {
    str_out.at(out_ctr) = str_in.at(in_ctr);
    out_ctr--;
  }

  return str_out;
}

bool mini_tool::isStringContains(
  std::string &containerStr,
  std::string testStr,
  bool isIgnoreCaseSensitive
) {
  if (isIgnoreCaseSensitive) {
    changeStringCase(&containerStr, UPPER_CASE_FLAG);
    changeStringCase(&testStr, UPPER_CASE_FLAG);
  }

  if (containerStr.find(testStr) != std::string::npos) return true;
  return false;
}

bool mini_tool::isStringEquals(
  std::string &containerStr,
  std::string testStr,
  bool isIgnoreCaseSensitive
) {
  if (isIgnoreCaseSensitive) {
    changeStringCase(&containerStr, UPPER_CASE_FLAG);
    changeStringCase(&testStr, UPPER_CASE_FLAG);
  }

  if (containerStr == testStr) return true;
  return false;
}

int mini_tool::getInStringCharCount(std::string str, char ch) {
  int count = 0;

  for (int i = 0; i < str.size(); i++) {
    size_t found =  str.find(ch);

    if (found != std::string::npos) {
      count++;
      str.erase(str.begin() + found, str.begin() + found + 1);
    }
    else break;
  }

  return count;
}

void mini_tool::changeStringCase(
  std::string *string_ptr,
  int caseFlag,
  int onlyCharDex // index of single character change
) {
  if (string_ptr->size() == 0) return;

  int aToADifference = CH_A_LOWER_CODE - CH_A_UPPER_CODE, // positive
    startDex, borderDex;

  if (onlyCharDex != -1) {
    if (onlyCharDex <= string_ptr->size() - 1 &&
      isALetter(string_ptr->at(onlyCharDex))
    ) {
      startDex = onlyCharDex;
      borderDex = onlyCharDex + 1;
    }
  }
  else {
    startDex = 0;
    borderDex = string_ptr->size();
  }

  for (int i = startDex; i < borderDex; i++) {

    int chCode = std::abs(int(string_ptr->at(i))),
      diffSign = 0;

    // upper detected
    if (chCode < CH_A_LOWER_CODE &&
      caseFlag == LOWER_CASE_FLAG
    ) { diffSign = 1; }
    // lower detected
    else if (
      chCode > CH_Z_UPPER_CODE &&
      caseFlag == UPPER_CASE_FLAG
    ) { diffSign = -1; }

    if (diffSign != 0) {
      string_ptr->at(i) = char(chCode + diffSign * aToADifference);
    }
  }
}

std::string mini_tool::changeStringCase(
  std::string string_in,
  int caseFlag,
  int onlyCharDex // index of single character change
) {
  changeStringCase(&string_in, caseFlag, onlyCharDex);
  return string_in;
}

void mini_tool::completeDegreeCoordinateSecondsSign(
  std::string &coorStr
) {
  int ctr = 0,
    letterCtr = 0,
    letterDex[2];

  for (auto &ch : coorStr) {
    if (isALetter(ch)) {

      letterDex[letterCtr] = ctr;
      letterCtr++;

      if (letterCtr == 2) {
        coorStr.insert(coorStr.begin() + letterDex[0], '\"');
        letterDex[1]++;
        coorStr.insert(coorStr.begin() + letterDex[1], '\"');
        break;
      }
    }
    ctr++;
  }
}

double mini_tool::filterStringDecimal(std::string strIn) {
  int ctr = 0,
    dcPtCtr = 0; // decimal point counter

  std::string strVal = "";
  bool isHasNumber = false;

  for (auto &ch : strIn) {
    if (isANumber(ch)) {
      strVal += ch;
      isHasNumber = true;
    }
    else if (dcPtCtr == 0 && ch == '.') {
      if (ctr = 0) strVal += "0";
      dcPtCtr++;
    }
    else if (ctr == 0 && ch == '-') {
      strVal += ch;
    }
    ctr++;
  }

  if (!isHasNumber) return 0.000;
  return std::stod(strVal);
}

double mini_tool::safe_stod(const std::string &str) {
  double num;

  try { num = std::stod(str); }
  catch (...) {}

  return num;
}

#endif // __MINI_TOOL_CPP__