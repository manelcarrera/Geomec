// DsbPickPointsDlg.h: interface for the CDsbPickPointsDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DSBPICKPOINTSDLG_H__E3AC605D_4102_4B8C_92B8_18618D13077E__INCLUDED_)
#define AFX_DSBPICKPOINTSDLG_H__E3AC605D_4102_4B8C_92B8_18618D13077E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h> 
#include "WellPoint.h"
#include "WellSection.h"
#include "WellSectionList.h"
#include "WellPointList.h"
#include "WellDefinitionPointList.h"
#include "NewWellPath.h"
#include "WellPointDrawSpec.h"
#include "WellSectionDrawSpec.h"
#include "WellSceneInterMed.h"
#include "WellPointPickDlg.h"


class CDsbPickPointsDlg : public CWellPointPickDlg  
{
public:
  CDsbPickPointsDlg::CDsbPickPointsDlg
  ( std::list<CNewWellPoint> *pList
  , CNewWellPathBase& wellpath
  , std::list<std::list<INewWellSection *> > Sections
  , CDoubleQuantity::UNIT unit
  , CWnd* pParent = NULL
  );

  CNewWellPoint OnCreateWellPoint
  ( CNewWellPathBase& wellpath
  , double tmd
  , bool &bDefined
  );
  virtual void OnAddDefault(std::list<CNewWellPoint> &list);

  virtual CString OnOutOfBoundaryMessage();

  virtual ~CDsbPickPointsDlg();

};



#endif // !defined(AFX_DSBPICKPOINTSDLG_H__E3AC605D_4102_4B8C_92B8_18618D13077E__INCLUDED_)
