#ifndef __GEOTON__GEOMETRIC__SIZE_HPP__
#define __GEOTON__GEOMETRIC__SIZE_HPP__

#include "point.h"

namespace geoton {
namespace geometric {

  class Size : public Point {
  public:
    double W() const { return x; }
    double H() const { return y; }

    void W(mt::CR_DBL val) { x = val; }
    void H(mt::CR_DBL val) { y = val; }

    Size() : Point() {}
    Size(double side) : Point(side, side) {}
    Size(double w_in, double h_in) : Point(w_in, h_in) {}
  };
}}

#endif // __GEOTON__GEOMETRIC__SIZE_HPP__