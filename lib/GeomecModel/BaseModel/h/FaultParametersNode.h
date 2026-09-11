#ifndef _FAULTPARAMETERSNODE_H_
#define _FAULTPARAMETERSNODE_H_

class CHorizonBase;
class CDepletionStage;

#include "CohesionQuantity.h"
#include "AngleQuantity.h"
#include "StorageNode.h"
#include "GeomecModelVisitor.h"

class CValueType;

namespace geo {
  class IPoint;
  class IElement;
}

class CFaultParametersNode : public CStorageNode
{
public:
  CFaultParametersNode(CHorizonBase& fault, CDepletionStage& stage);
  CFaultParametersNode(const CFaultParametersNode& rhs);

  CFaultParametersNode& operator=(const CFaultParametersNode& rhs);
  bool operator==(const CFaultParametersNode& rhs) const;

  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint = Default);

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool CanDisconnectItem(const CGraphNode& item) const;

 	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  bool CohesionSameAsPrevious() const;
  bool FrictionSameAsPrevious() const;
  bool DStiffSameAsPrevious() const;
  void CohesionSameAsPrevious(bool b);
  void FrictionSameAsPrevious(bool b);
  void DStiffSameAsPrevious(bool b);

	// Constant settings for cohesion and friction and dstiff
	const CCohesionQuantity& Cohesion() const;
	const CAngleQuantity& FrictionAngle() const;

  // these two functions return the values without looking at previous stages
  double UserDStiff1() const;
  double UserDStiff2() const;

  void Cohesion(double dCohesion);
  void FrictionAngle(double dFrictionAngle);
  void UserDStiff1(double dDStiff1);
  void UserDStiff2(double dDStiff2);

	// Distributed settings for cohesion and friction
  bool DistributedCohesion() const;
  bool DistributedFriction() const;
  bool CohesionExtrapolated() const;
  bool FrictionExtrapolated() const;
  void CohesionExtrapolated(bool b);
  void FrictionExtrapolated(bool b);

	double Cohesion(const geo::IPoint& point, const CQuantity::UNIT unit) const;
	std::vector<double> Cohesion(const geo::IElement& element, const CQuantity::UNIT unit) const;
	double FrictionAngle(const geo::IPoint& point, const CQuantity::UNIT unit) const;
	std::vector<double> FrictionAngle(const geo::IElement& element, const CQuantity::UNIT unit) const;

  // these two functions return the values for the analysis, using values
  // from previous stages if necessary
  double DStiffNormal() const;
  double DStiffShear() const;

  const CFaultParametersNode* Previous() const; // NULL if none exists
  CFaultParametersNode* Previous(); // NULL if none exists

  const CDepletionStage& Stage() const;
  const CHorizonBase& Fault() const;

  virtual bool Less(const CGraphNode& node) const;

  ACCEPT_GEOMECMODELVISITORS(VisitFaultParametersNode);

private:
  CHorizonBase& m_fault;
  CDepletionStage& m_stage;
  CDepletionStage* m_pBranchStage;
  bool m_bCohesionSameAsPrevious;
  bool m_bFrictionSameAsPrevious;
  bool m_bDStiffSameAsPrevious;
  bool m_bCohesionExtrapolated;
  bool m_bFrictionExtrapolated;

	CCohesionQuantity  m_Cohesion;
	CAngleQuantity     m_FrictionAngle;
  double             m_dUserDStiff1;
  double             m_dUserDStiff2;
  double             m_dFractureDStiff1;
  double             m_dFractureDStiff2;
	const CValueType*  m_pValueTypeCohesion;
	const CValueType*  m_pValueTypeFriction;
};

#endif // _FAULTPARAMETERSNODE_H_
