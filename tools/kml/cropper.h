#ifndef __KML_CROPPER_H__
#define __KML_CROPPER_H__

#include "placemark.h"

namespace kml {

  /**
   * Cropping by rectangular area
   * from start to end point.
   */
  class Cropper {
  public:
    Cropper() {}
    
    void printNotification(Menu &menu);
    
    std::vector<xml::Node*> cutPins(
      xml::Node *pinsContainerNode,
      Point startPt,  // decimal coordinate
      Point endPt,    // decimal coordinate
      bool isFolderInsertion,
      bool isIncludeFolders,  // if true the 'isFolderInsertion' must be true
      bool isParentFolderNamedAType
    );
    
    std::vector<xml::Node*> cutPaths(
      xml::Node *pathsContainerNode,
      Point startPt,  // decimal coordinate
      Point endPt,    // decimal coordinate
      bool isFolderInsertion, 
      bool isIncludeFolders,  // if true the 'isFolderInsertion' must be true
      bool isParentFolderNamedAType
    );
    
    std::vector<xml::Node*> cutAll(
      xml::Node *placemarksContainerNode,
      Point startPt,  // decimal coordinate
      Point endPt,    // decimal coordinate
      bool isIncludeFolders
    );
    
  private:
    struct LineEquation {
      // 'm' is gradient and 'c' is constant
      LD m, c;
    };
    
    LineEquation produceLineEquation(Point &ptA, Point &ptB);
    
    /**
     * Choose what selection rect 'intersect' function will be used
     * and get the intersection point.
     */
    void obtainNewPointFromIntersection(
      Point &ptA,
      Point &ptB,
      LineEquation &linEq_AB,
      const std::function<void(Point&)> &callback
    );
    
    enum {
      IN_OUT_KEEPEDGEFLAG,
      OUT_IN_KEEPEDGEFLAG,
      OUT_OUT_KEEPEDGEFLAG
    };
    
    // add additional point at selection rect intersection (must intersected)
    void keepPathEdge(
      std::vector<Point> &outsiderPtVec,  // this two vectors should be filled except,
      std::vector<Point> &insiderPtVec,   // 'insiderPtVec' can be empty if only path beyonds selection rect but intersects it
      int keepEdgeFlag,
      LineEquation *linEq_AB_hooked = nullptr
    );
    
    // set selection rect points with statictly 'BL, TL, TR, BR' formation
    void setupSelectionRect(Point &startPt, Point &endPt);
    
    // 'ptA' and 'ptB' are path segment/couple points that beyond selection rect
    bool isSegmentIntersectsSelectionRect(
      Point &ptA,
      Point &ptB,
      LineEquation *linEq_AB_hook = nullptr
    );
    
    // selection rect points
    struct SelRect {
      Point ptArr[4];
      LineEquation linEqArr[4];
      std::function<bool(LineEquation&, Point*)> intersect[4];
    } selRect;
  };
}

#endif // __KML_CROPPER_H__