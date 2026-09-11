// WellSection.h: interface for the CWellSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLSECTION_H__71B283EB_EBFC_424F_834D_155AABE019FB__INCLUDED_)
#define AFX_WELLSECTION_H__71B283EB_EBFC_424F_834D_155AABE019FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IWellSection.h"
#include "WellPoint.h"
#include "WellSchemeInclude.h"

//*******************************************************************
//
//  FILE:       WellSection.h
//  AUTHOR:     htg
//  PROJECT:    WellScheme
//  COMPONENT:  CWellSection
//  DATE:       17.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   -
//
//*******************************************************************

namespace well {

class CWellPathBase;
class CWellPoint;
class CWellPointList;

class CWellSection : public IWellSection {
  Q_OBJECT;

public:
  CWellSection(CWellPathBase &pWellPath, double TopTMD, double BottomTMD);
  virtual ~CWellSection();

  void SetDescription(QString description);

  // IWellSection interface
  virtual const CWellPoint &Top() const;
  virtual const CWellPoint &Bottom() const;
  virtual QString GetDescription() const;

protected:
  CWellPoint m_Top;
  CWellPoint m_Bottom;
  QString m_Description;
};
} // namespace well

#endif // !defined(AFX_WELLSECTION_H__71B283EB_EBFC_424F_834D_155AABE019FB__INCLUDED_)
