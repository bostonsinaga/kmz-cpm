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
      ostr = current->ostringy() + ostr;
      cstr += current->cstringy();
      return true;
    });

    return ostr + cstr;
  }

  void Node::redepths() {
    if (children()) {
      children()->traverse(RIGHT, [&](Node *current)->bool {
        current->depth -= depth;
        return true;
      });
    }
  }

  void Node::init() {
    redepths();
    depth = 0;
    quad[WEST] = this;
    quad[EAST] = this;
  }

  void Node::connect(Node *node) {
    if (node) {
      node->quad[NORTH] = parent();
      node->depth = depth;
      node->quad[WEST] = prev();
      prev()->quad[EAST] = node;
      quad[WEST] = node;
      node->quad[EAST] = this;
    }
  }

  void Node::connectList(Node *list) {
    if (list) {
      list->forEach(RIGHT, [&](Node *current)->bool {
        current->quad[NORTH] = parent();
        current->depth = depth;
        return true;
      });

      Node *tail = list->prev();
      list->quad[WEST] = prev();
      prev()->quad[EAST] = list;
      quad[WEST] = tail;
      tail->quad[EAST] = this;
    }
  }

  void Node::insert(Node *node) {
    if (node) {
      node->quad[NORTH] = parent();
      node->depth = depth;
      node->quad[EAST] = next();
      next()->quad[WEST] = node;
      node->quad[WEST] = this;
      quad[EAST] = node;
    }
  }

  void Node::insertList(Node *list) {
    if (list) {
      list->forEach(RIGHT, [&](Node *current)->bool {
        current->quad[NORTH] = parent();
        current->depth = depth;
        return true;
      });

      list->prev()->quad[EAST] = next();
      next()->quad[WEST] = list->prev();
      list->quad[WEST] = this;
      quad[EAST] = list;
    }
  }

  void Node::setParent(Node *node) {
    if (node) node->addChild(this);
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

  void Node::addChildren(Node *list) {
    if (list) {
      if (children()) children()->connectList(list);
      else {
        quad[SOUTH] = list;

        list->forEach(RIGHT, [&](Node *current)->bool {
          current->quad[NORTH] = this;
          current->depth = depth + 1;
          return true;
        });
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

    bool continued = false;
    std::stack<mt::PAIR<Node*>> loaded;
    loaded.push({this, this});

    while (!loaded.empty()) {
      // go to children
      if (loaded.top().second->children()) {
        loaded.push({
          loaded.top().second->children(),
          loaded.top().second->children()
        });
      }
      // at leaf
      else {
        // call callback up to root
        while (!loaded.empty() && callback(loaded.top().second)) {

          // shift to neighbor
          loaded.top().second = loaded.top().second->quad[direction];

          // back to start position
          if (loaded.top().second == loaded.top().first) {
            loaded.pop();
          }
          else { // go to neighbor
            continued = true;
            break;
          }
        }

        // go to neighbor
        if (continued) continued = false;
        // callback error or the 'loaded' is already empty
        else break;
      }
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

  void Node::release() {
    if (parent()) parent()->quad[SOUTH] = nullptr;

    forEach(RIGHT, [](Node *current)->bool {
      current->quad[NORTH] = nullptr;
      current->redepths();
      return true;
    });
  }

  void Node::annihilate(mt::CR<DIRECTION> direction) {
    mt::VEC<Node*> deletions;

    traverse(
      direction, [&](Node *current)->bool {
        deletions.push_back(current);
        return true;
      }
    );

    for (Node *current : deletions) {
      delete current;
    }
  }

  void Node::destroy() {
    detach();
    annihilate();
  }
}}

#endif // __KMZ_CPM__XML__NODE_CPP__