#ifndef _GeomecMaterialModel2RGI_h_
#define _GeomecMaterialModel2RGI_h_

class RGInterface;
class CModelBase;
class CGraphNode;

#include <vector>

namespace GeomecRGI
{

class CGeomecMaterialModel2RGI
{
  public:
    CGeomecMaterialModel2RGI(RGInterface& rgi, const CModelBase& modelBase);
    ~CGeomecMaterialModel2RGI();

    std::vector <int> getMaterialTypes() const;

  private:
    CGeomecMaterialModel2RGI(const CGeomecMaterialModel2RGI& rhs);
    CGeomecMaterialModel2RGI& operator = (const CGeomecMaterialModel2RGI& rhs);

    const CModelBase& m_modelBase;
    std::vector <CGraphNode*> m_formations;
    std::vector <int> m_materialModels;
    std::vector <int> m_materialTypes;
};

} // namespace GeomecRGI

#endif  // _GeomecMaterialModel2RGI_h_
