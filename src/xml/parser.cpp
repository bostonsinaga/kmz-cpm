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

    for (mt::CR_UCH ch : {' ', '\r', '\n', '\t', '\f', '\v'}) {
      table[ch] = true;
    }

    return table;
  }

  mt::ARR_BOL<256>
    Parser::IS_FORBIDDEN_NAME_CHAR = Parser::createForbiddenNameCharTable(),
    Parser::IS_WHITESPACE_CHAR = Parser::createWhitespaceCharTable();

  void Parser::Member::init(mt::CR_STR raw) {
    it = raw.begin();
    end = raw.end();
    name = "";
    key = "";
    text = "";
    strptr = nullptr;
    point[ROW] = 0;
    point[COL] = 0;
    errorOccurred = false;
    test[MAIN_TEST] = any_mark;
    test[STOPBY_TEST] = nullptr;
    status = Phase::ZERO;
  }

  void Parser::Member::create(mt::CR_STR raw) {
    init(raw);
    root = new Node("$ROOT");
    root->setCompleted();
    pointer = root;
  }

  void Parser::Member::refresh(mt::CR_STR raw) {
    init(raw);
    pointer = root->children() ? root->children()->prev() : root;
  }

  void Parser::shiftChar() {
    if (*_mem.it == '\n') {
      _mem.point[ROW]++;
      _mem.point[COL] = 1;
    }
    else _mem.point[COL]++;

    _mem.it++;
  }

  bool Parser::isAlphabet() {
    return mt_uti::StrTool::isAlphabet(*_mem.it) || *_mem.it == '_';
  }

  bool Parser::isForbiddenName() {
    return IS_FORBIDDEN_NAME_CHAR[static_cast<mt::UCH>(*_mem.it)];
  }

  bool Parser::isSpace() {
    return IS_WHITESPACE_CHAR[static_cast<mt::UCH>(*_mem.it)];
  }

  bool Parser::setError() {
    _mem.errorOccurred = true;
    return false;
  }

  bool Parser::continueTest() {
    _mem.test[MAIN_TEST] = _mem.test[STOPBY_TEST];
    return true;
  }

  bool Parser::setNextTest(mt::CR<TEST_CALLBACK> cb) {
    _mem.test[MAIN_TEST] = cb;
    return true;
  }

  bool Parser::stopByTest(mt::CR<TEST_CALLBACK> cb) {
    _mem.test[STOPBY_TEST] = _mem.test[MAIN_TEST];
    return setNextTest(cb);
  }

  bool Parser::endMultiline() {

    // single node as child of root is completed
    if (_mem.pointer && _mem.pointer->isCompleted() &&
      _mem.pointer->parent() == _mem.root
    ) {
      return false;
    }

    return setError();
  }

  bool Parser::charMultiline(mt::CR_VEC<CHCB> chsCbs) {
    while (_mem.it != _mem.end) {

      // each character and callback pair
      for (mt::CR<CHCB> chCb : chsCbs) {
        if (*_mem.it == chCb.first) {
          shiftChar();
          return setNextTest(chCb.second);
        }
      }

      // ignore space
      if (isSpace()) shiftChar();
      // unknown character
      else return setError();
    }

    return endMultiline();
  }

  bool Parser::textMultiline(mt::CR_VEC<CHCB> chsCbs) {
    while (_mem.it != _mem.end) {

      // each character and callback pair
      for (mt::CR<CHCB> chCb : chsCbs) {
        if (*_mem.it == chCb.first) {
          shiftChar();
          return setNextTest(chCb.second);
        }
      }

      // ignore space
      if (isSpace()) shiftChar();
      // entity reference
      else if (*_mem.it == '&') {
        shiftChar();
        _mem.strptr = &_mem.text;
        _mem.test[STOPBY_TEST] = fill_fext;
        return setNextTest(entity_ref);
      }
      // fill text
      else return setNextTest(fill_fext);
    }

    return endMultiline();
  }

  bool Parser::fill_fext() {
    _mem.status = Phase::FILL_TEXT;
    _mem.text = "";

    while (_mem.it != _mem.end) {
      if (*_mem.it == '<') {
        shiftChar();
        _mem.pointer->setText(_mem.text);
        return setNextTest(select_mark);
      }
      else if (*_mem.it == '&') {
        shiftChar();
        _mem.strptr = &_mem.text;
        return stopByTest(entity_ref);
      }
      else if (*_mem.it == '>') return setError();

      _mem.text += *_mem.it;
      shiftChar();
    }

    return setError();
  }

  bool Parser::fillValue(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    if (_mem.it != _mem.end) {
      std::string val;
      char vwrap = '\0';

      if (*_mem.it == '"' || *_mem.it == '\'') vwrap = *_mem.it;
      shiftChar();

      while (_mem.it != _mem.end) {
        if (*_mem.it == vwrap) {
          // found the value wrapper closer
          static_cast<Tag*>(_mem.pointer)->setValue(_mem.key, val);

          // reinit
          val = "";
          _mem.key = "";
          shiftChar();

          while (_mem.it != _mem.end) {
            if (isSpace()) shiftChar();
            else {
              // each character and callback pair
              for (mt::CR<CHCB> chCb : chsCbs) {
                if (*_mem.it == chCb.first) {
                  shiftChar();
                  return setNextTest(chCb.second);
                }
              }

              // space for key
              if (isAlphabet()) return setNextTest(attKeyCb);
              // forbidden character for name
              return setError();
            }
          }
        }

        val += *_mem.it;
        shiftChar();
      }
    }

    return setError();
  }

  bool Parser::fillName(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    *_mem.strptr = "";

    if (_mem.it != _mem.end && isAlphabet()) {
      bool spaced = false;

      while (_mem.it != _mem.end) {
        if (isSpace()) spaced = true;
        else {
          for (mt::CR<CHCB> chCb : chsCbs) {
            if (*_mem.it == chCb.first) {
              shiftChar();
              return setNextTest(chCb.second);
            }
          }

          if (isForbiddenName()) break;
          else if (spaced) {
            return setNextTest(attKeyCb);
          }

          *_mem.strptr += *_mem.it;
        }

        shiftChar();
      }
    }

    return setError();
  }

  bool Parser::entity_ref() {
    bool detected = false;

    std::function<void(mt::CR_CH)> onSemicolon = [&](mt::CR_CH oriCh) {
      shiftChar();

      if (*_mem.it == ';') {
        *_mem.strptr += oriCh;
        detected = true;
      }
    };

    switch (*_mem.it) {
      case 'a': { // amp or apos
        shiftChar();

        switch (*_mem.it) {
          case 'm': { // amp
            shiftChar();

            switch (*_mem.it) {
              case 'p': onSemicolon('&'); break;
            }
          } break;
          case 'p': { // apos
            shiftChar();

            switch (*_mem.it) {
              case 'o': {
                shiftChar();

                switch (*_mem.it) {
                  case 's': onSemicolon('\''); break;
                }
              } break;
            }
          } break;
        }
      } break;
      case 'g': { // gt
        shiftChar();

        switch (*_mem.it) {
          case 't': onSemicolon('>'); break;
        }
      } break;
      case 'l': { // lt
        shiftChar();

        switch (*_mem.it) {
          case 't': onSemicolon('<'); break;
        }
      } break;
      case 'q': { // quot
        shiftChar();

        switch (*_mem.it) {
          case 'u': {
            shiftChar();

            switch (*_mem.it) {
              case 'o': {
                shiftChar();

                switch (*_mem.it) {
                  case 't': onSemicolon('"'); break;
                }
              } break;
            }
          } break;
        }
      } break;
    }

    if (detected) return continueTest();
    return setError();
  }

  /** DECLARATION TESTS */

  bool Parser::decla_name() {
    _mem.strptr = &_mem.name;
    _mem.status = Phase::DECLA_NAME;
    _mem.pointer->addChild(new Declaration());
    _mem.pointer = _mem.pointer->children()->prev();
    return fillName({{'?', decla_off}}, decla_att_key);
  }

  bool Parser::decla_att_key() {
    _mem.strptr = &_mem.key;
    _mem.status = Phase::DECLA_ATT_KEY;
    static_cast<Tag*>(_mem.pointer)->setValue("type", _mem.name);
    return fillName({{'=', decla_att_val}}, decla_att_key);
  }

  bool Parser::decla_att_val() {
    _mem.status = Phase::DECLA_ATT_VAL;
    return fillValue({{'?', decla_off}}, decla_att_key);
  }

  bool Parser::decla_off() {
    _mem.status = Phase::DECLA_OFF;
    return charMultiline({{'>', decla_set}});
  }

  bool Parser::decla_set() {

    // multiple or invalid position of declaration
    if (_mem.pointer->parent() != _mem.root) {
      return setError();
    }

    static_cast<Declaration*>(_mem.pointer)->version.parse(
      static_cast<Tag*>(_mem.pointer)->getValue("version")
    );

    _mem.pointer->setCompleted();
    return setNextTest(any_mark);
  }

  /** OPEN TAG TESTS */

  bool Parser::tag_name() {
    Tag *tag = new Tag();

    if (_mem.pointer->isCompleted()) {      
      _mem.pointer->connect(tag);
    }
    else _mem.pointer->addChild(tag);

    _mem.pointer = tag;
    _mem.strptr = &_mem.name;
    _mem.status = Phase::TAG_NAME;

    return fillName({
      {'>', tag_off},
      {'/', tag_over}
    }, tag_att_key);
  }

  bool Parser::tag_att_key() {
    _mem.strptr = &_mem.key;
    _mem.status = Phase::TAG_ATT_KEY;
    return fillName({{'=', tag_att_val}}, tag_att_key);
  }

  bool Parser::tag_att_val() {
    _mem.status = Phase::TAG_ATT_VAL;

    return fillValue({{
      {'>', tag_off},
      {'/', tag_over}
    }}, tag_att_key);
  }

  bool Parser::tag_over() {
    _mem.status = Phase::TAG_OVER;
    _mem.pointer->setName(_mem.name);

    return charMultiline({{'>', []()->bool {
      _mem.pointer->setCompleted();
      return setNextTest(any_mark);
    }}});
  }

  bool Parser::tag_off() {
    _mem.status = Phase::TAG_OFF;
    _mem.pointer->setName(_mem.name);
    return textMultiline({{'<', select_mark}});
  }

  /** CLOSE TAG TESTS */

  bool Parser::xtag_name() {
    _mem.status = Phase::XTAG_NAME;

    if (_mem.pointer->isCompleted()) {
      _mem.pointer = _mem.pointer->parent();
    }

    _mem.strptr = &_mem.name;
    return fillName({{'>', xtag_off}}, xtag_noatt_key);
  }

  bool Parser::xtag_noatt_key() {
    _mem.status = Phase::XTAG_NOATT_KEY;
    return setError();
  }

  bool Parser::xtag_off() {
    _mem.status = Phase::XTAG_OFF;

    if (_mem.pointer->getName() == _mem.name) {
      _mem.pointer->setCompleted();
      return setNextTest(any_mark);
    }

    return setError();
  }

  /** SELECTION TESTS */

  bool Parser::any_mark() {
    _mem.status = Phase::ANY_MARK;
    return charMultiline({{'<', select_mark}});
  }

  bool Parser::select_mark() {
    _mem.status = Phase::SELECT_MARK;

    // pattern types selection
    if (_mem.it != _mem.end) {
      if (isAlphabet()) {
        _mem.status = Phase::TAG_NAME;
        return setNextTest(tag_name);
      }
      else if (*_mem.it == '/') {
        shiftChar();
        _mem.status = Phase::XTAG_NAME;
        return setNextTest(xtag_name);
      }
      else if (*_mem.it == '!') {
        shiftChar();
        _mem.status = Phase::SELECT_EXCLA;
        return setNextTest(select_excla);
      }
      else if (*_mem.it == '?') {
        shiftChar();
        _mem.status = Phase::DECLA_MARK;
        return setNextTest(decla_name);
      }
    }
    // parsing completed
    else if (_mem.pointer->parent() == _mem.root) {
      return false;
    }

    return setError();
  }

  bool Parser::select_excla() {
    if (_mem.it != _mem.end) {
      if (*_mem.it == '-') {
        shiftChar();
        _mem.status = Phase::COMMENT_MARK;
        return setNextTest(comment_content);
      }
      else if (*_mem.it == '[') {
        shiftChar();
        _mem.status = Phase::CDATA_MARK;
        return setNextTest(cdata_mark);
      }
    }

    return setError();
  }

  /** CDATA TESTS */

  bool Parser::cdata_mark() {
    if (_mem.it != _mem.end) {
      if (*_mem.it == 'C') {
        enum alps {_c, _d, _a, _t, a_} phase = _c;
        shiftChar();

        while (_mem.it != _mem.end) {
          if (phase == _c && *_mem.it == 'D') {
            phase = _d;
          }
          else if (phase == _d && *_mem.it == 'A') {
            phase = _a;
          }
          else if (phase == _a && *_mem.it == 'T') {
            phase = _t;
          }
          else if (phase == _t && *_mem.it == 'A') {
            phase = a_;
          }
          else if (phase == a_ && *_mem.it == '[') {
            shiftChar();
            return setNextTest(cdata_content);
          }
          else return setError();

          shiftChar();
        }
      }
    }

    return setError();
  }

  bool Parser::cdata_content() {
    int offSqrCt = 0;
    std::string notOffSqr;
    CDATA *cdata = new CDATA();
    std::string cdtext;
    _mem.status = Phase::CDATA_CONTENT;

    if (_mem.pointer->isCompleted()) {
      _mem.pointer->connect(cdata);
    }
    else _mem.pointer->addChild(cdata);

    while (_mem.it != _mem.end) {
      if (offSqrCt == 0) {
        if (*_mem.it == ']') {
          offSqrCt++;
          notOffSqr += *_mem.it;
        }
        else cdtext += *_mem.it;
      }
      else if (offSqrCt == 1 && *_mem.it == ']') {
        offSqrCt++;
        notOffSqr += *_mem.it;
      }
      else if (offSqrCt == 2 && *_mem.it == '>') {

        // go to sub stream
        Member memOrigin = _mem;
        Node *cdroot = Parser::build(cdtext);

        // 'cdtext' treated as plain text
        if (_mem.errorOccurred) {
          cdata->setText(cdtext);
        }
        else { // parsed 'cdtext' added to main stream without '$ROOT'
          Node *cdsubroot = cdroot->children();
          cdsubroot->detach();
          cdata->addChild(cdsubroot);
        }

        // back to main stream
        cdroot->destroy();
        _mem = memOrigin;
        _mem.pointer->setCompleted();
        shiftChar();

        return setNextTest(xtag_name);
      }
      else {
        cdtext += notOffSqr;
        notOffSqr = "";
        offSqrCt = 0;
      }

      shiftChar();
    }

    return setError();
  }

  /** COMMENT TEST */

  bool Parser::comment_content() {
    if (_mem.it != _mem.end) {
      if (*_mem.it == '-') {
        int offHypCt = 0;
        std::string notOffHyp, comtext;
        Comment *comment = new Comment();
        _mem.status = Phase::COMMENT_CONTENT;

        if (_mem.pointer->isCompleted()) {
          _mem.pointer->connect(comment);
        }
        else _mem.pointer->addChild(comment);

        while (_mem.it != _mem.end) {
          if (offHypCt == 0) {
            if (*_mem.it == '-') {
              offHypCt++;
              notOffHyp += *_mem.it;
            }
            else comtext += *_mem.it;
          }
          else if (offHypCt == 1 && *_mem.it == '-') {
            offHypCt++;
            notOffHyp += *_mem.it;
          }
          else if (offHypCt == 2 && *_mem.it == '>') {
            shiftChar();
            comment->setText(comtext);
            return setNextTest(fill_fext);
          }
          else {
            comtext += notOffHyp;
            notOffHyp = "";
            offHypCt = 0;
          }

          shiftChar();
        }
      }
    }

    return setError();
  }

  /** INITIAL TEST */

  Node *Parser::build(mt::CR_STR raw) {
    _mem.create(raw);
    while (_mem.test[MAIN_TEST]()) {}
    return _mem.root;
  }

  Node *Parser::join(mt::CR_STR raw) {
    if (_mem.root) {
      _mem.refresh(raw);

      // iterator shifting is in scattered calls
      while (_mem.test[MAIN_TEST]()) {}

      // expected to return the parsed additional relative root
      if (_mem.errorOccurred) {
        return _mem.root;
      }

      return _mem.root->children()->prev();
    }

    return build(raw);
  }

  std::string Parser::getStatusMessage() {
    std::string represent;

    // whitespace character representation
    if (*_mem.it == '\0') represent = "\\0";
    else if (*_mem.it == ' ') represent = "\\s";
    else if (*_mem.it == '\n') represent = "\\n";
    else if (*_mem.it == '\r') represent = "\\r";
    else if (*_mem.it == '\t') represent = "\\t";
    else if (*_mem.it == '\f') represent = "\\f";
    else if (*_mem.it == '\v') represent = "\\v";
    else represent = *_mem.it;

    // latest created node name to display
    std::string createdName = "...";

    if (_mem.pointer && !_mem.pointer->getName().empty()) {
      createdName = _mem.pointer->getName();
    }

    // on testing node name to display
    std::string ongoingName = _mem.name.empty() ? "..." : _mem.name;

    return ( // no title
      std::string("char   : ") + represent + '\n' +
      "row    : " + std::to_string(_mem.point[ROW]) + '\n' +
      "col    : " + std::to_string(_mem.point[COL]) + '\n' +
      "phase  : " + Phase::labels[_mem.status] + '\n' +
      "created: " + createdName + '\n' +
      "ongoing: " + ongoingName + '\n'
    );
  }
}}

#endif // __KMZ_CPM__XML__PARSER_CPP__