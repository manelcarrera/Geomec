// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER

#include "Dependency.h"
#include "CDataModel.h"
//#include "DSealectDoc.h"
//#include "Global.h"
#include "CFormation.h"

#include "CDependenciesManager.h"
#include "MF.h"
#include "CFormationTopography.h"

//##ModelId=3C2043F10271
void CFormation::InitQuantities()
{
	m_qBottomTVD				= CLengthQuantity(1.0,0.0,DOUBLE_MAX_,true,false),
	m_qGeothermalGradient		= CTemperatureGradientQuantity(1.0,DOUBLE_MIN_,DOUBLE_MAX_,true,false),
	m_qMaxHorizontalStressRatio	= CNoneQuantity(1.0,0.0,DOUBLE_MAX_,false,false),
	m_qMinHorizontalStressRatio	= CNoneQuantity(1.0,0.0,DOUBLE_MAX_,true,false),
	m_qAzimuthOfSHWithNorth		= CAngleQuantity(1.0,0.0,360.0,true,false),
	m_qTopPorePressure			= CSinglePressure(1.0,0.0,DOUBLE_MAX_,false,false),
	m_qBottomPorePressure		= CSinglePressure(1.0,0.0,DOUBLE_MAX_,false,false),
	
	
	m_qBottomTVD.Invalidate();
	m_qGeothermalGradient.Invalidate();
	m_qMaxHorizontalStressRatio.Invalidate();
	m_qMinHorizontalStressRatio.Invalidate();
	m_qAzimuthOfSHWithNorth.Invalidate();
	m_qTopPorePressure.Invalidate();
	m_qBottomPorePressure.Invalidate();

	m_bPorous = true;
}

//##ModelId=3C0DDDAD013A
CFormation::CFormation(CString Name,double bottomTVD, CDataModel* pDataModel)
{
	m_pDataModel = pDataModel;
	InitQuantities();
	BottomTVD()->Value(bottomTVD,CDoubleQuantity::SI_UNIT);
	FormationName(Name);
	CreateDependencies();
}

//##ModelId=3C0DDDAD0169
CFormation::CFormation(const CFormation& rhs)
{	
	m_pDataModel= rhs.m_pDataModel;
	InitQuantities();
	m_qBottomTVD.Value(rhs.m_qBottomTVD.Value(),CDoubleQuantity::SI_UNIT);

	FormationName(rhs.FormationName());
	CreateDependencies();
}

//##ModelId=3C0DDDAD015B
CFormation& CFormation::operator = (const CFormation& rhs) 
{
	m_pDataModel= rhs.m_pDataModel;
	m_qBottomTVD.Value(rhs.m_qBottomTVD.Value(),CDoubleQuantity::SI_UNIT);
	FormationName(rhs.FormationName());
	return *this;
}

//##ModelId=3C0DDDAD014A
CFormation::~CFormation()
{
	DeleteDependencies();
}

//CLengthQuantity* BottomTVD() const
//{
//	return &m_qBottomTVD;
//}


//##ModelId=3C0DDDAD0080
void CFormation::DeleteDependencies()
{	
	
	m_pDataModel->DependenciesManager.RemoveDependency(m_pFormationDependency1);
	delete m_pFormationDependency1;

	m_pDataModel->DependenciesManager.RemoveDependency(m_pFormationDependency2);
	delete m_pFormationDependency2;

	m_pDataModel->DependenciesManager.RemoveDependency(m_pFormationDependency3);
	delete m_pFormationDependency3;
	
}


//##ModelId=3C0DDDAD007F
void CFormation::CreateDependencies()
{
	m_pFormationDependency1= new CFormationDependency1(this);
	m_pDataModel->DependenciesManager.AddDependency(m_pFormationDependency1);

	m_pFormationDependency2= new CFormationDependency2(this);
	m_pDataModel->DependenciesManager.AddDependency(m_pFormationDependency2);

	m_pFormationDependency3= new CFormationDependency3(this);
	m_pDataModel->DependenciesManager.AddDependency(m_pFormationDependency3);
}


