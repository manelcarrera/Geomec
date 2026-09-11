// DerivedWellPickDlg.h: interface for the CDerivedWellPickDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DERIVEDWELLPICKDLG_H__A968BE32_BFE6_4FA5_9125_9891A885941D__INCLUDED_)
#define AFX_DERIVEDWELLPICKDLG_H__A968BE32_BFE6_4FA5_9125_9891A885941D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CDerivedWellPoint: public well::CWellPoint
{
public:
  CDerivedWellPoint(well::CWellPathBase& WellPath, double TMD,int extra );
  virtual ~CDerivedWellPoint();

};


class CDerivedWellPickDlg : public CWellPointPickDlg  
{
public:
  CDerivedWellPickDlg(well::CWellPointList* pList,
            CWellSceneInterMed* pSceneInterMed,
            QPtrVector<well::CWellSectionList> Sections,
            CDoubleQuantity::UNIT unit = CDoubleQuantity::SI_UNIT,
            CWnd* pParent = NULL);   

  virtual well::CWellPoint*	OnCreateWellPoint(well::CWellPathBase& wellpath,double tmd);
  virtual void OnAddDefault(well::CWellPointList& list){};
  virtual ~CDerivedWellPickDlg();

};

#endif // !defined(AFX_DERIVEDWELLPICKDLG_H__A968BE32_BFE6_4FA5_9125_9891A885941D__INCLUDED_)
