#ifndef _FORMATIONLOADGRADIENTCOMPONENT_H_
#define _FORMATIONLOADGRADIENTCOMPONENT_H_

#include "FormationLoadComponent.h"
#include "LengthQuantity.h"

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
class CFormationLoadGradientComponent : public CFormationLoadComponent<VALUE_TYPE>
{
	SINGLE_TYPE			m_ReferenceValue;
	CLengthQuantity		m_ReferenceDepth;
	GRADIENT_TYPE		m_Gradient;

public:
	// Contruction
	CFormationLoadGradientComponent(CFormationLoadBase<VALUE_TYPE> &formationload, bool bEmpty = false);
	CFormationLoadGradientComponent(const CFormationLoadGradientComponent &rhs);

	// IconId and TypeName
//	virtual unsigned int TypeId() const;
//	virtual unsigned int IconId() const;
//	virtual QString TypeName() const;

	// Get and set values
	const SINGLE_TYPE& ReferenceValue() const;
	const CLengthQuantity& ReferenceDepth() const;
	const GRADIENT_TYPE& Gradient() const;
	void Set(const double& dReferenceValue,
			 const double& dReferenceDepth,
			 const double& dGradient,
			 const CQuantity::UNIT unit = CQuantity::SI_UNIT);

	// Value implementation
	virtual bool Defined() const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

	// Assignment
	bool operator==(const CFormationLoadGradientComponent &rhs) const;
	CFormationLoadGradientComponent& operator=(const CFormationLoadGradientComponent& rhs);

