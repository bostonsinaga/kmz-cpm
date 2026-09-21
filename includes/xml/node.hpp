#ifndef __KMZ_CPM__XML__NODE_HPP__
#define __KMZ_CPM__XML__NODE_HPP__

#include "mini-tools.hpp"

namespace kmzcpm {
namespace xml {

  class Node;

  struct Declaration {
    std::string type, encoding;

    struct Version {
      int major = 0, minor = 0;
    } version;
  };

  struct CDATA {
    Node *innerNode = nullptr;
    std::string innerText = "";
    std::string stringify() const { return ""; }
  };

  class Node {
  public:
    enum COMPASS { NORTH, EAST, SOUTH, WEST };
    enum DIRECTION { LEFT = 3, RIGHT = 1 };

    typedef std::function<bool(Node*)> CALLBACK;

  private:
    mt::UNORMAP_STR<std::string> attributes;
    mt::UNORMAP_STR<CDATA> cattributes;
    Node *quad[4] { nullptr, nullptr, nullptr, nullptr };

    ~Node() {}
    void init();

  public:
    inline static Declaration declaration;

    mt::VEC_STR names;
    std::string text;
    CDATA cdata;

    Node() { init(); }
    Node(mt::CR_STR name_in) : names({name_in}) { init(); };
    Node(mt::CR_STR name_in, mt::CR_STR text_in) : names({name_in}), text(text_in) { init(); };
    Node(mt::CR_VEC_STR names_in) : names(names_in) { init(); };
    Node(mt::CR_VEC_STR names_in, mt::CR_STR text_in) : names(names_in), text(text_in) { init(); };

    /** GETTERS */

    inline Node *prev() const { return quad[WEST]; }
    inline Node *next() const { return quad[EAST]; }
    inline Node *parent() const { return quad[NORTH]; }
    inline Node *children() const { return quad[SOUTH]; }
    inline bool alone() const { return prev() == this; }

    std::string uniname();
    std::string value(mt::CR_STR key);
    CDATA valuec(mt::CR_STR key);
    Node *root();

    /** SETTERS */

    void connect(Node *node);
    void parent(Node *node);
    void children(Node *node);

    void value(mt::CR_STR key, mt::CR_STR str) {
      attributes[key] = str;
    }

    void valuec(mt::CR_STR key, mt::CR<CDATA> cdt) {
      cattributes[key] = cdt;
    }

    /** ITERATIONS */

    void foreach(
      mt::CR<DIRECTION> direction,
      mt::CR<CALLBACK> callback
    );

    void traverse(
      mt::CR<DIRECTION> direction,
      mt::CR<CALLBACK> callback
    );

    void bubble(mt::CR<CALLBACK> callback);

    /** REMOVALS */

    void detach();
    void destroy(mt::CR_BOL detached = true);
    void annihilate(mt::CR<DIRECTION> direction = LEFT);
  };
}}

#endif // __KMZ_CPM__XML__NODE_HPP__