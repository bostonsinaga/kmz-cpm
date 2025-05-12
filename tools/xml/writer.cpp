#ifndef __XML_WRITER_CPP__
#define __XML_WRITER_CPP__

#include "writer.h"

namespace xml {

void Writer::stringify(
    std::string fileDir_out,
    Node *root,
    bool isWithRoot
) {
    /* the 'root' is not include to write */

    std::cout << "XML-> Writing '" + fileDir_out + "'...\n";
    Node *includedRoot;

    if (isWithRoot) {
        includedRoot = root;
        root = new Node("SuperRoot");
        root->addChild(includedRoot);
    }

    /* write into 'fileDir_out' */

    std::ofstream writeFile(fileDir_out);

    std::string text = (
        XML_TOPPER + std::string("\n") + pullChildrenStrings(root->getChildren())
    );

    writeFile << text;
    writeFile.close();

    if (isWithRoot) {
        root->releaseChildren();
        delete root; // delete 'SuperRoot'
    }

    std::cout << "XML-> Stringify into '" + fileDir_out + "' completed!\n";
}

std::string Writer::stringifyAttributes(Attribute &attributes) {
    std::string retTxt = "";

    for (auto &att : attributes) {
        retTxt += " " + att.first + "=\"" + att.second + "\"";
    }
    
    return retTxt;
}

std::string Writer::pullChildrenStrings(std::vector<Node*> *children) {

    std::string
        retTxt = "",
        tabSpaces = "    ",
        indentSpaces[2] = {"", ""};

    for (auto &child : *children) {

        // produce indent spaces
        int indentCtr = -1;
        Node *parent = child->getParent();

        while (parent) {
            indentCtr++;
            parent = parent->getParent();
        }

        for (int i = 0; i < indentCtr; i++) {
            indentSpaces[0] += tabSpaces;
        }

        // never has children (MALE)
        if (child->getGender() == MALE_XMLGEN) {
            retTxt += (
                indentSpaces[0] + "<" + child->getName()
                + stringifyAttributes(child->attributes) + "/>\n"
            );
        }
        else { // FEMALE element
            std::string pulledStrings = pullChildrenStrings(child->getChildren());

            // no children conditions
            std::string noChild_extraIndent = "";

            // has children
            if (pulledStrings != "") {
                pulledStrings = "\n" + pulledStrings;
                indentSpaces[1] = indentSpaces[0];
            }
            else {
                /*  NOTE:
                *   The pulled strings is keep its state.
                *   So tag indentation may look not suit because
                *   white space appearance. But white space is
                *   just a string that get kept as state.
                *
                *   example:
                *
                *    input:
                *       <Node>          --> contains '\n'
                *       </Node>             and white space
                *    
                *    output:
                *           <Node>      --> indentation increased
                *       </Node>         --> keep the state
                */
                pulledStrings = child->getInnerText();
            }

            retTxt += (
                indentSpaces[0] + "<" + child->getName()
                    + stringifyAttributes(child->attributes) + ">"
                        + pulledStrings
                + indentSpaces[1] + "</" + child->getName() + ">\n"
            );
        }

        indentSpaces[0] = "";
        indentSpaces[1] = "";
    }

    return retTxt;
}
}

#endif // __XML_WRITER_CPP__