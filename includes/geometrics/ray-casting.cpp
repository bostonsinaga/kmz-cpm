#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_CPP__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_CPP__

#include <cmath>
#include "ray-casting.h"
#include "../../utils/vec-tools.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  bool RayCasting::checkInside(
    Point point,
    Polygon &polygon,
    double step
  ) {
    if (step <= 0 || !polygon.getRect().contains(point)) {
      return false;
    }

    int last = polygon.verticesCount() - 1,
        catchCount = 0;

    bool lastPair = false;
    std::vector<Point> laserIntersections;

    /** Load 'laserIntersections' */

    for (int i = 0; i < last; i++) {

      // next index
      int j;
      if (lastPair) { i = last; j = 0; }
      else j = i + 1;

      Point currentVer = polygon.getVertice(i),
            nextVer = polygon.getVertice(j);

      if ((point.Y() < currentVer.Y() && point.Y() < nextVer.Y()) ||
        (point.Y() > currentVer.Y() && point.Y() > nextVer.Y())
      ) {
        continue; // only for potential segments
      }

      // nearly horizontal line
      if (currentVer.X() - nextVer.X() != 0) {

        double gradient = currentVer.gradientTo(nextVer);

        // skip point with zero gradient
        if (gradient != 0) {
          double constant = currentVer.X() - currentVer.Y() / gradient;

          laserIntersections.push_back(Point(
            (point.Y() - constant) / gradient,
            point.Y()
          ));
        }
      }
      // infinite gradient (vertical line)
      else {
        laserIntersections.push_back(Point(
          currentVer.X(), point.Y()
        ));
      }

      // back to first index once after last index
      if (lastPair) break;
      else if (i == last - 1) {
        lastPair = true;
        i = 0;
      }
    }

    /** Check 'laserIntersections' */

    utils::VecTools<Point>::cleanDuplicateInside(laserIntersections);

    for (int i = 0; i < laserIntersections.size(); i++) {

      Rect verRect = Rect(laserIntersections[i], Size(step));

      for (
        double x = polygon.getFarMin().X();
        x <= point.X();
        x += step
      ) {
        if (verRect.contains(Point(x, point.Y()))) {
          catchCount++;
        }
      }
    }

    // even is outside
    if (catchCount % 2 == 0) return false;
    // uneven is inside
    return true;
  }

  bool RayCasting::checkInside(
    Point point,
    Polygon &&polygon,
    double step
  ) {
    return checkInside(point, polygon, step);
  }
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__RAY_CASTING_CPP_