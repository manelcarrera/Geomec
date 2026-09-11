// ExperimentData.cpp: implementation of the CExperimentData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ExperimentData.h"
#include <math.h>
#include "ProgressDlg_MFC.h"
#include "Excel8.h"
#include "GeomecStringTable.h"
#include "GlobalMessage.h"


#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAXBUF 255
#define NR_PARAMETERS_NEEDED 5

#define MAX_DIFF 0.02

CExperimentData::CExperimentData()
: m_dWeightFactor(1.0)
, m_dLowerLimitFractionElastic(0.0)
, m_dUpperLimitFractionElastic(1.0)
, m_dLowerLimitFractionPlastic(0.0)
, m_dUpperLimitFractionPlastic(1.0)
, m_eFitType(ELASTIC)
, m_bSelected(false)
{
}

CExperimentData::~CExperimentData()
{
}

CExperimentData::CExperimentData(const CExperimentData &ED)
{
	m_strFileName = ED.m_strFileName;
	m_strLabel = ED.m_strLabel;
	m_dWeightFactor = ED.m_dWeightFactor;
	m_dLowerLimitFractionElastic = ED.m_dLowerLimitFractionElastic;
	m_dUpperLimitFractionElastic = ED.m_dUpperLimitFractionElastic;
	m_dLowerLimitFractionPlastic = ED.m_dLowerLimitFractionPlastic;
	m_dUpperLimitFractionPlastic = ED.m_dUpperLimitFractionPlastic;
        m_eFitType = ED.m_eFitType;
        m_bSelected= ED.m_bSelected;

	m_vcStressStrainStep.resize(ED.m_vcStressStrainStep.size());
	for(size_t i=0; i<ED.m_vcStressStrainStep.size(); ++i)
	{
		m_vcStressStrainStep[i] = ED.m_vcStressStrainStep[i];
	}
}

CExperimentData& CExperimentData::operator =(const CExperimentData &ED)
{
	m_strFileName = ED.m_strFileName;
	m_strLabel = ED.m_strLabel;
	m_dWeightFactor = ED.m_dWeightFactor;
	m_dLowerLimitFractionElastic = ED.m_dLowerLimitFractionElastic;
	m_dUpperLimitFractionElastic = ED.m_dUpperLimitFractionElastic;
	m_dLowerLimitFractionPlastic = ED.m_dLowerLimitFractionPlastic;
	m_dUpperLimitFractionPlastic = ED.m_dUpperLimitFractionPlastic;
        m_eFitType = ED.m_eFitType;
        m_bSelected= ED.m_bSelected;

	m_vcStressStrainStep.resize(ED.m_vcStressStrainStep.size());
	for(size_t i=0; i<ED.m_vcStressStrainStep.size(); ++i)
	{
		m_vcStressStrainStep[i] = ED.m_vcStressStrainStep[i];
	}

	return *this;
}

BOOL CExperimentData::GetUnitString(Range &cells, int iRow, int iCol, CString &strUnit)
{
	Range cell;
	VARIANT val;
	BOOL bValid;

	COleVariant vCol((long) iCol);
	COleVariant vRow((long) iRow);

  VARIANT var1 = cells.GetItem(vRow, vCol);
	cell.AttachDispatch(V_DISPATCH(&var1));
	val = cell.GetValue();
	bValid = (val.vt == VT_BSTR);

	strUnit.Empty();
	if(bValid) strUnit = val.bstrVal;

	if(strUnit.IsEmpty())
	{
		CString msg;
		msg.Format("No valid unit identifier found in cell (%d, %d)", iCol, iRow);
		_m()->msg(msg, MB_OK);
		return FALSE;
	}

	return TRUE;
}

BOOL CExperimentData::IsUnit(CString strCheck, CString strUnit)
{
	// This function checks if strUnit contains one of the unit strings represented by strCheck.
	// The string represented by strCheck can contain several possibilities, separated by commas (e.g. 'h,hr,hour').
	// The string being compared (i.e. strUnit) may contain more characters than just the exact string (e.g. [hr]).

	assert(strCheck.GetLength());
	if(!strCheck.GetLength()) return FALSE;

	// make both strings uppercase, so check is case insensitive
	strCheck.MakeUpper();
	strUnit.MakeUpper();

	int i=0;
	int j;

	do
	{
		for(j=i+1; j<strCheck.GetLength() && strCheck[j] != ','; j++);
		CString strPart = strCheck.Mid(i, j-i);
		if(strUnit.Find(strPart) != -1)
		{
			return TRUE;
		}
		i = j + 1; // skip comma
	} while(i < strCheck.GetLength());

	return FALSE;
}

BOOL CExperimentData::GetStressUnitFactor(CString strUnit, int iCol, double &dUnitFac)
{
	if(IsUnit("mpa", strUnit)) dUnitFac = 1;
	else if(IsUnit("kap", strUnit)) dUnitFac = 1.e-3; 
	else if(IsUnit("pa", strUnit)) dUnitFac = 1.e-6;
	else if(IsUnit("psi", strUnit)) dUnitFac = 0.006894757;
	else if(IsUnit("b,bar", strUnit)) dUnitFac = 1.e-1;
	else
	{
		CString msg;
		msg.Format("Unrecognized unit found in cell (%d, %d): '%s'", iCol, 2, strUnit);
		_m()->msg(msg, MB_OK);
		return FALSE;
	}

	return TRUE;
}

