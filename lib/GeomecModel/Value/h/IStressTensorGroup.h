// IStressTensorGroup.h: interface for the IStressTensorGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISTRESSTENSORGROUP_H__F80227D0_CE5C_421D_8C65_C50A7E6B61CF__INCLUDED_)
#define AFX_ISTRESSTENSORGROUP_H__F80227D0_CE5C_421D_8C65_C50A7E6B61CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITensorGroupTemplate.h"

class CResultInfo;

class IStressTensorGroup : public ITensorGroupTemplate<CStressTensor> {
public:
  // The principal stress length 'change'
  class CPrincipalLengthChangeComposite : public ITensorGroup::CVectorComposite {
  public:
    class CLengthChangeComponent : public ITensorGroup::CVectorComposite::CLengthComponent {
    public:
      CLengthChangeComponent(unsigned int uName, CPrincipalLengthChangeComposite &parent, const CDepletionStage &stage,
                             const CAnalysisType &antype, int nRegister);
      CLengthChangeComponent(const QString &sName, CPrincipalLengthChangeComposite &parent,
                             const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

      virtual geo::CValue ValuePoint(const geo::IPoint &pt,
                                     const IValueComponentBase::UNIT unit = IQuantityDouble::SI_UNIT,
                                     geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values,
                                   IResultComponent::TMapType map_type,
                                   IValueComponentBase::UNIT unit = CQuantity::SI_UNIT,
                                   geo::IParallelInitializationCallback *cb = 0) const;

      const CPrincipalLengthChangeComposite &Composite() const;
      const IStressTensorGroup &AbsoluteStressTensorGroup() const;
      const IStressTensorGroup &ChangeStressTensorGroup() const;

      virtual bool NeedParallelInitializationCallback() const;

    private:
      typedef std::pair<const IResultComponent *, const IResultComponent *> TSourceComponentPair;
      TSourceComponentPair SourceComponents() const;
    };

    CPrincipalLengthChangeComposite(unsigned int uName, CResultGroup &group, PRINCIPAL_DIRECTION direction);
    CPrincipalLengthChangeComposite(const QString &name, CResultGroup &group, PRINCIPAL_DIRECTION direction);
    const IStressTensorGroup &AbsoluteStressTensorGroup() const;
    const IStressTensorGroup &ChangeStressTensorGroup() const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual unsigned int IconId() const;
    virtual QString ExportLabel(int nComponent) const;

    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;

    virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
    virtual void FinishMapping();

  private:
    PRINCIPAL_DIRECTION m_direction;

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
  };

  // The invariant class
  class CInvariantComposite : public IResult {
  public:
    enum INVARIANT_TYPE {
      INV_I1 = 0,
      INV_I2,
      INV_I3,
      INV_VONMIS,
      INV_TRESCA,
      INV_MEAN,
      INV_VONMIS_GRADIENT,
      INV_MEAN_GRADIENT
    };

    class CInvariantComponent : public IResultComponent {
      INVARIANT_TYPE m_type;
      geo::CValue Value(const CStressTensor &tensor, CQuantity::UNIT unit) const;

    public:
      CInvariantComponent(unsigned int uInvariantName,  // Name from resource
                          CInvariantComposite &parent,  // The invariant composite parent
                          enum INVARIANT_TYPE invtype,  // Invariant type
                          const CDepletionStage &stage, // Depletion stage of the inv. type
                          const CAnalysisType &antype,  // Analysis type
                          int nRegister);               // Result register index
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                    geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual IValueDomainScalar::TValue Value(const geo::IElement & /*mesh_element*/, int /*nNodeIndex*/,
                                               UNIT /*unit*/) const {
        assert(false);
        return geo::CValue();
      }
      virtual bool Gradient() const;

      virtual bool CanComputeOnPoints() const;
      virtual bool NeedParallelInitializationCallback() const;
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();
    };

  protected:
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  public:
    CInvariantComposite(unsigned int uName, ITensorGroup &group);
    CInvariantComposite(const QString &name, ITensorGroup &group);
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    virtual unsigned int IconId() const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;

    virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
  };

  class CWellPathStressComposite : public IResult {
  public:
    class CWellPathStressComponent : public IResultComponent {
    public:
      typedef enum { TC_AXIAL = 0, TC_RADIAL_MIN = 1, TC_RADIAL_MAX = 2 } TComponent;

