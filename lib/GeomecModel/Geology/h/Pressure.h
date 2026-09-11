// Pressure.h: interface for the CPressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_)
#define AFX_PRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonPressureComponent.h"
#include "FormationLoadRepeaterComponent.h"

class CPressure;

#include "GlobalValueObserver.h"

class CComponentGWC : public CCommonPressureComponent<TPressure> {
  double GWCValue(const geo::IPoint &pt, const CQuantity::UNIT unit) const;

public:
  class CGWCPressure : public CSinglePressure {
    CPressure &m_current;

  public:
    CGWCPressure(CPressure &current, const double &value, const UNIT unit = SI_UNIT);
    CGWCPressure(const CGWCPressure &rhs);
    virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
    virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
    virtual CGWCPressure &operator=(const CSinglePressure &rhs);
    CGWCPressure &operator=(const CGWCPressure &rhs);
    virtual bool operator==(const CGWCPressure &rhs) const;
  };

  class CGWCDepth : public CLengthQuantity {
    CPressure &m_current;

  public:
    CGWCDepth(CPressure &current, const double &value, const UNIT unit = SI_UNIT);
    CGWCDepth(const CGWCDepth &rhs);
    virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
    virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
    virtual CGWCDepth &operator=(const CLengthQuantity &rhs);
    CGWCDepth &operator=(const CGWCDepth &rhs);
    virtual bool operator==(const CGWCDepth &rhs) const;
  };

private:
  CGWCPressure m_contact_pressure;
  CGWCPressure m_trans_pressure;
  CGWCDepth m_contact_depth;
  CGWCDepth m_trans_depth;
  CPressureGradientQuantity *m_pUpperGradient;
  CPressureGradientQuantity *m_pLowerGradient;
  bool m_bTransitionFromPrevious;

public:
  // Contruction
  CComponentGWC(CPressure &pressure, bool bEmpty = false);
  CComponentGWC(const CComponentGWC &rhs);
  virtual ~CComponentGWC();

  // IconId and TypeName
  virtual QString TypeName() const;

  // Get and set values
  const CSinglePressure &ContactPressure() const;
  const CSinglePressure &TransitionPressure() const;

  const CLengthQuantity &ContactDepth() const;
  const CLengthQuantity &TransitionDepth() const;

  const CPressureGradientQuantity &UpperGradient() const;
  const CPressureGradientQuantity &LowerGradient() const;

  bool TransitionFromPrevious() const;
  void TransitionFromPrevious(bool bNew);

  bool CheckContactDepth(const double &dContactDepth) const;
  bool CheckTransitionDepth(const double &dTransitionDepth) const;

  void Set(const double &dContactPressure, const double &dTransitionPressure, const double &dContactDepth,
           const double &dTransitionDepth, const double &dUpperGradient, const double &dLowerGradient,
           bool bTransitionFromPrevious, const CQuantity::UNIT unit = CQuantity::SI_UNIT);

  bool IsFirstGWC() const;
  bool IsLastGWC() const;

  // find the first GWC
  const CComponentGWC &FirstGWC() const;

  // find the previous GWC, asserts if IsFirstGWC()
  const CComponentGWC &PreviousGWC() const;

  // find the next GWC, asserts if IsLastGWC()
  const CComponentGWC &NextGWC() const;

  // Value implementation
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement &elm,
                                                     const CQuantity::UNIT unit = CQuantity::SI_UNIT,
                                                     geo::IParallelInitializationCallback *cb = 0) const;

  // Assignment and equal
  bool operator==(const CComponentGWC &rhs) const;
  CComponentGWC &operator=(const CComponentGWC &rhs);

  // Save and load
  virtual void LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveStream(TSTREAM &stream, TPROGRESS &progress);

  ACCEPT_GEOMECMODELVISITORS(VisitComponentGWC);
};

class CComponentRepeater : public CFormationLoadRepeaterComponent<TPressure, CComponentConstant<TPressure>,
                                                                  CComponentRepeater, CComponentGWC> {
public:
  CComponentRepeater(CPressure &pressure);
  CComponentRepeater(const CComponentRepeater &rhs);

  // IconId and TypeName
  virtual unsigned int TypeId() const;
  virtual unsigned int IconId() const;
  virtual QString TypeName() const;

  virtual QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
  virtual QString ExportLabel() const;

  virtual const TFormationLoad &PreviousFormationLoad() const;
  virtual const IValueComponentBase &GlobalValues() const;

  ACCEPT_GEOMECMODELVISITORS(VisitComponentRepeater);
};

typedef CFormationLoad<TPressure, CComponentConstant<TPressure>, CComponentRepeater, CComponentGWC> TPressureBase;

class CPressure : public TPressureBase {
public:
  CPressure(CFormationBase &f, CDepletionStage &d, bool bEmpty = false); // Undefined initialisation
  CPressure(CFormationBase &f, CDepletionStage &d, const double &dReferencePressure, const double &dReferenceDepth,
            const double &dGradient,
            const CQuantity::UNIT unit = CQuantity::SI_UNIT); // Constant initialisation
  CPressure(CFormationBase &f, CDepletionStage &d, const double &dContactPressure, const double &dTransitionPressure,
            const double &dContactDepth, const double &dTransitionDepth, const double &dUpperGradient,
            const double &dLowerGradient, bool bTransitionFromPrevious,
            const CQuantity::UNIT unit = CQuantity::SI_UNIT);
  CPressure(CFormationBase &f, CDepletionStage &d, TPressure &p);
  CPressure(const CPressure &rhs);
  CPressure &operator=(const CPressure &rhs);

  virtual unsigned int TypeId() const;
  virtual QString TypeName() const;

  const CPressure *ParentPressure() const;

  virtual void OnNeighbourDeleted(const CGraphNode &node);
  virtual void OnNewNeighbour(const CGraphNode &node);
  virtual bool IsFirstValidDepletionStage() const;

  ACCEPT_GEOMECMODELVISITORS(VisitPressure);

private:
  virtual void LoadComponents(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress);
  virtual void SaveComponents(TSTREAM &stream, TPROGRESS &progress);
  virtual TPressureBase &FormationStageLoad(CFormationBase &formation, CDepletionStage &stage);
  void init(CFormationBase &f, CDepletionStage &d, bool bEmpty = false);

  CGlobalValueObserver<CPressure, CGlobalPressure> m_GlobalPressureObserver;
};

typedef CConstSingleCommandTemplate<CPressure> TPressureCommand;

#endif // !defined(AFX_PRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_)
