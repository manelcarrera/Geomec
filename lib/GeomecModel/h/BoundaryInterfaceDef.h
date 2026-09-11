#ifndef _BOUNDARYINTERFACEDEF_H_
#define _BOUNDARYINTERFACEDEF_H_

class CBoundaryBase;
class CBoundaryInterfaceMaterial;
class CSinglePressure;
class CStressGradientQuantity;

#include "StorageNode.h"
#include "SingleQuantity.h"
#include "SinglePressure.h"
#include "StressGradientQuantity.h"
#include "GeomecModelVisitor.h"

namespace geo
{
class CInterfaceElement;
}

class CBoundaryInterfaceDef : public CStorageNode
{
public:
  enum eIntMatType { G = 0, K, L }; // how is the interface material defined, Shear modulus G or
                                    // via the Krad and Ktan values

  typedef enum
  {
    BSURF_TOP = 0,
    BSURF_SIDE,
    BSURF_BOTTOM
  } TBoundarySurface;

public:
  CBoundaryInterfaceDef(CBoundaryBase& boundary);
  CBoundaryInterfaceDef(const CBoundaryInterfaceDef& rhs);
  virtual ~CBoundaryInterfaceDef();

  bool operator==(const CBoundaryInterfaceDef& rhs) const;
  CBoundaryInterfaceDef& operator=(const CBoundaryInterfaceDef& rhs);

  inline eIntMatType IntMatType() const { return m_eIntMatType; }
  void IntMatType(eIntMatType val);

  inline const CSinglePressure &Shear() const { return m_ShearQuantity; }
  void Shear(const double &val, CDoubleQuantity::UNIT unit);

  inline double Lambda() const { return m_lambda; }
  void Lambda(double val);

  inline const CStressGradientQuantity &KradTop() const { return m_KradTop; }
  void KradTop(const double &val, CDoubleQuantity::UNIT unit);
  inline const CStressGradientQuantity &KradBottom() const { return m_KradBottom; }
  void KradBottom(const double &val, CDoubleQuantity::UNIT unit);
  inline const CStressGradientQuantity &KradSides() const { return m_KradSides; }
  void KradSides(const double &val, CDoubleQuantity::UNIT unit);

  inline const CStressGradientQuantity &KtanTop() const { return m_KtanTop; }
  void KtanTop(const double &val, CDoubleQuantity::UNIT unit);
  inline const CStressGradientQuantity &KtanBottom() const { return m_KtanBottom; }
  void KtanBottom(const double &val, CDoubleQuantity::UNIT unit);
  inline const CStressGradientQuantity &KtanSides() const { return m_KtanSides; }
  void KtanSides(const double &val, CDoubleQuantity::UNIT unit);

  // K values based on G value (equal for all sides)
  double Krad(CDoubleQuantity::UNIT unit) const;
  double Ktan(CDoubleQuantity::UNIT unit) const;

  const CBoundaryInterfaceMaterial& InterfaceMaterialTop(const geo::CInterfaceElement& iface) const;
  const CBoundaryInterfaceMaterial& InterfaceMaterialBottom(const geo::CInterfaceElement& iface) const;
  const CBoundaryInterfaceMaterial& InterfaceMaterialSides(const geo::CInterfaceElement& iface) const;

  void ClearInterfaceMaterials(); // is called when the OK button in the interface attribute dialog

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual bool Empty() const;
  virtual long SavedItems() const;

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitBoundaryInterfaceDef);

private:
  void GenerateInterfaceMaterials() const;
  CBoundaryInterfaceMaterial& FindOrCreateInterfaceMaterial(const geo::CInterfaceElement& iface, bool isBottom = false) const;

private:
  CBoundaryBase& m_boundary;
  CSinglePressure m_ShearQuantity;
  double                  m_lambda;
  CStressGradientQuantity m_KradTop;
  CStressGradientQuantity m_KradBottom;
  CStressGradientQuantity m_KradSides;
  CStressGradientQuantity m_KtanTop;
  CStressGradientQuantity m_KtanBottom;
  CStressGradientQuantity m_KtanSides;

  mutable std::map<const geo::CInterfaceElement *, CBoundaryInterfaceMaterial *> m_materials;

  eIntMatType m_eIntMatType;
  mutable CBoundaryInterfaceMaterial *m_pBoundIntMatTop;
  mutable CBoundaryInterfaceMaterial *m_pBoundIntMatBottom;
  mutable CBoundaryInterfaceMaterial *m_pBoundIntMatSides;
  mutable bool m_updateLastSideInterfaceElement;
  mutable const geo::CInterfaceElement *m_lastSideInterfaceElement;
};

#endif // _BOUNDARYINTERFACEDEF_H_
