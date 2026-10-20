#ifndef __KMZ_CPM__XML__NODE_HPP__
#define __KMZ_CPM__XML__NODE_HPP__

#include "mini-tools.hpp"

namespace kmzcpm {
namespace xml {

  class Node {
  public:
    enum COMPASS { NORTH, EAST, SOUTH, WEST };
    enum DIRECTION { LEFT = 3, RIGHT = 1 };

    typedef std::function<bool(Node*)> CALLBACK;

  private:
    size_t depth = 0;
    Node *quad[4] { nullptr, nullptr, nullptr, nullptr };
    void init();

  protected:
    std::string name, text;
    ~Node() {}

  public:
    Node() { init(); }
    Node(mt::CR_STR name_in) : name(name_in) { init(); }
    Node(mt::CR_STR name_in, mt::CR_STR text_in) : name(name_in), text(text_in) { init(); }

    /** GETTERS */

    inline Node *prev() const { return quad[WEST]; }
    inline Node *next() const { return quad[EAST]; }
    inline Node *parent() const { return quad[NORTH]; }
    inline Node *children() const { return quad[SOUTH]; }
    inline bool alone() const { return prev() == this; }

    Node *root();

    std::string stringify();
    std::string getValue(mt::CR_STR key);
    std::string getName() const { return name; }
    std::string getText() const { return text; }
    size_t getDepth() const { return depth; }

    virtual std::string ostringy() { return ""; }
    virtual std::string cstringy() { return ""; }

    /** SETTERS */

    void connect(Node *node);
    void setParent(Node *node);
    void addChild(Node *node);
    void cleanChildren();

    virtual void setName(mt::CR_STR newName) {
      name = newName;
    }

    void setText(mt::CR_STR str) {
      cleanChildren();
      text = str;
    }

    /** ITERATIONS */

    void forEach(
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
    void destroy();
    void annihilate(mt::CR<DIRECTION> direction = LEFT);
  };
}}

#endif // __KMZ_CPM__XML__NODE_HPP__