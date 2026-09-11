/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "TestFormation.h"
#include "TestHorizon.h"
#include "TestIModelObjectReceiver.h"
#include "TestMaterial.h"
#include <cassert>

namespace test_lib {

CFormation::CFormation(QTextStream &os) : IModelObject(), m_pUpper(0), m_pLower(0), m_pMaterial(0) {
  new CModelObjectReceiver(*this, os);
}

CFormation::CFormation(const QString &sName, QTextStream &os)
    : IModelObject(sName), m_pUpper(0), m_pLower(0), m_pMaterial(0) {
  new CModelObjectReceiver(*this, os);
}

CFormation::~CFormation() {}

void CFormation::referenceRemoved(const IModelObject &origin, const IModelObject &reference) {
  if (m_pUpper == &reference)
    m_pUpper = 0;
  if (m_pLower == &reference)
    m_pLower = 0;
  if (m_pMaterial == &reference)
    m_pMaterial = 0;
  IModelObject::referenceRemoved(origin, reference);
}

CHorizon *CFormation::Upper() { return m_pUpper; }

CHorizon *CFormation::Lower() { return m_pLower; }

void CFormation::Upper(CHorizon *pUpper) {
  if (m_pUpper)
    removeReference(*m_pUpper);
  assert(m_pUpper == 0);
  m_pUpper = pUpper;
  if (pUpper)
    insertReference(*pUpper);
}

void CFormation::Lower(CHorizon *pLower) {
  if (m_pLower)
    removeReference(*m_pLower);
  assert(m_pLower == 0);
  m_pLower = pLower;
  if (pLower)
    insertReference(*pLower);
}

CMaterial *CFormation::Material() { return m_pMaterial; }

void CFormation::Material(CMaterial *pMaterial) {
  if (m_pMaterial)
    removeReference(*m_pMaterial);
  assert(m_pMaterial == 0);
  m_pMaterial = pMaterial;
  if (pMaterial)
    insertReference(*pMaterial);
}

void CFormation::storeReferences(TStream &stream) const {
  IModelObject::storeReferences(stream);
  int index = -1;
  if (m_pUpper)
    index = isReferenced(*m_pUpper);
  stream << index;
  index = -1;
  if (m_pLower)
    index = isReferenced(*m_pLower);
  stream << index;
  index = -1;
  if (m_pMaterial)
    index = isReferenced(*m_pMaterial);
  stream << index;
}

void CFormation::restoreReferences(TStream &stream, const CStreamVersion &fileVersion) {
  IModelObject::restoreReferences(stream, fileVersion);
  int index;
  stream >> index;
  if (index == -1)
    m_pUpper = 0;
  else
    m_pUpper = (CHorizon *)&referenceAt(index);
  stream >> index;
  if (index == -1)
    m_pLower = 0;
  else
    m_pLower = (CHorizon *)&referenceAt(index);
  stream >> index;
  if (index == -1)
    m_pMaterial = 0;
  else
    m_pMaterial = (CMaterial *)&referenceAt(index);
}

CFormationContainer::CFormationContainer(QTextStream &os) : CModelContainer<CFormation>() {
  new CModelObjectReceiver(*this, os);
}

CFormationContainer::CFormationContainer(const QString &text, QTextStream &os) : CModelContainer<CFormation>(text) {
  new CModelObjectReceiver(*this, os);
}

} // namespace test_lib