//##ModelId=3C0DDDAD0158
bool CFormation::operator < (const CFormation& rhs) const
{
	return BottomTVD()->Value(CDoubleQuantity::SI_UNIT) < rhs.BottomTVD()->Value(CDoubleQuantity::SI_UNIT);
}

//##ModelId=3C8D9DFD0234
double CFormation::GetTopTVD() const
{
	long FormationIndex = m_pDataModel->FormationTopography()->GetFormationIndex(this);
	if(this == m_pDataModel->FormationTopography()->GetFirstFormation())
	{
		//this is the first formation
		return m_pDataModel->FormationTopography()->Overburden()->OverburdenBottomTVD()->Value();
	}
	else
	{
		return m_pDataModel->FormationTopography()->GetFormationAt(FormationIndex-1)->BottomTVD()->Value();
	}
}

//##ModelId=3C8D9DFD01C6
double CFormation::GetHeight() const
{
	return BottomTVD()->Value()-GetTopTVD();
}

const CFormationMaterial* CFormation::Material() const
{
  return Model().FormationMatList()->GetMaterial(MaterialName());
}

CFormationMaterial* CFormation::Material()
{
  return Model().FormationMatList()->GetMaterial(MaterialName());
}

CFormation::TDepthValueVector CFormation::VerticalStresses() const
{
  std::vector<std::pair<double, double> > vcDensities;
  if(Model().DensityProfile().empty())
  {
    assert(Material());
    assert(!Material()->Density()->Undefined());
    double density = Material()->Density()->Value() * 1e6; // density quantity works in Mkg/m3 (!)
    vcDensities.push_back(std::make_pair(GetTopTVD(), density));
    vcDensities.push_back(std::make_pair(BottomTVD()->Value(), density));
  }
  else
  {
    vcDensities = Model().DensityProfile().ValuesFromTo(GetTopTVD(), BottomTVD()->Value());
  }

  double dPrevStress;
	if(this == m_pDataModel->FormationTopography()->GetFirstFormation())
  {
		dPrevStress = m_pDataModel->FormationTopography()->Overburden()->GetVertStressAtBottom();
  }
  else
  {
	  long FormationIndex = m_pDataModel->FormationTopography()->GetFormationIndex(this);

	  CFormation* previous = m_pDataModel->FormationTopography()->GetFormationAt(FormationIndex-1);
    std::vector<std::pair<double, double> > vcPreviousStresses = previous->VerticalStresses();
    assert(vcPreviousStresses.size() >= 2);
	  dPrevStress = vcPreviousStresses.rbegin()->second;
  }

  std::vector<std::pair<double, double> > vcStresses;
  vcStresses.push_back(std::make_pair(vcDensities[0].first, dPrevStress));

  // stress distribution becomes quadratic due to linear density distribution
  // divide each depth section into N parts
  static const double N = 10.;
  for(int i = 0; i < vcDensities.size() - 1; ++i)
  {
    // get start and end values of section i
    std::pair<double, double>& pr0 = vcDensities[i];
    std::pair<double, double>& pr1 = vcDensities[i+1];
    for(int j = 0; j < N; ++j)
    {
      // start (0) and end (1) values of part j in section i
      double z0 = pr0.first + double(j) / N * (pr1.first - pr0.first);
      double z1 = pr0.first + double(j+1) / N * (pr1.first - pr0.first);
      double r0 = pr0.second + double(j) / N * (pr1.second - pr0.second);
      double r1 = pr0.second + double(j+1) / N * (pr1.second - pr0.second);

      double y = (r1 - r0) / (z1 - z0);

      // delta sigma for this part
      double ds = 9.81 * (0.5 * y * (z1 - z0) * (z1 - z0) + r1 * (z1 - z0));
      dPrevStress += ds * 1e-6; // Pa -> MPa
      vcStresses.push_back(std::make_pair(z1, dPrevStress));
    }
  }

  return vcStresses;
}

