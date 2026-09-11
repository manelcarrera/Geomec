#include "stdafx.h"

#include "ResultTree_Delegate.h"
#include "ResultTree.h"

CResultTree_Delegate::CEffectiveStressResult_Delegate::
  CEffectiveStressResult_Delegate(
    CResultTree::CEffectiveStressResult* effectiveStressResult)
: IStressTensorGroup_Delegate(effectiveStressResult)
, m_effectiveStressResult(effectiveStressResult)
{
}

CResultTree_Delegate::CTotalStressResult_Delegate::
  CSurfaceStressComposite_Delegate::CSurfaceStressComposite_Delegate(
    CResultTree::CTotalStressResult::CSurfaceStressComposite*
      surfaceStressComposite)
: IResult_Delegate(surfaceStressComposite)
, m_surfaceStressComposite(surfaceStressComposite)
{
}

CResultTree_Delegate::CTotalStressResult_Delegate::CTotalStressResult_Delegate(
  CResultTree::CTotalStressResult* totalStressResult)
: IStressTensorGroup_Delegate(totalStressResult)
, m_totalStressResult(totalStressResult)
{
}

CResultTree_Delegate::CTotalStrainResult_Delegate::CTotalStrainResult_Delegate(
  CResultTree::CTotalStrainResult* totalStrainResult)
: IStrainTensorGroup_Delegate(totalStrainResult)
, m_totalStrainResult(totalStrainResult)
{
}

CResultTree_Delegate::CPlasticStrainResult_Delegate::
  CPlasticStrainResult_Delegate(CResultTree::CPlasticStrainResult*
    plasticStrainResult)
: IStrainTensorGroup_Delegate(plasticStrainResult)
, m_plasticStrainResult(plasticStrainResult)
{
}

CResultTree_Delegate::CScalarResult_Delegate::
  CScalarResultComponent_Delegate::CScalarResultComponent_Delegate(
    CResultTree::CScalarResult::CScalarResultComponent* scalarResultComponent)
: IScalarResultComponent_Delegate(scalarResultComponent)
, m_scalarResultComponent(scalarResultComponent)
{
}

CResultTree_Delegate::CScalarResult_Delegate::CScalarResult_Delegate(
  CResultTree::CScalarResult* scalarResult)
: IResult_Delegate(scalarResult)
, m_scalarResult(scalarResult)
{
}

CResultTree_Delegate::CDisplacementResult_Delegate::
  CDisplacementResult_Delegate(
    CResultTree::CDisplacementResult* displacementResult)
: CVectorResult_Delegate(displacementResult)
, m_displacementResult(displacementResult)
{
}

CResultTree_Delegate::CFaultScalarResult_Delegate::
  CFaultScalarResultComponent_Delegate::CFaultScalarResultComponent_Delegate(
    CResultTree::CFaultScalarResult::CFaultScalarResultComponent*
      faultScalarResultComponent)
: IScalarResultComponent_Delegate(faultScalarResultComponent)
, m_faultScalarResultComponent(faultScalarResultComponent)
{
}

CResultTree_Delegate::CFaultScalarResult_Delegate::CFaultScalarResult_Delegate(
  CResultTree::CFaultScalarResult* faultScalarResult)
: IResult_Delegate(faultScalarResult)
, m_faultScalarResult(faultScalarResult)
{
}

CResultTree_Delegate::CFaultDisplacementResult_Delegate::
  CFaultDisplacementResult_Delegate(CResultTree::CFaultDisplacementResult*
    faultDisplacementResult)
: CVectorResult_Delegate(faultDisplacementResult)
, m_faultDisplacementResult(faultDisplacementResult)
{
}

CResultTree_Delegate::CFaultPlasticSlipResult_Delegate::
  CFaultPlasticSlipResult_Delegate(CResultTree::CFaultPlasticSlipResult*
    faultPlasticSlipResult)
: IVectorResult_Delegate(faultPlasticSlipResult)
, m_faultPlasticSlipResult(faultPlasticSlipResult)
{
}

CResultTree_Delegate::CFaultNormalStress_Delegate::CFaultNormalStress_Delegate(
  CResultTree::CFaultNormalStress* faultNormalStress)
: IResult_Delegate(faultNormalStress)
, m_faultNormalStress(faultNormalStress)
{
}

CResultTree_Delegate::CFaultShearStress_Delegate::CFaultShearStress_Delegate(
  CResultTree::CFaultShearStress* faultShearStress)
: CVectorResult_Delegate(faultShearStress)
, m_faultShearStress(faultShearStress)
{
}

CResultTree_Delegate::CFaultShearStressLengthChangeComposite_Delegate::
  CFaultShearStressLengthChangeComponent_Delegate::
    CFaultShearStressLengthChangeComponent_Delegate(
      CResultTree::CFaultShearStressLengthChangeComposite::
        CFaultShearStressLengthChangeComponent*
          faultShearStressLengthChangeComponent)
: IVectorResult_Delegate::CVectorComponent_Delegate(
    faultShearStressLengthChangeComponent)
, m_faultShearStressLengthChangeComponent(
    faultShearStressLengthChangeComponent)
{
}

CResultTree_Delegate::CFaultShearStressLengthChangeComposite_Delegate::
  CFaultShearStressLengthChangeComposite_Delegate(
    CResultTree::CFaultShearStressLengthChangeComposite*
      faultShearStressLengthChangeComposite)
: CVectorResult_Delegate(faultShearStressLengthChangeComposite)
, m_faultShearStressLengthChangeComposite(
    faultShearStressLengthChangeComposite)
{
}

CResultTree_Delegate::CRTCIStrainResult_Delegate::CRTCIStrainResult_Delegate(
  CResultTree::CRTCIStrainResult* rtciStrainResult)
: IResult_Delegate(rtciStrainResult)
, m_rtciStrainResult(rtciStrainResult)
{
}

CResultTree_Delegate::CResultTree_Delegate(CResultTree* resultTree)
: CResultGroup_Delegate(resultTree)
, m_resultTree(resultTree)
{
}
