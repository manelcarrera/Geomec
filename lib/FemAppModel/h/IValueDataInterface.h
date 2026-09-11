#ifndef _I_VALUE_DATA_INTERFACE_H_
#define _I_VALUE_DATA_INTERFACE_H_

class IValueComponentScalar;
class ISymbol;


#include "SingleQuantity.h"
#include "IValueDomain.h"

namespace geo
{
  class IObject;
  class IPoint;
  class IElement;
  class IParallelInitializationCallback;
}

#include "VectorTempl.h"
#include "IElementSet.h"
#include "IProgressBase.h"
#include "Value.h"

class IValueDataInterface
{
public:
  enum TValueDataType : int
  {
  SCALAR = 0,
  VECTOR,
  TENSOR
  };

  typedef IQuantityDouble::UNIT UNIT;

  virtual TValueDataType Type() const = 0;
  virtual TValueDataType Type() = 0;

  virtual void ResetMinMax() const {}
};


template<int DATA_TYPE, class VALUE_DOMAIN>
class IValueDataInterfaceTempl : public IValueDataInterface
{
public:
  typedef typename VALUE_DOMAIN::TMinMax TMinMax;
  typedef typename VALUE_DOMAIN::TValue TValue;
  typedef typename VALUE_DOMAIN::TValueVec TValueVec;

