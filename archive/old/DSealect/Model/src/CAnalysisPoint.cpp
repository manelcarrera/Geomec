// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CDataModel.h"
#include "CAnalysisPoint.h"
#include "CFormation.h"
#include "CFormationTopography.h"


//##ModelId=3C2043F70010
CAnalysisPoint::CAnalysisPoint(const double& TMD, CDataModel* pDataModel)
: m_qTMD(TMD, 0.0, DOUBLE_MAX_, false, false),
  m_ProductionLoads(*this)
{
	m_pDataModel = pDataModel;

	m_pAPDependency1= new CAPDependency1(this);
	m_pDataModel->DependenciesManager.AddDependency(m_pAPDependency1);
}

//##ModelId=3CA0119A03E7
CAnalysisPoint::CAnalysisPoint(const CAnalysisPoint& rhs)
: m_ProductionLoads(*this)
{
	m_qTMD.Invalidate();
	m_pDataModel=rhs.m_pDataModel;
	if(!rhs.m_qTMD.Undefined())
		m_qTMD.Value(rhs.m_qTMD.Value(),CDoubleQuantity::SI_UNIT);

	m_pAPDependency1= new CAPDependency1(this);
	m_pDataModel->DependenciesManager.AddDependency(m_pAPDependency1);
	
}

//##ModelId=3CA0119B01B5
CAnalysisPoint::~CAnalysisPoint()
{
	if(m_pAPDependency1)
	{
		m_pDataModel->DependenciesManager.RemoveDependency(m_pAPDependency1);
		delete m_pAPDependency1;
	}
}

//##ModelId=3C2043F7010A
bool CAnalysisPoint::operator < (const CAnalysisPoint& rhs) const
{
	return m_qTMD.Value(CDoubleQuantity::SI_UNIT) < rhs.m_qTMD.Value(CDoubleQuantity::SI_UNIT);
}

//From: Depths for Analysis
//Label: Analysis Point TMD
//Descr: TVD will be calculated automatically
//##ModelId=3C0DDDAF02B1
CLengthQuantity *CAnalysisPoint::TMD()
{
	return &m_qTMD;
}

const CLengthQuantity *CAnalysisPoint::TMD() const
{
	return &m_qTMD;
}


//##ModelId=3C0DDDAF02B2
CProductionLoads *CAnalysisPoint::ProductionLoads()
{
	return &m_ProductionLoads;
}

const CProductionLoads *CAnalysisPoint::ProductionLoads() const
{
	return &m_ProductionLoads;
}

//##ModelId=3C0DDDAF02C0
CPerforationLoads *CAnalysisPoint::PerforationLoads()
{
	return &m_PerforationLoads;
}

const CPerforationLoads *CAnalysisPoint::PerforationLoads() const
{
	return &m_PerforationLoads;
}

//##ModelId=3C0DDDAF02C1
CFracturingLoads *CAnalysisPoint::FracturingLoads()
{
	return &m_FracturingLoads;
}

const CFracturingLoads *CAnalysisPoint::FracturingLoads() const
{
	return &m_FracturingLoads;
}

//##ModelId=3C0DDDAF02C2
CInjectionLoads *CAnalysisPoint::InjectionLoads()
{
	return &m_InjectionLoads;
}

const CInjectionLoads *CAnalysisPoint::InjectionLoads() const
{
	return &m_InjectionLoads;
}

//##ModelId=3C0DDDAF02C3
CEvacuationLoads *CAnalysisPoint::EvacuationLoads()
{
	return &m_EvacuationLoads;
}

const CEvacuationLoads *CAnalysisPoint::EvacuationLoads() const
{
	return &m_EvacuationLoads;
}

//##ModelId=3C0DDDAF02C4
CPriorToCompletionLoads *CAnalysisPoint::PriorToCompletionLoads()
{
	return &m_PriorToCompletionLoads;
}

const CPriorToCompletionLoads *CAnalysisPoint::PriorToCompletionLoads() const
{
	return &m_PriorToCompletionLoads;
}

CSinglePressure CAnalysisPoint::TestPressure() const
{
  CSinglePressure sp;

  CSinglePressure* pPressure = m_pDataModel->TestLoads()->TestPressure();
  CPressureGradient2* pGradient = m_pDataModel->TestLoads()->FluidPressureGradient();
  if(!pPressure->Undefined() && !pGradient->Undefined())
  {
    double dSurfacePressure = pPressure->Value();
    double dGradient = pGradient->Value() * 1e-3; // kPa/m -> MPa/m
    double dTopTVD;
    if(m_pDataModel->FormationTopography()->Offshore())
      dTopTVD = m_pDataModel->FormationTopography()->Seawater()->SeaBedTVD().Value();
    else
      dTopTVD = m_pDataModel->FormationTopography()->ElevationTVD()->Value();
    assert(m_pDataModel->TMDtoTVD(TMD()->Value()).second);
    double dPointTVD = m_pDataModel->TMDtoTVD(TMD()->Value()).first;
    sp.SetValue(dSurfacePressure + (dPointTVD - dTopTVD) * dGradient);
  }

  return sp;
}

