#ifndef __KMZ_CPM__XML__NODE_HPP__
#define __KMZ_CPM__XML__NODE_HPP__

#include "mini-tools.hpp"

namespace kmzcpm {
namespace xml {

  class Node;

  class CDATA {
  public:
    Node *innerNode;
    std::string innerText;
    std::string stringify();
  };

  class Node {
  public:
    enum DIRECTION { UP, DOWN, LEFT, RIGHT };
    enum VERTICAL { V_UP, V_DOWN };
    enum HORIZONTAL { H_LEFT = 2, H_RIGHT = 3 };

  private:
    std::string name;
    mt::UNORMAP_STR<std::string> attributes;
    Node *around[4] { nullptr, nullptr, nullptr, nullptr };
    ~Node();

  public:
    Node();
    Node(mt::CR_STR str);

    /** GETTERS */

    inline Node *prev() const { return around[Left()]; }
    inline Node *next() const { return around[Right()]; }
    inline Node *parent() const { return around[Up()]; }
    inline Node *children() const { return around[Down()]; }
    inline std::string tag() const { return name; }
    inline bool alone() const { return prev() == this; }
    std::string value(mt::CR_STR key);
    Node *root();

    /** SETTERS */

    void next(Node *node);
    void prev(Node *node);
    void parent(Node *node);
    void children(Node *node);
    void tag(mt::CR_STR str) { name = str; }

    void foreach(
      mt::CR<HORIZON> horizon,
      mt::CR<std::function<bool(Node*)>> callback
    );

    void traverse(mt::CR<std::function<bool(Node*)>> callback);
    void bubble(mt::CR<std::function<bool(Node*)>> callback);

    void remove() { delete this; }
    void release();

    void value(mt::CR_STR key, mt::CR_STR str) {
      attributes[key] = str;
    }

    void value(mt::CR_STR key, mt::CR<CDATA> cdata) {
      attributes[key] = cdata.stringify();
    }
  };
}}

#endif // __KMZ_CPM__XML__NODE_HPP__