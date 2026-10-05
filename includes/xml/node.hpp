#ifndef __KMZ_CPM__XML__NODE_HPP__
#define __KMZ_CPM__XML__NODE_HPP__

#include "mini-tools.hpp"

namespace kmzcpm {
namespace xml {

  class Node;
  class CDATA;
  class Comment;

  struct Declaration {
    std::string type, encoding;

    struct Version {
      int major = 0, minor = 0;

      std::string stringify() {
        return std::to_string(major) + '.' + std::to_string(minor);
      }
    } version;

    std::string stringify() {
      return "";
    }
  };

  enum NodeType {
    BaseType, CDATAType, CommentType
  };

  class Node {
  public:
    enum COMPASS { NORTH, EAST, SOUTH, WEST };
    enum DIRECTION { LEFT = 3, RIGHT = 1 };

    typedef std::function<bool(Node*)> CALLBACK;

  private:
    size_t depth = 0;
    mt::UNORMAP_STR<std::string> attributes;
    Node *quad[4] { nullptr, nullptr, nullptr, nullptr };

  protected:
    NodeType type;
    std::string text;

    ~Node() {}
    virtual void init();

  public:
    mt::VEC_STR names {""};
    inline static Declaration declaration;

    Node() { init(); }
    Node(mt::CR_STR name_in) : names({name_in}) { init(); }
    Node(mt::CR_STR name_in, mt::CR_STR text_in) : names({name_in}), text(text_in) { init(); }
    Node(mt::CR_VEC_STR names_in) : names(names_in) { init(); }
    Node(mt::CR_VEC_STR names_in, mt::CR_STR text_in) : names(names_in), text(text_in) { init(); }

    /** GETTERS */

    inline Node *prev() const { return quad[WEST]; }
    inline Node *next() const { return quad[EAST]; }
    inline Node *parent() const { return quad[NORTH]; }
    inline Node *children() const { return quad[SOUTH]; }
    inline bool alone() const { return prev() == this; }

    Node *root();

    std::string uniname();
    std::string stringify();
    std::string getValue(mt::CR_STR key);
    std::string getText() { return text; }
    size_t getDepth() { return depth; }

    virtual std::string ostringy();
    virtual std::string cstringy();

    /** SETTERS */

    void connect(Node *node);
    void setParent(Node *node);
    void addChild(Node *node);
    void cleanChildren();

    void setValue(mt::CR_STR key, mt::CR_STR str) {
      attributes[key] = str;
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

  class CDATA : public Node {
  private:
    void init() override;
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };

  class Comment : public Node {
  private:
    void init() override;
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };
}}

#endif // __KMZ_CPM__XML__NODE_HPP__