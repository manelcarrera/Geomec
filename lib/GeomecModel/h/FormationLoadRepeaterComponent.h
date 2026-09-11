#ifndef _FORMATIONLOADREPEATERCOMPONENT_H_
#define _FORMATIONLOADREPEATERCOMPONENT_H_

#include "FormationLoad.h"

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
class CFormationLoadRepeaterComponent : public CFormationLoadComponent<VALUE_TYPE>
{
protected:
  typedef CFormationLoad<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> TFormationLoad;

public:
  // Contruction
  CFormationLoadRepeaterComponent(CFormationLoadBase<VALUE_TYPE> &formationload);
  CFormationLoadRepeaterComponent(const CFormationLoadRepeaterComponent& rhs);
  
  // Previous depletion stage load
  virtual const TFormationLoad& PreviousFormationLoad() const = 0;
  virtual const IValueComponentBase& GlobalValues() const = 0;

  virtual bool Defined() const;
  virtual IValueDomainScalar::TValue ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;
  virtual IValueDomainScalar::TValueVec ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit = CQuantity::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const;

  // Save and Load
  virtual long SavedItems() const;
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  virtual bool PrepareMapping(const geo::IElementSet *pElementSet);
};

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CFormationLoadRepeaterComponent(CFormationLoadBase<VALUE_TYPE> &formationload)
: CFormationLoadComponent<VALUE_TYPE>(formationload, CFormationLoadBase<VALUE_TYPE>::REPEAT_MODE)
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::CFormationLoadRepeaterComponent(const CFormationLoadRepeaterComponent& rhs)
: CFormationLoadComponent<VALUE_TYPE>(rhs)
{
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::Defined() const
{
  return true;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
IValueDomainScalar::TValueVec CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ValueElement(const geo::IElement& elm, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const TFormationLoad* pFormationLoad = dynamic_cast<const TFormationLoad*>(&CFormationLoadComponent <VALUE_TYPE> ::FormationLoadBase());
  assert(pFormationLoad);
  
  switch(pFormationLoad->Type())
  {
  case CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE:
  return pFormationLoad->Constant().ValueElement(elm, unit, cb);
  case CFormationLoadBase<VALUE_TYPE>::REPEAT_MODE:
    {
      IValueDomainScalar::TValueVec vcRet = CFormationLoadComponent<VALUE_TYPE>::ValueElement(elm, unit, cb);
      IValueDomainScalar::TValueVec vcPrevious(0);
      for(size_t i = 0; i < vcRet.size(); i++)
      {
        if(!vcRet[i].Valid())
        {
          if(vcPrevious.size() == 0)
          {
            if(pFormationLoad->IsFirstValidDepletionStage())
              vcPrevious = GlobalValues().ScalarData().ValueElement(elm, unit, cb);
            else
              vcPrevious = PreviousFormationLoad().Component().ScalarData().ValueElement(elm, unit, cb);
          }
          vcRet[i] = vcPrevious[i];
        }
      }
      return vcRet;
    }
  case CFormationLoadBase<VALUE_TYPE>::GWC_MODE:
  return pFormationLoad->GWC().ScalarData().ValueElement(elm, unit, cb);
  default:
    assert(false);
  };
  
  return IValueDomainScalar::TValueVec(elm.NrOfPoints());

}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
IValueDomainScalar::TValue CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::ValuePoint(const geo::IPoint& pt, const CQuantity::UNIT unit, geo::IParallelInitializationCallback *cb) const
{
  const TFormationLoad* pFormationLoad = dynamic_cast<const TFormationLoad*>(&CFormationLoadComponent <VALUE_TYPE> ::FormationLoadBase());
  assert(pFormationLoad);

  switch(pFormationLoad->Type())
  {
  case CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE:
  return pFormationLoad->Constant().ValuePoint(pt, unit, cb);
  case CFormationLoadBase<VALUE_TYPE>::REPEAT_MODE:
    {
      geo::CValue value = CFormationLoadComponent<VALUE_TYPE>::ValuePoint(pt, unit, cb);
      if(value.Valid())
        return value;

      if(pFormationLoad->IsFirstValidDepletionStage())
    return GlobalValues().ScalarData().ValuePoint(pt, unit, cb);

      return PreviousFormationLoad().Component().ScalarData().ValuePoint(pt, unit, cb);
    }
  case CFormationLoadBase<VALUE_TYPE>::GWC_MODE:
  return pFormationLoad->GWC().ScalarData().ValuePoint(pt, unit, cb);
  default:
    assert(false);
  };
  
  return 0;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::LoadStream(TSTREAM& stream, CStreamVersion &version,TPROGRESS& progress)
{
  CFormationLoadComponent<VALUE_TYPE>::LoadStream(stream,version,progress);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
void CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::SaveStream(TSTREAM& stream, TPROGRESS& progress)
{
  CFormationLoadComponent<VALUE_TYPE>::SaveStream(stream,progress);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
long CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::SavedItems() const
{
  return CFormationLoadComponent<VALUE_TYPE>::SavedItems();
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE, class GWC_TYPE>
bool CFormationLoadRepeaterComponent<VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>::PrepareMapping(const geo::IElementSet *pElementSet)
{
  const TFormationLoad* pFormationLoad = dynamic_cast<const TFormationLoad*>(&CFormationLoadComponent <VALUE_TYPE> ::FormationLoadBase());
  assert(pFormationLoad);

  bool bCheck = false;

  switch (pFormationLoad->Type())
  {
  case CFormationLoadBase<VALUE_TYPE>::CONSTANT_MODE:
  bCheck = true;
  break;
  case CFormationLoadBase<VALUE_TYPE>::REPEAT_MODE:
  {
  if (pFormationLoad->IsFirstValidDepletionStage())
      bCheck = const_cast<IValueComponentBase&>(GlobalValues()).PrepareMapping(pElementSet);
  else
      bCheck = const_cast<IValueComponentBase&>(PreviousFormationLoad().Component()).PrepareMapping(pElementSet);
  break;
  }
  case CFormationLoadBase<VALUE_TYPE>::GWC_MODE:
  bCheck = const_cast<GWC_TYPE&>(pFormationLoad->GWC()).PrepareMapping(pElementSet);
  break;
  default:
  assert(false);
  };


  if (!const_cast<TFormationLoad *>(pFormationLoad)->PrepareMapping(pElementSet, this))
  bCheck = false;

  return bCheck;
}


#endif // _FORMATIONLOADREPEATERCOMPONENT_H_