//##ModelId=3C8D9DFC00BD
//double CFormation::GetGeoThermalTemperatureAtBottom() const
//{
//	double gradient = GeothermalGradient()->Value();
//	return GetGeoThermalTemperatureAtTop() + gradient*GetHeight();
//}

//##ModelId=3C8D9DFC0040
//double CFormation::GetGeoThermalTemperatureAtTop() const
//{
//	if(this == m_pDataModel->FormationTopography()->GetFirstFormation())
//		return m_pDataModel->FormationTopography()->Overburden()->GetGeoThermalTemperatureAtBottom();
//
//
//	long FormationIndex = m_pDataModel->FormationTopography()->GetFormationIndex(this);
//
//	CFormation* previuos = m_pDataModel->FormationTopography()->GetFormationAt(FormationIndex-1);
//	return previuos->GetGeoThermalTemperatureAtBottom();
//}

#if 0
//##ModelId=3C8D9DFC036C
double CFormation::GetEffectiveVerticalStressAtTop() const
{
	if(Porous())
	{
		return GetVerticalStressAtTop() - TopPorePressure()->Value();
	}
	else
	{
		return GetVerticalStressAtTop();
	}
}

//##ModelId=3C8D9DFD006F
double CFormation::GetEffectiveMaxHorStressAtTop() const
{
	if(Porous())
	{
		return GetMaxHorStressAtTop() - TopPorePressure()->Value();
	}
	else
	{
		return GetMaxHorStressAtTop();
	}
}

//##ModelId=3C8D9DFD0001
double CFormation::GetEffectiveMaxHorStressAtBottom() const
{
	if(Porous())
	{
		return GetMaxHorStressAtBottom() - BottomPorePressure()->Value();
	}
	else
	{
		return GetMaxHorStressAtBottom();
	}
}

//##ModelId=3C8D9DFD00DC
double CFormation::GetEffectiveMinHorStressAtBottom() const
{
	if(Porous())
	{
		return GetMinHorStressAtBottom() - BottomPorePressure()->Value();
	}
	else
	{
		return GetMinHorStressAtBottom();
	}
}

//##ModelId=3C8D9DFD0159
double CFormation::GetEffectiveMinHorStressAtTop() const
{
	if(Porous())
	{
		return GetMinHorStressAtTop() - TopPorePressure()->Value();
	}
	else
	{
		return GetMinHorStressAtTop();
	}
}
#endif

CFormation::TDepthValueVector CFormation::HorStresses(double dK) const
{
  TDepthValueVector vcVertStresses = VerticalStresses();
  TDepthValueVector vcHorStresses(vcVertStresses.size());

  for(int i = 0; i < vcVertStresses.size(); ++i)
    vcHorStresses[i] = std::make_pair(vcVertStresses[i].first, dK * vcVertStresses[i].second);

  return vcHorStresses;
}

CFormation::TDepthValueVector CFormation::MaxHorStresses() const
{
  return HorStresses(MaxHorizontalStressRatio()->Value());
}

CFormation::TDepthValueVector CFormation::MinHorStresses() const
{
  return HorStresses(MinHorizontalStressRatio()->Value());
}

CFormation::TDepthValueVector CFormation::Temperatures() const
{
  CValueProfile::TDepthValueVector vcTemperatures;

  if(Model().TemperatureProfile().empty())
  {
    double dTopValue;
    if(this == m_pDataModel->FormationTopography()->GetFirstFormation())
    {
      dTopValue = m_pDataModel->FormationTopography()->Overburden()->GetGeoThermalTemperatureAtBottom();
    }
    else
    {
	    long FormationIndex = m_pDataModel->FormationTopography()->GetFormationIndex(this);
	    CFormation* previous = m_pDataModel->FormationTopography()->GetFormationAt(FormationIndex-1);
      CValueProfile::TDepthValueVector vcPrevTemps = previous->Temperatures();
      dTopValue = vcPrevTemps.rbegin()->second;
    }

    vcTemperatures.push_back(std::make_pair(GetTopTVD(), dTopValue));

    double dBottomValue = dTopValue + GetHeight() * GeothermalGradient()->Value();
    vcTemperatures.push_back(std::make_pair(BottomTVD()->Value(), dBottomValue));
  }
  else
  {
    vcTemperatures = Model().TemperatureProfile().ValuesFromTo(GetTopTVD(), BottomTVD()->Value());
  }

  return vcTemperatures;
}

