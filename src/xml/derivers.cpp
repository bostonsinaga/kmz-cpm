#ifndef __KMZ_CPM__XML__DERIVERS_CPP__
#define __KMZ_CPM__XML__DERIVERS_CPP__

#include "xml/derivers.hpp"

namespace kmzcpm {
namespace xml {

  /** TAG */

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

  void Tag::setName(mt::CR_STR newName) {
    Node::setName(newName);
    splitNamespaces();
  }

  std::string Tag::getValue(mt::CR_STR key) {
    if (attributes.find(key) != attributes.end()) {
      return attributes[key];
    }
    return "";
  }

  void Tag::setValue(mt::CR_STR key, mt::CR_STR val) {
    attributes[key] = val;
  }

  std::string Tag::ostringy() {
    std::string attstr;

    for (mt::CR_PAIR_STR keyval : attributes) {
      attstr += ' ' + keyval.first + "=\"" + keyval.second + '"';
    }

    return '<' + name + attstr + (children() ? ">" : "/>");
  }

  std::string Tag::cstringy() {
    return children() ? ("</" + name + '>') : "";
  }

  /** DECLARATION */

  std::string Declaration::Version::stringify() {
    return std::to_string(major) + '.' + std::to_string(minor);
  }

  void Declaration::Version::parse(mt::CR_STR verstr) {
    std::string majstr, minstr;
    bool dotted = false;

    for (mt::CR_CH ch : verstr) {
      if (ch == '.') dotted = true;
      else if (dotted) minstr += ch;
      else majstr += ch;
    }

    major = mt_uti::Scanner::stringToNumber<int>(majstr);
    minor = mt_uti::Scanner::stringToNumber<int>(minstr);
  }

  std::string Declaration::ostringy() {
    return "<?" + attributes["type"]
    + " version=\"" + version.stringify() + '"'
    + " encoding=\"" + attributes["encoding"] + "\"?>";
  }

  /** CDATA */

  std::string CDATA::ostringy() {
    return "<![CDATA[" + (
      children() ? children()->stringify() : text
    ) + "]]>";
  }

  /** COMMENT */

  std::string Comment::ostringy() {
    return "<!-- " + text + " -->";
  }
}}

#endif // __KMZ_CPM__XML__DERIVERS_CPP__