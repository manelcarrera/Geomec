#ifndef _ValidateModel_h_
#define _ValidateModel_h_

class CModelBase;
class CTetraBoundary;
class CFormationBase;

namespace geo
{
  class CSurfaceDesc;
  class CVector;
  class ISurface;
}

#include <vector>

class CValidateModel
{
public:
  CValidateModel(CModelBase* modelBase);

  bool checkModel(bool rockMech) const;

  bool checkMesh() const;
  bool checkReservoir() const;
  bool checkExistenceDepletionStage() const;
  bool checkMaterial() const;
  bool checkMaterial(const CFormationBase &formation) const;
  bool checkValues() const;
  bool checkValues(const CFormationBase &formation) const;
  bool checkBoundaryConditions() const;
  bool checkNoEmptyFormations() const;
  bool checkNoEmptyFaults() const;

private:
  CValidateModel(const CValidateModel& rhs);
  CValidateModel& operator = (const CValidateModel& rhs);

  bool collectNormalVectors(const geo::CSurfaceDesc& Desc,
    std::vector <geo::CVector>& vcNormals, CTetraBoundary* pTetBound) const;
  void addNorm(const geo::ISurface& surf,
    std::vector <geo::CVector>& normvec) const;

  CModelBase* m_ModelBase;
};

#endif  // _ValidateModel_h_