double CFormation::ValueAtDepth(const TDepthValueVector& vcValues, double dDepth) const
{
  assert(dDepth >= vcValues.begin()->first);
  assert(dDepth <= vcValues.rbegin()->first);

  double dValue = 0;

  for(int i = 1; i < vcValues.size(); ++i)
  {
    if(dDepth < vcValues[i].first)
      dValue = vcValues[i-1].second + (dDepth - vcValues[i-1].first) / (vcValues[i].first - vcValues[i-1].first) * (vcValues[i].second - vcValues[i-1].second);
  }

  return dValue;
}

double CFormation::VerticalStressAtDepth(double dDepth) const
{
  TDepthValueVector vcStresses = VerticalStresses();
  return ValueAtDepth(vcStresses, dDepth);
}

double CFormation::MaxHorizontalStressAtDepth(double dDepth) const
{
  TDepthValueVector vcStresses = MaxHorStresses();
  return ValueAtDepth(vcStresses, dDepth);
}

double CFormation::MinHorizontalStressAtDepth(double dDepth) const
{
  TDepthValueVector vcStresses = MinHorStresses();
  return ValueAtDepth(vcStresses, dDepth);
}

double CFormation::TemperatureAtDepth(double dDepth) const
{
  TDepthValueVector vcTemperatures = Temperatures();
  return ValueAtDepth(vcTemperatures, dDepth);
}

//##ModelId=3C0DDDAD01D9
bool CFormation::CheckComplete() const
{
	if(FormationName().IsEmpty())
		return false;

	if(!CheckInsituStressesComplete())
		return false;
	
	return true;
}

//##ModelId=3C0DDDAD01E5
CString CFormation::GetInfo()
{
	// NOTE: Requires a correct return value to compile.
	return CString("Formation Info");
}


//##ModelId=3C0DDDAD019A
CTemperatureGradientQuantity* CFormation::GeothermalGradient()
{
	return &m_qGeothermalGradient;
}

const CTemperatureGradientQuantity* CFormation::GeothermalGradient() const
{
	return &m_qGeothermalGradient;
}

//##ModelId=3C0DDDAD01A7
CNoneQuantity *CFormation::MaxHorizontalStressRatio()
{
	return &m_qMaxHorizontalStressRatio;
}

const CNoneQuantity *CFormation::MaxHorizontalStressRatio() const
{
	return &m_qMaxHorizontalStressRatio;
}


//##ModelId=3C0DDDAD01A8
CNoneQuantity *CFormation::MinHorizontalStressRatio()
{
	return &m_qMinHorizontalStressRatio;
}

const CNoneQuantity *CFormation::MinHorizontalStressRatio() const
{
	return &m_qMinHorizontalStressRatio;
}

//##ModelId=3C0DDDAD01A9
CAngleQuantity *CFormation::AzimuthOfSHWithNorth()
{
	return &m_qAzimuthOfSHWithNorth;
}

const CAngleQuantity *CFormation::AzimuthOfSHWithNorth() const
{
	return &m_qAzimuthOfSHWithNorth;
}

//##ModelId=3C0DDDAD01AA
bool CFormation::Porous() const
{
	return m_bPorous;	
}

//##ModelId=3C0DDDAD01B6
void CFormation::Porous(bool bPorous)
{
	m_bPorous= bPorous;
}

//##ModelId=3C0DDDAD01B8
CSinglePressure *CFormation::TopPorePressure() 
{
	return &m_qTopPorePressure;
}

const CSinglePressure *CFormation::TopPorePressure() const
{
	return &m_qTopPorePressure;
}


//##ModelId=3C0DDDAD01B9
CSinglePressure *CFormation::BottomPorePressure() 
{
	return &m_qBottomPorePressure;
}

