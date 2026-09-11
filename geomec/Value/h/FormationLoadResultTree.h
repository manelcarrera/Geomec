#ifndef _FORMATIONLOADRESULTTREE_H_
#define _FORMATIONLOADRESULTTREE_H_

#include "Result.h"

class CFormationLoadResultTree;

class IFormationLoadResult : public IResult
{
public:
	class IFormationLoadResultComponent : public IResultComponent
	{
	public:
		IFormationLoadResultComponent(const CString& strName, const CDepletionStage& stage, IFormationLoadResult& parent);
		virtual ~IFormationLoadResultComponent();
		virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;
		virtual	QString UnitName(const UNIT unit) const;
		virtual long SavedItems() const;
		virtual geo::CValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT) const;
		IValueDomainScalar::TValueVec ElementValues(const geo::IElement& elm, const UNIT unit = CQuantity::SI_UNIT) const;
  	virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT) const;
		virtual QString ExportLabel() const;

    const IFormationLoadResult& FormationLoadResult() const;

  private:
    class CValueSet : public geo::IValueSetTemplate<geo::CValue>
    {
    public:
      CValueSet(const IFormationLoadResultComponent& component);
      virtual const geo::IElementSet& ElementSet() const;
      virtual void ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
      virtual bool IsEmpty() const;
      virtual bool IsValid(const geo::CValue& value) const;
      void Unit(IQuantityDouble::UNIT unit);

    private:
      const IFormationLoadResultComponent& m_component;
      IQuantityDouble::UNIT m_unit;
    };

  private:
    mutable CValueSet m_valueset;
	};

public:
	IFormationLoadResult(CString strName, CFormationLoadResultTree& resulttree);
	virtual ~IFormationLoadResult();

	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual bool Less(const CGraphNode &node) const;
	virtual bool OnDefined(const IResultComponent& component) const;
	virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
	virtual QString ExportLabel(int nComponent) const;
  virtual void OnNeighbourDeleted(const CGraphNode& node);

  const CString& StoredExportLabel() const;
	QString UnitName(const UNIT unit) const;
  double UnitFactor(const UNIT unit) const;

protected:
	virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
  virtual IResultComponent* OnGetResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const;

private:
  CString m_strExportLabel;
  CString m_strUnitNameSI;
  CString m_strUnitNameField;
  double m_dFieldFactor;
  typedef std::map<int, IFormationLoadResultComponent*> TComponentMap; // depletion stage index to result component
  TComponentMap m_mpComponents;
};

class CPressureResult : public IFormationLoadResult
{
public:
  CPressureResult(CFormationLoadResultTree& resulttree);
};

class CFormationLoadResultTree : public CResultGroup
{
	CFormationLoadResultTree(CModelBase& model);
	virtual unsigned int IconId() const;
};

#endif // _FORMATIONLOADRESULTTREE_H_
