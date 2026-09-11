#ifndef _ResultTree_Delegate_h_
#define _ResultTree_Delegate_h_

class CResultTree;

#include "Result_Delegate.h"
#include "IStressTensorGroup_Delegate.h"
#include "ResultTree.h"
#include "IStrainTensorGroup_Delegate.h"
#include "IVectorResult_Delegate.h"

class CResultTree_Delegate : public CResultGroup_Delegate
{
public:
  class CEffectiveStressResult_Delegate : public IStressTensorGroup_Delegate
  {
  public:
  CEffectiveStressResult_Delegate(
      CResultTree::CEffectiveStressResult* effectiveStressResult);

  private:
  CEffectiveStressResult_Delegate(
      const CEffectiveStressResult_Delegate& rhs);
  CEffectiveStressResult_Delegate& operator = (
      const CEffectiveStressResult_Delegate& rhs);

  CResultTree::CEffectiveStressResult* m_effectiveStressResult;

  REGISTER_DELEGATE(CResultTree::CEffectiveStressResult,
      CEffectiveStressResult_Delegate);
  };

  class CTotalStressResult_Delegate : public IStressTensorGroup_Delegate
  {
  public:
  class CSurfaceStressComposite_Delegate : public IResult_Delegate
  {
  public:
      CSurfaceStressComposite_Delegate(
    CResultTree::CTotalStressResult::CSurfaceStressComposite*
          surfaceStressComposite);

  private:
      CSurfaceStressComposite_Delegate(
    const CSurfaceStressComposite_Delegate& rhs);
      CSurfaceStressComposite_Delegate& operator = (
    const CSurfaceStressComposite_Delegate& rhs);

      CResultTree::CTotalStressResult::CSurfaceStressComposite*
    m_surfaceStressComposite;

      REGISTER_DELEGATE(
    CResultTree::CTotalStressResult::CSurfaceStressComposite,
    CSurfaceStressComposite_Delegate);
  };

  CTotalStressResult_Delegate(
      CResultTree::CTotalStressResult* totalStressResult);

  private:
  CTotalStressResult_Delegate(const CTotalStressResult_Delegate& rhs);
  CTotalStressResult_Delegate& operator = (
      const CTotalStressResult_Delegate& rhs);

  CResultTree::CTotalStressResult* m_totalStressResult;

  REGISTER_DELEGATE(CResultTree::CTotalStressResult,
      CTotalStressResult_Delegate);
  };

  class CTotalStrainResult_Delegate : public IStrainTensorGroup_Delegate
  {
  public:
  CTotalStrainResult_Delegate(
      CResultTree::CTotalStrainResult* totalStrainResult);

  private:
  CTotalStrainResult_Delegate(const CTotalStrainResult_Delegate& rhs);
  CTotalStrainResult_Delegate& operator = (
      const CTotalStrainResult_Delegate& rhs);

  CResultTree::CTotalStrainResult* m_totalStrainResult;

  REGISTER_DELEGATE(CResultTree::CTotalStrainResult,
      CTotalStrainResult_Delegate);
  };

  class CPlasticStrainResult_Delegate : public IStrainTensorGroup_Delegate
  {
  public:
  CPlasticStrainResult_Delegate(
      CResultTree::CPlasticStrainResult* plasticStrainResult);

  private:
  CPlasticStrainResult_Delegate(const CPlasticStrainResult_Delegate& rhs);
  CPlasticStrainResult_Delegate& operator = (
      const CPlasticStrainResult_Delegate& rhs);

  CResultTree::CPlasticStrainResult* m_plasticStrainResult;

  REGISTER_DELEGATE(CResultTree::CPlasticStrainResult,
      CPlasticStrainResult_Delegate);
  };

  class CScalarResult_Delegate : public IResult_Delegate
  {
  public:
  class CScalarResultComponent_Delegate :
      public IScalarResultComponent_Delegate
  {
  public:
      CScalarResultComponent_Delegate(
    CResultTree::CScalarResult::CScalarResultComponent*
          scalarResultComponent);

  private:
      CScalarResultComponent_Delegate(
    const CScalarResultComponent_Delegate& rhs);
      CScalarResultComponent_Delegate& operator = (
    const CScalarResultComponent_Delegate& rhs);

      CResultTree::CScalarResult::CScalarResultComponent*
    m_scalarResultComponent;

      REGISTER_DELEGATE(CResultTree::CScalarResult::CScalarResultComponent,
    CScalarResultComponent_Delegate);
  };

