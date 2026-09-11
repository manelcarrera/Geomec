// WellSection.cpp: implementation of the CWellSection class.
//
//////////////////////////////////////////////////////////////////////
#include "WellPoint.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"
#include "WellSection.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

CWellSection::CWellSection(CWellPathBase& WellPath , double TopTMD , double BottomTMD)
:IWellSection(WellPath),m_Top(WellPath,TopTMD),m_Bottom(WellPath,BottomTMD)
{

}

CWellSection::~CWellSection()
{
}

const CWellPoint& CWellSection::Top() const
{
  return m_Top;
}

const CWellPoint& CWellSection::Bottom() const
{
  return m_Bottom;
}

QString CWellSection::GetDescription() const
{
  return m_Description;
}

void CWellSection::SetDescription(QString description) 
{
  m_Description=description;
}



}//end namespace well
