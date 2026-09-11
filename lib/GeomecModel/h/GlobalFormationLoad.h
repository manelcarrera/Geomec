#ifndef _GLOBALFORMATIONLOAD_H_
#define _GLOBALFORMATIONLOAD_H_

#include "FormationLoadBase.h"
#include "BoundaryBase.h"
#include "BranchState.h"
#include "ModelBase.h"

template <class VALUE_TYPE, class CONSTANT_TYPE>
class CGlobalFormationLoad : public CFormationLoadBase<VALUE_TYPE>
{
public:
	CGlobalFormationLoad(const QString& strName,
						 CFemAppModel& model);
	CGlobalFormationLoad(const CGlobalFormationLoad& rhs);
	virtual ~CGlobalFormationLoad();

	// Equal and assignment
	bool operator==(const CGlobalFormationLoad &rhs) const;
	CGlobalFormationLoad& operator=(const CGlobalFormationLoad& rhs);

	virtual bool CanConnectItem(const CGraphNode &item) const;
	virtual bool CanDisconnectItem(const CGraphNode& item) const;

	// Access to contant
	const CONSTANT_TYPE& Constant() const;
	CONSTANT_TYPE& Constant();

	// Area of definition
	virtual geo::CPoint Min() const;
	virtual geo::CPoint Max() const;

	// GUI
	virtual unsigned int TypeId() const;

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif //_DEBUG

	// Save and load
	virtual long SavedItems() const;
	virtual void LoadStream(typename CFormationLoadBase <VALUE_TYPE> ::TSTREAM& stream, CStreamVersion& version, typename CFormationLoadBase <VALUE_TYPE> ::TPROGRESS& progress);
	virtual void SaveStream(typename CFormationLoadBase <VALUE_TYPE> ::TSTREAM& stream, typename CFormationLoadBase <VALUE_TYPE> ::TPROGRESS& progress);

	virtual bool ReadOnly() const;
};

template <class VALUE_TYPE, class CONSTANT_TYPE>
CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::CGlobalFormationLoad(const QString& strName,
																	  CFemAppModel& model)
: CFormationLoadBase<VALUE_TYPE>(strName, model)
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::~CGlobalFormationLoad()
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::CGlobalFormationLoad(const CGlobalFormationLoad& rhs)
: CFormationLoadBase<VALUE_TYPE>(rhs)
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
bool CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::operator==(const CGlobalFormationLoad &rhs) const
{
	if(!CFormationLoadBase<VALUE_TYPE>::operator ==(rhs))
		return false;

	return Constant() == rhs.Constant();
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>& CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::operator=(const CGlobalFormationLoad& rhs)
{
	CFormationLoadBase<VALUE_TYPE>::operator =(rhs);

	Constant() = rhs.Constant();

	return *this;
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
bool CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::CanConnectItem(const CGraphNode &item) const
{
	return !ReadOnly() && CFormationLoadBase<VALUE_TYPE>::CanConnectItem(item);
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
bool CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::CanDisconnectItem(const CGraphNode& item) const
{
	return !ReadOnly() && CFormationLoadBase<VALUE_TYPE>::CanDisconnectItem(item);
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
const CONSTANT_TYPE& CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::Constant() const
{
	assert(0 == CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE);
	return (const CONSTANT_TYPE&) IValueComposite::Component(0, CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE);
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
CONSTANT_TYPE& CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::Constant()
{
  return const_cast <CONSTANT_TYPE&> (
    static_cast <const CGlobalFormationLoad <VALUE_TYPE, CONSTANT_TYPE>&>
      (*this).Constant());
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
geo::CPoint CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::Min() const
{
	const CModelBase* pModel =
		(const CModelBase*)(&CFormationLoadBase <VALUE_TYPE> ::Model());

	return pModel->Boundary().Min();
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
geo::CPoint CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::Max() const
{
	const CModelBase* pModel =
		(const CModelBase*)(&CFormationLoadBase <VALUE_TYPE> ::Model());

	return pModel->Boundary().Max();
}

/*
template <class VALUE_TYPE, class CONSTANT_TYPE>
unsigned int CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::IconId() const
{
	return IDI_COMPONENT_PRESSURE_GLOBAL;
}
*/

template <class VALUE_TYPE, class CONSTANT_TYPE>
unsigned int CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::TypeId() const
{
	return 0;
}

#ifdef _DEBUG
template <class VALUE_TYPE, class CONSTANT_TYPE>
void CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::AssertValid() const
{

}
#endif //_DEBUG

template <class VALUE_TYPE, class CONSTANT_TYPE>
long CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::SavedItems() const
{
	return CFormationLoadBase<VALUE_TYPE>::SavedItems() + Constant().SavedItems();
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
void CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::LoadStream(typename CFormationLoadBase <VALUE_TYPE> ::TSTREAM& stream, CStreamVersion &version,typename CFormationLoadBase <VALUE_TYPE> ::TPROGRESS& progress)
{
	CFormationLoadBase<VALUE_TYPE>::LoadStream(stream,version,progress);
	if(version < CStreamVersion(3, 0, 21))
	{
		int distriOnly;
		stream >> distriOnly;
		CFormationLoadBase <VALUE_TYPE> ::DistributedOnly(distriOnly);
	}

	Constant().LoadStream(stream,version,progress);
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
void CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::SaveStream(typename CFormationLoadBase <VALUE_TYPE> ::TSTREAM& stream, typename CFormationLoadBase <VALUE_TYPE> ::TPROGRESS& progress)
{
	CFormationLoadBase<VALUE_TYPE>::SaveStream(stream,progress);
	Constant().SaveStream(stream,progress);
}

template <class VALUE_TYPE, class CONSTANT_TYPE>
bool CGlobalFormationLoad<VALUE_TYPE, CONSTANT_TYPE>::ReadOnly() const
{
	// global initial value is read-only in branch models
	return (static_cast<const CModelBase&>(CFormationLoadBase <VALUE_TYPE> ::Model())).BranchState().IsBranch();
}

#endif // _GLOBALFORMATIONLOAD_H_
