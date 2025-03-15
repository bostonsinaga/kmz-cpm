#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_H__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_H__

#include <vector>
#include "rect.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {

  class Polygon {
  private:
    Rect rect;
    std::vector<Point> vertices;
    Point farMin, farMax;

    void createRect(Point *anc = nullptr);
    bool withinVertices(int idx);

  public:
    Polygon() {}

    Polygon(
      std::vector<Point> &pts,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(pts, anc);
    }

    Polygon(
      std::vector<Point> &&pts,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(pts, anc);
    }

    Polygon(
      std::vector<double> &vec_x,
      std::vector<double> &vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    Polygon(
      std::vector<double> &&vec_x,
      std::vector<double> &vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    Polygon(
      std::vector<double> &vec_x,
      std::vector<double> &&vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    Polygon(
      std::vector<double> &&vec_x,
      std::vector<double> &&vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    /** RECT */

    void setPosition(Point pt);
    void setPosition(double x, double y);

    void setAnchor(Point pt) { rect.setAnchor(pt); }
    void setAnchor(double x, double y) { rect.setAnchor(Point(x, y)); }

    Point getPosition() { return rect.getPosition(); }
    Point getAnchor() { return rect.getAnchor(); }
    Size getSize() { return rect.getSize(); }

    Rect getRect() { return rect; }  // returns the copy
    Point getFarMin() { return farMin; }
    Point getFarMax() { return farMax; }

    /** VERTICES */

    void editVertices(int idx, Point pt);
    void insertVertices(int idx, Point pt);

    void setVertices(
      std::vector<Point> &pts,
      Point anc = Point(0.5, 0.5)
    );

    void setVertices(
      std::vector<Point> &&pts,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(pts, anc);
    }

    void setVertices(
      std::vector<double> &vec_x,
      std::vector<double> &vec_y,
      Point anc = Point(0.5, 0.5)
    );

    void setVertices(
      std::vector<double> &&vec_x,
      std::vector<double> &vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    void setVertices(
      std::vector<double> &vec_x,
      std::vector<double> &&vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    void setVertices(
      std::vector<double> &&vec_x,
      std::vector<double> &&vec_y,
      Point anc = Point(0.5, 0.5)
    ) {
      setVertices(vec_x, vec_y, anc);
    }

    Point removeVertices(int idx);
    std::vector<Point> removeVertices(int start, int end);

    std::vector<Point> removeVertices(std::vector<int> &idxes);
    std::vector<Point> removeVertices(std::vector<int> &&idxes) {
      return removeVertices(idxes);
    }

    Point getVertice(int idx);
    std::vector<Point> getVertices() { return vertices; }  // returns the copy
    int verticesCount() { return vertices.size(); }
  };
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_H__