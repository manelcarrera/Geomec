#if !defined(TENSOR_VALUE_SET_TEMPLATE_INCLUDED_)
#define TENSOR_VALUE_SET_TEMPLATE_INCLUDED_

#include "IInterfaceElement.h"
#include "IValueSetTemplate.h"
#include "MeshBase.h"
#include "ScalarValueSet.h"
#include "ValueType.h"

class CResultRegister;
class CDepletionStage;
class CAnalysisType;

template <class TENSOR_TYPE> class CTensorValueSet : public geo::IValueSetTemplate<TENSOR_TYPE> {
public:
  // for clarity, as we're also using geo::IValueSet::TValueVec in the methods
  typedef typename geo::IValueSetTemplate<TENSOR_TYPE>::TValueVec TTensorValueVec;

  CTensorValueSet(const CResultRegister &parent, const CDepletionStage &stage, const CAnalysisType &antype, int nStart,
                  bool bTotal, bool bChange);
  virtual const geo::IElementSet &ElementSet() const;
  bool ElementValuesAvailable(int nElementIndex) const;
  virtual void ElementValues(TTensorValueVec &values, int iElementIndex,
                             geo::IParallelInitializationCallback *cb) const;
  typedef CValueType::TMapEnum TMapType;
  void MapValueElement(const geo::IElement &elm, TTensorValueVec &values, TMapType map_type,
                       geo::IParallelInitializationCallback *cb) const;
  virtual bool IsEmpty() const;
  virtual bool IsValid(const TENSOR_TYPE &value) const { return !value.Empty(); }

protected:
  virtual void FilterElements(std::vector<int> &indices) const;

private:
  int m_nStart;
  CAnalysisType m_antype;
  bool m_bTotal;
  bool m_bChange;
  const CDepletionStage *m_pStage;
  const CResultRegister *m_pParent;
  const double *ValuesFromCache(int nElementIndex, const CAnalysisType &antype, const CDepletionStage &stage,
                                int nStart) const;
};

/*!
  Constructor of the tensor result
*/
template <class TENSOR_TYPE>
CTensorValueSet<TENSOR_TYPE>::CTensorValueSet(const CResultRegister &parent, const CDepletionStage &stage,
                                              const CAnalysisType &antype, int nStart, bool bTotal, bool bChange)
    : m_nStart(nStart), m_antype(antype), m_bTotal(bTotal), m_bChange(bChange), m_pStage(&stage), m_pParent(&parent) {}

/*!
  Returns the mesh assiociated to the tensor valueset.
*/
template <class TENSOR_TYPE> const geo::IElementSet &CTensorValueSet<TENSOR_TYPE>::ElementSet() const {
  assert(m_pParent);
  return m_pParent->Mesh().Mesh();
}

/*!
  Returns a pointer to the array of values of the result register
*/
template <class TENSOR_TYPE>
const double *CTensorValueSet<TENSOR_TYPE>::ValuesFromCache(int nElementIndex, const CAnalysisType &antype,
                                                            const CDepletionStage &stage, int nStart) const {
  return m_pParent->ValuesFromCache(nElementIndex, m_pParent->ColumnNumber(antype, stage.Index(), nStart));
}

// This function will filter out all interface elements. This is called from
// IValueSetTemplate::ValuePoint. This was implemented to make sure that interpolated
// values are not invalid just because one (or more) of the elements that a point lies in
// is an interface element.
template <class TENSOR_TYPE> void CTensorValueSet<TENSOR_TYPE>::FilterElements(std::vector<int> &indices) const {
  for (int i = (int)indices.size() - 1; i >= 0; --i) {
    if (dynamic_cast<const geo::IInterfaceElement *>(&ElementSet().Element(indices[i]))) {
      indices.erase(indices.begin() + i);
    }
  }
}
/*!
  returns the tensor values of the element
*/
template <class TENSOR_TYPE>
void CTensorValueSet<TENSOR_TYPE>::ElementValues(TTensorValueVec &values, int iElementIndex,
                                                 geo::IParallelInitializationCallback * /*cb*/) const {
  assert(values.size() == ElementSet().Element(iElementIndex).NrOfNodes());

  if (!ElementValuesAvailable(iElementIndex) ||
      dynamic_cast<const geo::IInterfaceElement *>(&ElementSet().Element(iElementIndex))) {
    // Result does not exist
    for (size_t i = 0; i < values.size(); i++)
      values[i] = TENSOR_TYPE();
    return;
  }

  // Read in tensor
  const double *pTensorVal[6];
  for (int i = 0; i < 6; i++) {
    pTensorVal[i] = ValuesFromCache(iElementIndex, m_antype, *m_pStage, m_nStart + i);
    assert(pTensorVal[i]);
  }

  if (m_bChange) {
    const CDepletionStage *pIniStage = &m_pParent->DepletionStageEntry().MarkedAsInitialStage();

    const double *pInitTensorVal[6];
    for (int i = 0; i < 6; i++) {
      pInitTensorVal[i] = ValuesFromCache(iElementIndex, m_antype, *pIniStage, m_nStart + i);
      assert(pTensorVal[i]);
    }

    for (size_t i = 0; i < values.size(); i++) {
      double dPressure = 0;
      double dInitPressure = 0;
      if (m_bTotal) {
        dPressure = ValuesFromCache(iElementIndex, m_antype, *m_pStage, CI_POREPRES)[i];
        dInitPressure = ValuesFromCache(iElementIndex, m_antype, *pIniStage, CI_POREPRES)[i];
      }
      values[i] = TENSOR_TYPE(pTensorVal[0][i] - pInitTensorVal[0][i] + dPressure - dInitPressure,
                              pTensorVal[1][i] - pInitTensorVal[1][i] + dPressure - dInitPressure,
                              pTensorVal[2][i] - pInitTensorVal[2][i] + dPressure - dInitPressure,
                              pTensorVal[3][i] - pInitTensorVal[3][i], pTensorVal[4][i] - pInitTensorVal[4][i],
                              pTensorVal[5][i] - pInitTensorVal[5][i]);
    }
  } else {
    for (size_t i = 0; i < values.size(); i++) {
      double dPressure = 0;
      if (m_bTotal)
        dPressure = ValuesFromCache(iElementIndex, m_antype, *m_pStage, CI_POREPRES)[i];
      values[i] = TENSOR_TYPE(pTensorVal[0][i] + dPressure, pTensorVal[1][i] + dPressure, pTensorVal[2][i] + dPressure,
                              pTensorVal[3][i], pTensorVal[4][i], pTensorVal[5][i]);
    }
  }
}

