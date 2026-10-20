#ifndef __KMZ_CPM__XML__DERIVERS_HPP__
#define __KMZ_CPM__XML__DERIVERS_HPP__

#include "node.hpp"

namespace kmzcpm {
namespace xml {

  class Tag : public Node {
  protected:
    mt::UNORMAP_STR<std::string> attributes;

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

    std::string getValue(mt::CR_STR key);
    void setValue(mt::CR_STR key, mt::CR_STR str);

    std::string ostringy() override;
    std::string cstringy() override;
  };

  class Declaration : public Tag {
  public:
    struct Version {
      int major = 0, minor = 0;
      std::string stringify();
      void parse(mt::CR_STR verstr);
    } version;

    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };

  class CDATA : public Node {
  private:
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };

  class Comment : public Node {
  private:
    std::string ostringy() override;
    std::string cstringy() override { return ""; }
  };
}}

#endif // __KMZ_CPM__XML__DERIVERS_HPP__