const CSinglePressure *CFormation::BottomPorePressure() const
{
	return &m_qBottomPorePressure;
}

double CFormation::GetActualTopPorePressure()
{
  if(!m_pDataModel->PorePressureProfile().empty())
    return m_pDataModel->PorePressureProfile().ValueAt(GetTopTVD());

  return TopPorePressure()->Value();
}

double CFormation::GetActualBottomPorePressure()
{
  if(!m_pDataModel->PorePressureProfile().empty())
    return m_pDataModel->PorePressureProfile().ValueAt(BottomTVD()->Value());

  return BottomPorePressure()->Value();
}

std::vector<std::pair<double, double> > CFormation::PorePressureValues() const
{
  if(!m_pDataModel->PorePressureProfile().empty())
    return m_pDataModel->PorePressureProfile().ValuesFromTo(GetTopTVD(), BottomTVD()->Value());

  std::vector<std::pair<double, double> > vcValues;
  vcValues.push_back(std::make_pair(GetTopTVD(), TopPorePressure()->Value()));
  vcValues.push_back(std::make_pair(BottomTVD()->Value(), BottomPorePressure()->Value()));

  return vcValues;
}

double CFormation::GetPorePressureAt(double dDepth) const
{
  std::vector<std::pair<double, double> > vcValues = PorePressureValues();

  // depth must be in this formation's range
  assert(dDepth >= vcValues[0].first);
  assert(dDepth <= vcValues[vcValues.size() - 1].first);

  double dValue;

  for(int i = 0; i < vcValues.size() - 1; ++i)
  {
    if(dDepth >= vcValues[i].first && dDepth <= vcValues[i+1].first)
    {
      double d1 = vcValues[i].first;
      double d2 = vcValues[i+1].first;
      double v1 = vcValues[i].second;
      double v2 = vcValues[i+1].second;

      dValue = v1 + (dDepth - d1) * (v2 - v1) / (d2 - d1);
      break;
    }
  }

  return dValue;
}


//##ModelId=3C60D5670204
CFormation::CFormationDependency1::CFormationDependency1(CFormation* form)
{
	m_qVec.push_back(form->MaxHorizontalStressRatio());
	m_qVec.push_back(form->MinHorizontalStressRatio());
}
//##ModelId=3C60D5670206
bool CFormation::CFormationDependency1::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;
	//THE RULE !!!
	return m_qVec[0]->Value() >= m_qVec[1]->Value();
}
//##ModelId=3C60D5670209
std::string CFormation::CFormationDependency1::GetDependencyString() const
{
	return "Max. horizontal stress >= Min. horizontal stress";
}



//##ModelId=3DDA0C4502A4
CFormation::CFormationDependency2::CFormationDependency2(CFormation* form)
{
	m_pFormation=form;
	m_qVec.push_back(&form->m_qBottomPorePressure);
	m_qVec.push_back(form->m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->MudCompressibility());
	m_qVec.push_back(form->m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface());
}

//##ModelId=3DDA0C4502A6
bool CFormation::CFormationDependency2::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	double Ppore = m_qVec[0]->Value();
	double FluidStressGrad = m_qVec[2]->Value()*ACCELERATION_OF_GRAVITY;
	double FluidCompressibility = m_qVec[1]->Value(); 
	double FluidPressure = 0.0;
	double dDepth = m_pFormation->BottomTVD()->Value();

	double Pmud = MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);

	// Pmud>Ppore is the check, but user want a warning about the parameter he enters; use this function for back calculation
	double CalcFluidStressGrad = MF::CalcFluidStressGradient(dDepth, FluidPressure, FluidCompressibility,Ppore);
	
	
	if (!(Pmud>Ppore))
	{
		QU::UNIT us = m_pFormation->m_pDataModel->DependenciesManager.GetUnit();
		CStressGradientQuantity tmpSGQ;
		tmpSGQ.Value(CalcFluidStressGrad,CDoubleQuantity::SI_UNIT);
		
		CDensityQuantity tmpDQ;
		tmpDQ.Value(CalcFluidStressGrad/ACCELERATION_OF_GRAVITY,CDoubleQuantity::SI_UNIT);
		
		m_DependencyString.clear();
		CString sVec0;
		sVec0.Format("%#1.2f",tmpSGQ.Value(us));
		
		CString sVec0dens;
		sVec0dens.Format("%#1.2f",tmpDQ.Value(us));
		m_DependencyString= "Mud density > " + sVec0dens + "\nMud pressure gradient > " + sVec0;
	}
	
	return Pmud>Ppore;

}

