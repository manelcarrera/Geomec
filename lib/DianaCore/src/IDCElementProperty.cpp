
#include <typeinfo>

#include "IDCElementProperty.h"

/*!
\class IElementProperty
\brief Base class for materials, geometries and datas
*/
namespace dia {

IElementProperty::IElementProperty()
{

}

IElementProperty::~IElementProperty()
{

}

bool IElementProperty::operator<(const dia::IElementProperty &rhs) const
{
  if(typeid(*this) == typeid(rhs))
  return this < &rhs; // sort on pointer value

  // sort on type
  return (typeid(*this).before(typeid(rhs)) != 0);
}


int IElementProperty::Type() const
{
  return -1;
}

int IElementProperty::WriteFilosParamSize(IDianaRunner& /*diarunner*/) const
{
  return 0;
}

bool IElementProperty::WriteFilosParamName(IDianaRunner& /*diarunner*/, int /*i*/, char * /*name*/) const
{
  return false;
}

void IElementProperty::WriteFilosParamValues(IDianaRunner& /*diarunner*/, double * /*values*/, int /*stride*/) const
{
}

bool IElementProperty::isInterfaceElementMaterial() const
{
  return false;
}


} // namespace dia
