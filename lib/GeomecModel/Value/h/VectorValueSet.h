// VectorValueSet.h: interface for the CVectorValueSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORVALUESET_H__169C8AD2_4112_4A7F_BB6E_C4A7E47F2BE1__INCLUDED_)
#define AFX_VECTORVALUESET_H__169C8AD2_4112_4A7F_BB6E_C4A7E47F2BE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IValueSetTemplate.h"

class CResultRegister;
class CDepletionStage;

#include "AnalysisType.h"
#include "ValueType.h"

class CVectorValueSet : public geo::IValueSetTemplate<geo::CVector>
{
public:
    CVectorValueSet(const CResultRegister& parent, 
		            const CDepletionStage& stage, 
					const CAnalysisType& antype, 
					int nStart,
					bool bShear,
					bool bInterface,
					bool bChange);
	virtual const geo::IElementSet &ElementSet() const;
	bool ElementValuesAvailable(int nElementIndex ) const;
  virtual void ElementValues(TValueVec& values, int iElementIndex, geo::IParallelInitializationCallback *cb) const;
	typedef CValueType::TMapEnum TMapType;
  virtual void MapValueElement(const geo::IElement& elm, TValueVec& values, TMapType map_type, geo::IParallelInitializationCallback *cb) const;
	virtual bool IsEmpty() const;
  virtual bool IsValid(const geo::CVector& value) const { return !value.Empty(); }
	geo::CVector Normal() const;
protected:
  void FilterElements(std::vector<int> &indices) const;
private:
  int                    m_nStart;
	CAnalysisType          m_antype;
	bool                   m_bShear;
	bool				           m_bInterface;
	bool                   m_bChange;
	const CDepletionStage* m_pStage;
	const CResultRegister* m_pParent;
	const double* ValuesFromCache(int nElementIndex,
		                          const CAnalysisType& antype,
								  const CDepletionStage& stage,
								  int nStart) const;
};



#endif // !defined(AFX_VECTORVALUESET_H__169C8AD2_4112_4A7F_BB6E_C4A7E47F2BE1__INCLUDED_)
