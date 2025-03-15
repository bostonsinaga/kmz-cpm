#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__

#include <vector>
#include "polygon.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class RayCasting {
  public:
    static bool checkCollide();
    static bool checkIntersect();

    static bool checkInside(
      Point point,
      Polygon &polygon,
      double step
    );

    static bool checkInside(
      Point point,
      Polygon &&polygon,
      double step
    );
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_H__