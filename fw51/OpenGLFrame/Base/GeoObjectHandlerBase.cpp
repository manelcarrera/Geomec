/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "GeoObjectHandlerBase.h"
#include "dimple.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGeoObjectHandlerBase::CGeoObjectHandlerBase(bool autodelete /*=true*/) : m_bAutoDelete(autodelete) {}

CGeoObjectHandlerBase::~CGeoObjectHandlerBase() { RemoveAllObjects(); }

int CGeoObjectHandlerBase::Size() const { return m_DisplayList.count(); }

CDisplayPair *CGeoObjectHandlerBase::GetPair(int index) { return m_DisplayList.at(index); }

bool CGeoObjectHandlerBase::AutoDelete() const { return m_bAutoDelete; }

//////////////////////////////////////////////////////////////////////
CDisplayPair *CGeoObjectHandlerBase::CreateNewPair() {
  CDisplayPair *p = new CDisplayPair(this);
  m_DisplayList.append(p);
  return p;
}

//////////////////////////////////////////////////////////////////////

CDisplayPair *CGeoObjectHandlerBase::CreateNewPair(CDrawDef *dd, geo::CPtrArray<geo::IObject> *ptr_array) {
  CDisplayPair *p = new CDisplayPair(dd, ptr_array, this);
  m_DisplayList.append(p);
  return p;
}

void CGeoObjectHandlerBase::Update(CDisplayPair *pObject) {
  emit GeometryObjectUpdated(&pObject->Array(), &pObject->DrawDef());
}

void CGeoObjectHandlerBase::RemoveObject(CDisplayPair *pObject) {
  emit GeometryObjectRemoved(&pObject->Array());
  int r = m_DisplayList.removeAll(pObject);
  DIA_ASSERT(r == 1);
}

void CGeoObjectHandlerBase::ClearAllObject() {
  int i;
  for (i = 0; i < m_DisplayList.count(); i++)
    m_DisplayList.at(i)->Array().ClearAndDelete();
}

void CGeoObjectHandlerBase::RemoveAllObjects() {
  while (m_DisplayList.count()) {
    delete m_DisplayList.at(0);
  }
}
