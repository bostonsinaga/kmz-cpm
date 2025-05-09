#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__

#include "placemark.h"

namespace kml {
  
  class Classifier {
    public:
    Classifier() {}
    
    void rearrange(
      xml::Node *kmlNode,
      bool isCleanFolders,
      bool isIncludeFolders
    );
    
    bool filterString(xml::Node *kmlNode, std::string searchStr);
  };
}

#endif // __CLASSIFIER_H__