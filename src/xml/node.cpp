#ifndef __KMZ_CPM__XML__NODE_CPP__
#define __KMZ_CPM__XML__NODE_CPP__

#include "xml/node.hpp"

namespace kmzcpm {
namespace xml {

  Node::Node() {
    prev(this);
    next(this);
  }

  Node::Node(mt::CR_STR str) {
    name = str;
    prev(this);
    next(this);
  }

  ~Node::Node() {
    Node *buffer = prev();
    prev()->next(next());
    next()->prev(buffer);
  }

  Node *root() {
    Node *current = this;
    while (current->parent()) current = current->parent();
    return current;
  }

  std::string Node::value(mt::CR_STR key) {
    if (attributes.find(key) != attributes.end()) {
      return attributes[key];
    }
    return "";
  }

  void Node::next(Node *node) {
    if (node) {
      if (node->alone()) {
        
      }
    }
  }

  void Node::prev(Node *node) {

  }

  void Node::parent(Node *node) {

  }

  void Node::children(Node *node) {

  }

  void Node::foreach(
    mt::CR<HORIZON> horizon,
    mt::CR<std::function<bool(Node*)>> callback
  ) {

  }

  void Node::traverse(mt::CR<std::function<bool(Node*)>> callback) {
    Node *current = this;

    do {
      if (!callback(current)) break;
      current = current->parent();
    }
    while (current->parent());
  }

  void Node::bubble(mt::CR<std::function<bool(Node*)>> callback) {
    Node *current = this;

    do {
      if (!callback(current)) break;
      current = current->parent();
    }
    while (current->parent());
  }

  void Node::release() {
    
  }
}}

#endif // __KMZ_CPM__XML__NODE_CPP__