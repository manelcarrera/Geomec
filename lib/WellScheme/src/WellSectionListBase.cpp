// WellSectionListBase.cpp: implementation of the CWellSectionListBase class.
//
//////////////////////////////////////////////////////////////////////



#include "WellSectionListBase.h"
#include "WellPoint.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "wellpathbase.h"
#include "IWellSection.h"
#include "WellSectionList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace well {

//##ModelId=3F7AA15602DA
CWellSectionListBase::CWellSectionListBase(const CWellPathBase& WellPath,  bool bAutoDelete)
: m_pWellPath(&WellPath)
{
	AutoDelete(bAutoDelete);
}

//##ModelId=3F7AA15602DD
CWellSectionListBase::~CWellSectionListBase()
{
	clear();
}

CWellSectionListBase& CWellSectionListBase::operator=(const CWellSectionListBase& rhs)
{
  m_pWellPath = rhs.m_pWellPath;
  m_Description = rhs.m_Description;
  m_lstSections = rhs.m_lstSections;
  m_bAutoDelete = false;
	return *this;
}

CWellSectionListBase::CWellSectionListBase(const CWellSectionListBase& rhs)
: QObject(),  // QObject(const QObject&) is private,
              // hence QObject(QObject* parent = 0)
  m_pWellPath(rhs.m_pWellPath),
  m_Description(rhs.m_Description),
  m_lstSections(rhs.m_lstSections),
  m_bAutoDelete(false)
{
}


//##ModelId=3F7AA15602EA
bool CWellSectionListBase::AutoDelete() const
{
	return m_bAutoDelete;
}

void CWellSectionListBase::AutoDelete(bool autodelete)
{
	m_bAutoDelete = autodelete;
}



int CWellSectionListBase::NrOfSections() const
{
	return m_lstSections.size();
}


bool CWellSectionListBase::AllSectionsDefined() const
{
	Iterator it = begin();
	while(it != end())
	{
		if(!(*it)->Defined())
			return false;

		++it;
	}
	return true;
}




int CWellSectionListBase::GetIndex(IWellSection* section) const
{
  return m_lstSections.indexOf(section);
}

bool CWellSectionListBase::Exist(IWellSection* section) const
{
	return !(GetIndex(section)==-1); 
}







//virtual
int CWellSectionListBase::compareItems(IWellSection* item1, IWellSection* item2 )
{
	IWellSection* s1 =(IWellSection*)item1;
	IWellSection* s2 =(IWellSection*)item2;

	if(*s1 == *s2)
		return 0;

	if(*s1 > *s2)
		return 1;

	return -1;

}


CWellPathBase& CWellSectionListBase::WellPath()
{
	return *const_cast<CWellPathBase*>(m_pWellPath);
}

const CWellPathBase& CWellSectionListBase::WellPath() const
{
	return *m_pWellPath;
}


QString CWellSectionListBase::GetDescription() const
{
	return m_Description;
}

void CWellSectionListBase::SetDescription(QString description) 
{
	m_Description=description;
}


CWellSectionList CWellSectionListBase::GetSections(const CWellPoint& point, bool IncludeEdge ) const
{
	CWellSectionList ret(*m_pWellPath);
	ret.AutoDelete(false);
	CWellSectionListBase::Iterator it = begin();
	while(it != end())
	{
  	IWellSection* section = *it;
		if(section->Contains(point,IncludeEdge))
		{
			ret.AddSection(section);
		}
		++it;
	}
	return ret;
}


CWellSectionList::Iterator CWellSectionListBase::begin() const
{
	return m_lstSections.begin();
}

CWellSectionList::Iterator CWellSectionListBase::end() const
{
  return m_lstSections.end();
}

const IWellSection* CWellSectionListBase::LookUpNext(IWellSection* section) const
{
  int index = m_lstSections.indexOf(section);
  if(index > -1 && index < NrOfSections())
    return m_lstSections.at(index + 1);

  return 0;
}

const IWellSection* CWellSectionListBase::LookUpPrevious(IWellSection* section) const
{
  int index = m_lstSections.indexOf(section);
  if(index > 0)
    return m_lstSections.at(index - 1);

  return 0;
}

void CWellSectionListBase::clear()
{
  if(m_bAutoDelete)
  {
    Iterator it;
    for(it = begin(); it != end(); ++it)
      delete *it;
  }

  m_lstSections.clear();
}

}