//##ModelId=3DDA0C4502A9
std::string CFormation::CFormationDependency2::GetDependencyString() const
{

	//return CString("Bottom Pore pressure < Mud Presure");
	return m_DependencyString;
}


//##ModelId=3DDA0C4502D8
CFormation::CFormationDependency3::CFormationDependency3(CFormation* form)
{
	m_pFormation=form;
	m_qVec.push_back(&form->m_qTopPorePressure);
	m_qVec.push_back(form->m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->MudCompressibility());
	m_qVec.push_back(form->m_pDataModel->DrillingScheme()->GetDrillingOperation(0)->MudDensityAtSurface());
}
//##ModelId=3DDA0C4502E3
bool CFormation::CFormationDependency3::CheckQuantity(const CDoubleQuantity* pQuantity) const
{
	if (RemainingsUndefined(pQuantity))
		return TRUE;

	//THE RULE !!!
	double Ppore = m_qVec[0]->Value();
	double FluidStressGrad = m_qVec[2]->Value()*ACCELERATION_OF_GRAVITY;
	double FluidCompressibility = m_qVec[1]->Value(); //DrillingScheme()->GetDrillingOperation(0)->MudCompressibility()->Value();
	double FluidPressure = 0.0;
	double dDepth = m_pFormation->GetTopTVD();

	double Pmud = MF::CalcCompFluidPressure(dDepth, FluidStressGrad,FluidCompressibility,FluidPressure);

	// Pmud>Ppore is the check, but user want a warning about the parameter he enters; use this function for back calculation
	double CalcFluidStressGrad = MF::CalcFluidStressGradient(dDepth, FluidPressure, FluidCompressibility,Ppore);
		
	if (!(Pmud>Ppore))
	{
		QU::UNIT us = m_pFormation->m_pDataModel->DependenciesManager.GetUnit();
		CStressGradientQuantity tmpSGQ;
		tmpSGQ.Value(CalcFluidStressGrad,CDoubleQuantity::SI_UNIT);
		
		CDensityQuantity tmpDQ;
		tmpDQ.Value(CalcFluidStressGrad/ACCELERATION_OF_GRAVITY,CDoubleQuantity::SI_UNIT);
		
		m_DependencyString.clear();
		CString sVec0;
		sVec0.Format("%#1.2f",tmpSGQ.Value(us));
		
		CString sVec0dens;
		sVec0dens.Format("%#1.2f",tmpDQ.Value(us));
		m_DependencyString= "Mud density > " + sVec0dens + "\nMud pressure gradient > " + sVec0;
	}
	
	return Pmud>Ppore;

}
//##ModelId=3DDA0C4502E6
std::string CFormation::CFormationDependency3::GetDependencyString() const
{
	//return CString("Top Pore pressure < Mud Presure");
	return m_DependencyString;
}





//##ModelId=3C2043F102FE
bool CFormation::CheckInsituStressesComplete() const
{
	if(m_qGeothermalGradient.Undefined() && m_pDataModel->TemperatureProfile().empty())
		return false;

	if(m_qMaxHorizontalStressRatio.Undefined())
		return false;

	if(m_qMinHorizontalStressRatio.Undefined())
		return false;

	if(m_qAzimuthOfSHWithNorth.Undefined())
		return false;

	if(m_qTopPorePressure.Undefined() && m_pDataModel->PorePressureProfile().empty())
		return false;

	if(m_qBottomPorePressure.Undefined() && m_pDataModel->PorePressureProfile().empty())
		return false;
	
	
	return true;	

}