	// Save and Load
	virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
	virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
	virtual long SavedItems() const;

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
};

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::CFormationLoadGradientComponent(CFormationLoadBase<VALUE_TYPE> &formationload, bool bEmpty)
: CFormationLoadComponent<VALUE_TYPE>(formationload, CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE, bEmpty), m_ReferenceValue(0), m_ReferenceDepth(0), m_Gradient(0)
{
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::CFormationLoadGradientComponent(const CFormationLoadGradientComponent &rhs)
: CFormationLoadComponent<VALUE_TYPE>(rhs), 
  m_ReferenceValue(rhs.m_ReferenceValue),
  m_ReferenceDepth(rhs.m_ReferenceDepth),
  m_Gradient(rhs.m_Gradient)
{
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
const SINGLE_TYPE& CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::ReferenceValue() const
{
	return m_ReferenceValue;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
const CLengthQuantity& CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::ReferenceDepth() const
{
	return m_ReferenceDepth;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
const GRADIENT_TYPE& CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::Gradient() const
{
	return m_Gradient;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
void CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::Set(const double& dReferenceValue,
							 const double& dReferenceDepth,
							 const double& dGradient,
							 const CQuantity::UNIT unit)
{
	m_ReferenceValue.Value(dReferenceValue, unit);
	m_ReferenceDepth.Value(dReferenceDepth, unit);
	m_Gradient.Value(dGradient, unit);
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
bool CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::Defined() const
{
	return true;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
IValueDomainScalar::TValueVec CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const CModelBase& model = static_cast<const CModelBase&> (CFormationLoadComponent <VALUE_TYPE> ::Model());
//	if(model.Mesh().FormationElementSet(elm).InitialPressure())
//	{
//		const CPressure* pPressure = dynamic_cast<const CPressure*> (&Parent());
//		if(pPressure && &model.InitialDepletionStage() != &pPressure->DepletionStage())
//		{
//			return pPressure->Formation().Pressure(model.InitialDepletionStage()).Component(0).ValueElement(elm, unit);
//		}
//	}

	IValueDomainScalar::TValueVec vcRet = CFormationLoadComponent<VALUE_TYPE>::ValueElement(elm, unit, cb);

	for(int i = 0; i < vcRet.size(); i++)
	{
		if(!vcRet[i].Valid())
			vcRet[i] = geo::CValue((ReferenceDepth().Convert(model.Depth(elm.Point(i)), unit, CQuantity::SI_UNIT)  - ReferenceDepth().Value(unit)) * Gradient().Value(unit) + ReferenceValue().Value(unit));
	}

	return vcRet;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
IValueDomainScalar::TValue CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
	const CModelBase& model = static_cast<const CModelBase&> (CFormationLoadComponent <VALUE_TYPE> ::Model());
//	std::vector<int> vcCandidates = model.Mesh().Mesh().ElementsAt(pt);
//	bool bInitial = false;
//	int i;
//	for(size_t i = 0; i < vcCandidates.size(); i++)
//	{
//		if(model.Mesh().FormationElementSet(model.Mesh().Mesh().Element(vcCandidates[i])).InitialPressure())
//		{
//			const CPressure* pPressure = dynamic_cast<const CPressure*> (&Parent());
//			if(pPressure && &model.InitialDepletionStage() != &pPressure->DepletionStage())
//			{
//				return pPressure->Formation().Pressure(model.InitialDepletionStage()).Component(0).ValuePoint(pt, unit);
//			}
//			break;
//		}
//	}

	geo::CValue value = CFormationLoadComponent<VALUE_TYPE>::ValuePoint(pt, unit, cb);
	if(value.Valid())
		return value;

	return geo::CValue((ReferenceDepth().Convert(model.Depth(pt), unit, CQuantity::SI_UNIT)  - ReferenceDepth().Value(unit)) * Gradient().Value(unit) + ReferenceValue().Value(unit));
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
bool CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::operator==(const CFormationLoadGradientComponent &rhs) const
{
	if(rhs.IsCopy())
	{
		assert(CFormationLoadComponent <VALUE_TYPE> ::IsCopied());
		if( !CFormationLoadComponent<VALUE_TYPE>::operator ==(rhs) )
			return false;
	}
	
	return ((ReferenceValue() == rhs.ReferenceValue()) &&
			(ReferenceDepth() == rhs.ReferenceDepth()) &&
			(Gradient() == rhs.Gradient()));
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>& CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::operator=(const CFormationLoadGradientComponent& rhs)
{
	if(!((*this) == rhs))
	{
		CModelBase *pModel = dynamic_cast<CModelBase*> (&CFormationLoadComponent <VALUE_TYPE> ::Model());
		assert(pModel);
		pModel->ResultRegister().ClearLinear(false);
    pModel->ResultRegister().ClearNonLinear(false);
    pModel->ResultRegister().ClearMixture();
	}
		
	if(rhs.IsCopy())
	{
		assert(CFormationLoadComponent <VALUE_TYPE> ::IsCopied());
		CFormationLoadComponent<VALUE_TYPE>::operator=(rhs);
	}
	
	m_ReferenceValue = rhs.ReferenceValue();
	m_ReferenceDepth = rhs.ReferenceDepth();
	m_Gradient = rhs.Gradient();

	return *this;
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
void CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::SaveStream(TSTREAM &stream, TPROGRESS &progress)
{
	CFormationLoadComponent<VALUE_TYPE>::SaveStream(stream,progress);
	stream << m_ReferenceValue.Value();
	stream << m_ReferenceDepth.Value();
	stream << m_Gradient.Value();
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
void CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::LoadStream(TSTREAM &stream, CStreamVersion &version, TPROGRESS &progress)
{
	CFormationLoadComponent<VALUE_TYPE>::LoadStream(stream,version,progress);
	double dVal;
	stream >> dVal;
	m_ReferenceValue.Value(dVal,CQuantity::SI_UNIT);
	stream >> dVal;
	m_ReferenceDepth.Value(dVal,CQuantity::SI_UNIT);
	stream >> dVal;
	m_Gradient.Value(dVal,CQuantity::SI_UNIT);
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
long CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::SavedItems() const
{
	return CFormationLoadComponent<VALUE_TYPE>::SavedItems();
}

template <class VALUE_TYPE, class SINGLE_TYPE, class GRADIENT_TYPE>
bool CFormationLoadGradientComponent<VALUE_TYPE, SINGLE_TYPE, GRADIENT_TYPE>::PrepareMapping(const geo::IElementSet *pElementSet)
{
  return this->Parent().PrepareMapping(pElementSet, this);
}

#endif // _FORMATIONLOADGRADIENTCOMPONENT_H_
