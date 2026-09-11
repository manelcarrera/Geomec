// HorizonBase.h: interface for the CHorizonBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HORIZONBASE_H__C5E6672D_322F_43FA_B81B_5E50FAF92C22__INCLUDED_)
#define AFX_HORIZONBASE_H__C5E6672D_322F_43FA_B81B_5E50FAF92C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFaultPressure;
class CCohesionQuantity;
class CAngleQuantity;
class CDepletionStage;
class CFaultParametersNode;

namespace geo {
  class CElementGroup;
}

#include "colornode.h"
#include "IDCElementProperty.h"
#include "SingleQuantity.h"
#include "GeomecModelVisitor.h"

//This is the base class for horizon model horizons and gocad 
//type horizons.
//##ModelId=3D633E640039
class CHorizonBase : public CColorNode
{
public:
  typedef std::set<dia::IMaterial*, dia::IMaterial::CMaterialLess> TMaterialSet;
  // enumeration used when the horizon acts as a fault. It can either be
  // complete stick, complete slip or user defined.
  typedef enum eSlipType{ STICK = 0, SLIP, USER, FRACTURE } TSlipType;

  CHorizonBase(CFemAppModel& model);
  CHorizonBase(const QString& strInstanceName, CFemAppModel& model, bool bAttachToEntry = true);
  CHorizonBase(const CHorizonBase &rhs);
  virtual ~CHorizonBase();

  TSlipType SlipType() const;
  void SlipType(TSlipType slip_type);

  void Slip(bool bSlip);
  bool Slip() const;

  virtual bool DoubleSidedFault() const;
  virtual bool IgnoreFault() const;

  // ONLY valid when slipping!!
  const CFaultPressure& Pressure(const CDepletionStage& stage) const;
  CFaultPressure& Pressure(const CDepletionStage& stage);

  virtual const geo::CElementGroup* InterfaceElementGroup() const { return 0; }

  // determines whether this horizon can be marked as slipping, must be implemented by derived class
  virtual bool CanSlip() const { assert(false); return false; }

  // Assignment
  bool operator==(const CHorizonBase& rhs) const;
  CHorizonBase& operator=(const CHorizonBase& rhs);

  virtual bool Destroy();

  virtual void OnNeighbourModified(const CGraphNode &item, enum ModifiedHint uHint);
  virtual void OnNeighbourDeleted(const CGraphNode &item);
  virtual void OnNewNeighbour(const CGraphNode &node);

  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  CFaultParametersNode& FaultParameters(const CDepletionStage& stage);
  const CFaultParametersNode& FaultParameters(const CDepletionStage& stage) const;
  bool canHaveFaultParameters(const CDepletionStage& stage) const;
  bool hasFaultParameters(const CDepletionStage& stage) const;
  void UpdateFaultParameters();

  virtual unsigned int IconId() const;

  // Save and load
  virtual long SavedItems() const;

  // Stream
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  
  virtual bool Less(const CGraphNode &node) const;

  // materials
  virtual const dia::IMaterial &InterfaceMaterial(const geo::IElement &element, const CDepletionStage& stage) const;
  void ClearMaterials();

  void LinkToEntry();


#ifdef _DEBUG
  virtual void AssertValid() const;
#endif //_DEBUG

  void PrepareForAnalysis();

  ACCEPT_GEOMECMODELVISITORS(VisitHorizonBase);

protected:
  mutable TMaterialSet m_stMaterials;

  virtual double CalculateAverageDStif() const = 0;
  const double& AverageDStif() const;
  virtual void OnSlipToggled();

private:
  void CreatePressures();

private:
  // fault members ************************************************************************ //
  bool m_bSlip;
  TSlipType m_SlipType;

  typedef std::map<const CDepletionStage*, CFaultParametersNode*> TFaultParametersMap;
  TFaultParametersMap m_mpFaultParameters;

  typedef std::map<const CDepletionStage*, CFaultPressure*> TPressureMap;
  TPressureMap m_mpPressure;
  double m_dAverageDStif;
  // *************************************************************************************** //
};

typedef CGraphEntryTemp<CHorizonBase> THorizonBaseEntry;

#endif // !defined(AFX_HORIZONBASE_H__C5E6672D_322F_43FA_B81B_5E50FAF92C22__INCLUDED_)
