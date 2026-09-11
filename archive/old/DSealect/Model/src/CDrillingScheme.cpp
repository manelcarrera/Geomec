// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif  // _MSC_VER
#include <set>

#include "CDrillingScheme.h"
#include "CDependenciesManager.h"
#include "DSealectGlobal.h"
#include "DSealectDoc.h"
#include "CDataModel.h"
#include "COverburden.h"
#include "CFormationTopography.h"

//##ModelId=3C0DDDAD030E
CDrillingScheme::CDrillingScheme(CDataModel& model)
: m_model(model),
  m_qOverburdenBottomTMD(1.0, 0.0, DOUBLE_MAX_, false, false)
{
	AddDrillingOperation();
	AddDrillingDirectionSection();
	m_qOverburdenBottomTMD.Invalidate();
}

//##ModelId=3C0DDDAD030F
CDrillingScheme::~CDrillingScheme()
{
	for(int i=0;i<m_vecDrillingDirectionSection.size();i++)
	{
		delete m_vecDrillingDirectionSection[i];
	}

	for(int i=0;i<m_vecDrillingOperation.size();i++)
	{
		delete m_vecDrillingOperation[i];
	}
	
}

CLengthQuantity* CDrillingScheme::OverburdenBottomTMD()
{
  if(m_model.FormationTopography())
  {
    CFormationTopography* pFormations = m_model.FormationTopography();
    COverburden* pOverburden = pFormations->Overburden();

    if(!pOverburden || pOverburden->OverburdenBottomTVD()->Undefined() || !m_model.IsPolyLineCreated())
      m_qOverburdenBottomTMD.Invalidate();
    else if(m_qOverburdenBottomTMD.Undefined())
    {
      CLengthQuantity* pTVD = pFormations->Overburden()->OverburdenBottomTVD();
      double dTVD = pTVD->Value();
      for(int i = 0; i < m_vecDrillingDirectionSection.size(); ++i)
      {
        CDrillingDirectionSection* pSection = m_vecDrillingDirectionSection[i];
        double dBottomTMD = pSection->DrillingSectionBottomTMD()->Value();
        std::pair<double, bool> prBottomTVD = m_model.TMDtoTVD(dBottomTMD);
        if(prBottomTVD.second && prBottomTVD.first > dTVD)
        {
          double dBottomTVD = prBottomTVD.first;
          double dTopTMD = pSection->DrillingSectionTopTMD()->Value();
          std::pair<double, bool> prTopTVD = m_model.TMDtoTVD(dTopTMD);

          if(prTopTVD.second)
          {
            double dTopTVD = prTopTVD.first;
            double dTMD = dTopTMD + ((dBottomTMD - dTopTMD) * (dTVD - dTopTVD) / (dBottomTVD - dTopTVD));
            m_qOverburdenBottomTMD.SetValue(dTMD);
            break;
          }
        }
      }
    }
  }
  else
    m_qOverburdenBottomTMD.Invalidate();

  return &m_qOverburdenBottomTMD;
}

const CLengthQuantity* CDrillingScheme::OverburdenBottomTMD() const
{
  return (const_cast<CDrillingScheme*>(this))->OverburdenBottomTMD();
}

//##ModelId=3C0DDDAD031D
CDrillingDirectionSection* CDrillingScheme::GetDrillingDirectionSection(int Index)
{
	ASSERT(Index>=0 && Index<m_vecDrillingDirectionSection.size());
	return m_vecDrillingDirectionSection[Index];
}

const CDrillingDirectionSection* CDrillingScheme::GetDrillingDirectionSection(int Index) const
{
  return (const_cast<CDrillingScheme*>(this))->GetDrillingDirectionSection(Index);
}

CDrillingDirectionSection* CDrillingScheme::GetDrillingDirectionSectionAt(double dTMD)
{
  CDrillingDirectionSection* pDDS = 0;
  if(dTMD >= 0)
  {
    for(int i = 0; i < m_vecDrillingDirectionSection.size() && !pDDS; ++i)
    {
      if(dTMD < m_vecDrillingDirectionSection[i]->DrillingSectionBottomTMD()->Value())
        pDDS = m_vecDrillingDirectionSection[i];
    }
  }

  return pDDS;
}

const CDrillingDirectionSection* CDrillingScheme::GetDrillingDirectionSectionAt(double dTMD) const
{
  return (const_cast<CDrillingScheme*>(this))->GetDrillingDirectionSectionAt(dTMD);
}

//##ModelId=3C0DDDAD032D
CDrillingOperation* CDrillingScheme::GetDrillingOperation(int Index)
{
	ASSERT(Index>=0 && Index<m_vecDrillingOperation.size());
	return m_vecDrillingOperation[Index];
}

const CDrillingOperation* CDrillingScheme::GetDrillingOperation(int Index) const
{
  return (const_cast<CDrillingScheme*>(this))->GetDrillingOperation(Index);
}

//##ModelId=3C0DDDAD033D
bool CDrillingScheme::AddDrillingOperation()
{
	CDrillingOperation* pDO = new CDrillingOperation;
	m_vecDrillingOperation.push_back(pDO);
	return true;
}

//##ModelId=3C2043F201C6
bool CDrillingScheme::RemoveDrillingOperation(int Index)
{
	ASSERT(FALSE);
	return true;
}

//##ModelId=3C0DDDAD0340
bool CDrillingScheme::AddDrillingDirectionSection()
{
	CDrillingDirectionSection* pDDS = new CDrillingDirectionSection;
	m_vecDrillingDirectionSection.push_back(pDDS);
	return true;
}

//##ModelId=3C2043F202DF
bool CDrillingScheme::RemoveDrillingDirectionSection(int Index)
{
  if(Index < m_vecDrillingDirectionSection.size())
  {
    delete m_vecDrillingDirectionSection[Index];
    m_vecDrillingDirectionSection.erase(m_vecDrillingDirectionSection.begin() + Index);
  }

	return false;
}

void CDrillingScheme::RemoveAllDrillingDirectionSections()
{
  for(int i = 0; i < m_vecDrillingDirectionSection.size(); ++i)
    delete m_vecDrillingDirectionSection[i];
  m_vecDrillingDirectionSection.clear();
}

//##ModelId=3C2043F20129
int CDrillingScheme::NrOfDrillingOperations() const
{
	return m_vecDrillingOperation.size();
}

//##ModelId=3C2043F20177
int CDrillingScheme::NrOfDrillingDirections() const
{
	return m_vecDrillingDirectionSection.size();
}


//##ModelId=3CA0118C003E
bool CDrillingScheme::CheckComplete() const
{
	if(!GetDrillingOperation(0)->CheckComplete())
		return false;

  if(!NrOfDrillingDirections())
    return false;

	return true;

}
