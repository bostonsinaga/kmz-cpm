#ifndef __KMZ_CPM__XML__MAP_CPP__
#define __KMZ_CPM__XML__MAP_CPP__

#include "xml/map.hpp"

namespace kmzcpm {
namespace xml {

  bool Map::has(mt::CR_STR key) {
    return mapset.find(key) != mapset.end();
  }

  bool Map::has(mt::CR_STR key, Tag *tag) {
    if (has(key)) {
      return mapset[key].find(tag) != mapset[key].end();
    }
    return false;
  }

  bool Map::xhas(mt::CR_STR key, Tag *tag) {
    return mapset[key].find(tag) != mapset[key].end();
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

  void Map::massreg(Tag *root) {
    if (root) {
      root->traverse(Node::RIGHT, [](Node *current)->bool {

        if (current->getType() == TagType) {
          mapset[current->getName()].insert(static_cast<Tag*>(current));
        }

        return true;
      });
    }
  }

  void Map::reg(mt::CR_STR key) {
    mapset[key] = {};
  }

  void Map::reg(mt::CR_STR key, Tag *tag) {
    if (has(key)) mapset[key].insert(tag);
  }

  void Map::xreg(mt::CR_STR key, Tag *tag) {
    mapset[key].insert(tag);
  }

  void Map::unreg(mt::CR_STR key) {
    mapset.erase(key);
  }

  void Map::unreg(mt::CR_STR key, Tag *tag) {
    if (has(key)) mapset[key].erase(tag);
  }

  void Map::xunreg(mt::CR_STR key, Tag *tag) {
    mapset[key].erase(tag);
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

    for (const auto& [key, tag] : mapset) {
      ct += tag.size();
    }

    return ct;
  }
}}

#endif // __KMZ_CPM__XML__MAP_CPP__