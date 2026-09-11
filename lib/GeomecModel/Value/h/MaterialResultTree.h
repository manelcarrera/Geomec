#ifndef _MATERIALRESULTTREE_H_
#define _MATERIALRESULTTREE_H_

#include "Result.h"

class CMaterialResultTree;
class ResultInfo;

class CMaterialResult : public IResult
{
public:
	class CMaterialResultComponent : public IResultComponent
	{
	public:
		CMaterialResultComponent(const QString& strName, const CDepletionStage& stage, CMaterialResult& parent, const CAnalysisType& antype, int nRegister);
		virtual ~CMaterialResultComponent();
		virtual unsigned int IconId() const;
		virtual unsigned int TypeId() const;
		virtual	QString UnitName(const UNIT unit) const;
		virtual long SavedItems() const;
    virtual geo::CValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
		IValueDomainScalar::TValueVec ElementValues(const geo::IElement& elm, const UNIT unit = CQuantity::SI_UNIT) const;
    virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
		virtual QString ExportLabel() const;

    const CMaterialResult& MaterialResult() const;

    virtual bool PrepareMapping(const geo::IElementSet *pElementSet);

  private:
    class CValueSet : public geo::IValueSetTemplate<geo::CValue>
    {
    public:
      CValueSet(const CMaterialResultComponent& component);
      virtual const geo::IElementSet& ElementSet() const;
      virtual void ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
      virtual bool IsEmpty() const;
      virtual bool IsValid(const geo::CValue& value) const;
      void Unit(IQuantityDouble::UNIT unit);

    private:
      const CMaterialResultComponent& m_component;
      IQuantityDouble::UNIT m_unit;
    };

  protected:
    virtual geo::CValue SingleValueElement(const geo::IElement& elm, const UNIT unit) const;

  private:
    mutable CValueSet m_valueset;
	};

  class CBoundaryMaterialResultComponent : public CMaterialResultComponent
  {
  public:
    CBoundaryMaterialResultComponent(const QString& strName, const CDepletionStage& stage, CMaterialResult& parent, const CAnalysisType& antype, int nRegister, bool showRad);
    virtual ~CBoundaryMaterialResultComponent();
  protected:
    virtual geo::CValue SingleValueElement(const geo::IElement& elm, const UNIT unit) const;
  private:
    bool m_showRad;
  };

public:
	CMaterialResult(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group);
	virtual ~CMaterialResult();

	virtual bool Empty() const;
	virtual long SavedItems() const;
	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;
	virtual bool Less(const CGraphNode &node) const;
  virtual bool isMaterialResult() const;
	virtual bool OnDefined(const IResultComponent& component) const;
	virtual bool CanMap(const COpenGLNode& node, int nRegister) const;
	virtual QString ExportLabel(int nComponent) const;
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

  const QString& StoredExportLabel() const;
	QString UnitName(const UNIT unit) const;
  double UnitFactor(const UNIT unit) const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialResult);

protected:
	virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
  virtual IResultComponent* OnGetResultComponent(int nTimeStep, const CAnalysisType& antype, int nRegister, unsigned int nComponent) const;

private:
  unsigned int m_uiValueTypeID;
  unsigned int m_uiIconID;
  QString m_strExportLabel;
  QString m_strUnitNameSI;
  QString m_strUnitNameField;
  double m_dFieldFactor;
protected:
  typedef std::map<int, CMaterialResultComponent*> TComponentMap; // depletion stage index to result component
  TComponentMap m_mpComponents;
};

class CBoundaryMaterialResult : public CMaterialResult
{
public:
  CBoundaryMaterialResult(unsigned int uName, unsigned int uiValueTypeID, unsigned int uiIconID, CResultGroup& group);
protected:
  virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);
};

class CMaterialResultTree : public CResultGroup
{
public:
	CMaterialResultTree(CModelBase& model);
	virtual unsigned int IconId() const;

  ACCEPT_GEOMECMODELVISITORS(VisitMaterialResultTree);
};

#endif // _MATERIALRESULTTREE_H_
