#ifndef _WELLCASINGCEMENTINTERFACE_H_
#define _WELLCASINGCEMENTINTERFACE_H_

#include "IMaterialServerTempl.h"
#include "MaterialServerParent.h"
#include "InterfaceMaterial.h"
#include "WellCasingMesh.h"

class CWellCasingModel;
class COpenGLNode;

class CWellCasingCementInterfaceMaterialServer;
typedef CMaterialServerParent<COpenGLNode, CWellCasingCementInterfaceMaterialServer> TWellCasingCementInterfaceMaterialServerParent;

class CWellCasingCementInterfaceMaterialServer : public IMaterialServerTempl<CInterfaceMaterial, TWellCasingCementInterfaceMaterialServerParent>
{
  typedef IMaterialServerTempl<CInterfaceMaterial, TWellCasingCementInterfaceMaterialServerParent> TBase;

public:
  CWellCasingCementInterfaceMaterialServer(TWellCasingCementInterfaceMaterialServerParent& parent, const CDepletionStage& stage);

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingCementInterfaceMaterialServer);

protected:
  virtual CInterfaceMaterial* FindIndexInEntry(int nIndex) const;
};

class CWellCasingCementInterface : public TWellCasingCementInterfaceMaterialServerParent
{
  typedef TWellCasingCementInterfaceMaterialServerParent TBase;

public:
  typedef enum eSlipType{ STICK = 0, SLIP, USER } TSlipType;

public:
  CWellCasingCementInterface(const QString& strName, CWellCasingModel& model);
  CWellCasingCementInterface(CWellCasingModel& model);

  bool operator==(const CWellCasingCementInterface& rhs) const;
  CWellCasingCementInterface& operator=(const CWellCasingCementInterface& rhs);

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;
  virtual bool Less(const CGraphNode& node) const;

  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  virtual bool Empty() const;
  virtual long SavedItems() const;

  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;
  virtual TColor Color() const;
  void Color(TColor nColor);

  void OnShowElements();
  void OnShowMidpoints();

  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  TSlipType SlipType() const;
  void SlipType(TSlipType sliptype);

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingCementInterface);

private:
  mutable CElementCenterPointCache m_centerpoints;
  bool m_bShowMidpoints;
  TColor m_color;
  TSlipType m_sliptype;
};

#endif // _WELLCASINGCEMENTINTERFACE_H_
