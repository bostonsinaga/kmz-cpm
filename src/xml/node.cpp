#ifndef __KMZ_CPM__XML__NODE_CPP__
#define __KMZ_CPM__XML__NODE_CPP__

#include "xml/node.hpp"
#include <stack>

namespace kmzcpm {
namespace xml {

  Node *Node::root() {
    Node *topmost = this;
    while (topmost->parent()) topmost = topmost->parent();
    return topmost;
  }

  std::string Node::uniname() {
    std::string uname;

    for (mt::CR_STR nm : names) {
      uname += nm + ':';
    }

    if (!uname.empty()) uname.pop_back();
    return uname;
  }

  std::string Node::value(mt::CR_STR key) {
    if (attributes.find(key) != attributes.end()) {
      return attributes[key];
    }
    return "";
  }

  CDATA Node::valuec(mt::CR_STR key) {
    if (cattributes.find(key) != cattributes.end()) {
      return cattributes[key];
    }
    return CDATA();
  }

  void Node::init() {
    quad[WEST] = this;
    quad[EAST] = this;
  }

  void Node::connect(Node *node) {
    if (node) {
      Node *tail = node->quad[WEST];
      node->quad[WEST] = prev();
      tail->quad[EAST] = this;
      prev()->quad[EAST] = node;
      quad[WEST] = tail;
    }
  }

  void Node::parent(Node *node) {
    if (node) {
      node->children(this);

      foreach(RIGHT, [&](Node *current)->bool {
        current->quad[NORTH] = node;
        return true;
      });
    }
  }

  void Node::children(Node *node) {
    if (node) {
      if (children()) children()->connect(node);
      else quad[SOUTH] = node;
      node->quad[NORTH] = this;
    }
  }

  void Node::foreach(
    mt::CR<DIRECTION> direction,
    mt::CR<CALLBACK> callback
  ) {
    Node *current = this;

    do {
      if (!callback(current)) break;
      current = current->quad[direction];
    }
    while (current != this);
  }

  void Node::traverse(
    mt::CR<DIRECTION> direction,
    mt::CR<CALLBACK> callback
  ) {
    bool stopped = false;
    std::stack<mt::PAIR<Node*>> loaded;
    loaded.push({this, this});

    while (!loaded.empty()) {
      if (loaded.top().second->children()) {
        loaded.push({
          loaded.top().second->children(),
          loaded.top().second->children()
        });
      }
      else {
        if (!callback(loaded.top().second)) break;
        loaded.top().second = loaded.top().second->quad[direction];

        while (loaded.top().first == loaded.top().second) {
          loaded.pop();

          if (loaded.empty()) break;
          else {
            if (!callback(loaded.top().second)) {
              stopped = true;
              break;
            }

            loaded.top().second = loaded.top().second->quad[direction];
          }
        }
      }

      if (stopped) break;
    }
  }

  void Node::bubble(mt::CR<CALLBACK> callback) {
    Node *current = this;

    do {
      if (!callback(current)) break;
      current = current->parent();
    }
    while (current->parent());
  }

  void Node::detach() {
    if (parent()) {
      if (alone()) {
        parent()->quad[SOUTH] = nullptr;
      }
      else if (parent()->children() == this) {
        parent()->quad[SOUTH] = next();
      }
    }

    next()->quad[WEST] = prev();
    prev()->quad[EAST] = next();
    init();
  }

  void Node::destroy(mt::CR_BOL detached) {
    if (children()) {
      children()->traverse(
        RIGHT, [](Node * current)->bool {
          delete current;
          return true;
        }
      );
    }

    if (detached) detach();
    delete this;
  }

  void Node::annihilate(mt::CR<DIRECTION> direction) {
    quad[direction]->foreach(direction, [](Node *node)->bool {
      node->destroy(false);
      return true;
    });
  }
}}

#endif // __KMZ_CPM__XML__NODE_CPP__