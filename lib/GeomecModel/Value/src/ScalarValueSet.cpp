// ScalarValueSet.cpp: implementation of the CScalarValueSet class.
//
//////////////////////////////////////////////////////////////////////
#include "ResultRegister.h"
#include "ScalarValueSet.h"
#include "DepletionStage.h"
#include "MeshBase.h"
#include "IInterfaceElement.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*!
  Constructor of the tensor result
*/
CScalarValueSet::CScalarValueSet(const CResultRegister& parent, 
                 const CDepletionStage& stage, 
                 const CAnalysisType& antype, 
                 int nStart,
                 bool bInterface,
                 bool bChange)
: m_nStart(nStart), 
  m_antype(antype), 
  m_bChange(bChange),
  m_bInterface(bInterface), 
  m_pStage(&stage), 
  m_pParent(&parent)
{
}

/*!
  Returns the mesh assiociated to the tensor valueset.
*/
const geo::IElementSet &CScalarValueSet::ElementSet() const
{
  assert(m_pParent);
  return m_pParent->Mesh().Mesh();
}

/*!
  Returns a pointer to the array of values of the result register
*/
const double* CScalarValueSet::ValuesFromCache( int nElementIndex,
                                           const CAnalysisType& antype,
                               const CDepletionStage& stage,
                               int nStart) const 
{
  return m_pParent->ValuesFromCache(nElementIndex, 
                  m_pParent->ColumnNumber(antype, stage.Index(), nStart));
}

/*!
  returns the tensor values of the element
*/
void CScalarValueSet::ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback * /*cb*/) const
{
  assert(values.size() == ElementSet().Element(iElementIndex).NrOfNodes());

  bool bInterface = dynamic_cast<const geo::IInterfaceElement*>( &ElementSet().Element(iElementIndex) );

  if(!ElementValuesAvailable(iElementIndex) || bInterface != m_bInterface ) {
    // Result does not exist
    for(size_t i = 0; i < values.size(); i++) values[i] = geo::CValue();
    return;
  }

  // Read in tensor
  const double *pVectorVal = ValuesFromCache(iElementIndex, m_antype, *m_pStage, m_nStart);
  assert(pVectorVal);

  if( m_bChange ) {
    const CDepletionStage& init_stage = m_pParent->DepletionStageEntry().MarkedAsInitialStage();
    const double *pInitVectorVal = ValuesFromCache(iElementIndex, m_antype, init_stage, m_nStart);
    assert(pInitVectorVal);
    for(size_t i = 0; i < values.size(); i++) {
      values[i] = geo::CValue(pVectorVal[i] - pInitVectorVal[i]);
    }
  } else {
    for(size_t i = 0; i < values.size(); i++) {
      values[i] = geo::CValue(pVectorVal[i]);
    }
  }
}

/*!
  Returns true the tensor is not available
*/
bool CScalarValueSet::IsEmpty() const
{
  return true;
}

bool CScalarValueSet::ElementValuesAvailable(int nElementIndex ) const
{
  if(!m_pParent->ResultAvailable(*m_pStage, m_antype, nElementIndex, m_nStart)) return false;

  if(m_bChange) {
  const CDepletionStage& init_stage = m_pParent->DepletionStageEntry().MarkedAsInitialStage();
  if(!m_pParent->ResultAvailable(init_stage, m_antype, nElementIndex, m_nStart)) return false;
  }

  return true;
}
