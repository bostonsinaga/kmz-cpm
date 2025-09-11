#ifndef __KMZ_CPM__KML__READ_HPP__
#define __KMZ_CPM__KML__READ_HPP__

#include "mini-tools.hpp"

namespace kmz_cpm {
namespace kml {

  class Read final {
  private:
    static void kml(mt::CR_STR filename);
    static void kmz(mt::CR_STR filename);

  public:
    static bool tree(mt::CR_STR filename);
  };
}}

#endif // __KMZ_CPM__KML__READ_HPP__