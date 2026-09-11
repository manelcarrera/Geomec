// InitialStress.cpp: implementation of the CInitialStress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CDataModel.h"
#include "CBatchTable.h"
#include "InitialStress.h"

#ifdef _DEBUG
#ifdef _MSC_VER#undef THIS_FILE
static char THIS_FILE[]=__FILE__;#endif  // _MSC_VER
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=3C8D9DF50269
CInitialStress::CInitialStress(CDataModel* pDatamodel,CBatchTableRow *pBatchTableRow)
{
	m_pDatamodel=pDatamodel;
	m_pBatchTableRow=pBatchTableRow;
}

//##ModelId=3C8D9DF50274
CInitialStress::~CInitialStress()
{

}

//##ModelId=3C8D9DF50207
double CInitialStress::EffStressXX() const
{
	return m_LocalEffectiveStress.XX();
}

//##ModelId=3C8D9DF50215
double CInitialStress::EffStressYY() const
{
	return m_LocalEffectiveStress.YY();
}

//##ModelId=3C8D9DF50217
double CInitialStress::EffStressZZ() const
{
	return m_LocalEffectiveStress.ZZ();
}


//##ModelId=3C8D9DF50219
double CInitialStress::EffStressXY() const
{
	return m_LocalEffectiveStress.XY();
}


//##ModelId=3C8D9DF50224
double CInitialStress::EffStressYZ() const
{
	return m_LocalEffectiveStress.YZ();
}

//##ModelId=3C8D9DF50226
double CInitialStress::EffStressZX() const
{
	return m_LocalEffectiveStress.XZ();
}


//##ModelId=3C8D9DF50206
void CInitialStress::CalculateStress()
{
	m_LocalEffectiveStress= m_pDatamodel->GetLocalEffectiveStressesAtDepth(m_pBatchTableRow->GetDepth());
}



//##ModelId=3C8D9DF50243
double CInitialStress::PorePressure() const
{
	return m_pDatamodel->GetPorepressureAtDepth(m_pBatchTableRow->GetDepth());
}

//##ModelId=3DDA0C430023
double CInitialStress::UndrainedPorePressure() const
{
	return m_pDatamodel->GetUndrainedPorepressureAtDepth(m_pBatchTableRow->GetDepth());
}

//##ModelId=3C8D9DF50254
double CInitialStress::DrillingMudPressure() const
{
	double Depth = m_pBatchTableRow->GetDepth();
	//double FluidStressGrad = m_pDatamodel->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface()->Value()*ACCELERATION_OF_GRAVITY;
	//double FluidCompressibility =m_pDatamodel->DrillingScheme()->GetDrillingOperation(0)->MudCompressibility()->Value();
	//double FluidPressure = 0.0;

	//return MF::CalcCompFluidPressure(Depth, FluidStressGrad,FluidCompressibility,FluidPressure);
	return m_pDatamodel->DrillingMudPressureAtDepth(Depth);
}

//##ModelId=3C8D9DF50263
double CInitialStress::CementSlurryPressure() const
{
	CCementingOperation *pCO=m_pDatamodel->DrillingScheme()->GetDrillingOperation(0)->CementingOperation();

	double dAnalysisPointTMD = m_pBatchTableRow->AnalysisPoint()->TMD()->Value();

	return m_pDatamodel->CementSlurryPressureAtTMD(dAnalysisPointTMD);	
}


//##ModelId=3C8D9DF50265
double CInitialStress::CasingAxialStress() const
{
	CCasingOperation *pCaO = m_pDatamodel->DrillingScheme()->GetDrillingOperation(0)->CasingOperation();
	CCasingMaterial* pCM =m_pBatchTableRow->CasingMaterial();

	double Depth =				m_pBatchTableRow->GetDepth();
	double GuideShoeTMD =		pCaO->GuideShoeTMD()->Value();
  assert(m_pDatamodel->TMDtoTVD(GuideShoeTMD).second);
	double GuideShoeTVD =		m_pDatamodel->TMDtoTVD(GuideShoeTMD).first;
	double WeightPUnitLength =	pCM->WeightPerUnitLength()->Value();
	double OuterRadius =		pCM->CasingOuterRadius();
	double InnerRadius =		pCM->CasingInnerRadius();
	
	double AxialStressAtGuideShoe;
	double VerticalStressGradient;

	AxialStressAtGuideShoe =  m_pDatamodel->CementSlurryPressureAtTMD(GuideShoeTMD);

	if(pCaO->AdvancedAxialCasingStresses())
	{
		double hookStressAtSurface = pCaO->HookLoad()->Value() / (PI * (pow(OuterRadius,2) - pow(InnerRadius,2)));
		return hookStressAtSurface + (Depth/GuideShoeTVD) * (AxialStressAtGuideShoe - hookStressAtSurface);
	}
	else //automatic
	{
		VerticalStressGradient= ACCELERATION_OF_GRAVITY * WeightPUnitLength / (PI * (pow(OuterRadius,2) - pow(InnerRadius,2)));
		return AxialStressAtGuideShoe + VerticalStressGradient * (Depth - GuideShoeTVD);
	}
}

//##ModelId=3C8D9DF50267
double CInitialStress::WaitingOnCementFluidPressure() const
{
	double Depth = m_pBatchTableRow->GetDepth();

	return m_pDatamodel->WaitingOnCementFluidPressureAtDepth(Depth);

}

