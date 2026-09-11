#ifdef _WIN32
#define MAKESTRING2(str) #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4800 (bool to int in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable : 4800)
#endif
#include "getPressureParameters.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

#include "FormationBase.h"

namespace cora {

template <>
double calculateTotalSize<CFormationBase, geo::IElementSet>(CFormationBase *object,
                                                            const geo::IElementSet & /*elementSet*/) {
  double totalSize = 0;

  for (int s = 0; s < object->ElementSetSize(); ++s) {
    for (int e = 0; e < object->ElementSet(s).ElementSet().ElementSize(); ++e) {
      const geo::IElement &element = object->ElementSet(s).ElementSet().Element(e);

      totalSize += element.Size();
    }
  }

  return totalSize;
}

template <>
void getPressureParameters<CFormationBase, geo::IElementSet, CParameter::TFormationParameter4Pressure, CPressure>(
    TParameters &parameters, CModelBase * /*modelBase*/, const CDepletionStage &depletionStage, CFormationBase *object,
    const geo::IElementSet &elementSet, const CParameter::TFormationParameter4Pressure &parameterType,
    unsigned int valueTypeID, unsigned int /*valueTypeName*/, const QString &pressureType) {
  const CPressure &pressure = object->Pressure(depletionStage);
  const IValueComponentBase &valueComponentBase = pressure.Component();
  const IValueDataInterfaceScalar &valueDataInterfaceScalar = valueComponentBase.ScalarData();
  double min = std::numeric_limits<double>::max();
  double max = -std::numeric_limits<double>::max();
  double weightedMean = 0;
  double totalSize = calculateTotalSize(object, elementSet);

  for (int s = 0; s < object->ElementSetSize(); ++s) {
    for (int e = 0; e < object->ElementSet(s).ElementSet().ElementSize(); ++e) {
      const geo::IElement &element = object->ElementSet(s).ElementSet().Element(e);
      const IValueDomainScalar::TValueVec scalarValueVector = valueDataInterfaceScalar.ValueElement(element);
      double mean = CUtilities4ValueVector::calculateAverage(scalarValueVector);

      CUtilities4ValueVector::verifyValueVector(pressureType, scalarValueVector);

      max = std::max(max, CUtilities4ValueVector::calculateAverage(scalarValueVector));
      min = std::min(min, CUtilities4ValueVector::calculateAverage(scalarValueVector));
      weightedMean += mean * (element.Size() / totalSize);
    }
  }

  std::pair<geo::CValue, geo::CValue> range = std::make_pair(min, max);

  parameters.push_back(TParameter(new CParameter(
      parameterType, QString("%1_D%2").arg(pressure.Component().ExportLabel()).arg(depletionStage.Index()), range.first,
      range.second, weightedMean, object, depletionStage.Index(), valueTypeID)));
}

template <>
void getPressureParameters<CFormationBase, geo::IElementSet, CParameter::TFormationParameter4PressureChange, CPressure>(
    TParameters &parameters, CModelBase * /*modelBase*/, const CDepletionStage &depletionStage, CFormationBase *object,
    const geo::IElementSet &elementSet, const CParameter::TFormationParameter4PressureChange &parameterType,
    unsigned int valueTypeID, unsigned int /*valueTypeName*/, const QString &pressureType) {
  const CPressure &initialPressure = object->Pressure(depletionStage.InitialStage());
  const CPressure &pressure = object->Pressure(depletionStage);
  const IValueComponentBase &initialValueComponentBase = initialPressure.Component();
  const IValueComponentBase &valueComponentBase = pressure.Component();
  const IValueDataInterfaceScalar &initialValueDataInterfaceScalar = initialValueComponentBase.ScalarData();
  const IValueDataInterfaceScalar &valueDataInterfaceScalar = valueComponentBase.ScalarData();
  double min = std::numeric_limits<double>::max();
  double max = -std::numeric_limits<double>::max();
  double weightedMean = 0;
  double totalSize = calculateTotalSize(object, elementSet);

  for (int s = 0; s < object->ElementSetSize(); ++s) {
    for (int e = 0; e < object->ElementSet(s).ElementSet().ElementSize(); ++e) {
      const geo::IElement &element = object->ElementSet(s).ElementSet().Element(e);
      const IValueDomainScalar::TValueVec initialScalarValueVector =
          initialValueDataInterfaceScalar.ValueElement(element);
      const IValueDomainScalar::TValueVec scalarValueVector = valueDataInterfaceScalar.ValueElement(element);

      CUtilities4ValueVector::verifyValueVector(pressureType, initialScalarValueVector);
      CUtilities4ValueVector::verifyValueVector(pressureType, scalarValueVector);

      double initialMean = CUtilities4ValueVector::calculateAverage(initialScalarValueVector);
      double mean = CUtilities4ValueVector::calculateAverage(scalarValueVector);
      double initialAverage = CUtilities4ValueVector::calculateAverage(initialScalarValueVector);
      double average = CUtilities4ValueVector::calculateAverage(scalarValueVector);

      max = std::max(max, (average - initialAverage));
      min = std::min(min, (average - initialAverage));
      weightedMean += (mean - initialMean) * (element.Size() / totalSize);
    }
  }

  std::pair<geo::CValue, geo::CValue> range = std::make_pair(min, max);

  parameters.push_back(TParameter(new CParameter(
      parameterType, QString("Dlt_%1_D%2").arg(pressure.Component().ExportLabel()).arg(depletionStage.Index()),
      range.first, range.second, weightedMean, object, depletionStage.Index(), valueTypeID)));
}

} // namespace cora
