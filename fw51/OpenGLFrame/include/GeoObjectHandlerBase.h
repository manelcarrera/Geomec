/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// GeoObjectHandlerBase.h: interface for the CGeoObjectHandlerBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOOBJECTHANDLERBASE_H__7A775DF9_C75E_4648_B95D_84A087A7048D__INCLUDED_)
#define AFX_GEOOBJECTHANDLERBASE_H__7A775DF9_C75E_4648_B95D_84A087A7048D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IDrawDef.h"

#include "DisplayPair.h"
#include <QList>
#include <QObject>

#include "OpenGLFrameExports.h"

class OPENGLFRAME_EXPORT CGeoObjectHandlerBase : public QObject {
  friend class CDisplayPair; // calls RemoveObject in its destructor

  Q_OBJECT

public:
  CGeoObjectHandlerBase(bool autodelete = true);
  virtual ~CGeoObjectHandlerBase();

signals:
  void GeometryObjectUpdated(const geo::IObject *pObject, IDrawDef *pDrawDef);
  void GeometryObjectRemoved(const geo::IObject *pObject);

public:
  // construction destruction
  void Update(CDisplayPair *pObject);
  CDisplayPair *CreateNewPair();

  // construction with drawdef and array,see constructors of CDisplayPair
  CDisplayPair *CreateNewPair(CDrawDef *dd, geo::CPtrArray<geo::IObject> *ptr_array);

  // clears and deletes all objects in arrays but leave all arrays
  void ClearAllObject();

  void RemoveAllObjects();

  int Size() const;
  CDisplayPair *GetPair(int index);

  bool AutoDelete() const;

private:
  void RemoveObject(CDisplayPair *pObject); // called from CDisplayPair destructor

protected:
  QList<CDisplayPair *> m_DisplayList;
  bool m_bAutoDelete;
};

#endif // !defined(AFX_GEOOBJECTHANDLERBASE_H__7A775DF9_C75E_4648_B95D_84A087A7048D__INCLUDED_)
