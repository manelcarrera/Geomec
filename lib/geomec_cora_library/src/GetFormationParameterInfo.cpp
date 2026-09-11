#include <limits>

#include "FormationBase.h"
#include "GetFormationParameterInfo.h"
#include "GetValueTypeInfo.h"
#include "LibraryMaterialParameter.h"
#include "Utilities4ValueVector.h"
#include "ValueType.h"
#include "ValueVectorContainsInvalidValues.h"

namespace cora {

CGetFormationParameterInfo::CGetFormationParameterInfo(const CFormationBase *formationBase)
    : m_formationBase(formationBase), m_totalVolume(calculateVolume(m_formationBase)) {}

double CGetFormationParameterInfo::getMean(const CLibraryMaterialParameter &libraryMaterialParameter,
                                           const std::set<CValueType *> &valueTypes) const {
  for (std::set<CValueType *>::const_iterator valueType = valueTypes.begin(); valueType != valueTypes.end();
       ++valueType) {
    if (libraryMaterialParameter.ValueTypeID() == (*valueType)->TypeId()) {
      if (m_formationBase->ElementSetSize() > 0) {
        return getMean(libraryMaterialParameter, *valueType);
      } else {
        assert(false);

        return 0;
      }
    }
  }

  return libraryMaterialParameter.Value();
}

std::pair<geo::CValue, geo::CValue>
CGetFormationParameterInfo::getRange(const CLibraryMaterialParameter &libraryMaterialParameter,
                                     const std::set<CValueType *> &valueTypes) const {
  CGetValueTypeInfo &getValueTypeInfo(CGetValueTypeInfo::instance(
      const_cast<CModelBase *>(dynamic_cast<const CModelBase *>(&m_formationBase->Model()))));
  std::pair<geo::CValue, geo::CValue> range = getValueTypeInfo.getRange(libraryMaterialParameter);

  for (std::set<CValueType *>::const_iterator valueType = valueTypes.begin(); valueType != valueTypes.end();
       ++valueType) {
    if (libraryMaterialParameter.ValueTypeID() == (*valueType)->TypeId()) {
      if (m_formationBase->ElementSetSize() > 0) {
        return getRange(libraryMaterialParameter, *valueType);
      } else {
        assert(false);

        return range;
      }
    }
  }

  return range;
}

// private

double CGetFormationParameterInfo::calculateVolume(const CFormationBase *formationBase) {
  double totalVolume = 0;

  for (int set = 0; set < formationBase->ElementSetSize(); ++set) {
    const IFormationElementSet &formationElementSet = formationBase->ElementSet(set);
    const geo::IElementSet &elementSet = formationElementSet.ElementSet();

    for (int element = 0; element < elementSet.ElementSize(); ++element) {
      const geo::IBody &body = dynamic_cast<const geo::IBody &>(elementSet.Element(element));

      totalVolume += body.Volume();
    }
  }

  return totalVolume;
}

double CGetFormationParameterInfo::getMean(const CLibraryMaterialParameter & /*libraryMaterialParameter*/,
                                           const CValueType *valueType) const {
  double mean = 0;

  for (int set = 0; set < m_formationBase->ElementSetSize(); ++set) {
    const IFormationElementSet &formationElementSet = m_formationBase->ElementSet(set);
    const geo::IElementSet &elementSet = formationElementSet.ElementSet();

    for (int element = 0; element < elementSet.ElementSize(); ++element) {
      IValueDomainScalar::TValueVec valueVec =
          valueType->Component().ScalarData().ValueElement(elementSet.Element(element));
      double totalValue = 0;
      size_t count = 0;

      for (IValueDomainScalar::TValueVec::const_iterator value = valueVec.begin(); value != valueVec.end(); ++value) {
        if ((*value).Valid()) {
          ++count;
          totalValue += (*value).Value();
        }
      }

      mean += (elementSet.Element(element).Size() / m_totalVolume) * (totalValue / (count == 0 ? 1 : count));
    }
  }

  return mean;
}

namespace {

void validateValueVector(const IValueDomainScalar::TValueVec &valueVec, const CValueType *valueType) {
  if (!CUtilities4ValueVector::isValueVectorValid(valueVec)) {
    throw CValueVectorContainsInvalidValues(valueType->ExportLabel());
  }
}

} // anonymous namespace

std::pair<geo::CValue, geo::CValue>
CGetFormationParameterInfo::getRange(const CLibraryMaterialParameter & /*libraryMaterialParameter*/,
                                     const CValueType *valueType) const {
  std::pair<geo::CValue, geo::CValue> range =
      std::make_pair(std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());

  for (int set = 0; set < m_formationBase->ElementSetSize(); ++set) {
    const IFormationElementSet &formationElementSet = m_formationBase->ElementSet(set);
    const geo::IElementSet &elementSet = formationElementSet.ElementSet();

    for (int element = 0; element < elementSet.ElementSize(); ++element) {
      IValueDomainScalar::TValueVec valueVec =
          valueType->Component().ScalarData().ValueElement(elementSet.Element(element));

      validateValueVector(valueVec, valueType);

      range.first = std::min(range.first.Value(), CUtilities4ValueVector::calculateAverage(valueVec));
      range.second = std::max(range.second.Value(), CUtilities4ValueVector::calculateAverage(valueVec));
    }
  }

  return range;
}

} // namespace cora
