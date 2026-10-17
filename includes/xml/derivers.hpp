#ifndef __KMZ_CPM__XML__DERIVERS_HPP__
#define __KMZ_CPM__XML__DERIVERS_HPP__

#include "node.hpp"

namespace kmzcpm {
namespace xml {

  class Tag : public Node {
  private:
    mt::VEC_STR namespaces;
    bool completed = false;
    void splitNamespaces();

  public:
    void setName(mt::CR_STR newName) override;
    size_t numofNamespaces() const { return namespaces.size(); }
    mt::VEC_STR getNamespaces() const { return namespaces; }

    bool isCompleted() const { return completed; }
    void setCompleted() { completed = true; }
  };

  class CDATA : public Node {
  private:
    void init() override;
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };

  class Comment : public Node {
  private:
    void init() override;
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };
}}

#endif // __KMZ_CPM__XML__DERIVERS_HPP__