#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_H__

#include "size.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class Rect {
  private:
    Point position, anchor = Point(0.5, 0.5);
    Size size;

  public:
    Rect() {}

    Rect(
      Point position_in,
      Size size_in,
      Point anchor_in = Point(0.5, 0.5)
    );

    Rect(
      double x, double y,
      double w, double h,
      double anc_x = 0.5,
      double anc_y = 0.5
    );

    /** VALUES */

    Point getPosition() { return position; }
    Size getSize() { return size; }
    Point getAnchor() { return anchor; }
    void setPosition(Point pt) { position = pt; }
    void setSize(Size sz) { size = sz; }
    void setAnchor(Point pt) { anchor = pt; }

    double getLeft();
    double getRight();
    double getBottom();
    double getTop();

    /** RELATIONS */

    bool contains(Point pt);
    bool contains(Rect bb);
    bool inside(Rect bb);
    bool intersect(Rect bb);
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_H__