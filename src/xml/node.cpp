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

  std::string Node::stringify() {
    std::string ostr, cstr;

    traverse(RIGHT, [&](Node* current)->bool {
      ostr += current->ostringy();
      cstr += current->cstringy();
      return true;
    });

    return ostr + cstr;
  }

  void Node::init() {
    if (children()) {
      children()->traverse(RIGHT, [&](Node *current)->bool {
        current->depth -= depth;
        return true;
      });
    }

    depth = 0;
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
      node->quad[NORTH] = quad[NORTH];
      node->depth = depth;
    }
  }

  void Node::setParent(Node *node) {
    if (node) {
      node->addChild(this);

      forEach(RIGHT, [&](Node *current)->bool {
        current->quad[NORTH] = node;
        return true;
      });
    }
  }

  void Node::addChild(Node *node) {
    if (node) {
      if (children()) children()->connect(node);
      else {
        quad[SOUTH] = node;
        node->quad[NORTH] = this;
        node->depth = depth + 1;
      }
    }
  }

  void Node::cleanChildren() {
    if (children()) {
      children()->annihilate();
      quad[NORTH] = nullptr;
      quad[EAST] = nullptr;
      quad[SOUTH] = nullptr;
      quad[WEST] = nullptr;
    }
  }

  void Node::forEach(
    mt::CR<DIRECTION> direction,
    mt::CR<CALLBACK> callback
  ) {
    if (!callback) return;
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
    if (!callback) return;

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
    if (!callback) return;
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

  void Node::destroy() {
    if (children()) {
      children()->traverse(
        RIGHT, [](Node *current)->bool {
          delete current;
          return true;
        }
      );
    }

    delete this;
  }

  void Node::annihilate(mt::CR<DIRECTION> direction) {
    quad[direction]->forEach(direction, [](Node *node)->bool {
      node->destroy();
      return true;
    });
  }
}}

#endif // __KMZ_CPM__XML__NODE_CPP__