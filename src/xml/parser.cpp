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
    if (!trackNewline()) _point[COL]++;
    _it++;
  }

  void Parser::cancelTag() {
    if (static_cast<Tag*>(_pointer)->isCompleted()) {

      Tag *containers[] = { static_cast<Tag*>(_pointer->parent()), nullptr };
      if (containers[0]) containers[1] = static_cast<Tag*>(containers[0]->parent());

      while (containers[1]) {
        if (!containers[0]->isCompleted() && containers[1]->isCompleted()) {
          containers[0]->detach();
          containers[0]->destroy();

          if (containers[1]->children()) {
            _pointer = containers[1]->children()->prev();
          }
          else _pointer = containers[1];

          break;
        }
        else {
          containers[0] = containers[1];
          containers[1] = static_cast<Tag*>(containers[1]->parent());
        }
      }
    }
    else {
      if (_pointer->alone()) {
        Node *container = _pointer->parent();
        _pointer->detach();
        _pointer->destroy();

        if (container) _pointer = container;
        else _pointer = nullptr;
      }
      else {
        Node *prev = _pointer->prev();
        _pointer->detach();
        _pointer->destroy();
        _pointer = static_cast<Tag*>(prev);
      }
    }
  }

  bool Parser::trackNewline() {
    if (*_it == '\n') {
      _point[ROW]++;
      _point[COL] = 1;
      return true;
    }
    return false;
  }

  bool Parser::isAlphabet() {
    return mt_uti::StrTool::isAlphabet(*_it) || *_it == '_';
  }

  bool Parser::isSpace() {
    return trackNewline() || IS_WHITESPACE_CHAR[static_cast<mt::UCH>(*_it)];
  }

  bool Parser::setError() {
    _errorOccurred = true;

    // only tag allowed
    if (Tag* tag = dynamic_cast<Tag*>(_pointer)) {
      cancelTag();
    }

    return false;
  }

  bool Parser::continueTest() {
    _test[MAIN_TEST] = _test[STOPBY_TEST];
    return true;
  }

  bool Parser::setNextTest(mt::CR<TEST_CALLBACK> cb) {
    _test[MAIN_TEST] = cb;
    return true;
  }

  bool Parser::stopByTest(mt::CR<TEST_CALLBACK> cb) {
    _test[STOPBY_TEST] = _test[MAIN_TEST];
    _test[MAIN_TEST] = cb;
    return true;
  }

  bool Parser::checkMultiline(mt::CR_VEC<CHCB> chsCbs) {

    // tolerates the spaces before the first target char detected
    while (_it != _end) {
      for (mt::CR<CHCB> chCb : chsCbs) {
        if (*_it == chCb.first) {
          shiftChar();
          return setNextTest(chCb.second);
        }
      }

      if (isSpace()) shiftChar();
      else return setError();
    }

    Tag* tag = dynamic_cast<Tag*>(_pointer);

    // single tag as child of root is completed
    if (tag && tag->isCompleted() &&
      tag->parent() == ROOT
    ) {
      return false;
    }

    return setError();
  }

  bool Parser::fill_fext() {
    Phase::status = Phase::FILL_TEXT;
    _text = "";

    while (_it != _end) {
      if (*_it == '<') {
        shiftChar();
        _pointer->setText(_text);
        return setNextTest(select_mark);
      }
      else if (*_it == '&') {
        shiftChar();
        _strptr = &_text;
        return stopByTest(entity_ref);
      }
      else if (*_it == '>') return setError();

      _text += *_it;
      shiftChar();
    }

    return setError();
  }

  bool Parser::fillValue(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    if (_it != _end) {
      std::string val;
      char vwrap = '\0';

      if (*_it == '"' || *_it == '\'') vwrap = *_it;
      shiftChar();

      while (_it != _end) {
        if (*_it == vwrap) {
          static_cast<Tag*>(_pointer)->setValue(_key, val);
          val = "";
          _key = "";
          shiftChar();

          while (_it != _end) {
            if (isSpace()) shiftChar();
            else {
              for (mt::CR<CHCB> chCb : chsCbs) {
                if (*_it == chCb.first) {
                  shiftChar();
                  return setNextTest(chCb.second);
                }
              }

              if (isAlphabet()) return setNextTest(attKeyCb);
              return setError();
            }
          }
        }

        val += *_it;
        shiftChar();
      }
    }

    return setError();
  }

  bool Parser::fillName(
    mt::CR_VEC<CHCB> chsCbs,
    mt::CR<TEST_CALLBACK> attKeyCb
  ) {
    *_strptr = "";

    if (_it != _end) {
      if (isAlphabet()) {
        bool spaced = false;

        while (_it != _end) {
          if (isSpace()) spaced = true;
          else {
            for (mt::CR<CHCB> chCb : chsCbs) {
              if (*_it == chCb.first) {
                shiftChar();
                return setNextTest(chCb.second);
              }
            }

            if (IS_FORBIDDEN_NAME_CHAR[static_cast<mt::UCH>(*_it)]) break;
            else if (spaced) {
              _key += *_it;
              shiftChar();
              return setNextTest(attKeyCb);
            }

            *_strptr += *_it;
          }

          shiftChar();
        }
      }
    }

    return setError();
  }

  bool Parser::entity_ref() {
    bool detected = false;

    std::function<void(mt::CR_CH)> onSemicolon = [&](mt::CR_CH oriCh) {
      shiftChar();

      if (*_it == ';') {
        *_strptr += oriCh;
        detected = true;
      }
    };

    switch (*_it) {
      case 'a': { // amp or apos
        shiftChar();

        switch (*_it) {
          case 'm': { // amp
            shiftChar();

            switch (*_it) {
              case 'p': onSemicolon('&'); break;
            }
          } break;
          case 'p': { // apos
            shiftChar();

            switch (*_it) {
              case 'o': {
                shiftChar();

                switch (*_it) {
                  case 's': onSemicolon('\''); break;
                }
              } break;
            }
          } break;
        }
      } break;
      case 'g': { // gt
        shiftChar();

        switch (*_it) {
          case 't': onSemicolon('>'); break;
        }
      } break;
      case 'l': { // lt
        shiftChar();

        switch (*_it) {
          case 't': onSemicolon('<'); break;
        }
      } break;
      case 'q': { // quot
        shiftChar();

        switch (*_it) {
          case 'u': {
            shiftChar();

            switch (*_it) {
              case 'o': {
                shiftChar();

                switch (*_it) {
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
    _strptr = &_name;
    Phase::status = Phase::DECLA_NAME;
    _pointer->addChild(new Declaration());
    _pointer = _pointer->children()->prev();
    return fillName({{'?', decla_off}}, decla_att_key);
  }

  bool Parser::decla_att_key() {
    _strptr = &_key;
    Phase::status = Phase::DECLA_ATT_KEY;
    static_cast<Tag*>(_pointer)->setValue("type", _name);
    return fillName({{'=', decla_att_val}}, decla_att_key);
  }

  bool Parser::decla_att_val() {
    Phase::status = Phase::DECLA_ATT_VAL;
    return fillValue({{'?', decla_off}}, decla_att_key);
  }

  bool Parser::decla_off() {
    Phase::status = Phase::DECLA_OFF;
    return checkMultiline({{'>', decla_set}});
  }

  bool Parser::decla_set() {

    // multiple or invalid position of declaration
    if (_pointer->parent() != ROOT) {
      return setError();
    }

    static_cast<Declaration*>(_pointer)->version.parse(
      static_cast<Tag*>(_pointer)->getValue("version")
    );

    static_cast<Declaration*>(_pointer)->setCompleted();
    return setNextTest(any_mark);
  }

  /** OPEN TAG TESTS */

  bool Parser::tag_name() {
    Tag *tag = new Tag();

    if (static_cast<Tag*>(_pointer)->isCompleted()) {      
      _pointer->connect(tag);
    }
    else _pointer->addChild(tag);

    _pointer = tag;
    _strptr = &_name;
    Phase::status = Phase::TAG_NAME;

    return fillName({
      {'>', tag_over},
      {'/', tag_off}
    }, tag_att_key);
  }

  bool Parser::tag_att_key() {
    _strptr = &_key;
    Phase::status = Phase::TAG_ATT_KEY;
    return fillName({{'=', tag_att_val}}, tag_att_key);
  }

  bool Parser::tag_att_val() {
    Phase::status = Phase::TAG_ATT_VAL;

    return fillValue({{
      {'>', tag_over},
      {'/', tag_off}
    }}, tag_att_key);
  }

  bool Parser::tag_off() {
    Phase::status = Phase::TAG_OFF;
    return checkMultiline({{'>', tag_over}});
  }

  bool Parser::tag_over() {
    Phase::status = Phase::TAG_OVER;
    _pointer->setName(_name);

    if (*_it == '<') {
      shiftChar();
      return setNextTest(select_mark);
    }

    return setNextTest(fill_fext);
  }

  /** CLOSE TAG TESTS */

  bool Parser::xtag_name() {
    Phase::status = Phase::XTAG_NAME;

    if (static_cast<Tag*>(_pointer)->isCompleted()) {
      _pointer = static_cast<Tag*>(_pointer->parent());
    }

    _strptr = &_name;
    return fillName({{'>', xtag_off}}, xtag_noatt_key);
  }

  bool Parser::xtag_noatt_key() {
    Phase::status = Phase::XTAG_NOATT_KEY;
    return setError();
  }

  bool Parser::xtag_off() {
    Phase::status = Phase::XTAG_OFF;

    if (_pointer->getName() == _name) {
      static_cast<Tag*>(_pointer)->setCompleted();
      return setNextTest(any_mark);
    }

    return setError();
  }

  /** SELECTION TESTS */

  bool Parser::any_mark() {
    Phase::status = Phase::ANY_MARK;
    return checkMultiline({{'<', select_mark}});
  }

  bool Parser::select_mark() {
    TEST_CALLBACK cb;
    Phase::status = Phase::SELECT_MARK;

    if (_it != _end) {
      if (isAlphabet()) {
        _name = *_it;
        cb = tag_name;
        Phase::status = Phase::TAG_NAME;
      }
      else if (*_it == '/') {
        cb = xtag_name;
        Phase::status = Phase::XTAG_NAME;
      }
      else if (*_it == '!') {
        cb = select_excla;
        Phase::status = Phase::SELECT_EXCLA;
      }
      else if (*_it == '?') {
        cb = decla_name;
        Phase::status = Phase::DECLA_MARK;
      }
      else return setError();

      shiftChar();
      return setNextTest(cb);
    }

    return setError();
  }

  bool Parser::select_excla() {
    if (_it != _end) {
      if (*_it == '-') {
        shiftChar();
        Phase::status = Phase::COMMENT_MARK;
        return setNextTest(comment_content);
      }
      else if (*_it == '[') {
        shiftChar();
        Phase::status = Phase::CDATA_MARK;
        return setNextTest(cdata_mark);
      }
    }

    return setError();
  }

  /** CDATA TESTS */

  bool Parser::cdata_mark() {
    if (_it != _end) {
      if (*_it == 'C') {
        enum alps {_c, _d, _a, _t, a_} phase = _c;

        while (_it != _end) {
          if (phase == _c && *_it == 'D') {
            phase = _d;
          }
          else if (phase == _d && *_it == 'A') {
            phase = _a;
          }
          else if (phase == _a && *_it == 'T') {
            phase = _t;
          }
          else if (phase == _t && *_it == 'A') {
            phase = a_;
          }
          else if (phase == a_ && *_it == '[') {
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
    Phase::status = Phase::CDATA_CONTENT;

    if (static_cast<Tag*>(_pointer)->isCompleted()) {
      _pointer->connect(cdata);
    }
    else _pointer->addChild(cdata);

    while (_it != _end) {
      if (offSqrCt == 0) {
        if (*_it == ']') {
          offSqrCt++;
          notOffSqr += *_it;
        }
        else {
          trackNewline();
          cdtext += *_it;
        }
      }
      else if (offSqrCt == 1 && *_it == ']') {
        offSqrCt++;
        notOffSqr += *_it;
      }
      else if (offSqrCt == 2 && *_it == '>') {
        Node *oriRoot = ROOT,
          *oriPtr = _pointer;

        Parser::scan(cdtext);

        if (_errorOccurred) {
          cdata->setText(cdtext);
          _errorOccurred = false;
        }
        else cdata->addChild(ROOT);

        shiftChar();
        ROOT = oriRoot;
        _pointer = oriPtr;
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
    if (_it != _end) {
      if (*_it == '-') {
        int offHypCt = 0;
        std::string notOffHyp, comtext;
        Comment *comment = new Comment();
        Phase::status = Phase::COMMENT_CONTENT;

        if (static_cast<Tag*>(_pointer)->isCompleted()) {
          _pointer->connect(comment);
        }
        else _pointer->addChild(comment);

        while (_it != _end) {
          if (offHypCt == 0) {
            if (*_it == '-') {
              offHypCt++;
              notOffHyp += *_it;
            }
            else {
              trackNewline();
              comtext += *_it;
            }
          }
          else if (offHypCt == 1 && *_it == '-') {
            offHypCt++;
            notOffHyp += *_it;
          }
          else if (offHypCt == 2 && *_it == '>') {
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

  /** INIT TEST */

  void Parser::scan(mt::CR_STR raw) {
    // iterators init
    _it = raw.begin();
    _end = raw.end();
    _test[MAIN_TEST] = any_mark;

    // nodes init
    ROOT = new Node("#ROOT");
    _pointer = ROOT;

    // iterator shifting is in scattered calls
    while (_test[MAIN_TEST]()) {}
  }

  std::string Parser::getStatusMessage() {
    std::string represent;

    if (*_it == '\0') represent = "\\0";
    else if (*_it == ' ') represent = "␣";
    else if (*_it == '\n') represent = "\\n";
    else if (*_it == '\r') represent = "\\r";
    else if (*_it == '\t') represent = "\\t";
    else if (*_it == '\f') represent = "\\f";
    else if (*_it == '\v') represent = "\\v";

    std::string openerName = "...", closerName = "...";

    if (_pointer && !_pointer->getName().empty()) {
      openerName = _pointer->getName();
    }

    if (!_name.empty()) closerName = _name;

    return "STATUS:\n" +
    std::string("char  : ") + represent + '\n' +
    "row   : " + std::to_string(_point[ROW]) + '\n' +
    "col   : " + std::to_string(_point[COL]) + '\n' +
    "phase : " + Phase::labels[Phase::status] + '\n' +
    "opener: " + openerName + '\n' +
    "closer: " + closerName + '\n';
  }
}}

#endif // __KMZ_CPM__XML__PARSER_CPP__