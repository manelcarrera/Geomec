// DCInitialStressLoad.cpp: implementation of the CInitialStressLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "DCInitialStressLoad.h"
#include "IDCLoad.h"

#include "DCLoadCase.h"

#include "IBody.h"
#include "IElement.h"
#include "LineInterface.h"
#include "Vector.h"

#include "lbcx.h"
#include "lbfl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace dia {

// same value in all nodes (tensor)
CInitialStressLoad::CInitialStressLoad(CLoadCase &loadcase, const ITensor &values, const geo::IElement &element)
    : IStructuralLoad(loadcase, 0), m_element(element), m_pvcTensors(0), m_pvcVectors(0) {
  m_pvcTensors = new std::vector<CTensor>(1);
  (*m_pvcTensors)[0] = CTensor(values);
}

// same value in all nodes (vector
CInitialStressLoad::CInitialStressLoad(CLoadCase &loadcase, const geo::IVector &values, const geo::IElement &element)
    : IStructuralLoad(loadcase, 0), m_element(element), m_pvcTensors(0), m_pvcVectors(0) {
  m_pvcVectors = new std::vector<geo::CVector>(1);
  (*m_pvcVectors)[0] = geo::CVector(values);
}

// vector of values for each node (vcValues.size() must equal to body.NrOfPoints())
CInitialStressLoad::CInitialStressLoad(CLoadCase &loadcase, const std::vector<CTensor> &vcValues,
                                       const geo::IElement &element)
    : IStructuralLoad(loadcase, 0), m_element(element), m_pvcTensors(0), m_pvcVectors(0) {
  assert(vcValues.size() == element.NrOfPoints());
  m_pvcTensors = new std::vector<CTensor>(vcValues.size());
  for (size_t i = 0; i < vcValues.size(); i++)
    (*m_pvcTensors)[i] = vcValues[i];
}

// vector of vector values for each node (vcValues.size() must equal to body.NrOfPoints())
CInitialStressLoad::CInitialStressLoad(CLoadCase &loadcase, const std::vector<geo::CVector> &vcValues,
                                       const geo::IElement &element)
    : IStructuralLoad(loadcase, 0), m_element(element), m_pvcTensors(0), m_pvcVectors(0) {
  m_pvcVectors = new std::vector<geo::CVector>(vcValues.size());
  for (size_t i = 0; i < vcValues.size(); i++)
    (*m_pvcVectors)[i] = vcValues[i];
}

CInitialStressLoad::~CInitialStressLoad() {}

bool CInitialStressLoad::WriteFilos() const {
  ftn_int_t idx = Inquire("ELEMEN", "DIM");
  if (idx < 0)
    idx = 0;
  idx++;

  assert(!XistIndexed("ELEMEN/", &idx));

  PushDir();

  ChangeIndexedDir("ELEMEN/", &idx);

  ftn_int_t idxelm = (ftn_int_t)(Element().Index() + 1);
  PutItemLength("ELEMEN", &idxelm, 1);

  ftn_int_t idxcase = (ftn_int_t)(LoadCase().Index() + 1);
  PutItem("CASE", &idxcase);

  ftn_double_t *pPrestr;

  if (m_pvcTensors != 0) {
    assert(m_pvcVectors == 0);
    pPrestr = (ftn_double_t *)DiMalloc(6 * ValueSize() * sizeof(ftn_double_t), "CInitialStressLoad::WriteFilos");
    int index = 0;
    int i;
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueTensor(i).XX();
      index++;
    }
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueTensor(i).YY();
      index++;
    }
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueTensor(i).ZZ();
      index++;
    }
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueTensor(i).XY();
      index++;
    }

    // hack for 2D / 3D elements
    if (dynamic_cast<const geo::IBody *>(&Element())) {
      for (i = 0; i < ValueSize(); i++) {
        pPrestr[index] = (ftn_double_t)ValueTensor(i).YZ();
        index++;
      }
      for (i = 0; i < ValueSize(); i++) {
        pPrestr[index] = (ftn_double_t)ValueTensor(i).XZ();
        index++;
      }
    }
    assert((index == 6 * ValueSize() && dynamic_cast<const geo::IBody *>(&Element())) || index == 4 * ValueSize());
    PutItemLength("PRESTR", pPrestr, index);
  } else {
    pPrestr = (ftn_double_t *)DiMalloc(3 * ValueSize() * sizeof(ftn_double_t), "CInitialStressLoad::WriteFilos");
    int index = 0;
    int i;
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueVector(i).X();
      index++;
    }
    for (i = 0; i < ValueSize(); i++) {
      pPrestr[index] = (ftn_double_t)ValueVector(i).Y();
      index++;
    }

    // hack for 2D interfaces
    if (!dynamic_cast<const geo::CLineInterface *>(&Element())) {
      for (i = 0; i < ValueSize(); i++) {
        pPrestr[index] = (ftn_double_t)ValueVector(i).Z();
        index++;
      }
    }
    assert((index == 3 * ValueSize() && (!dynamic_cast<const geo::CLineInterface *>(&Element()))) ||
           (index == 2 * ValueSize()));
    PutItemLength("PRESTR", pPrestr, index);
  }

  DiFree(pPrestr, "CInitialStressLoad::WriteFilos");

  PopDir();

  return true;
}

int CInitialStressLoad::ValueSize() const {
  if (m_pvcTensors != 0)
    return m_pvcTensors->size();
  else
    return m_pvcVectors->size();
}

const ITensor &CInitialStressLoad::ValueTensor(int nIndex) const {
  assert(m_pvcTensors != 0);
  return (*m_pvcTensors)[nIndex];
}

const geo::IVector &CInitialStressLoad::ValueVector(int nIndex) const {
  assert(m_pvcVectors != 0);
  return (*m_pvcVectors)[nIndex];
}

const geo::IElement &CInitialStressLoad::Element() const { return m_element; }

} // namespace dia
