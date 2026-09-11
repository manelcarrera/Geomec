// ResultComponentAdapter.cpp: implementation of the CResultComponentAdapter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif // _MSC_VER
#include "ResultAdapter.h"
#include "ResultComponentAdapter.h"
#include "geomec.h"
// #include "componententry.h"
#include "result.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResultComponentAdapter::CResultComponentAdapter(CResultAdapter &result_adapter, IResultComponent &result_component)
    : IValueComponent(result_adapter, 0,
                      ((IResult &)result_component.Parent()).Mode(result_component.Linear(), result_component.Stage())),
      m_component(result_component) {}

CResultComponentAdapter::~CResultComponentAdapter() {}

const IResultComponent &CResultComponentAdapter::ResultComponent() const { return m_component; }

IResultComponent &CResultComponentAdapter::ResultComponent() { return m_component; }

BOOL CResultComponentAdapter::Defined() const { return m_component.Defined(); }

CResultComponentAdapter::TMinMax CResultComponentAdapter::MinMax(const CQuantity::UNIT unit) const {
  return m_component.MinMax(unit);
}

CString CResultComponentAdapter::UnitName(const CQuantity::UNIT unit) const { return m_component.UnitName(unit); }

geo::CValue CResultComponentAdapter::ValuePointAt(const geo::IPoint &pt, const CQuantity::UNIT unit) const {
  return m_component.ValuePoint(pt, unit);
}

CResultComponentAdapter::TValueVec CResultComponentAdapter::ValueElementAt(const geo::IElement &elm,
                                                                           const CQuantity::UNIT unit) const {
  return m_component.ValueElement(elm, unit);
}

UINT CResultComponentAdapter::IconId() const { return m_component.IconId(); }

UINT CResultComponentAdapter::TypeNameId() const { return m_component.TypeNameId(); }

BOOL CResultComponentAdapter::Empty() const { return TRUE; }

void CResultComponentAdapter::Load(TLOCATION &location, TPROGRESS &progress) {}

void CResultComponentAdapter::Save(TLOCATION &location, TPROGRESS &progress) {}

long CResultComponentAdapter::SavedItems() const { return 0; }

CString CResultComponentAdapter::ExportLabel() const { return m_component.ExportLabel(); }