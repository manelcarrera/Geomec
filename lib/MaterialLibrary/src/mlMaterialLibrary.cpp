#include "mlMaterialLibrary.h"

#include <cassert>

/*!
 * \namespace ml
 * \brief Contains all classes of the MaterialLibrary system
 */
namespace ml {

/*!
 * \class CMaterialLibrary
 * \brief Entry point of the MaterialLibrary system
 * Owns all ml::CMaterial objects.
 */

CMaterialLibrary::CMaterialLibrary() {}

CMaterialLibrary::~CMaterialLibrary() { Clear(); }

int CMaterialLibrary::MaterialSize() const { return int(m_vcMaterials.size()); }

const CMaterial &CMaterialLibrary::Material(size_t i) const {
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcMaterials.size());
  return *m_vcMaterials[i];
}

CMaterial &CMaterialLibrary::Material(size_t i) {
  // comparison of unsigned expression >= 0 is always true

  assert(/*i >= 0 &&*/ i < m_vcMaterials.size());
  return *m_vcMaterials[i];
}

CMaterial &CMaterialLibrary::AddMaterial(const QString &strName, CMaterial::CCreator &creator) {
  CMaterial *pMat = creator.OnCreateMaterial();
  creator.Create(strName, *pMat);

  AddMaterial(*pMat);

  return *pMat;
}

void CMaterialLibrary::AddMaterial(ml::CMaterial &mat) {
  m_vcMaterials.push_back(&mat);
  NotifyMaterialAdded(mat);
}

void CMaterialLibrary::DeleteMaterial(CMaterial &material) {
  size_t i;
  for (i = 0; i < m_vcMaterials.size(); ++i) {
    if (m_vcMaterials[i] == &material) {
      NotifyDeleteMaterial(material);
      m_vcMaterials.erase(m_vcMaterials.begin() + i);
      delete &material;
      break;
    }
  }
}

void CMaterialLibrary::Clear() {
  size_t i;
  for (i = 0; i < m_vcMaterials.size(); ++i) {
    NotifyDeleteMaterial(*m_vcMaterials[i]);
    delete m_vcMaterials[i];
  }
  m_vcMaterials.clear();
}

void CMaterialLibrary::RegisterObserver(IObserver &observer) {
  if (std::find(m_vcObservers.begin(), m_vcObservers.end(), &observer) == m_vcObservers.end())
    m_vcObservers.push_back(&observer);
}

void CMaterialLibrary::UnregisterObserver(IObserver &observer) {
  TObserverVector::iterator it = std::find(m_vcObservers.begin(), m_vcObservers.end(), &observer);
  if (it != m_vcObservers.end())
    m_vcObservers.erase(it);
}

void CMaterialLibrary::NotifyMaterialAdded(CMaterial &mat) {
  for (TObserverVector::iterator it = m_vcObservers.begin(); it != m_vcObservers.end(); ++it)
    (*it)->OnMaterialAdded(mat);
}

void CMaterialLibrary::NotifyDeleteMaterial(CMaterial &mat) {
  for (TObserverVector::iterator it = m_vcObservers.begin(); it != m_vcObservers.end(); ++it)
    (*it)->OnDeleteMaterial(mat);
}

} // namespace ml
