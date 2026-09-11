// WellSectionListBase.h: interface for the CWellSectionListBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSECTIONLISTBASE_H__F0B92149_C184_41E4_8D6A_A6FEA6731E27__INCLUDED_)
#define AFX_WELLSECTIONLISTBASE_H__F0B92149_C184_41E4_8D6A_A6FEA6731E27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include <qptrvector.h> 
#include "WellSchemeInclude.h"
#include "IWellSection.h"

namespace well {

class CWellPathBase;
class CWellSectionList;

//*******************************************************************
//              
//  FILE:       WellSectionListBase.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellSectionListBase
//  DATE:       10.05.2004
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   a list with WellSections sorted lexigraphic (top and bottom)
//				see compareItems.
//              this class is derived from QPtrList<IWellSection> for the virtual override of compareItems.
//              
//*******************************************************************
class CWellSectionListBase : public QObject// , protected QList<IWellSection*>
{

  Q_OBJECT;

public:

  typedef QList<IWellSection*>::iterator Iterator;

  //return all sections containing this point
  //autodelete of return value = false
  CWellSectionList GetSections(const CWellPoint& point, bool IncludeEdge =true) const;


  CWellSectionListBase(const CWellPathBase& WellPath,  bool bAutoDelete=true);
  virtual ~CWellSectionListBase();
  CWellSectionListBase& operator=(const CWellSectionListBase& rhs);
  CWellSectionListBase(const CWellSectionListBase& rhs);
    

  //list operations
  int NrOfSections() const;

  bool AutoDelete() const;
  void AutoDelete(bool autodelete);

  int GetIndex(IWellSection* section) const; //return -1 if not exists
  bool Exist(IWellSection* section) const; 

  const IWellSection* LookUpNext(IWellSection* section) const;
  const IWellSection* LookUpPrevious(IWellSection* section) const;


  //state
  bool AllSectionsDefined() const;
  
  //
  CWellPathBase& WellPath();
  const CWellPathBase& WellPath() const;

  QString GetDescription() const;
  
  Iterator begin() const;
  Iterator end() const;
  void clear();

public slots:
  void SetDescription(QString description);

protected:
  const CWellPathBase* m_pWellPath;
  QString m_Description;
  mutable QList<IWellSection*> m_lstSections;
  int compareItems(IWellSection* item1, IWellSection* item2 );

private:
  bool m_bAutoDelete;
};

}


#endif // !defined(AFX_WELLSECTIONLISTBASE_H__F0B92149_C184_41E4_8D6A_A6FEA6731E27__INCLUDED_)