      CWellPathStressComponent(unsigned int uName, CWellPathStressComposite &parent, TComponent comp_type,
                               const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                    geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual IValueDomainScalar::TValue Value(const geo::IElement & /*mesh_element*/, int /*nNodeIndex*/,
                                               UNIT /*unit*/) const {
        assert(false);
        return geo::CValue();
      }

      virtual bool NeedParallelInitializationCallback() const;

    private:
      TComponent m_comp_type;
    };

  public:
    CWellPathStressComposite(unsigned int uName, CResultGroup &group);
    CWellPathStressComposite(const QString &sName, CResultGroup &group);

    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    virtual unsigned int IconId() const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;

    virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
    virtual void FinishMapping();

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  private:
    const IStressTensorGroup *m_pTensorGroup;
  };

  class CVerticalGradientComposite : public IResult {
  public:
    class CVerticalGradientComponent : public IResultComponent {
    public:
      CVerticalGradientComponent(unsigned int uName, CVerticalGradientComposite &parent, const CDepletionStage &stage,
                                 const CAnalysisType &antype, int nRegister);

      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint &pt, const UNIT unit = CQuantity::SI_UNIT,
                                                    geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement &elm, IValueDomainScalar::TValueVec &values, TMapType map_type,
                                   UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual IValueDomainScalar::TValue Value(const geo::IElement &mesh_element, int nNodeIndex, UNIT unit) const;

      virtual bool Gradient() const { return true; }

      virtual bool NeedParallelInitializationCallback() const;
      virtual bool PrepareMapping(const geo::IElementSet * /*pElementSet*/);

    private:
      const IResultComponent &VerticalComponent() const;
    };

    CVerticalGradientComposite(unsigned int uName, CResultGroup &group);

    virtual unsigned int IconId() const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual QString ExportLabel(int nComponent) const;
    bool ValidName(const std::string &name, CResultInfo &resultInfo) const;
    virtual bool CanMap(const COpenGLNode &node, int nRegister) const;

    const IStressTensorGroup &TensorGroup() const;

    virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
    virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
    virtual void FinishMapping();

  protected:
    virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister);

  private:
    const IStressTensorGroup *m_pTensorGroup;
  };

private:
  bool m_bChange;
  CPrincipalLengthChangeComposite *m_pLengthChange[3];
  CInvariantComposite *m_pInVariant;
  CWellPathStressComposite *m_pWPStressComposite;
  CVerticalGradientComposite *m_pVerticalGradientComposite;

protected:
  virtual void CreateComposite();
  virtual double GetTypicalEpsilon() const { return 1e-4; }

public:
  IStressTensorGroup(unsigned int uResultName, TValueSetFunction function, CResultGroup &group, bool bChange);
  IStressTensorGroup(const QString &sResultName, TValueSetFunction function, CResultGroup &group, bool bChange);

  const CInvariantComposite *InvariantComposite() const { return m_pInVariant; }
  const CWellPathStressComposite *WellPathStressComposite() const { return m_pWPStressComposite; }
  CVerticalGradientComposite *VerticalGradientComposite() const { return m_pVerticalGradientComposite; }
  virtual ~IStressTensorGroup();

  virtual bool Change() const { return m_bChange; }
  virtual TTensorType Type() const { return TT_STRESS; }
  virtual bool OnBuildComponent(const CDepletionStage &stage, const CAnalysisType &antype, int nRegister) const;

  virtual double ConvertToField(const double &dDouble) const;
  virtual QString UnitName(CQuantity::UNIT unit) const;

  virtual unsigned int WellPathStressIndex() const = 0;
  const CPrincipalLengthChangeComposite *MaxPrincipleLengthChange() const;
  const CPrincipalLengthChangeComposite *MedPrincipleLengthChange() const;
  const CPrincipalLengthChangeComposite *MinPrincipleLengthChange() const;

  ACCEPT_GEOMECMODELVISITORS(VisitIStressTensorGroup);
};

#endif // !defined(AFX_ISTRESSTENSORGROUP_H__F80227D0_CE5C_421D_8C65_C50A7E6B61CF__INCLUDED_)
