#ifndef __GEOTON__GEOMETRIC__POINT_HPP__
#define __GEOTON__GEOMETRIC__POINT_HPP__

#include "mini-tools.h"

namespace geoton {
namespace geometric {

  class Point : public LinkedList {
  protected:
    double x = 0, y = 0;

  public:
    typedef const Point& CR_PT;

    double X() const { return x; }
    double Y() const { return y; }

    void X(mt::CR_DBL val) { x = val; }
    void Y(mt::CR_DBL val) { y = val; }

    Point() {}

    Point(mt::CR_DBL num) {
      x = num; y = num;
    }

    Point(mt::CR_DBL x_in, mt::CR_DBL y_in) {
      x = x_in; y = y_in;
    }

    double gradient();
    double gradientTo(CR_PT pt);

    double hypotenuseZero();
    double hypotenuseTo(CR_PT pt);

    Point operator+(CR_PT pt);
    Point operator+(mt::CR_DBL num);

    Point operator-(CR_PT pt);
    Point operator-(mt::CR_DBL num);

    Point operator*(CR_PT pt);
    Point operator*(mt::CR_DBL num);

    Point operator/(CR_PT pt);
    Point operator/(mt::CR_DBL num);

    void operator+=(CR_PT pt);
    void operator+=(mt::CR_DBL num);

    void operator-=(CR_PT pt);
    void operator-=(mt::CR_DBL num);

    void operator*=(CR_PT pt);
    void operator*=(mt::CR_DBL num);

    void operator/=(CR_PT pt);
    void operator/=(mt::CR_DBL num);

    bool operator==(CR_PT pt) const;
    bool operator==(mt::CR_DBL num) const;

    bool operator!=(CR_PT pt);
    bool operator!=(mt::CR_DBL num);

    bool operator<(CR_PT pt);
    bool operator<(mt::CR_DBL num);

    bool operator<=(CR_PT pt);
    bool operator<=(mt::CR_DBL num);

    bool operator>(CR_PT pt);
    bool operator>(mt::CR_DBL num);

    bool operator>=(CR_PT pt);
    bool operator>=(mt::CR_DBL num);

  private:
    Point divideCopy(CR_PT pt);
    Point divideCopy(mt::CR_DBL num);
    void divideSelf(CR_PT pt);
    void divideSelf(mt::CR_DBL num);
  };
}}

#endif // __GEOTON__GEOMETRIC__POINT_HPP__