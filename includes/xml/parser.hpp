#ifndef __KMZ_CPM__XML__PARSER_HPP__
#define __KMZ_CPM__XML__PARSER_HPP__

#include "xml/map.hpp"

namespace kmzcpm {
namespace xml {

  class Parser final {
  private:
    typedef std::function<bool()> TEST_CALLBACK;
    typedef mt::PAIR2<char, TEST_CALLBACK> CHCB;

    enum { MAIN_TEST, STOPBY_TEST };
    enum { ROW, COL };

    struct Phase {
      enum CODE {
        // free text
        ZERO, FILL_TEXT,
        // declaration
        DECLA_NAME, DECLA_ATT_KEY, DECLA_ATT_VAL, DECLA_OFF,
        // tag
        TAG_NAME, TAG_ATT_KEY, TAG_ATT_VAL, TAG_OVER, TAG_OFF,
        // xtag
        XTAG_NAME, XTAG_NOATT_KEY, XTAG_OFF,
        // free mark
        ANY_MARK, SELECT_MARK, SELECT_EXCLA,
        // cdata
        CDATA_MARK, CDATA_CONTENT,
        // commnet
        COMMENT_MARK, COMMENT_CONTENT
      };

      inline static constexpr std::string labels[21] {
        // free text
        "ZERO", "FILL_TEXT",
        // declaration
        "DECLA_NAME", "DECLA_ATT_KEY", "DECLA_ATT_VAL", "DECLA_OFF",
        // tag
        "TAG_NAME", "TAG_ATT_KEY", "TAG_ATT_VAL", "TAG_OVER", "TAG_OFF",
        // xtag
        "XTAG_NAME", "XTAG_NOATT_KEY", "XTAG_OFF",
        // free mark
        "ANY_MARK", "SELECT_MARK", "SELECT_EXCLA",
        // cdata
        "CDATA_MARK", "CDATA_CONTENT",
        // commnet
        "COMMENT_MARK", "COMMENT_CONTENT"
      };
    };

    // available for multiple versions
    struct Member {
      std::string::const_iterator it, end;
      std::string name, key, text;
      std::string *strptr = nullptr;

      size_t point[2] {1, 1};
      bool errorOccurred = false;

      Node *root = nullptr, *pointer = nullptr;
      TEST_CALLBACK test[2] {nullptr, nullptr};
      Phase::CODE status = Phase::ZERO;

      Member() {}
      void init(mt::CR_STR raw);
      void refresh(mt::CR_STR raw);
    };

    inline static Member _mem {};

    static mt::ARR_BOL<256> createForbiddenNameCharTable();
    static mt::ARR_BOL<256> createWhitespaceCharTable();
    static mt::ARR_BOL<256> IS_FORBIDDEN_NAME_CHAR, IS_WHITESPACE_CHAR;

    static void shiftChar();
    static bool isAlphabet();
    static bool isForbiddenName();
    static bool isSpace();
    static bool setError();
    static bool continueTest();
    static bool setNextTest(mt::CR<TEST_CALLBACK> cb);
    static bool stopByTest(mt::CR<TEST_CALLBACK> cb);
    static bool endMultiline();
    static bool charMultiline(mt::CR_VEC<CHCB> chsCbs);
    static bool textMultiline(mt::CR_VEC<CHCB> chsCbs);
    static bool fill_fext();

    static bool fillValue(
      mt::CR_VEC<CHCB> chsCbs,
      mt::CR<TEST_CALLBACK> attKeyCb
    );

    static bool fillName(
      mt::CR_VEC<CHCB> chsCbs,
      mt::CR<TEST_CALLBACK> attKeyCb
    );

    /** TESTS */

    static bool entity_ref();
    static bool decla_name();
    static bool decla_att_key();
    static bool decla_att_val();
    static bool decla_off();
    static bool decla_set();
    static bool tag_name();
    static bool tag_att_key();
    static bool tag_att_val();
    static bool tag_over();
    static bool tag_off();
    static bool xtag_name();
    static bool xtag_noatt_key();
    static bool xtag_off();
    static bool any_mark();
    static bool select_mark();
    static bool select_excla();
    static bool cdata_mark();
    static bool cdata_content();
    static bool comment_content();

  public:
    Parser() = delete;
    static Node *build(mt::CR_STR raw);
    static Node *join(mt::CR_STR raw);

    /**
     * Undetected pattern (root has no children)
     * or invalid pattern is considered error.
     */
    static std::string getStatusMessage();
    static bool isError() { return _mem.errorOccurred; }
  };
}}

#endif // __KMZ_CPM__XML__PARSER_HPP__