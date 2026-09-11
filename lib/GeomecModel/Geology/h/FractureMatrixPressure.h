// FractureMatrixPressure.h: interface for the CFractureMatrixPressure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRACTUREMATRIXPRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_)
#define AFX_FRACTUREMATRIXPRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommonPressureComponent.h"
#include "FormationLoadRepeaterComponent.h"

class CFractureMatrixPressure;

#include "GlobalValueObserver.h"

class CFractureMatrixComponentGWC : public CCommonPressureComponent <TFractureMatrixPressure>
{
	double GWCValue(const geo::IPoint& pt, const CQuantity::UNIT unit) const;
public:
	class CGWCPressure : public CSinglePressure
	{
		CFractureMatrixPressure& m_current;
	public:
		CGWCPressure(CFractureMatrixPressure &current, const double &value, const UNIT unit = SI_UNIT);
		CGWCPressure(const CGWCPressure &rhs);
		virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
		virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
		virtual CGWCPressure &operator=(const CSinglePressure &rhs);
		CGWCPressure& operator = (const CGWCPressure& rhs);
		virtual bool operator==(const CGWCPressure &rhs) const;
	};

	class CGWCDepth : public CLengthQuantity
	{
		CFractureMatrixPressure& m_current;
		public:
		CGWCDepth(CFractureMatrixPressure &current, const double &value, const UNIT unit = SI_UNIT);
		CGWCDepth(const CGWCDepth &rhs);
		virtual double MaxValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
		virtual double MinValue(const int nComponent = 0, const UNIT unit = SI_UNIT) const;
		virtual CGWCDepth& operator=(const CLengthQuantity &rhs);
		CGWCDepth& operator = (const CGWCDepth& rhs);
		virtual bool operator==(const CGWCDepth &rhs) const;
	};

private:
	CGWCPressure				m_contact_pressure;
	CGWCPressure				m_trans_pressure;
	CGWCDepth					m_contact_depth;
	CGWCDepth					m_trans_depth;
	CPressureGradientQuantity*	m_pUpperGradient;
	CPressureGradientQuantity*	m_pLowerGradient;
	bool						m_bTransitionFromPrevious;
public:
// Contruction
	CFractureMatrixComponentGWC(CFractureMatrixPressure &pressure, bool bEmpty = false);
	CFractureMatrixComponentGWC(const CFractureMatrixComponentGWC &rhs);
	virtual ~CFractureMatrixComponentGWC();

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

	void Set(const double &dContactPressure,
			 const double &dTransitionPressure,
			 const double &dContactDepth,
			 const double &dTransitionDepth,
			 const double &dUpperGradient,
			 const double &dLowerGradient,
			 bool bTransitionFromPrevious,
			 const CQuantity::UNIT unit = CQuantity::SI_UNIT);

	bool IsFirstGWC() const;
	bool IsLastGWC() const;

// find the first GWC
	const CFractureMatrixComponentGWC &FirstGWC() const;

// find the previous GWC, asserts if IsFirstGWC()
	const CFractureMatrixComponentGWC &PreviousGWC() const;

// find the next GWC, asserts if IsLastGWC()
	const CFractureMatrixComponentGWC &NextGWC() const;

// Value implementation
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

// Assignment and equal
	bool operator==(const CFractureMatrixComponentGWC &rhs) const;
	CFractureMatrixComponentGWC& operator=(const CFractureMatrixComponentGWC& rhs);

// Save and load
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitFractureMatrixComponentGWC);
};

class CFractureMatrixComponentRepeater;

typedef CFormationLoadRepeaterComponent <TFractureMatrixPressure, CComponentConstant <TFractureMatrixPressure>, CFractureMatrixComponentRepeater, CFractureMatrixComponentGWC> TFractureMatrixComponentRepeaterBase;

class CFractureMatrixComponentRepeater : public TFractureMatrixComponentRepeaterBase
{
public:
	CFractureMatrixComponentRepeater(const CPressure* pressure, CFractureMatrixPressure &fractureMatrixPressure);
	CFractureMatrixComponentRepeater(const CFractureMatrixComponentRepeater& rhs);

	// IconId and TypeName
	virtual unsigned int TypeId() const;
	virtual unsigned int IconId() const;
	virtual QString TypeName() const;

	virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;
	virtual QString ExportLabel() const;

	virtual const TFormationLoad& PreviousFormationLoad() const;
	virtual const IValueComponentBase& GlobalValues() const;

  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  ACCEPT_GEOMECMODELVISITORS(VisitFractureMatrixComponentRepeater);

private:
  const CPressure* m_Pressure;
  CFractureMatrixPressure &m_FractureMatrixPressure;
};

typedef CFormationLoad<TFractureMatrixPressure, CComponentConstant <TFractureMatrixPressure>, CFractureMatrixComponentRepeater, CFractureMatrixComponentGWC> TFractureMatrixPressureBase;

class CFractureMatrixPressure : public TFractureMatrixPressureBase
{
public:
	CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, CDepletionStage &d, bool bEmpty = false);			// Undefined initialisation
	CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
			  CDepletionStage &d,
			  const double &dReferencePressure,
			  const double &dReferenceDepth,
			  const double &dGradient,
			  const CQuantity::UNIT unit = CQuantity::SI_UNIT); // Constant initialisation
	CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
			  CDepletionStage &d,
			  const double &dContactPressure,
			  const double &dTransitionPressure,
			  const double &dContactDepth,
			  const double &dTransitionDepth,
			  const double &dUpperGradient,
			  const double &dLowerGradient,
			  bool bTransitionFromPrevious,
			  const CQuantity::UNIT unit = CQuantity::SI_UNIT);
	CFractureMatrixPressure(const CPressure* pressure, CFormationBase &f, 
			  CDepletionStage &d,
			  TFractureMatrixPressure &p);
	CFractureMatrixPressure(const CFractureMatrixPressure& rhs);
  CFractureMatrixPressure& operator=(const CFractureMatrixPressure& rhs);

	virtual unsigned int TypeId() const;
	virtual QString TypeName() const;

  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual void OnNewNeighbour(const CGraphNode& node);
  virtual bool IsFirstValidDepletionStage() const;

	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version,
    TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitFractureMatrixPressure);

private:
	virtual void LoadComponents(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveComponents(TSTREAM& stream, TPROGRESS& progress);
	virtual TFractureMatrixPressureBase& FormationStageLoad(CFormationBase& formation, CDepletionStage& stage);
	void init(CFormationBase &f, CDepletionStage &d, bool bEmpty = false);

  CGlobalValueObserver <CFractureMatrixPressure, CGlobalPressure> m_GlobalPressureObserver;
  const CPressure* m_Pressure;
};

typedef CConstSingleCommandTemplate<CFractureMatrixPressure> TFractureMatrixPressureCommand;

#endif // !defined(AFX_FRACTUREMATRIXPRESSURE_H__F07C68DD_5FD3_4CCD_9EFF_7BF026A583CE__INCLUDED_)