int CExperimentData::GetValueFromCell(Range &cells, int iCol, int iRow, double &dValue)
{
	// return values:
	//  0: error getting value
	//  1: OK
	// -1: empty cell

	COleVariant vCol((long) iCol);
	COleVariant vRow((long) iRow);
	Range cell;
	VARIANTARG val;

  VARIANT var1 = cells.GetItem(vRow, vCol);
	cell.AttachDispatch(V_DISPATCH(&var1));
	val = cell.GetValue();

	if(val.vt == VT_EMPTY) return -1;

	HRESULT hr = VariantChangeType(&val, &val, 0, VT_R8);

	if(hr != S_OK)
	{
		CString msg;
		msg.Format("An invalid value was found in cell (%d, %d)", iCol, iRow);
		_m()->msg(msg, MB_OK);
		return 0;
	}

	dValue = val.dblVal;
	return 1;
}

BOOL CExperimentData::IsCellEmpty(Range &cells, int iCol, int iRow)
{
	COleVariant vCol((long) iCol);
	COleVariant vRow((long) iRow);
	Range cell;
	VARIANTARG val;

  VARIANT var1 = cells.GetItem(vRow, vCol);
	cell.AttachDispatch(V_DISPATCH(&var1));
	val = cell.GetValue();

	return (val.vt == VT_EMPTY);
}

BOOL CExperimentData::ReadFromFile(_Worksheet &ws, CString strLabel, CWnd *pParentWnd)
{
	Range cells, cell;
	cells.AttachDispatch(ws.GetCells());

	BOOL bContinue = TRUE;

	int iColTime = 0;
	int iColAxStress = 0;
	int iColRadStress = 0;
	int iColAxStrain = 0;
	int iColRadStrain = 0;
	int iColPorePres = 0;

	VARIANT val;
	COleVariant vRow1((long) 1);
	int iCol;
	int iRow;

	CString strUnit;

	double dUnitFacTime = 1;
	double dUnitFacAxStress = 1;
	double dUnitFacRadStress = 1;
	double dUnitFacPorePres = 1;

	for(iCol=1;; iCol++)
	{
		COleVariant vCol((long) iCol);
    VARIANT var1 = cells.GetItem(vRow1, vCol);
		cell.AttachDispatch(V_DISPATCH(&var1));
		val = cell.GetValue();
		if(val.vt != VT_BSTR) break;
		CString strHeader = val.bstrVal;
		strHeader.TrimLeft();
		strHeader.TrimRight();
		if(strHeader.IsEmpty()) break;

		if(!strHeader.CompareNoCase(getColumnHeader(Time))) iColTime = iCol;
		else if(!strHeader.CompareNoCase(getColumnHeader(AxialStress))) iColAxStress = iCol;
		else if(!strHeader.CompareNoCase(getColumnHeader(RadialStress))) iColRadStress = iCol;
		else if(!strHeader.CompareNoCase(getColumnHeader(AxialStrain))) iColAxStrain = iCol;
		else if(!strHeader.CompareNoCase(getColumnHeader(RadialStrain))) iColRadStrain = iCol;
		else if(!strHeader.CompareNoCase(getColumnHeader(PorePressure))) iColPorePres = iCol;
		else continue;
	}

	if(!iColAxStress || !iColRadStress || !iColAxStrain || !iColRadStrain)
	{
		CString msg;
		msg.Format("The sheet '%s' does not contain valid experiment data", strLabel);
		_m()->msg(msg, MB_OK);
		return FALSE;
	}

	// get unit for time
	if(iColTime)
	{
		if(!GetUnitString(cells, 2, iColTime, strUnit)) return FALSE;
		if(IsUnit("d,day,days", strUnit)) dUnitFacTime = 24 * 60 * 60;
		else if(IsUnit("h,hs,hr,hrs,hour,hours", strUnit)) dUnitFacTime = 60 * 60;
		else if(IsUnit("m,min,mins,minute,minutes", strUnit)) dUnitFacTime = 60;
		else if(IsUnit("s,sec,secs,second,seconds", strUnit)) dUnitFacTime = 1;
		else
		{
			CString msg;
			msg.Format("Unrecognized unit found in cell (%d, %d): '%s'", iColTime, 2, strUnit);
			_m()->msg(msg, MB_OK);
			return FALSE;
		}
	}

	// get unit for axial stress
	if(!GetUnitString(cells, 2, iColAxStress, strUnit)) return FALSE;
	if(!GetStressUnitFactor(strUnit, iColAxStress, dUnitFacAxStress)) return FALSE;

	// get unit for radial stress
	if(!GetUnitString(cells, 2, iColRadStress, strUnit)) return FALSE;
	if(!GetStressUnitFactor(strUnit, iColRadStress, dUnitFacRadStress)) return FALSE;

	// get unit for pore pressure
	if(iColPorePres)
	{
		if(!GetUnitString(cells, 2, iColPorePres, strUnit)) return FALSE;
		if(!GetStressUnitFactor(strUnit, iColPorePres, dUnitFacPorePres)) return FALSE;
	}

	CProgressDlg_MFC ProgressDlg(strLabel, pParentWnd);
	ProgressDlg.StartDialog();

	// get maximum number of rows for progress indicator
	int nMaxRows = 0;
	for(iRow=3;; iRow++)
	{
		// check if stresses and strains still have values
		if(IsCellEmpty(cells, iColAxStress, iRow)) break;
		if(IsCellEmpty(cells, iColRadStress, iRow)) break;
		if(IsCellEmpty(cells, iColAxStrain, iRow)) break;
		if(IsCellEmpty(cells, iColRadStrain, iRow)) break;
		nMaxRows++;
	}

	for(iRow=3; iRow<3+nMaxRows; iRow++)
	{
		double dPorePres = 0;
		double dTime = 0;
		double dAxStress;
		double dRadStress;
		double dAxStrain;
		double dRadStrain;
		int iRet;

		if(iColPorePres)
		{
			iRet = GetValueFromCell(cells, iColPorePres, iRow, dPorePres);
			if(iRet == -1) dPorePres = 0;
			if(!iRet)
			{
				ProgressDlg.DestroyWindow();
				return FALSE;
			}
		}

		if(iColTime)
		{
			iRet = GetValueFromCell(cells, iColTime, iRow, dTime);
			if(iRet == -1) dTime = 0;
			if(!iRet)
			{
				ProgressDlg.DestroyWindow();
				return FALSE;
			}
		}

		iRet = GetValueFromCell(cells, iColAxStress, iRow, dAxStress);
		if(!iRet)
		{
			ProgressDlg.DestroyWindow();
			return FALSE;
		}

		iRet = GetValueFromCell(cells, iColRadStress, iRow, dRadStress);
		if(!iRet)
		{
			ProgressDlg.DestroyWindow();
			return FALSE;
		}

		iRet = GetValueFromCell(cells, iColAxStrain, iRow, dAxStrain);
		if(!iRet)
		{
			ProgressDlg.DestroyWindow();
			return FALSE;
		}

		iRet = GetValueFromCell(cells, iColRadStrain, iRow, dRadStrain);
		if(!iRet)
		{
			ProgressDlg.DestroyWindow();
			return FALSE;
		}

		// convert
		dPorePres *= dUnitFacPorePres;
		dTime *= dUnitFacTime;
		dAxStress *= dUnitFacAxStress;
		dRadStress *= dUnitFacRadStress;

		// make effective stresses
		if(iColPorePres)
		{
			dAxStress -= dPorePres;
			dRadStress -= dPorePres;
		}

		m_vcStressStrainStep.push_back(CStressStrain(dAxStress, dAxStrain, dRadStress, dRadStrain, dPorePres, dTime));
		ProgressDlg.SetProgress(iRow, nMaxRows);
	}

	ProgressDlg.DestroyWindow();
	m_strLabel = strLabel;

	return TRUE;
}

