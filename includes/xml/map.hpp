#ifndef __KMZ_CPM__XML__MAP_HPP__
#define __KMZ_CPM__XML__MAP_HPP__

#include "derivers.hpp"

namespace kmzcpm {
namespace xml {

  class Map final {
  private:
    inline static mt::UNORMAP_STR<mt::UNORSET<Tag*>> mapset;

  public:
    static bool has(mt::CR_STR key);
    static bool has(mt::CR_STR key, Tag *tag);
    static bool xhas(mt::CR_STR key, Tag *tag);
    static bool empty();
    static bool empty(mt::CR_STR key);
    static bool xempty(mt::CR_STR key);
    static void massreg(Tag *root);
    static void reg(mt::CR_STR key);
    static void reg(mt::CR_STR key, Tag *tag);
    static void xreg(mt::CR_STR key, Tag *tag);
    static void unreg(mt::CR_STR key);
    static void unreg(mt::CR_STR key, Tag *tag);
    static void xunreg(mt::CR_STR key, Tag *tag);
    static size_t numof();
    static size_t numof(mt::CR_STR key);
    static size_t xnumof(mt::CR_STR key);
    static size_t total();
  };
}}

#endif // __KMZ_CPM__XML__MAP_HPP__