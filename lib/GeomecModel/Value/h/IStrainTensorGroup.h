// IStrainTensorGroup.h: interface for the IStrainTensorGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISTRAINTENSORGROUP_H__4571FD65_9AF9_4B9E_87E6_A4BDF31968E7__INCLUDED_)
#define AFX_ISTRAINTENSORGROUP_H__4571FD65_9AF9_4B9E_87E6_A4BDF31968E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CResultInfo;

#include "ITensorGroupTemplate.h"

class IStrainTensorGroup : public ITensorGroupTemplate<CStrainTensor>
{
public:
  class CInvariantComposite : public IResult
  {
  public:
    enum INVARIANT_TYPE { INV_I1 = 0, INV_I2, INV_I3, INV_VONMIS };
    
    class CInvariantComponent : public IResultComponent
    {
      INVARIANT_TYPE m_type;
      geo::CValue Value(const CStrainTensor& tensor, CQuantity::UNIT unit) const;

    public:
      CInvariantComponent(unsigned int uInvariantName,				// Name from resource
                CInvariantComposite &parent,		// The invariant composite parent
                enum INVARIANT_TYPE invtype,		// Invariant type
                const CDepletionStage &stage,	// Depletion stage of the inv. type
                const CAnalysisType& antype,					// Analysis type
                int nRegister);					// Result register index				
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual geo::CValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual geo::CValue Value(const geo::IElement& /*mesh_element*/, int /*nNodeIndex*/, UNIT /*unit*/) const { assert(false); return geo::CValue(); }
      INVARIANT_TYPE InvariantType() const { return m_type; }

      virtual bool NeedParallelInitializationCallback() const;
      virtual bool PrepareMapping(const geo::IElementSet *);
      virtual void FinishMapping();
  };
  protected:
    virtual void BuildComponent(const CDepletionStage &stage, const CAnalysisType& antype, int nRegister);
  public:
    CInvariantComposite(unsigned int uName, IStrainTensorGroup &group);
    CInvariantComposite(const QString &name, IStrainTensorGroup &group);
        bool ValidName ( const std::string &name , CResultInfo &resultInfo) const;
    virtual unsigned int IconId() const;
    virtual bool OnDefined(const IResultComponent &component) const;
    virtual QString ExportLabel(int nComponent) const;
    virtual bool CanMap(const COpenGLNode& node, int nRegister) const;

  virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
  };

  class CWellPathAxialStrainComposite : public IResult
  {
  public:
  class CWellPathAxialStrainComponent : public IResultComponent
  {
  public:
      CWellPathAxialStrainComponent(unsigned int uName,
                                    CWellPathAxialStrainComposite &parent,
                                    const CDepletionStage &stage,
                                    const CAnalysisType& antype,
                                    int nRegister);
      virtual unsigned int IconId() const;
      virtual unsigned int TypeId() const;
      virtual QString UnitName(const UNIT unit) const;
      virtual geo::CValue ValuePoint(const geo::IPoint& pt, const UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual void MapValueElement(const geo::IElement& elm, IValueDomainScalar::TValueVec& values, TMapType map_type, UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
      virtual geo::CValue Value(const geo::IElement& /*mesh_element*/, int /*nNodeIndex*/, UNIT /*unit*/) const { assert(false); return geo::CValue(); }

      virtual bool NeedParallelInitializationCallback() const;
  };

  public:
    CWellPathAxialStrainComposite(unsigned int uName, CResultGroup& group);
    CWellPathAxialStrainComposite(const QString& sName, CResultGroup& group);
        bool ValidName ( const std::string &name , CResultInfo &resultInfo) const;

    virtual unsigned int IconId() const;
    virtual bool OnDefined(const IResultComponent& component) const;
    virtual QString ExportLabel() const;
  virtual QString ExportLabel(int /*nComponent*/) const { return ExportLabel(); }
    virtual bool CanMap(const COpenGLNode& node, int nRegister) const;

  virtual geo::IParallelInitializationCallback *GetParallelInitializationCallback();
  virtual bool PrepareMapping(const geo::IElementSet *, const IValueComponentBase *);
  virtual void FinishMapping();

  protected:
    virtual bool OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const;
    virtual void BuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister);

  private:
    const IStrainTensorGroup* m_pTensorGroup;
  };

private:
  CInvariantComposite* m_pInVariant;
  CWellPathAxialStrainComposite* m_pAxialStrainComposite;

protected:
  virtual void CreateComposite();
  virtual double GetTypicalEpsilon() const { return 1e-10; }

public:
  IStrainTensorGroup(unsigned int uResultName, TValueSetFunction function, CResultGroup& group);
  IStrainTensorGroup(const QString &sResultName, TValueSetFunction function, CResultGroup& group);
  virtual ~IStrainTensorGroup();
  const CInvariantComposite *InvariantComposite() const { return m_pInVariant; }
  const CWellPathAxialStrainComposite *WellPathAxialStrainComposite() const { return  m_pAxialStrainComposite; }
  virtual bool Change() const { return false; }
  virtual TTensorType Type() const { return TT_STRAIN; }
  virtual bool OnBuildComponent(const CDepletionStage& stage, const CAnalysisType& antype, int nRegister) const;

  const CInvariantComposite& Invariants() const { return *m_pInVariant; }

  virtual double ConvertToField(const double& dDouble) const;
  virtual	QString UnitName(CQuantity::UNIT unit) const; 

  virtual unsigned int WellPathAxialStrainIndex() const = 0;

  ACCEPT_GEOMECMODELVISITORS(VisitIStrainTensorGroup);
};

#endif // !defined(AFX_ISTRAINTENSORGROUP_H__4571FD65_9AF9_4B9E_87E6_A4BDF31968E7__INCLUDED_)