const CStressStrainArray& CExperimentData::GetStressStrainSteps() const
{
  return m_vcStressStrainStep;
}

double CExperimentData::WeightFactor() const
{
  return m_dWeightFactor;
}

void CExperimentData::Selected(bool bSelected)
{
  m_bSelected = bSelected;
}

bool CExperimentData::Selected() const
{
  return m_bSelected;
}

void CExperimentData::SetFitType(FitType eFitType)
{
  m_eFitType = eFitType;
}

namespace
{

const CString TIME = "Time";
const CString AXIAL_STRESS = "Axial Stress";
const CString RADIAL_STRESS = "Radial Stress";
const CString AXIAL_STRAIN = "Axial Strain";
const CString RADIAL_STRAIN = "Radial Strain";
const CString PORE_PRESSURE = "Pore Pressure";
const CString INVALID_COLUMN = "Invalid Column";

} // anonymous namespace

const CString& CExperimentData::getColumnHeader(
  const ExperimentDataType& experimentDataType)
{
  switch (experimentDataType)
  {
  case Time:
    return TIME;
  case AxialStress:
    return AXIAL_STRESS;
  case RadialStress:
    return RADIAL_STRESS;
  case AxialStrain:
    return AXIAL_STRAIN;
  case RadialStrain:
    return RADIAL_STRAIN;
  case PorePressure:
    return PORE_PRESSURE;
  default:
    assert(false);
  }

  return INVALID_COLUMN;
}

BOOL CExperimentData::GetParameterFromString(const char *str, double &Param)
{
	const char *p = str;

	while(p && *p && !isdigit(*p) && *p != '.' && *p != '-' && *p != '+') p++;

	if(!p || !*p) return FALSE;
	Param = atof(p);

	return TRUE;
}

