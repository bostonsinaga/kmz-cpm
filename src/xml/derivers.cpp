#ifndef __KMZ_CPM__XML__DERIVERS_CPP__
#define __KMZ_CPM__XML__DERIVERS_CPP__

#include "xml/derivers.hpp"

namespace kmzcpm {
namespace xml {

  void Tag::setName(mt::CR_STR newName) {
    Node::setName(newName);
    splitNamespaces();
  }

  void Tag::splitNamespaces() {
    if (!name.empty()) {
      namespaces = {""};

      for (mt::CR_CH ch : name) {
        if (ch == ':') {
          namespaces.push_back("");
        }
        else namespaces.back() += ch;
      }
    }
  }

  void CDATA::init() {
    Node::init();
    type = CDATAType;
  }

  std::string CDATA::ostringy() {
    return "<![CDATA[" + (
      children() ? children()->stringify() : text
    ) + "]]>";
  }

  void Comment::init() {
    Node::init();
    type = CommentType;
  }

  std::string Comment::ostringy() {
    return "<!-- " + text + " -->";
  }
}}

#endif // __KMZ_CPM__XML__DERIVERS_CPP__