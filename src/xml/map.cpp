#ifndef __KMZ_CPM__XML__MAP_CPP__
#define __KMZ_CPM__XML__MAP_CPP__

#include "xml/map.hpp"

namespace kmzcpm {
namespace xml {

  bool Map::has(mt::CR_STR key) {
    return mapset.find(key) != mapset.end();
  }

  bool Map::has(mt::CR_STR key, Node *node) {
    if (has(key)) {
      return mapset[key].find(node) != mapset[key].end();
    }
    return false;
  }

  bool Map::xhas(mt::CR_STR key, Node *node) {
    return mapset[key].find(node) != mapset[key].end();
  }

  bool Map::empty() {
    return mapset.empty();
  }

  bool Map::empty(mt::CR_STR key) {
    if (has(key)) return mapset[key].empty();
    return false;
  }

  bool Map::xempty(mt::CR_STR key) {
    return mapset[key].empty();
  }

  void Map::massreg(Node *root) {
    if (root) {
      root->traverse(Node::RIGHT, [](Node *current)->bool {
        mapset[current->getName()].insert(current);
        return true;
      });
    }
  }

  void Map::reg(mt::CR_STR key) {
    mapset[key] = {};
  }

  void Map::reg(mt::CR_STR key, Node *node) {
    if (has(key)) mapset[key].insert(node);
  }

  void Map::xreg(mt::CR_STR key, Node *node) {
    mapset[key].insert(node);
  }

  void Map::unreg(mt::CR_STR key) {
    mapset.erase(key);
  }

  void Map::unreg(mt::CR_STR key, Node *node) {
    if (has(key)) mapset[key].erase(node);
  }

  void Map::xunreg(mt::CR_STR key, Node *node) {
    mapset[key].erase(node);
  }

  size_t Map::numof() {
    return mapset.size();
  }

  size_t Map::numof(mt::CR_STR key) {
    if (has(key)) return mapset[key].size();
    return 0;
  }

  size_t Map::xnumof(mt::CR_STR key) {
    return mapset[key].size();
  }

  size_t Map::total() {
    size_t ct = 0;

    for (const auto& [key, val] : mapset) {
      ct += val.size();
    }

    return ct;
  }
}}

#endif // __KMZ_CPM__XML__MAP_CPP__