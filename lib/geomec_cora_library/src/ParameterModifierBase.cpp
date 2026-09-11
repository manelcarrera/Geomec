#include "ParameterModifierBase.h"
#include "Value.h"

namespace cora
{

CParameterModifierBase::CParameterModifierBase()
{
}

CParameterModifierBase::~CParameterModifierBase()
{
}

namespace
{

class TModify
{
public:
  TModify(CParameterModifierBase& parameterModifierBase)
  : m_parameterModifierBase(parameterModifierBase)
  {
  }

  void operator () (geo::CValue& rhs)
  {
    rhs = m_parameterModifierBase.modify(rhs);
  }

private:
  CParameterModifierBase& m_parameterModifierBase;
};

} // anonymous namespace

CParameterModifierBase::TValueVec CParameterModifierBase::modify(
  TValueVec& valueVec)
{
  TValueVec newValueVec = valueVec;
  TModify modify(*this);

  std::for_each(newValueVec.begin(), newValueVec.end(), modify);

  return newValueVec;
}

// protected

namespace
{

size_t SINGLE_VALUE = 1;
size_t FIRST_VALUE = 0;

} // anonymous namespace

double CParameterModifierBase::assignSingleValue(std::vector <double>& value)
{
  assert(value.size() == SINGLE_VALUE);

  return value[FIRST_VALUE];
}

namespace
{

size_t TWO_VALUES = 2;
size_t SECOND_VALUE = 1;

} // anonymous namespace

double CParameterModifierBase::assignFirstValue(std::vector <double>& value)
{
  assert(value.size() == TWO_VALUES);

  return value[FIRST_VALUE];
}

double CParameterModifierBase::assignSecondValue(std::vector <double>& value)
{
  assert(value.size() == TWO_VALUES);

  return value[SECOND_VALUE];
}

} // namespace cora
