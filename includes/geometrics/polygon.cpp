#ifndef __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_CPP__
#define __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_CPP__

#include "polygon.h"
#include "../../utils/vec-tools.h"

namespace mini_tools {
namespace algorithms {
namespace geometrics {
  //______|
  // RECT |
  //______|

  void Polygon::createRect(Point *anchor) {
    if (vertices.size() == 0) return;

    farMin = vertices[0];
    farMax = vertices[0];

    for (Point &pt : vertices) {

      if (pt.X() < farMin.X()) {
        farMin.X(pt.X());
      }
      else if (pt.X() > farMax.X()) {
        farMax.X(pt.X());
      }

      if (pt.Y() < farMin.Y()) {
        farMin.Y(pt.Y());
      }
      else if (pt.Y() > farMax.Y()) {
        farMax.Y(pt.Y());
      }
    }

    rect = Rect(
      (farMin.X() + farMax.X()) / 2,
      (farMin.Y() + farMax.Y()) / 2,
      farMax.X() - farMin.X(),
      farMax.Y() - farMin.Y()
    );

    if (anchor) rect.setAnchor(*anchor);
  }

  void Polygon::setPosition(Point pt) {
    Point diff = pt - rect.getPosition();
    rect.setPosition(pt);
    for (Point &v : vertices) v += diff;
  }

  void Polygon::setPosition(double x, double y) {
    setPosition(Point(x, y));
  }

  //_________|
  //VERTICES |
  //_________|

  bool Polygon::withinVertices(int idx) {
    if (idx >= 0 && idx < vertices.size()) {
      return true;
    }
    else return false;
  }

  void Polygon::editVertices(int idx, Point pt) {
    if (withinVertices(idx) && vertices[idx] != pt) {
      vertices[idx] = pt;
      createRect();
    }
  }

  void Polygon::insertVertices(int idx, Point pt) {
    if (idx < 0) idx = 0;
    else if (idx >= vertices.size()) idx = vertices.size() - 1;
    vertices.insert(vertices.begin() + idx, pt);
  }

  void Polygon::setVertices(
    std::vector<Point> &pts,
    Point anchor
  ) {
    vertices = pts;
    createRect(&anchor);
  }

  void Polygon::setVertices(
    std::vector<double> &vec_x,
    std::vector<double> &vec_y,
    Point anchor
  ) {
    int size = vec_x.size();
    if (vec_y.size() < size) size = vec_y.size();
    vertices = {};

    for (int i = 0; i < size; i++) {
      vertices.push_back(
        Point(vec_x[i], vec_y[i])
      );
    }

    createRect(&anchor);
  }

  Point Polygon::removeVertices(int idx) {
    Point wasted;
    int prevCount = verticesCount();

    utils::VecTools<Point>::cutSingle(vertices, wasted, idx);

    if (verticesCount() < prevCount) createRect();
    return wasted;
  }

  std::vector<Point> Polygon::removeVertices(int start, int end) {
    std::vector<Point> wasted;

    utils::VecTools<Point>::cutInterval(vertices, wasted, start, end);

    if (wasted.size() > 0) createRect();
    return wasted;
  }

  std::vector<Point> Polygon::removeVertices(std::vector<int> &idxes) {
    std::vector<Point> wasted;

    utils::VecTools<Point>::cutIndexes(vertices, wasted, idxes);

    if (wasted.size() > 0) createRect();
    return wasted;
  }

  Point Polygon::getVertice(int idx) {
    if (withinVertices(idx)) {
      return vertices[idx];
    }
    else return Point(0, 0);
  }
}}}

#endif // __MINI_TOOLS__ALGORITHMS__GEOMETRICS__POLYGON_CPP__