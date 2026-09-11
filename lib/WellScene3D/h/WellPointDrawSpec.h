// WellPointDrawSpec.h: interface for the CWellPointDrawSpec class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WELLPOINTDRAWSPEC_H__7446DA9C_5000_4003_8B42_140D1B615171__INCLUDED_)
#define AFX_WELLPOINTDRAWSPEC_H__7446DA9C_5000_4003_8B42_140D1B615171__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




//*******************************************************************
//              
//  FILE:       WellPointDrawSpec.h
//  AUTHOR:     htg
//  PROJECT:    TestViewExe
//  COMPONENT:  CWellPointDrawSpec
//  DATE:       22.09.2003
//  COPYRIGHT:  TNO-DIANA BV 2003
//  COMMENTS:   CWellPointDrawSpec objects can only be created by CWellSceneInterMed
//              it describes how a point on the well is shown
//*******************************************************************

namespace well {
  class CWellPoint;
}

#include "WellDrawSpecBase.h"
#include "Vector.h"

class CWellPointDrawSpec  : public CWellDrawSpecBase
{
  Q_OBJECT

friend class CWellSceneInterMed;

public:
  enum TYPE
  {
    SINGLE_DOT,
    ARROW
  };

  virtual ~CWellPointDrawSpec();

  void SetArrowDirection(const geo::CVector& dir);
private slots:
  void OnTMDChanged();
  void OnPointDestroyed(const well::CWellPoint& wellpoint);

private:
  virtual void CreateScene();

  //only CWellSceneInterMed can construct a CWellPointDrawSpec
  CWellPointDrawSpec(CWellSceneInterMed& WellSceneInterMed, well::CWellPoint& WellPoint,TYPE type = SINGLE_DOT);
  
  well::CWellPoint* m_pWellPoint;

  void			  CreateArrow();
  void			  CreateDot();
  TYPE m_Type;
  geo::CVector m_ArrowDirection;
};

#endif // !defined(AFX_WELLPOINTDRAWSPEC_H__7446DA9C_5000_4003_8B42_140D1B615171__INCLUDED_)
