#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__SIZE_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__SIZE_H__

#include "point.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class Size : public Point {
  public:
    double W() { return x; }
    double H() { return y; }

    void W(double val) { x = val; }
    void H(double val) { y = val; }

    Size() : Point() {}
    Size(double side) : Point(side, side) {}
    Size(double w_in, double h_in) : Point(w_in, h_in) {}
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__SIZE_H__