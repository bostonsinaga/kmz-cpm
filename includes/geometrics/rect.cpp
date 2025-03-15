#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_CPP__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_CPP__

#include "rect.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  Rect::Rect(
    Point position_in,
    Size size_in,
    Point anchor_in
  ) {
    position = position_in;
    size = size_in;
    anchor = anchor_in;
  }

  Rect::Rect(
    double x, double y,
    double w, double h,
    double anc_x, double anc_y
  ) {
    position = Point(x, y);
    size = Size(w, h);
    anchor = Point(anc_x, anc_y);
  }

  /** VALUES */

  double Rect::getLeft() { return position.X() - size.W() * anchor.X(); }
  double Rect::getRight() { return position.X() + size.W() * (1 - anchor.X()); }
  double Rect::getBottom() { return position.Y() - size.H() * anchor.Y(); }
  double Rect::getTop() { return position.Y() + size.H() * (1 - anchor.Y()); }

  /** RELATIONS */

  bool Rect::contains(Point pt) {
    if (pt.X() >= getLeft() && pt.X() <= getRight() &&
      pt.Y() >= getBottom() && pt.Y() <= getTop()
    ) { return true; }
    return false;
  }

  bool Rect::contains(Rect bb) {
    if (bb.getLeft() >= getLeft() &&
      bb.getRight() <= getRight() &&
      bb.getBottom() >= getBottom() &&
      bb.getTop() <= getTop()
    ) { return true; }
    return false;
  }

  bool Rect::inside(Rect bb) {
    if (getLeft() >= bb.getLeft() &&
      getRight() <= bb.getRight() &&
      getBottom() >= bb.getBottom() &&
      getTop() <= bb.getTop()
    ) { return true; }
    return false;
  }

  bool Rect::intersect(Rect bb) {
    if ((
      (getLeft() >= bb.getLeft() &&
      getLeft() <= bb.getRight()) ||
      (getRight() >= bb.getLeft() &&
      getRight() <= bb.getRight())
    ) && (
      (getBottom() >= bb.getBottom() &&
      getBottom() <= bb.getTop()) ||
      (getTop() >= bb.getBottom() &&
      getTop() <= bb.getTop())
    )) {
      return true;
    }
    return false;
  }
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__BOUNDING_BOX_CPP__