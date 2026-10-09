#ifndef __KMZ_CPM__XML__PARSER_CPP__
#define __KMZ_CPM__XML__PARSER_CPP__

#include "xml/parser.hpp"
#include <stack>

namespace kmzcpm {
namespace xml {

  mt::ARR_BOL<256> Parser::createForbiddenNameCharTable() {
    mt::ARR_BOL<256> table {};

    for (mt::CR_UCH ch : {
      '<', '>', '&', '"', '\'', '=', '/',
      '?', '*', '+', '!', '@', '#', '$',
      '%', '^', '~', '\\', '|', '(', ')',
      '[', ']', '{', '}'
    }) {
      table[ch] = true;
    }

    return table;
  }

  mt::ARR_BOL<256> Parser::createWhitespaceCharTable() {
    mt::ARR_BOL<256> table {};

    for (mt::CR_UCH ch : {' ', '\r', '\t', '\f', '\v'}) {
      table[ch] = true;
    }

    return table;
  }

  mt::ARR_BOL<256>
    Parser::IS_FORBIDDEN_NAME_CHAR = Parser::createForbiddenNameCharTable(),
    Parser::IS_WHITESPACE_CHAR = Parser::createWhitespaceCharTable();

  void Parser::shiftChar() {
    if (!trackNewline()) Error::point[1]++;
    it++;
  }

  void Parser::cancelNode() {
    if (_pointer->isCompleted()) {
      Parnode *container = static_cast<Parnode*>(_pointer->parent());

      while (container) {
        if (container->isCompleted()) {
          _pointer->detach();
          _pointer->destroy();

          if (container->children()) {
            _pointer = static_cast<Parnode*>(container->children()->prev());
          }
          else _pointer = container;

          break;
        }
        else {
          _pointer = container;
          container = static_cast<Parnode*>(container->parent());
        }
      }
    }
    else {
      if (_pointer->alone()) {
        Parnode *container = static_cast<Parnode*>(_pointer->parent());
        _pointer->detach();
        _pointer->destroy();

        if (container) _pointer = container;
        else _pointer = nullptr;
      }
      else {
        Node *prev = _pointer->prev();
        _pointer->detach();
        _pointer->destroy();
        _pointer = static_cast<Parnode*>(prev);
      }
    }
  }

  bool Parser::setCompleted() {
    return false;
  }

  bool Parser::setSyntaxError() {
    Error::ch = *it;
    Error::status = ERR_SYNTAX;
    cancelNode();
    return false;
  }

  bool Parser::setPairError() {
    Error::pair_name = {_pointer->uniname(), {}};
    Error::status = ERR_PAIR;
    cancelNode();
    return false;
  }

  bool Parser::setPairError(mt::CR_VEC_STR openNames) {
    Error::pair_name = {
      mt_uti::StrTool::joinVector(openNames, ":"),
      _pointer->uniname()
    };

    Error::status = ERR_PAIR;
    cancelNode();
    return false;
  }

  bool Parser::setCDATAError() {
    Error::status = ERR_CDATA;
    cancelNode();
    return false;
  }

  bool Parser::setCommentError() {
    Error::status = ERR_COMMENT;
    cancelNode();
    return false;
  }

  bool Parser::trackNewline() {
    if (*it == '\n') {
      Error::point[0]++;
      Error::point[1] = 1;
      return true;
    }
    return false;
  }

  bool Parser::isAlphabet() {
    return mt_uti::StrTool::isAlphabet(*it) || *it == '_';
  }

  bool Parser::isSpace() {
    return trackNewline() || IS_WHITESPACE_CHAR[static_cast<mt::UCH>(*it)];
  }

  bool Parser::continueTest() {
    test[MAIN_TEST] = test[STOPBY_TEST];
    return true;
  }

  bool Parser::setNextTest(mt::CR<TEST_CALLBACK> cb) {
    test[MAIN_TEST] = cb;
    return true;
  }

  bool Parser::stopByTest(mt::CR<TEST_CALLBACK> cb) {
    test[STOPBY_TEST] = test[MAIN_TEST];
    test[MAIN_TEST] = cb;
    return true;
  }