//##ModelId=3CA0119A0252
bool CAnalysisPoint::CheckComplete() const
{
	
	return (CheckCompleteCompAndTest() && CheckCompleteProdAndInject() && CheckCompleteExtremeOper());
}

//##ModelId=3CA0119A01D5
bool CAnalysisPoint::CheckCompleteCompAndTest() const
{
	if(!PriorToCompletionLoads()->CheckComplete())
		return FALSE;

	if (!m_pDataModel->Control()->BalancedCompletion())
	{
		if(!PerforationLoads()->CheckComplete())
			return FALSE;
	}

	return TRUE;
	
}

//##ModelId=3CA0119A0149
bool CAnalysisPoint::CheckCompleteProdAndInject() const
{
		switch(m_pDataModel->Control()->WellType())
		{

			case CControl::OilProducer:
			case CControl::WaterSource:
				if(!ProductionLoads()->CheckComplete())
					return FALSE;

				if(m_pDataModel->Control()->Conversion())
				{
					if(!InjectionLoads()->CheckComplete())
						return FALSE;
				}
				break;

			case CControl::WaterInjector:
				if(!InjectionLoads()->CheckComplete())
					return FALSE;

				if(m_pDataModel->Control()->Conversion())
				{
					if(!ProductionLoads()->CheckComplete())
						return FALSE;
				}
				break;
			
			case CControl::GasStorage:
				if(!ProductionLoads()->CheckComplete())
					return FALSE;
				
				if(!InjectionLoads()->CheckComplete())
					return FALSE;
				
				break;
			
			case CControl::GasProducer:
			case CControl::ThermalWell:
				if(!ProductionLoads()->CheckComplete())
					return FALSE;
									
				break;
				
			case CControl::SteamInjector:
				if(!InjectionLoads()->CheckComplete())
					return FALSE;
									
				break;
		}
		return TRUE;
}

//##ModelId=3CA0119A00BB
bool CAnalysisPoint::CheckCompleteExtremeOper() const
{
	switch(m_pDataModel->Control()->WellType())
	{
		case CControl::OilProducer:
		case CControl::GasProducer:
		case CControl::ThermalWell:
			if(m_pDataModel->Control()->Fracturing())
			{
				if(!FracturingLoads()->CheckComplete())
					return FALSE;
			}
			if(m_pDataModel->Control()->Evacuation())
			{
				if(!EvacuationLoads()->CheckComplete())
					return FALSE;
			}
			break;

		
		case CControl::WaterInjector:
			if(m_pDataModel->Control()->Fracturing())
			{
				if(!FracturingLoads()->CheckComplete())
					return FALSE;
			}
			break;
		
		case CControl::WaterSource:
			break; //always true
							
		case CControl::GasStorage:
		case CControl:: SteamInjector:
			if(m_pDataModel->Control()->Evacuation())
			{
				if(!EvacuationLoads()->CheckComplete())
					return FALSE;
			}
			break;
	}
	return TRUE;
}

//##ModelId=3CA0119B035C
CAnalysisPoint::CAPDependency1::CAPDependency1(CAnalysisPoint* ap)
{
	m_pPoint = ap;
	m_qVec.push_back(ap->PerforationLoads()->CompletionPressureDiff());
}

//##ModelId=3CA0119B036A
bool CAnalysisPoint::CAPDependency1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
  if(!m_pPoint->m_pDataModel->TMDtoTVD(m_pPoint->TMD()->Value()).second)
    return false;

	if(m_pPoint->m_pDataModel->Control()->UnderBalancedCompletion())
	{
		double dDepthTVD = m_pPoint->m_pDataModel->TMDtoTVD(m_pPoint->TMD()->Value()).first;
		double dPressure = m_pPoint->m_pDataModel->WaitingOnCementFluidPressureAtDepth(dDepthTVD);
		
	    //THE RULE !!!
		return m_qVec[0]->Value() <= dPressure;
	}
	else
	{
		return TRUE;
	}
}

//##ModelId=3CA0119B036D
std::string CAnalysisPoint::CAPDependency1::GetDependencyString() const
{
	return "Completion pressure <= Pore pressure at Analysis point depth";
}