  TMinMax MinMax(IProgressBase& progressBase, const geo::IObject &object, const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual TMinMax MinMax(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const = 0;

  TValue Average(IProgressBase& progressBase, const geo::IObject& object, const UNIT unit = IQuantityDouble::SI_UNIT) const;
  virtual TValue Average(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const = 0;

  virtual TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const = 0;
  virtual TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const = 0;

  virtual IValueDataInterface::TValueDataType Type() const { return static_cast<IValueDataInterface::TValueDataType>(DATA_TYPE); }
  virtual IValueDataInterface::TValueDataType Type() { return static_cast<IValueDataInterface::TValueDataType>(DATA_TYPE); }
  
};


template<int DATA_TYPE, class VALUE_DOMAIN>
typename VALUE_DOMAIN::TMinMax IValueDataInterfaceTempl<DATA_TYPE, VALUE_DOMAIN>::MinMax(IProgressBase& progressBase, const geo::IObject &object, const UNIT unit) const
{
  TMinMax ret;
  const geo::IArray* ptArr = dynamic_cast<const geo::IArray *> (&object);
  const geo::IPoint* pt = dynamic_cast<const geo::IPoint*> (&object);
  const geo::IElement *pElement = dynamic_cast<const geo::IElement*> (&object);
  const geo::IElementSet *pElementSet=dynamic_cast<const geo::IElementSet*> (&object);
  const ISymbol *pSymbol = dynamic_cast<const ISymbol*>(&object);

  if (ptArr)
  {
    TMinMax minArr;
    for(size_t i = 0; i < ptArr->Size(); i++)
    {
      minArr = MinMax(progressBase, ptArr->ObjectBase(i),unit);
      ret.first = ret.first.Min(minArr.first);
      ret.second =ret.second.Max(minArr.second);
    }
  }
  else
  if(pt)
  {
    TValue value( ValuePoint(*pt,unit) );
    ret.first = ret.first.Min(value);
    ret.second = ret.second.Max(value);
  }
  else
  if(pElement) {
    TValueVec values = ValueElement(*pElement,unit);
    for (typename TValueVec::const_iterator iter = values.begin(); iter != values.end(); iter++)
    {
      ret.first = ret.first.Min(*iter);
      ret.second = ret.second.Max(*iter);
    }
  }
  else	
  if (pElementSet)
  {
    TMinMax minArr;
    for(int i = 0; i < pElementSet->ElementSize(); i++)
    {
      minArr = MinMax(progressBase, pElementSet->Element(i),unit);
      ret.first = ret.first.Min(minArr.first);
      ret.second = ret.second.Max(minArr.second);
      progressBase.Step();
    }
  }
  else
    assert(pSymbol!=0);
  return ret;
}

template<int DATA_TYPE, class VALUE_DOMAIN>
typename VALUE_DOMAIN::TValue IValueDataInterfaceTempl<DATA_TYPE, VALUE_DOMAIN>::Average(IProgressBase& progressBase, const geo::IObject &object, const UNIT unit) const
{
  TValue ret;
  const geo::IArray* ptArr = dynamic_cast<const geo::IArray *> (&object);
  const geo::IPoint* pt = dynamic_cast<const geo::IPoint*> (&object);
  const geo::IElement *pElement = dynamic_cast<const geo::IElement*> (&object);
  const geo::IElementSet *pElementSet=dynamic_cast<const geo::IElementSet*> (&object);
  const ISymbol *pSymbol = dynamic_cast<const ISymbol*>(&object);

  if (ptArr && ptArr->Size() > 0)
  {
    double sumArr = 0;
    for(size_t i = 0; i < ptArr->Size(); i++)
    {
      TValue val = Average(progressBase, ptArr->ObjectBase(i), unit);
      if(val.Valid())
    sumArr += val.Value();
      else
    return TValue();
    }
  ret = sumArr / ptArr->Size();
  }
  else
  if(pt)
  {
    ret = ValuePoint(*pt,unit);
  }
  else
  if(pElement) {
    TValueVec values = ValueElement(*pElement,unit);
  if(!values.empty())
  {
      for (typename TValueVec::const_iterator iter = values.begin(); iter != values.end(); iter++)
      {
    if(iter->Valid())
          ret += *iter;
    else
          return TValue();
      }
  }
  }
  else	
  if (pElementSet && pElementSet->ElementSize() > 0)
  {
    double sumArr = 0;
    for(int i = 0; i < pElementSet->ElementSize(); i++)
    {
      TValue val = Average(progressBase, pElementSet->Element(i), unit);
      if(val.Valid())
    sumArr += val.Value();
      else
         return TValue();
      progressBase.Step();
    }
  ret = sumArr / pElementSet->ElementSize();
  }
  else
    assert(pSymbol!=0);
  return ret;
}


typedef IValueDataInterfaceTempl<IValueDataInterface::SCALAR, IValueDomainScalar> IValueDataInterfaceScalar;
typedef IValueDataInterfaceTempl<IValueDataInterface::VECTOR, IValueDomainVector> IValueDataInterfaceVector;
typedef IValueDataInterfaceTempl<IValueDataInterface::TENSOR, IValueDomainTensor> IValueDataInterfaceTensor;


template<class PARENT_COMPONENT>
class IValueDataInterfaceScalarTempl : public IValueDataInterfaceScalar
{
protected:
  const PARENT_COMPONENT* m_Parent;

public:
  IValueDataInterfaceScalarTempl(PARENT_COMPONENT& parent) : m_Parent(&parent) {}

  using IValueDataInterfaceTempl<IValueDataInterface::SCALAR, IValueDomainScalar>::MinMax;
  virtual TMinMax MinMax(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const
  {
  return m_Parent->MinMax(progressBase, unit);
  }

  using IValueDataInterfaceTempl<IValueDataInterface::SCALAR, IValueDomainScalar>::Average;
  virtual TValue Average(IProgressBase& progressBase, const UNIT unit = IQuantityDouble::SI_UNIT) const
  {
  return m_Parent->Average(progressBase, unit);
  }

  virtual TValue ValuePoint(const geo::IPoint& pt, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const
  {
  return m_Parent->ValuePoint(pt, unit, cb);
  }

  virtual TValueVec ValueElement(const geo::IElement& elm, const UNIT unit = IQuantityDouble::SI_UNIT, geo::IParallelInitializationCallback *cb = 0) const
  {
  return m_Parent->ValueElement(elm, unit, cb);
  }

};




#endif