  bool Parser::checkMultiline(mt::CR_VEC<CHCB> chsCbs) {

    // tolerates the spaces before the first target char detected
    while (it != end) {
      for (mt::CR<CHCB> chCb : chsCbs) {
        if (*it == chCb.first) {
          shiftChar();
          return setNextTest(chCb.second);
        }
      }

      if (isSpace()) shiftChar();
      else return setSyntaxError();
    }

    // single node as child of _subroot is completed
    if (_pointer->isCompleted() &&
      _pointer->parent() == _subroot
    ) {
      return setCompleted();
    }

    return setPairError();
  }

  bool Parser::fillText() {
    while (it != end) {
      if (*it == '<') {
        shiftChar();
        return setNextTest(select_open);
      }
      else if (*it == '&') {
        shiftChar();
        _strptr = &_text;
        return stopByTest(entity_ref);
      }
      else if (*it == '>') return setSyntaxError();

      _text += *it;
      shiftChar();
    }

    return setPairError();
  }

  bool Parser::fillValue(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    if (it != end) {
      std::string val;
      char vwrap = '\0';

      if (*it == '"' || *it == '\'') vwrap = *it;
      shiftChar();

      while (it != end) {
        if (*it == vwrap) {
          _pointer->setValue(_key, val);
          val = "";
          _key = "";
          shiftChar();

          while (it != end) {
            if (isSpace()) shiftChar();
            else {
              for (mt::CR<CHCB> chCb : chsCbs) {
                if (*it == chCb.first) {
                  shiftChar();
                  return setNextTest(chCb.second);
                }
              }

              if (isAlphabet()) return setNextTest(attKeyCb);
              return setPairError();
            }
          }
        }

        val += *it;
        shiftChar();
      }
    }

    return setPairError();
  }

  bool Parser::fillName(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    if (it != end) {
      if (isAlphabet()) {
        bool spaced = false;

        while (it != end) {
          if (isSpace()) spaced = true;
          else {
            for (mt::CR<CHCB> chCb : chsCbs) {
              if (*it == chCb.first) {
                shiftChar();
                return setNextTest(chCb.second);
              }
            }

            if (IS_FORBIDDEN_NAME_CHAR[static_cast<mt::UCH>(*it)]) break;
            else if (spaced) {
              _key += *it;
              shiftChar();
              return setNextTest(attKeyCb);
            }

            *_strptr += *it;
          }

          shiftChar();
        }
      }
    }

    return setPairError();
  }

  mt::VEC_STR Parser::splitNames() {
    mt::VEC_STR vec {""};

    for (mt::CR_CH ch : _name) {
      if (ch == ':') {
        vec.push_back("");
      }
      else vec.back() += ch;
    }

    _name = "";
    return vec;
  }

  bool Parser::entity_ref() {
    bool detected = false;

    std::function<void(mt::CR_CH)> onSemicolon = [&](mt::CR_CH oriCh) {
      shiftChar();

      if (*it == ';') {
        *_strptr += oriCh;
        detected = true;
      }
    };

    switch (*it) {
      case 'a': { // amp or apos
        shiftChar();

        switch (*it) {
          case 'm': { // amp
            shiftChar();

            switch (*it) {
              case 'p': onSemicolon('&'); break;
            }
          } break;
          case 'p': { // apos
            shiftChar();

            switch (*it) {
              case 'o': {
                shiftChar();

                switch (*it) {
                  case 's': onSemicolon('\''); break;
                }
              } break;
            }
          } break;
        }
      } break;
      case 'g': { // gt
        shiftChar();

        switch (*it) {
          case 't': onSemicolon('>'); break;
        }
      } break;
      case 'l': { // lt
        shiftChar();

        switch (*it) {
          case 't': onSemicolon('<'); break;
        }
      } break;
      case 'q': { // quot
        shiftChar();

        switch (*it) {
          case 'u': {
            shiftChar();

            switch (*it) {
              case 'o': {
                shiftChar();

                switch (*it) {
                  case 't': onSemicolon('"'); break;
                }
              } break;
            }
          } break;
        }
      } break;
    }

    if (detected) return continueTest();
    return setSyntaxError();
  }

  /** DECLARATION TESTS */

  bool Parser::decla_name() {
    _strptr = &_name;
    return fillName({{'?', decla_close}}, decla_att_key);
  }

