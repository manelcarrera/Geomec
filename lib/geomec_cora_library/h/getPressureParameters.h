#ifndef _cora_getPressureParameters_h_
#define _cora_getPressureParameters_h_

#include <limits>

#include "Parameter.h"
#include "IElementSet.h"
#include "Pressure.h"
#include "Utilities4ValueVector.h"

namespace cora
{

template <typename Object, typename ElementSet>
  double calculateTotalSize(Object* /*object*/, const ElementSet& elementSet)
{
  double totalSize = 0;

  for (int e = 0; e < elementSet.ElementSize(); ++e)
  {
    const geo::IElement& element = elementSet.Element(e);

    totalSize += element.Size();
  }

  return totalSize;
}

template <typename Object, typename ElementSet, typename ParameterType,
  typename Pressure>
  void getPressureParameters(TParameters& parameters,
    CModelBase* /*modelBase*/, const CDepletionStage& depletionStage,
    Object* object, const ElementSet& elementSet,
    const ParameterType& parameterType, unsigned int valueTypeID,
    unsigned int /*valueTypeName*/, const QString& pressureType)
{
  const Pressure& pressure = object->Pressure(depletionStage);
  const IValueComponentBase& valueComponentBase = pressure.Component();
  const IValueDataInterfaceScalar& valueDataInterfaceScalar =
    valueComponentBase.ScalarData();
  double min = std::numeric_limits <double> ::max();
  double max = -std::numeric_limits <double> ::max();
  double weightedMean = 0;
  double totalSize = calculateTotalSize(object, elementSet);

  for (int e = 0; e < elementSet.ElementSize(); ++e)
  {
    const geo::IElement& element = elementSet.Element(e);
    const IValueDomainScalar::TValueVec scalarValueVector =
      valueDataInterfaceScalar.ValueElement(element);
    double mean = CUtilities4ValueVector::calculateAverage(scalarValueVector);

    CUtilities4ValueVector::verifyValueVector(pressureType, scalarValueVector);

    max = std::max(max,
      CUtilities4ValueVector::calculateAverage(scalarValueVector));
    min = std::min(min,
      CUtilities4ValueVector::calculateAverage(scalarValueVector));
    weightedMean += mean * (element.Size() / totalSize);
  }

  std::pair <geo::CValue, geo::CValue> range = std::make_pair(min, max);

  parameters.push_back(TParameter(new CParameter(parameterType,
    QString("%1_D%2").arg(pressure.Component().ExportLabel()).
      arg(depletionStage.Index()),
    range.first, range.second, weightedMean, object, depletionStage.Index(),
    valueTypeID)));
}

template <>
  double calculateTotalSize <CFormationBase, geo::IElementSet> (
    CFormationBase* object, const geo::IElementSet& elementSet);

template <>
  void getPressureParameters <CFormationBase, geo::IElementSet,
    CParameter::TFormationParameter4Pressure, CPressure> (
      TParameters& parameters, CModelBase* modelBase,
      const CDepletionStage& depletionStage, CFormationBase* object,
      const geo::IElementSet& elementSet,
      const CParameter::TFormationParameter4Pressure& parameterType,
      unsigned int valueTypeID, unsigned int valueTypeName,
      const QString& pressureType);

template <>
  void getPressureParameters <CFormationBase, geo::IElementSet,
    CParameter::TFormationParameter4PressureChange, CPressure> (
      TParameters& parameters, CModelBase* modelBase,
      const CDepletionStage& depletionStage, CFormationBase* object,
      const geo::IElementSet& elementSet,
      const CParameter::TFormationParameter4PressureChange& parameterType,
      unsigned int valueTypeID, unsigned int valueTypeName,
      const QString& pressureType);

} // namespace cora

#endif  // _cora_getPressureParameters_h_