  CScalarResult_Delegate(CResultTree::CScalarResult* scalarResult);

  private:
  CScalarResult_Delegate(const CScalarResult_Delegate& rhs);
  CScalarResult_Delegate& operator = (const CScalarResult_Delegate& rhs);

  CResultTree::CScalarResult* m_scalarResult;

  REGISTER_DELEGATE(CResultTree::CScalarResult, CScalarResult_Delegate);
  };

  class CDisplacementResult_Delegate : public CVectorResult_Delegate
  {
  public:
  CDisplacementResult_Delegate(
      CResultTree::CDisplacementResult* displacementResult);

  private:
  CDisplacementResult_Delegate(const CDisplacementResult_Delegate& rhs);
  CDisplacementResult_Delegate& operator = (
      const CDisplacementResult_Delegate& rhs);

  CResultTree::CDisplacementResult* m_displacementResult;

  REGISTER_DELEGATE(CResultTree::CDisplacementResult,
      CDisplacementResult_Delegate);
  };

  class CFaultScalarResult_Delegate : public IResult_Delegate
  {
  public:
  class CFaultScalarResultComponent_Delegate :
      public IScalarResultComponent_Delegate
  {
  public:
      CFaultScalarResultComponent_Delegate(
    CResultTree::CFaultScalarResult::CFaultScalarResultComponent*
          faultScalarResultComponent);

  private:
      CFaultScalarResultComponent_Delegate(
    const CFaultScalarResultComponent_Delegate& rhs);
      CFaultScalarResultComponent_Delegate& operator = (
    const CFaultScalarResultComponent_Delegate& rhs);

      CResultTree::CFaultScalarResult::CFaultScalarResultComponent*
    m_faultScalarResultComponent;

      REGISTER_DELEGATE(
    CResultTree::CFaultScalarResult::CFaultScalarResultComponent,
    CFaultScalarResultComponent_Delegate);
  };

  CFaultScalarResult_Delegate(
      CResultTree::CFaultScalarResult* faultScalarResult);

  private:
  CFaultScalarResult_Delegate(const CFaultScalarResult_Delegate& rhs);
  CFaultScalarResult_Delegate& operator = (
      const CFaultScalarResult_Delegate& rhs);

  CResultTree::CFaultScalarResult* m_faultScalarResult;

  REGISTER_DELEGATE(CResultTree::CFaultScalarResult, CFaultScalarResult_Delegate);
  };

  class CFaultDisplacementResult_Delegate : public CVectorResult_Delegate
  {
  public:
  CFaultDisplacementResult_Delegate(
      CResultTree::CFaultDisplacementResult* faultDisplacementResult);

  private:
  CFaultDisplacementResult_Delegate(
      const CFaultDisplacementResult_Delegate& rhs);
  CFaultDisplacementResult_Delegate& operator = (
      const CFaultDisplacementResult_Delegate& rhs);

  CResultTree::CFaultDisplacementResult* m_faultDisplacementResult;

  REGISTER_DELEGATE(CResultTree::CFaultDisplacementResult,
      CFaultDisplacementResult_Delegate);
  };

  class CFaultPlasticSlipResult_Delegate : public IVectorResult_Delegate
  {
  public:
  CFaultPlasticSlipResult_Delegate(
      CResultTree::CFaultPlasticSlipResult* faultPlasticSlipResult);

  private:
  CFaultPlasticSlipResult_Delegate(
      const CFaultPlasticSlipResult_Delegate& rhs);
  CFaultPlasticSlipResult_Delegate& operator = (
      const CFaultPlasticSlipResult_Delegate& rhs);

  CResultTree::CFaultPlasticSlipResult* m_faultPlasticSlipResult;

  REGISTER_DELEGATE(CResultTree::CFaultPlasticSlipResult,
      CFaultPlasticSlipResult_Delegate);
  };