  bool Parser::decla_att_key() {
    _strptr = &_key;
    return fillName({{'=', decla_att_value}}, decla_att_key);
  }

  bool Parser::decla_att_value() {
    return fillValue({{'?', decla_close}}, decla_att_key);
  }

  bool Parser::decla_close() {
    if (checkMultiline({{'>', any_open}})) {
      Node::declaration.type = _name;
      Node::declaration.encoding = _pointer->getValue("encoding");

      std::string vecstr = _pointer->getValue("version");
      std::string majstr, minstr;
      bool dotted = false;

      for (mt::CR_CH ch : vecstr) {
        if (ch == '.') dotted = true;
        else if (dotted) minstr += ch;
        else majstr += ch;
      }

      Node::declaration.version.major = mt_uti::Scanner::stringToNumber<int>(majstr);
      Node::declaration.version.minor = mt_uti::Scanner::stringToNumber<int>(minstr);

      _pointer->destroy();
      _pointer = new Parnode();

      return true;
    }
    else return false;
  }

  /** OPEN TAG TESTS */

  bool Parser::tag_open_name() {
    Parnode *newptr = new Parnode();

    if (_pointer->isCompleted()) {      
      _pointer->connect(newptr);
    }
    else _pointer->addChild(newptr);

    _pointer = newptr;
    _strptr = &_name;

    return fillName({
      {'>', offer_names},
      {'/', tag_close}
    }, tag_att_key);
  }

  bool Parser::tag_att_key() {
    _strptr = &_key;
    return fillName({{'=', tag_att_value}}, tag_att_key);
  }

  bool Parser::tag_att_value() {
    return fillValue({{
      {'>', offer_names},
      {'/', tag_close}
    }}, tag_att_key);
  }

  bool Parser::tag_close() {
    return checkMultiline({{'>', offer_names}});
  }

  bool Parser::offer_names() {
    _pointer->names = splitNames();

    if (*it == '<') {
      shiftChar();
      return setNextTest(select_open);
    }

    return setNextTest(fillText);
  }

  /** CLOSE TAG TESTS */

  bool Parser::xtag_open_name() {
    if (_pointer->isCompleted()) {
      _pointer = static_cast<Parnode*>(_pointer->parent());
    }

    _strptr = &_name;
    return fillName({{'>', xtag_close}}, xtag_no_att_key);
  }

  bool Parser::xtag_no_att_key() {
    return setSyntaxError();
  }

  bool Parser::xtag_close() {
    mt::VEC_STR closerNames = splitNames();

    if (_pointer->names.size() == closerNames.size()) {
      for (int i = 0; i < closerNames.size(); i++) {

        if (_pointer->names[i] != closerNames[i]) {
          return setPairError(closerNames);
        }
      }
    }
    else return setPairError(closerNames);

    if (!_text.empty()) {
      _pointer->setText(_text);
      _text = "";
    }

    _pointer->setCompleted();
    return setNextTest(any_open);
  }

  /** SELECTION TESTS */

  bool Parser::any_open() {
    return checkMultiline({{'<', select_open}});
  }

  bool Parser::select_open() {
    TEST_CALLBACK cb;

    if (it != end) {
      if (isAlphabet()) {
        _name += *it;
        cb = tag_open_name;
      }
      else if (*it == '/') {
        cb = xtag_open_name;
      }
      else if (*it == '!') {
        cb = excla_select;
      }
      else if (*it == '?') {
        cb = decla_name;
      }
      else return setSyntaxError();

      shiftChar();
      return setNextTest(cb);
    }

    return setPairError();
  }

  bool Parser::excla_select() {
    if (it != end) {
      if (*it == '-') {
        shiftChar();
        return setNextTest(comment_inner_close);
      }
      else if (*it == '[') {
        shiftChar();
        return setNextTest(cdata_open);
      }
    }

    return setPairError();
  }

  /** CDATA TESTS */

