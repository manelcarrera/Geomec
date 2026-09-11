/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// DisplayPair.cpp: implementation of the CDisplayPair class.
//
//////////////////////////////////////////////////////////////////////
#include "dimple.h"

#include "DisplayPair.h"
#include "GeoObjectHandlerBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDisplayPair::CDisplayPair(CGeoObjectHandlerBase *Parent) : m_Parent(Parent) {
  m_pArray = new geo::CPtrArray<geo::IObject>;
  m_pDrawDef = new CDrawDef();
}

CDisplayPair::CDisplayPair(CDrawDef *dd, geo::CPtrArray<geo::IObject> *ptr_array, CGeoObjectHandlerBase *Parent)
    : m_Parent(Parent) {
  if (!dd)
    m_pDrawDef = new CDrawDef();
  else
    m_pDrawDef = dd;

  if (!ptr_array)
    m_pArray = new geo::CPtrArray<geo::IObject>;
  else
    m_pArray = ptr_array;
}

CDisplayPair::~CDisplayPair() {
  m_Parent->RemoveObject(this);

  // only delete geometry objects in autodelete mode
  if (m_Parent->AutoDelete())
    m_pArray->ClearAndDelete();

  delete m_pArray;
  delete m_pDrawDef;
}

void CDisplayPair::Update() {
  assert(m_Parent);
  assert(m_pArray);

  m_Parent->Update(this);
}

geo::CPtrArray<geo::IObject> &CDisplayPair::Array() { return *m_pArray; }

CDrawDef &CDisplayPair::DrawDef() { return *m_pDrawDef; }

void CDisplayPair::AddObject(geo::IObject &Object) { m_pArray->PushBack(Object); }
