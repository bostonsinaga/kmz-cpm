#ifndef __KMZ_CPM__XML__PARSER_HPP__
#define __KMZ_CPM__XML__PARSER_HPP__

#include "xml/node.hpp"

namespace kmzcpm {
namespace xml {

  class Parser final {
  private:
    typedef std::function<bool()> TEST_CALLBACK;
    typedef mt::PAIR2<char, TEST_CALLBACK> CHCB;
    inline static std::string::const_iterator _it, _end;

    class Parnode : public Node {
    private:
      bool completed = false;

    public:
      Parnode() : Node() {}
      Parnode(mt::CR_STR name_in) : Node({name_in}) {}
      bool isCompleted() const { return completed; }
      void setCompleted() { completed = true; }
    };

    inline static Parnode
      *_root = nullptr,
      *_subroot = nullptr,
      *_pointer = nullptr;

    enum { MAIN_TEST, STOPBY_TEST };
    enum { ROW, COL };

    inline static TEST_CALLBACK _test[] {nullptr, nullptr};
    inline static size_t _point[] {1, 1};
    inline static mt::PAIR_STR _name_pair;
    inline static std::string _key, _text;
    inline static std::string *_strptr;
    inline static bool _errorOccurred = false;

    static mt::ARR_BOL<256> createForbiddenNameCharTable();
    static mt::ARR_BOL<256> createWhitespaceCharTable();
    static mt::ARR_BOL<256> IS_FORBIDDEN_NAME_CHAR, IS_WHITESPACE_CHAR;

    static void shiftChar();
    static void cancelNode();
    static bool trackNewline();
    static bool isAlphabet();
    static bool isSpace();
    static bool setError();
    static bool continueTest();
    static bool setNextTest(mt::CR<TEST_CALLBACK> cb);
    static bool stopByTest(mt::CR<TEST_CALLBACK> cb);
    static bool checkMultiline(mt::CR_VEC<CHCB> chsCbs);
    static bool fillText();

    static bool fillValue(
      mt::CR_VEC<CHCB> chsCbs,
      mt::CR<TEST_CALLBACK> attKeyCb
    );

    static bool fillName(
      mt::CR_VEC<CHCB> chsCbs,
      mt::CR<TEST_CALLBACK> attKeyCb
    );

    static mt::VEC_STR splitNamespaces(mt::CR_STR name);

    /** TESTS */

    static bool entity_ref();
    static bool decla_name();
    static bool decla_att_key();
    static bool decla_att_value();
    static bool decla_close();
    static bool decla_set();
    static bool tag_open_name();
    static bool tag_att_key();
    static bool tag_att_value();
    static bool tag_close();
    static bool offer_names();
    static bool xtag_open_name();
    static bool xtag_no_att_key();
    static bool xtag_close();
    static bool any_open();
    static bool select_open();
    static bool excla_select();
    static bool cdata_open();
    static bool cdata_inner_close();
    static bool comment_inner_close();

    struct Phase {
      enum TYPE_CODE {
        TYPE_ZERO, DECLA, TAG,
        CDATA, COMMENT, INNER_STRING
      };

      enum PART_CODE {
        PART_ZERO, ENTITY_REF,
        NAME, TEXT,
        KEY, VALUE
      };

      inline static constexpr std::string type_labels[] {
        "TYPE_ZERO", "DECLA", "TAG",
        "CDATA", "COMMENT", "INNER_STRING"
      };

      inline static constexpr std::string part_labels[] {
        "PART_ZERO", "ENTITY_REF",
        "NAME", "TEXT",
        "KEY", "VALUE"
      };

      inline static TYPE_CODE type = TYPE_ZERO;
      inline static PART_CODE part = PART_ZERO;
    };

  public:
    Parser() = delete;
    static Node *scan(mt::CR_STR raw);
    static std::string getErrorMessage();
  };
}}

#endif // __KMZ_CPM__XML__PARSER_HPP__