  bool Parser::cdata_open() {
    if (it != end) {
      if (*it == 'C') {
        enum alps {_c, _d, _a, _t, a_} phase = _c;

        while (it != end) {
          if (phase == _c && *it == 'D') {
            phase = _d;
          }
          else if (phase == _d && *it == 'A') {
            phase = _a;
          }
          else if (phase == _a && *it == 'T') {
            phase = _t;
          }
          else if (phase == _t && *it == 'A') {
            phase = a_;
          }
          else if (phase == a_ && *it == '[') {
            shiftChar();
            return setNextTest(cdata_inner_close);
          }
          else return setCDATAError();

          shiftChar();
        }
      }
      else return setCDATAError();
    }

    return setPairError();
  }

  bool Parser::cdata_inner_close() {
    int offSqrCt = 0;
    std::string notOffSqr;
    CDATA *cdata = new CDATA();
    std::string cdtext;

    if (_pointer->isCompleted()) _pointer->connect(cdata);
    else _pointer->addChild(cdata);

    while (it != end) {
      if (offSqrCt == 0) {
        if (*it == ']') {
          offSqrCt++;
          notOffSqr += *it;
        }
        else {
          trackNewline();
          cdtext += *it;
        }
      }
      else if (offSqrCt == 1 && *it == ']') {
        offSqrCt++;
        notOffSqr += *it;
      }
      else if (offSqrCt == 2 && *it == '>') {
        Parnode *mainstream = static_cast<Parnode*>(_pointer);
        Node *cdnode = Parser::scan(cdtext);

        if (Error::status == ERR_NONE) {
          cdata->addChild(cdnode);
          _pointer = mainstream;
        }
        else cdata->setText(cdtext);

        shiftChar();
        return setNextTest(xtag_open_name);
      }
      else {
        cdtext += notOffSqr;
        notOffSqr = "";
        offSqrCt = 0;
      }

      shiftChar();
    }

    return setCDATAError();
  }

  /** COMMENT TEST */

  bool Parser::comment_inner_close() {
    if (it != end) {
      if (*it == '-') {
        int offHypCt = 0;
        std::string notOffHyp, comtext;
        Comment *comment = new Comment();

        if (_pointer->isCompleted()) _pointer->connect(comment);
        else _pointer->addChild(comment);

        while (it != end) {
          if (offHypCt == 0) {
            if (*it == '-') {
              offHypCt++;
              notOffHyp += *it;
            }
            else {
              trackNewline();
              comtext += *it;
            }
          }
          else if (offHypCt == 1 && *it == '-') {
            offHypCt++;
            notOffHyp += *it;
          }
          else if (offHypCt == 2 && *it == '>') {
            shiftChar();
            comment->setText(comtext);
            return setNextTest(fillText);
          }
          else {
            comtext += notOffHyp;
            notOffHyp = "";
            offHypCt = 0;
          }

          shiftChar();
        }
      }
      else return setSyntaxError();
    }

    return setCommentError();
  }

  /** INIT TEST */

  Node *Parser::scan(mt::CR_STR raw) {
    Error::reset();

    // iterators init
    it = raw.begin();
    end = raw.end();
    test[MAIN_TEST] = any_open;

    // nodes init
    _root = new Parnode();
    _subroot = new Parnode();
    _root->addChild(_subroot);
    _pointer = _subroot;

    // iterator shifting is in scattered calls
    while (test[MAIN_TEST]()) {}

    // release product from sentinel roots
    if (_subroot && _subroot->children()) {
      Node *original = _subroot->children();
      original->detach();
      _pointer = static_cast<Parnode*>(original);
    }
    // no nodes detected
    else _pointer = nullptr;

    // clean and return the product
    _root->destroy();
    return static_cast<Node*>(_pointer);
  }

  std::string Parser::Error::stringify() {
    static constexpr std::string errstr[] {
      "ERR_NONE", "ERR_SYNTAX", "ERR_PAIR",
      "ERR_CDATA", "ERR_COMMENT"
    };

    return (
      std::string("char: ") + Error::ch + '\n' +
      "pair name: " + Error::pair_name.first + '\n' +
      "at row: " + std::to_string(Error::point[0]) +
      " column: " + std::to_string(Error::point[1]) + '\n' +
      "status: " + errstr[Error::status] + '\n'
    );
  }

  void Parser::Error::reset() {
    Error::ch = '\0';
    Error::pair_name = {};
    Error::point[0] = 1;
    Error::point[1] = 1;
    Error::status = ERR_NONE;
  }
}}

#endif // __KMZ_CPM__XML__PARSER_CPP__