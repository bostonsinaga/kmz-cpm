#ifndef __XML_NODE_H__
#define __XML_NODE_H__

#define XML_TOPPER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define XML_TOPPER_COUNT 38

#include "../../wrappers/menu.h"

namespace xml {

  class Attribute {
  public:
    Attribute(std::string name_in, std::string value_in) {
      name = name_in;
      value = value_in;
    }

    std::string getName() { return name; }
    std::string getValue() { return value; }

    void setName(std::string name_in) {
      name = name_in;
    }

    void setValue(std::string value_in) {
      value = value_in;
    }

  private:
    std::string name, value;
  };

  #define MALE_XMLGEN 0  // unable to own children
  #define FEMALE_XMLGEN 1  // able to own children

  class Node {
  public:
    Node(std::string name_in, Node *parent_in = nullptr);
    ~Node();

    std::string getName();
    std::string getInnerText();
    Node *getParent();
    Node *getRoot();
    bool isRoot() { return !parent; }
    
    std::vector<Node*> *getChildren();
    std::vector<Attribute> *getAttributes();

    Node *getFirstChildByName(
      std::string searchName,
      bool isPrintMessage = false
    );

    std::vector<Node*> getChildrenByName(
      std::string searchName,
      bool isPrintMessage = false
    );

    /** Pull by name as first child from upper to lower generations */
    Node *getFirstDescendantByName(
      std::string searchName,
      bool isPrintMessage = false
    );

    /** Pull by name as children from upper to lower generations */
    std::vector<Node*> getDescendantsByName(
      std::string searchName,
      bool isPrintMessage = false
    );

    int getGender();
    void setGender(int genderFlag_in);

    /** NAME AND INNER TEXT */

    void setName(const std::string &name_in);
    void setName(std::string &name_in);

    void setInnerText(const std::string &text_in);
    void setInnerText(std::string &text_in);

    /** ATTRIBUTE */

    void setAttributes(std::vector<Attribute> attributes_in);
    void addAttribute(Attribute attribute_in);
    void setParent(Node *parent_in, bool isParentAddChild, bool isSwitchParent = false, int order = -1);
    void removeFromParent(bool isClean = false);
    void addChild(Node *newChild, int order = -1);
    void addChildren(std::vector<Node*> newChildren);
    std::vector<Node*> replaceChildren(std::vector<Node*> newChildren);
    void swapChildren(int aDex, int bDex);
    void swapChildren(Node *childA, Node *childB);
    void removeChild(Node *exChild, bool isClean = false);
    void removeChildren();
    std::vector<Node*> releaseChildren();

  private:
    void printChildNotFoundMessage(std::string searchName);

    int genderFlag = FEMALE_XMLGEN;
    std::string name, innerText;
    Node *parent = nullptr;

    std::vector<Node*> children;
    std::vector<Attribute> attributes;
  };
}

#endif // __XML_NODE_H__