// VectorValueSet.cpp: implementation of the CVectorValueSet class.
//
//////////////////////////////////////////////////////////////////////
#include "VectorValueSet.h"
#include "DepletionStage.h"
#include "IInterfaceElement.h"
#include "IValueSet.h"
#include "MeshBase.h"
#include "ResultRegister.h"
#include "ScalarValueSet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*!
  Constructor of the tensor result
*/
CVectorValueSet::CVectorValueSet(const CResultRegister &parent, const CDepletionStage &stage,
                                 const CAnalysisType &antype, int nStart, bool bShear, bool bInterface, bool bChange)
    : m_nStart(nStart), m_antype(antype), m_bShear(bShear), m_bInterface(bInterface), m_bChange(bChange),
      m_pStage(&stage), m_pParent(&parent) {}

// This function will filter out all interface elements, if we don't want them and vice versa.
// This is called from IValueSetTemplate::ValuePoint. This was implemented to make sure that interpolated
// values are not invalid just because one (or more) of the elements that a point lies in
// is an interface element.
void CVectorValueSet::FilterElements(std::vector<int> &indices) const {
  for (int i = indices.size() - 1; i >= 0; --i) {
    const geo::IInterfaceElement *ie = dynamic_cast<const geo::IInterfaceElement *>(&ElementSet().Element(indices[i]));
    if ((m_bInterface && !ie) || (!m_bInterface && ie)) {
      indices.erase(indices.begin() + i);
    }
  }
}

/*!
  Returns the mesh assiociated to the tensor valueset.
*/
const geo::IElementSet &CVectorValueSet::ElementSet() const {
  assert(m_pParent);
  return m_pParent->Mesh().Mesh();
}

/*!
  Returns a pointer to the array of values of the result register
*/
const double *CVectorValueSet::ValuesFromCache(int nElementIndex, const CAnalysisType &antype,
                                               const CDepletionStage &stage, int nStart) const {
  return m_pParent->ValuesFromCache(nElementIndex, m_pParent->ColumnNumber(antype, stage.Index(), nStart));
}

/*!
  returns the tensor values of the element
*/
void CVectorValueSet::ElementValues(TValueVec &values, int iElementIndex,
                                    geo::IParallelInitializationCallback * /*cb*/) const {
  assert(values.size() == ElementSet().Element(iElementIndex).NrOfNodes());

  const geo::IInterfaceElement *pInterface =
      dynamic_cast<const geo::IInterfaceElement *>(&ElementSet().Element(iElementIndex));

  geo::CVector normal;
  if (pInterface && m_bInterface && m_bShear) {
    const geo::IFace *pFace = dynamic_cast<const geo::IFace *>(&pInterface->Front());
    if (pFace)
      normal = pFace->Normal().UnitVector();
    const geo::ILine *pLine = dynamic_cast<const geo::ILine *>(&pInterface->Front());
    if (pLine)
      normal = pLine->Dir().CrossProduct(geo::CVector::Zaxis).UnitVector();
  }

  if (!ElementValuesAvailable(iElementIndex) && (pInterface != 0) == m_bInterface) {
    // Result does not exist
    for (size_t i = 0; i < values.size(); i++)
      values[i] = geo::CVector();
    return;
  }

  // Read in tensor
  const double *pVectorVal[3];

  for (int i = 0; i < 3; i++) {
    pVectorVal[i] = ValuesFromCache(iElementIndex, m_antype, *m_pStage, m_nStart + i);
    assert(pVectorVal[i]);
  }

  if (m_bChange) {
    const CDepletionStage *pIniStage = &m_pParent->DepletionStageEntry().MarkedAsInitialStage();

    const double *pInitVectorVal[6];
    for (int i = 0; i < 3; i++) {
      pInitVectorVal[i] = ValuesFromCache(iElementIndex, m_antype, *pIniStage, m_nStart + i);
      assert(pInitVectorVal[i]);
    }

    for (size_t i = 0; i < values.size(); i++) {
      values[i] = geo::CVector(pVectorVal[0][i] - pInitVectorVal[0][i], pVectorVal[1][i] - pInitVectorVal[1][i],
                               pVectorVal[2][i] - pInitVectorVal[2][i]);
      if (m_bShear)
        values[i] = values[i] - normal * values[i].DotProduct(normal);
    }
  } else {
    for (size_t i = 0; i < values.size(); i++) {
      values[i] = geo::CVector(pVectorVal[0][i], pVectorVal[1][i], pVectorVal[2][i]);
      if (m_bShear)
        values[i] = values[i] - normal * values[i].DotProduct(normal);
    }
  }
}

/*!
  Returns true the tensor is not available
*/
bool CVectorValueSet::IsEmpty() const { return true; }

bool CVectorValueSet::ElementValuesAvailable(int nElementIndex) const {
  int i;
  for (i = 0; i < 3; i++) {
    if (!m_pParent->ResultAvailable(*m_pStage, m_antype, nElementIndex, m_nStart + i))
      return false;
  }

  if (m_bChange) {
    const CDepletionStage *pIniStage = &m_pParent->DepletionStageEntry().MarkedAsInitialStage();

    for (i = 0; i < 3; i++) {
      if (!m_pParent->ResultAvailable(*pIniStage, m_antype, nElementIndex, m_nStart + i))
        return false;
    }
  }

  return true;
}

void CVectorValueSet::MapValueElement(const geo::IElement &elm, TValueVec &values, TMapType map_type,
                                      geo::IParallelInitializationCallback *cb) const {
  assert(values.size() == elm.NrOfPoints());
  const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&elm);
  if (map_type == CValueType::MT_POINT || !pBody) {
    ElementValue(elm, values, cb);
  } else {
    std::vector<geo::IValueSet::TValueVec> scalars(3, geo::IValueSet::TValueVec(values.size()));
    // Fetch tensor

    for (int i = 0; i < 3; i++) {
      CScalarValueSet tensorComponent(*m_pParent, *m_pStage, m_antype, m_nStart + i, m_bInterface, m_bChange);
      if (map_type == CValueType::MT_NONE)
        tensorComponent.MapValues(*pBody, scalars[i], geo::WT_NONE, cb);
      else if (map_type == CValueType::MT_VOIGHT)
        tensorComponent.MapValues(*pBody, scalars[i], geo::WT_VOIGHT, cb);
      else if (map_type == CValueType::MT_REUSS)
        tensorComponent.MapValues(*pBody, scalars[i], geo::WT_REUSS, cb);
      else {
        assert(false);
      }
    }

    // Make tensor
    for (size_t i = 0; i < values.size(); i++) {
      if (scalars[0][i].Valid() && scalars[1][i].Valid() && scalars[2][i].Valid()) {
        values[i] = geo::CVector(scalars[0][i].Value(), scalars[1][i].Value(), scalars[2][i].Value());
      } else {
        values[i] = geo::CVector();
      }
    }
  }
}