  class CFaultNormalStress_Delegate : public IResult_Delegate
  {
  public:
  CFaultNormalStress_Delegate(
      CResultTree::CFaultNormalStress* faultNormalStress);

  private:
  CFaultNormalStress_Delegate(const CFaultNormalStress_Delegate& rhs);
  CFaultNormalStress_Delegate& operator = (
      const CFaultNormalStress_Delegate& rhs);

  CResultTree::CFaultNormalStress* m_faultNormalStress;

  REGISTER_DELEGATE(CResultTree::CFaultNormalStress,
      CFaultNormalStress_Delegate);
  };

  class CFaultShearStress_Delegate : public CVectorResult_Delegate
  {
  public:
  CFaultShearStress_Delegate(
      CResultTree::CFaultShearStress* faultShearStress);

  private:
  CFaultShearStress_Delegate(const CFaultShearStress_Delegate& rhs);
  CFaultShearStress_Delegate& operator = (
      const CFaultShearStress_Delegate& rhs);

  CResultTree::CFaultShearStress* m_faultShearStress;

  REGISTER_DELEGATE(CResultTree::CFaultShearStress,
      CFaultShearStress_Delegate);
  };

  class CFaultShearStressLengthChangeComposite_Delegate :
  public CVectorResult_Delegate
  {
  public:
  class CFaultShearStressLengthChangeComponent_Delegate :
      public IVectorResult_Delegate::CVectorComponent_Delegate
  {
  public:
      CFaultShearStressLengthChangeComponent_Delegate(
    CResultTree::CFaultShearStressLengthChangeComposite::
          CFaultShearStressLengthChangeComponent*
      faultShearStressLengthChangeComponent);

  private:
      CFaultShearStressLengthChangeComponent_Delegate(
    const CFaultShearStressLengthChangeComponent_Delegate& rhs);
      CFaultShearStressLengthChangeComponent_Delegate& operator = (
    const CFaultShearStressLengthChangeComponent_Delegate& rhs);

      CResultTree::CFaultShearStressLengthChangeComposite::
    CFaultShearStressLengthChangeComponent*
          m_faultShearStressLengthChangeComponent;

      REGISTER_DELEGATE(CResultTree::CFaultShearStressLengthChangeComposite::
    CFaultShearStressLengthChangeComponent,
          CFaultShearStressLengthChangeComponent_Delegate);
  };

  CFaultShearStressLengthChangeComposite_Delegate(
      CResultTree::CFaultShearStressLengthChangeComposite*
    faultShearStressLengthChangeComposite);

  private:
  CFaultShearStressLengthChangeComposite_Delegate(
      const CFaultShearStressLengthChangeComposite_Delegate& rhs);
  CFaultShearStressLengthChangeComposite_Delegate& operator = (
      const CFaultShearStressLengthChangeComposite_Delegate& rhs);

  CResultTree::CFaultShearStressLengthChangeComposite*
      m_faultShearStressLengthChangeComposite;

  REGISTER_DELEGATE(CResultTree::CFaultShearStressLengthChangeComposite,
      CFaultShearStressLengthChangeComposite_Delegate);
  };

  class CRTCIStrainResult_Delegate : public IResult_Delegate
  {
  public:
  CRTCIStrainResult_Delegate(
      CResultTree::CRTCIStrainResult* rtciStrainResult);

  private:
  CRTCIStrainResult_Delegate(const CRTCIStrainResult_Delegate& rhs);
  CRTCIStrainResult_Delegate& operator = (
      const CRTCIStrainResult_Delegate& rhs);

  CResultTree::CRTCIStrainResult* m_rtciStrainResult;

  REGISTER_DELEGATE(CResultTree::CRTCIStrainResult,
      CRTCIStrainResult_Delegate);
  };

  CResultTree_Delegate(CResultTree* resultTree);

private:
  CResultTree_Delegate(const CResultTree_Delegate& rhs);
  CResultTree_Delegate& operator = (const CResultTree_Delegate& rhs);

  CResultTree* m_resultTree;

  REGISTER_DELEGATE(CResultTree, CResultTree_Delegate);
};

#endif  // _ResultTree_Delegate_h_
