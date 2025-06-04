#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__

#include <vector>
#include "polygon.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class RayCasting {
  public:
    static bool collide(
      const Point &point,
      const Polygon &polygon,
      mt::CR_DBL step
    );

    static bool intersect(
      const Point &point,
      const Polygon &polygon,
      mt::CR_DBL step
    );

    static bool inside(
      const Point &point,
      const Polygon &polygon,
      mt::CR_DBL step
    );
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__