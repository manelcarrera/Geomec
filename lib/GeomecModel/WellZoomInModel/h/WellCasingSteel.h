#ifndef _WELLCASINGSTEEL_H_
#define _WELLCASINGSTEEL_H_

#include "IMaterialServerTempl.h"
#include "MaterialServerParent.h"
#include "WellCasingSteelMaterial.h"
#include "WellCasingMesh.h"

class CWellCasingModel;

class CWellCasingMaterialServer;
typedef CMaterialServerParent<COpenGLNode, CWellCasingMaterialServer> TWellCasingMaterialServerParent;

class CWellCasingSteelMaterial;

class CWellCasingMaterialServer : public IMaterialServerTempl<CWellCasingSteelMaterial, TWellCasingMaterialServerParent>
{
  typedef IMaterialServerTempl<CWellCasingSteelMaterial, TWellCasingMaterialServerParent> TBase;

public:
  CWellCasingMaterialServer(TWellCasingMaterialServerParent& parent, const CDepletionStage& stage);
  const CFFMaterial& Material(const geo::IElement& element) const;

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingMaterialServer);

protected:
  virtual CWellCasingSteelMaterial* FindIndexInEntry(int nIndex) const;
};

class CWellCasingSteel : public TWellCasingMaterialServerParent
{
  typedef TWellCasingMaterialServerParent TBase;

public:
  CWellCasingSteel(const QString& strName, CWellCasingModel& model);
  CWellCasingSteel(CWellCasingModel& model);

  bool operator==(const CWellCasingSteel& rhs) const;
  CWellCasingSteel& operator=(const CWellCasingSteel& rhs);

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

  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingSteel);

private:
  mutable CElementCenterPointCache m_centerpoints;
  bool m_bShowMidpoints;
  TColor m_color;
};

#endif // _WELLCASINGSTEEL_H_
