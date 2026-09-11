#ifndef _CommonPressureComponent_h_
#define _CommonPressureComponent_h_

#include "PressureBase.h"

template <typename Pressure>
  class CCommonPressureComponent : public CPressureComponent <Pressure>
{
  public:
    CCommonPressureComponent<Pressure>(CFormationLoadBase <Pressure>& pressure,
      unsigned int mode, bool empty = false);

    virtual unsigned int IconId() const;
    virtual unsigned int TypeId() const;

  protected:
    CCommonPressureComponent<Pressure>(const CCommonPressureComponent <Pressure>& rhs);
    CCommonPressureComponent <Pressure>& operator = (
      const CCommonPressureComponent <Pressure>& rhs);

    virtual long SavedItems() const;
    virtual bool Defined() const;
};

template <typename Pressure>
  CCommonPressureComponent <Pressure>::CCommonPressureComponent(
    CFormationLoadBase <Pressure>& pressure, unsigned int mode, bool empty)
  : CPressureComponent <Pressure>(pressure, mode, empty)
{
}

template <typename Pressure>
  unsigned int CCommonPressureComponent <Pressure>::IconId() const
{
  return IDI_COMPONENT_PRESSURE_GWC;
}

template <typename Pressure>
  unsigned int CCommonPressureComponent <Pressure>::TypeId() const
{
  return IDT_COMPONENT_PRESSURE_GWC;
}

// protected

template <typename Pressure>
  CCommonPressureComponent <Pressure>::CCommonPressureComponent(
    const CCommonPressureComponent <Pressure>& rhs)
  : CPressureComponent <Pressure>(rhs)
{
}

template <typename Pressure>
  CCommonPressureComponent <Pressure>&
    CCommonPressureComponent <Pressure>::operator = (
      const CCommonPressureComponent <Pressure>& rhs)
{
  /*
   * The implementation below would be preferable when the function
   * 'CPressureComponent::swap' would be implemented.
   * Since this function is not implemented and this class does not contain
   * any members (yet) redirecting to the base class implementation is more
   * direct.

  CCommonPressureComponent <Pressure> tmp(rhs);

  tmp.swap(*this);

   */

  CPressureComponent <Pressure>::operator = (rhs);

  return *this;
}

template <typename Pressure>
  long CCommonPressureComponent <Pressure>::SavedItems() const
{
	return CPressureComponent <Pressure>::SavedItems();
}

template <typename Pressure>
  bool CCommonPressureComponent <Pressure>::Defined() const
{
  return true;
}

#endif  // _CommonPressureComponent_h_