/*!
  Returns true the tensor is not available
*/
template <class TENSOR_TYPE> bool CTensorValueSet<TENSOR_TYPE>::IsEmpty() const { return true; }

template <class TENSOR_TYPE> bool CTensorValueSet<TENSOR_TYPE>::ElementValuesAvailable(int nElementIndex) const {
  int i;
  for (i = 0; i < 6; i++) {
    if (!m_pParent->ResultAvailable(*m_pStage, m_antype, nElementIndex, m_nStart + i))
      return false;
  }

  if (m_bTotal && !m_pParent->ResultAvailable(*m_pStage, m_antype, nElementIndex, CI_POREPRES))
    return false;

  if (m_bChange) {
    const CDepletionStage *pIniStage = &m_pParent->DepletionStageEntry().MarkedAsInitialStage();

    for (i = 0; i < 6; i++) {
      if (!m_pParent->ResultAvailable(*pIniStage, m_antype, nElementIndex, m_nStart + i))
        return false;
    }

    if (m_bTotal && !m_pParent->ResultAvailable(*pIniStage, m_antype, nElementIndex, CI_POREPRES))
      return false;
  }

  return true;
}

template <class TENSOR_TYPE>
void CTensorValueSet<TENSOR_TYPE>::MapValueElement(const geo::IElement &elm, TTensorValueVec &values, TMapType map_type,
                                                   geo::IParallelInitializationCallback *cb) const {
  assert(values.size() == elm.NrOfPoints());
  const geo::IBody *pBody = dynamic_cast<const geo::IBody *>(&elm);
  if (map_type == CValueType::MT_POINT || !pBody) {
    this->ElementValue(elm, values, cb);
  } else {
    std::vector<geo::IValueSet::TValueVec> scalars(6, geo::IValueSet::TValueVec(values.size()));
    // Fetch tensor
    for (int i = 0; i < 6; i++) {
      CScalarValueSet tensorComponent(*m_pParent, *m_pStage, m_antype, m_nStart + i, false, m_bChange);
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
    if (m_bTotal) {
      // Add the porepressure
      geo::IValueSet::TValueVec dPressure(values.size());
      CScalarValueSet pressure(*m_pParent, *m_pStage, m_antype, CI_POREPRES, false, m_bChange);
      if (map_type == CValueType::MT_NONE)
        pressure.MapValues(*pBody, dPressure, geo::WT_NONE, cb);
      else if (map_type == CValueType::MT_VOIGHT)
        pressure.MapValues(*pBody, dPressure, geo::WT_VOIGHT, cb);
      else if (map_type == CValueType::MT_REUSS)
        pressure.MapValues(*pBody, dPressure, geo::WT_REUSS, cb);
      else {
        assert(false);
      }
      for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < values.size(); j++) {
          scalars[i][j] += dPressure[j];
        }
      }
    }
    // Make tensor
    for (size_t i = 0; i < values.size(); i++) {
      if (scalars[0][i].Valid() && scalars[1][i].Valid() && scalars[2][i].Valid() && scalars[3][i].Valid() &&
          scalars[4][i].Valid() && scalars[5][i].Valid()) {
        values[i] = TENSOR_TYPE(scalars[0][i].Value(), scalars[1][i].Value(), scalars[2][i].Value(),
                                scalars[3][i].Value(), scalars[4][i].Value(), scalars[5][i].Value());
      } else {
        values[i] = TENSOR_TYPE();
      }
    }
  }
}

#endif
