// StaborResult.h: interface for the CStaborResult class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STABORRESULT_H__421143D4_850C_401B_B794_CFEDA19CA100__INCLUDED_)
#define AFX_STABORRESULT_H__421143D4_850C_401B_B794_CFEDA19CA100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Result.h"

class CResultInfo;
class CNewWellPathEntry;

// The DStabor result is calculated by DStabor en is inserted in the result tree by this class.
class CStaborResult : public IResult
{
	CNewWellPathEntry& m_well_path_entry;
	TResultMap m_mpComponent;
protected:
	virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
	virtual QString ExportLabel(int nComponent) const;

public:
	// The DStabor component represents the one only result component of DStator
	class CStaborResultComponent : public IScalarResultComponent  
	{
		typedef std::map<const geo::IPoint*, double> TPointMap;
		TPointMap m_mpValue;
	public:
		CStaborResultComponent(CStaborResult& result, const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
		virtual ~CStaborResultComponent();

		void Clear();
		bool InsertValue(const geo::IPoint &pt, const double &value);

		virtual	QString UnitName(const CQuantity::UNIT unit = CQuantity::SI_UNIT) const;

		virtual bool Empty() const;
		virtual long SavedItems() const;

    virtual void ElementValues(IValueDomainScalar::TValueVec& values, const geo::IElement& element, UNIT unit, geo::IParallelInitializationCallback *cb) const;

		virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;

		virtual QString ExportLabel() const;
	};
	CStaborResult(CResultGroup& group);
	virtual ~CStaborResult();
	virtual void OnNeighbourDeleted(const CGraphNode &node);
	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
        bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  ACCEPT_GEOMECMODELVISITORS(VisitStaborResult);
};

#endif // !defined(AFX_STABORRESULT_H__421143D4_850C_401B_B794_CFEDA19CA100__INCLUDED_)
