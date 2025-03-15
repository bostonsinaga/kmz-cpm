#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POINT_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POINT_H__

#include "../../types.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class Point {
  protected:
    double x = 0, y = 0;

  public:
    typedef const Point& CR_PT;
    double X() { return x; }
    double Y() { return y; }
    void X(CR_DBL val) { x = val; }
    void Y(CR_DBL val) { y = val; }

    Point() {}

    Point(CR_DBL num) {
      x = num; y = num;
    }

    Point(CR_DBL x_in, CR_DBL y_in) {
      x = x_in; y = y_in;
    }

    double gradient();
    double gradientTo(CR_PT pt);

    double hypotenuseZero();
    double hypotenuseTo(CR_PT pt);

    Point operator+(CR_PT pt);
    Point operator+(CR_DBL num);

    Point operator-(CR_PT pt);
    Point operator-(CR_DBL num);

    Point operator*(CR_PT pt);
    Point operator*(CR_DBL num);

    Point operator/(CR_PT pt);
    Point operator/(CR_DBL num);

    void operator+=(CR_PT pt);
    void operator+=(CR_DBL num);

    void operator-=(CR_PT pt);
    void operator-=(CR_DBL num);

    void operator*=(CR_PT pt);
    void operator*=(CR_DBL num);

    void operator/=(CR_PT pt);
    void operator/=(CR_DBL num);

    bool operator==(CR_PT pt) const;
    bool operator==(CR_DBL num) const;

    bool operator!=(CR_PT pt);
    bool operator!=(CR_DBL num);

    bool operator<(CR_PT pt);
    bool operator<(CR_DBL num);

    bool operator<=(CR_PT pt);
    bool operator<=(CR_DBL num);

    bool operator>(CR_PT pt);
    bool operator>(CR_DBL num);

    bool operator>=(CR_PT pt);
    bool operator>=(CR_DBL num);

  private:
    Point divideCopy(CR_PT pt);
    Point divideCopy(CR_DBL num);
    void divideSelf(CR_PT pt);
    void divideSelf(CR_DBL num);
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POINT_H__