// ScalarValueSet.h: interface for the CScalarValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCALARVALUESET_H__EE5CA2D0_70E8_4FCE_8A5A_BCCD89E1861C__INCLUDED_)
#define AFX_SCALARVALUESET_H__EE5CA2D0_70E8_4FCE_8A5A_BCCD89E1861C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalysisType.h"

class CResultRegister;
class CDepletionStage;

#include "GeomecIValueSet.h"
#include "IValueSet.h"

class CScalarValueSet : public geo::IValueSet {
public:
  CScalarValueSet(const CResultRegister &parent, const CDepletionStage &stage, const CAnalysisType &antype, int nStart,
                  bool bInterface, bool bChange);
  virtual const geo::IElementSet &ElementSet() const;
  bool ElementValuesAvailable(int nElementIndex) const;
  virtual void ElementValues(TValueVec &values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
  virtual bool IsEmpty() const;
  virtual void Empty() { assert(false); }
  virtual int PushBack(const TValueVec & /*values*/) {
    assert(false);
    return -1;
  }
  virtual bool IsValid(const geo::CVector &value) const { return !value.Empty(); }

private:
  int m_nStart;
  CAnalysisType m_antype;
  bool m_bChange;
  bool m_bInterface;
  const CDepletionStage *m_pStage;
  const CResultRegister *m_pParent;
  const double *ValuesFromCache(int nElementIndex, const CAnalysisType &antype, const CDepletionStage &stage,
                                int nStart) const;
};

#endif // !defined(AFX_SCALARVALUESET_H__EE5CA2D0_70E8_4FCE_8A5A_BCCD89E1861C__INCLUDED_)
