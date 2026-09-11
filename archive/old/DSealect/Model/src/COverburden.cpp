// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "CDataModel.h"
#include "COverburden.h"
#include "CFormationTopography.h"
#include "DSealectGlobal.h"

//##ModelId=3C0DDDAA03C9
COverburden::COverburden(CDataModel *pDataModel)
:
m_qOBGeothermalGradient		(1.0, DOUBLE_MIN_, DOUBLE_MAX_, true, false),
m_qOBAverageDensity	(1.0, 0.0, DOUBLE_MAX_, false, false),
m_qSurfaceTemperature		(1.0, -50.0, 50.0, true, true),
m_qOverburdenBottomTVD		(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	m_pDataModel=pDataModel;
	m_qOBGeothermalGradient.Invalidate();
	m_qOBAverageDensity.Invalidate();
	m_qSurfaceTemperature.Invalidate();
	m_qOverburdenBottomTVD.Invalidate();
}

//From: Insitu Stresses
//Label: geothermal gradient
//descr: geothermal gradient, accessible if Formation = 
//overburden
//##ModelId=3C0DDDAA03CA
CTemperatureGradientQuantity *COverburden::OBGeothermalGradient() 
{
	return &m_qOBGeothermalGradient;
}

const CTemperatureGradientQuantity *COverburden::OBGeothermalGradient() const
{
	return &m_qOBGeothermalGradient;
}

CDensityQuantity *COverburden::OBAverageDensity() 
{
	return &m_qOBAverageDensity;
}

const CDensityQuantity *COverburden::OBAverageDensity() const
{
	return &m_qOBAverageDensity;
}

double COverburden::GetAverageDensity() const
{
  if(!GetModel()->DensityProfile().empty())
  {
    std::vector<std::pair<double, double> > vcValues =
      GetModel()->DensityProfile().ValuesFromTo(GetModel()->FormationTopography()->ElevationTVD()->Value(),
                                                OverburdenBottomTVD()->Value());

    assert(vcValues.size() >= 2);

    double dThicknessSum = 0;
    double dValueSum = 0;
    for(int i = 0; i < vcValues.size() - 1; ++i)
    {
      double dThickness = vcValues[i+1].first - vcValues[i].first;
      dThicknessSum += dThickness;

      double dValueAvg = (vcValues[i].second + vcValues[i+1].second) / 2.;
      dValueSum += dThickness * dValueAvg;
    }
    return dValueSum / dThicknessSum;
  }

  return OBAverageDensity()->Value();
}


//From; Insitu Stresses
//label; Surface Undisturbed Temperature
//descr: Onshore: Surface Undisturbed Temperature
//Offshore: Seabed Undisturbed Temperature,
//accessible if Formation = Overburden
//##ModelId=3C0DDDAA03DB
CTemperatureQuantity *COverburden::SurfaceTemperature() 
{
	return &m_qSurfaceTemperature;
}

const CTemperatureQuantity *COverburden::SurfaceTemperature() const
{
	return &m_qSurfaceTemperature;
}

//From: Formation Definition
//label: Overburden bottom TVD
//descr: Overburden bottom TVD
//##ModelId=3C0DDDAA03DC
CLengthQuantity *COverburden::OverburdenBottomTVD() 
{
	return &m_qOverburdenBottomTVD;
}

const CLengthQuantity *COverburden::OverburdenBottomTVD() const
{
	return &m_qOverburdenBottomTVD;
}

double COverburden::TopTVD() const
{
  return (m_pDataModel->FormationTopography()->Offshore() ?
    m_pDataModel->FormationTopography()->Seawater()->SeaBedTVD().Value() :
    m_pDataModel->FormationTopography()->ElevationTVD()->Value());
}

//##ModelId=3C2043EB030E
bool COverburden::CheckComplete() const
{
	if (m_qOBAverageDensity.Undefined() && GetModel()->DensityProfile().empty())
		return false;
		
	if (m_qOBGeothermalGradient.Undefined() && GetModel()->TemperatureProfile().empty())
		return false;

	
	if (m_qSurfaceTemperature.Undefined() && GetModel()->TemperatureProfile().empty())
		return false;

	
	if (m_qOverburdenBottomTVD.Undefined())
		return false;

	return true;
}

//##ModelId=3C8D9DF703CA
double COverburden::GetVertStressAtBottom() const
{
	ASSERT(CheckComplete());

	double dModelTopTVD = m_pDataModel->GetModelTopTVD().Value();
	double dAverageDensityOB;
	double dElevation = m_pDataModel->FormationTopography()->ElevationTVD()->Value();
  if(m_pDataModel->DensityProfile().empty())
  {
	  dAverageDensityOB = OBAverageDensity()->Value();
  }
  else
  {
    double dTopTVD = TopTVD();

    double dBottomTVD = OverburdenBottomTVD()->Value();
    CValueProfile::TDepthValueVector vcDensities =
      m_pDataModel->DensityProfile().ValuesFromTo(dTopTVD, dBottomTVD);

    double dWeightedDensity = 0;
    for(int i = 0; i < vcDensities.size() - 1; ++i)
    {
      dWeightedDensity += (vcDensities[i].second + vcDensities[i+1].second) *
        (vcDensities[i+1].first - vcDensities[i].first);
    }

    dAverageDensityOB = dWeightedDensity / (dBottomTVD - dTopTVD);
    dAverageDensityOB *= 1e-6; // kg/m3 -> Mkg/m3
  }
	double dGetVertEffStressAtOBBttm;

	if(m_pDataModel->FormationTopography()->Offshore())
	{
		double dSeaBed = m_pDataModel->FormationTopography()->Seawater()->SeaBedTVD().Value();
		dGetVertEffStressAtOBBttm =
      ACCELERATION_OF_GRAVITY * (WATER_DENSITY * (dSeaBed - dElevation) + dAverageDensityOB * (dModelTopTVD - dSeaBed));
	}
	else //onshore
	{
		dGetVertEffStressAtOBBttm=dAverageDensityOB*ACCELERATION_OF_GRAVITY*(dModelTopTVD-dElevation);
	}
	return dGetVertEffStressAtOBBttm;
}

//##ModelId=3C8D9DF8005F
double COverburden::GetGeoThermalTemperatureAtBottom() const
{
	ASSERT(CheckComplete());

  double GeoThermalTemperatureAtBottom;
  if(m_pDataModel->TemperatureProfile().empty())
  {
	  double modeltop = m_pDataModel->GetModelTopTVD().Value();
	  double gradient = OBGeothermalGradient()->Value();
	  double elevation = m_pDataModel->FormationTopography()->ElevationTVD()->Value();

	  if(m_pDataModel->FormationTopography()->Offshore())
	  {
		  double seabed = m_pDataModel->FormationTopography()->Seawater()->SeaBedTVD().Value();
		  GeoThermalTemperatureAtBottom = SurfaceTemperature()->Value() + gradient*(modeltop-seabed);
	  }
	  else //onshore
	  {
		  GeoThermalTemperatureAtBottom=SurfaceTemperature()->Value() + gradient*(modeltop-elevation);
	  }
  }
  else
  {
    GeoThermalTemperatureAtBottom = m_pDataModel->TemperatureProfile().ValueAt(OverburdenBottomTVD()->Value());
  }

	return GeoThermalTemperatureAtBottom;
}
