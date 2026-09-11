/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DisplayPair.h: interface for the CDisplayPair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAYPAIR_H__01ACF3F7_5A38_40B1_A6E7_225549CD0B8A__INCLUDED_)
#define AFX_DISPLAYPAIR_H__01ACF3F7_5A38_40B1_A6E7_225549CD0B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif               // _MSC_VER > 1000
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include "DrawDef.h"
#include "VectorTempl.h"
#include <QObject>

#include "OpenGLFrameExports.h"

class CGeoObjectHandlerBase;

class OPENGLFRAME_EXPORT CDisplayPair {

private:
  // meber attributes
  CGeoObjectHandlerBase *m_Parent;
  geo::CPtrArray<geo::IObject> *m_pArray;
  CDrawDef *m_pDrawDef;

public:
  // construction / destruction
  CDisplayPair(CGeoObjectHandlerBase *pParent);

  // construction with drawdef and array
  // these can be null, if null,these objects are created, if not null this object remains the owner and objects are
  // deleted on destruction
  CDisplayPair(CDrawDef *, geo::CPtrArray<geo::IObject> *, CGeoObjectHandlerBase *pParent);

  virtual ~CDisplayPair();

  // interface
  geo::CPtrArray<geo::IObject> &Array();
  CDrawDef &DrawDef();

  void AddObject(geo::IObject &Object);
  void Update();
};

#endif // !defined(AFX_DISPLAYPAIR_H__01ACF3F7_5A38_40B1_A6E7_225549CD0B8A__INCLUDED_)
