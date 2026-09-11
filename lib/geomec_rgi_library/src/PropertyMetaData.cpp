#include "PropertyMetaData.h"

#include <QString>
#include <QRegExp>

#include "ivaluecomposite.h"
#include "ivaluecomponent.h"

namespace GeomecRGI
{

QString translate_scalar_property_type(const QString& exportLabel, bool *processed)
{
  if (exportLabel.startsWith("CompactionCap"))
    *processed = true;
  else if (exportLabel.startsWith("Depth"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EPStress"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EStressInvMean"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EStressInvPressure"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EWPStressAxial"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EWPStressRadialMax"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_EWPStressRadialMin"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_FaultEStressNormal"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_FaultStressShearL"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_FaultTStressNormal"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_PStress1L"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_PStress2L"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_PStress3L"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_StressInvMean"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_StressInvPressure"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_Thickness"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_WPStressAxial"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_WPStressRadialMax"))
    *processed = true;
  else if (exportLabel.startsWith("Dlt_WPStressRadialMin"))
    *processed = true;
  else if (exportLabel.startsWith("Easting"))
    *processed = true;
  else if (exportLabel.startsWith("EStressGradient"))
    *processed = true;
  else if (exportLabel.startsWith("EStressInvMean"))
    *processed = true;
  else if (exportLabel.startsWith("EStressInvPressure"))
    *processed = true;
  else if (exportLabel.startsWith("EWPStressAxial"))
    *processed = true;
  else if (exportLabel.startsWith("EWPStressRadialMax"))
    *processed = true;
  else if (exportLabel.startsWith("EWPStressRadialMin"))
    *processed = true;
  else if (exportLabel.startsWith("FaultEStressNormal"))
    *processed = true;
  else if (exportLabel.startsWith("FaultPlasticEnergy"))
    *processed = true;
  else if (exportLabel.startsWith("FaultShearCap"))
    *processed = true;
  else if (exportLabel.startsWith("FaultTStressNormal"))
    *processed = true;
  else if (exportLabel.startsWith("Northing"))
    *processed = true;
  else if (exportLabel.startsWith("PermeabilityMultiplierHigh"))
    *processed = true;
  else if (exportLabel.startsWith("PermeabilityMultiplierInterm"))
    *processed = true;
  else if (exportLabel.startsWith("PermeabilityMultiplierLow"))
    *processed = true;
  else if (exportLabel.startsWith("PlasticEnergy"))
    *processed = true;
  else if (exportLabel.startsWith("SHtot/Svtot"))
    *processed = true;
  else if (exportLabel.startsWith("SHtotAzimuth"))
    *processed = true;
  else if (exportLabel.startsWith("ShearCap"))
    *processed = true;
  else if (exportLabel.startsWith("Shtot/Svtot"))
    *processed = true;
  else if (exportLabel.startsWith("StrainInvI2"))
    *processed = true;
  else if (exportLabel.startsWith("StrainInvI3"))
    *processed = true;
  else if (exportLabel.startsWith("StrainInvVolumetric"))
    *processed = true;
  else if (exportLabel.startsWith("StrainInvVonMises"))
    *processed = true;
  else if (exportLabel.startsWith("StrainPlasticInvI2"))
    *processed = true;
  else if (exportLabel.startsWith("StrainPlasticInvI3"))
    *processed = true;
  else if (exportLabel.startsWith("StrainPlasticInvVolumetric"))
    *processed = true;
  else if (exportLabel.startsWith("StrainPlasticInvVonMises"))
    *processed = true;
  else if (exportLabel.startsWith("StressGradient"))
    *processed = true;
  else if (exportLabel.startsWith("StressInvMean"))
    *processed = true;
  else if (exportLabel.startsWith("StressInvPressure"))
    *processed = true;
  else if (exportLabel.startsWith("Thickness"))
    *processed = true;
  else if (exportLabel.startsWith("WPAxialStrainPlastic"))
    *processed = true;
  else if (exportLabel.startsWith("WPAxialStrain"))
    *processed = true;
  else if (exportLabel.startsWith("WPStressAxial"))
    *processed = true;
  else if (exportLabel.startsWith("WPStressRadialMax"))
    *processed = true;
  else if (exportLabel.startsWith("WPStressRadialMin"))
    *processed = true;

  if (*processed)
    return "Dummy";
  else
    return exportLabel;
}

QString translate_vector_property_type(const QString& exportLabel, const std::string& propName, bool *processed)
{
  if (exportLabel.startsWith("FaultPlasticSlip"))
    *processed = true;
  else if (propName == "PStrain1N" || propName == "PStrain1E" || propName == "PStrain1V")
    *processed = true;
  else if (propName == "PStrain2N" || propName == "PStrain2E" || propName == "PStrain2V")
    *processed = true;
  else if (propName == "PStrain3N" || propName == "PStrain3E" || propName == "PStrain3V")
    *processed = true;
  else if (propName == "PStrain1PlasticN" || propName == "PStrain1PlasticE" || propName == "PStrain1PlasticV")
    *processed = true;
  else if (propName == "PStrain2PlasticN" || propName == "PStrain2PlasticE" || propName == "PStrain2PlasticV")
    *processed = true;
  else if (propName == "PStrain3PlasticN" || propName == "PStrain3PlasticE" || propName == "PStrain3PlasticV")
    *processed = true;

  if (*processed)
  {
    return "Strain";
  }

  if (propName == "PStress1N" || propName == "PStress1E" || propName == "PStress1V")
    *processed = true;
  else if (propName == "PStress2N" || propName == "PStress2E" || propName == "PStress2V")\
    *processed = true;
  else if (propName == "PStress3N" || propName == "PStress3E" || propName == "PStress3V")
    *processed = true;
  else if (propName == "EPStress1N" || propName == "EPStress1E" || propName == "EPStress1V")
    *processed = true;
  else if (propName == "EPStress2N" || propName == "EPStress2E" || propName == "EPStress2V")\
    *processed = true;
  else if (propName == "EPStress3N" || propName == "EPStress3E" || propName == "EPStress3V")
    *processed = true;
  else if (exportLabel.startsWith("FaultStressShear"))
    *processed = true;

  if (*processed)
  {
    return "Stress";
  }

  return exportLabel;
}

void PropertyMetaData::Setup(const std::map<std::string, const IValueComponentBase *>& properties, std::map<std::string, PropertyMetaData>& metaData)
{
  PropertyMetaData meta;
  QRegExp regexDepStage("_D[0-9]+$");

  for (std::map<std::string, const IValueComponentBase *>::const_iterator it = properties.begin(); it != properties.end(); ++it)
  {
    meta.propertyName = it->first;
    meta.componentIndex = -1;

    QString typeName = it->second->Parent().ExportLabel();
    if (typeName.isEmpty())
    {
      typeName = it->second->ExportLabel().remove(regexDepStage);
    }

    if (meta.propertyName == "Dlt_PStress1L" || meta.propertyName == "Dlt_PStress2L" || meta.propertyName == "Dlt_PStress3L")
    {
      typeName = meta.propertyName.c_str();
    }

    bool processed = false;

    typeName = translate_vector_property_type(typeName, meta.propertyName, &processed);

    if (processed)
    {
      meta.typeName = typeName.toStdString();
      meta.typeType = PropertyMetaData::TENSOR_VECTOR;

      char component = meta.propertyName.back();

      switch (component)
      {
      case 'N':
        meta.componentIndex = 0;
        break;
      case 'E':
        meta.componentIndex = 1;
        break;
      case 'V':
        meta.componentIndex = 2;
        break;
      default:
        break;
      }

      if (meta.componentIndex >= 0)
        meta.propertyName = meta.propertyName.substr(0, meta.propertyName.size() - 1);
    }
    else
    {
      typeName = translate_scalar_property_type(typeName, &processed);

      if (processed)
      {
        meta.typeName = typeName.toStdString();
        meta.typeType = PropertyMetaData::SCALAR;
        meta.componentIndex = 0;
      }
      else
      {
        if (typeName.startsWith("Dlt_"))
          typeName.remove(0, 4);
        if (typeName.startsWith("Fault"))
          typeName.remove(0, 5);

        if (typeName.startsWith("WPStress"))
          typeName.remove(0, 2);
        else if (typeName.startsWith("EWPStress"))
          typeName.remove(0, 3);
        else if (typeName.startsWith("WPAxial"))
          typeName.remove(0, 7);
        else if (typeName.startsWith("EPStress"))
          typeName.remove(0, 2);
        else if (typeName.startsWith("EStress") || typeName.startsWith("PStress") || typeName.startsWith("PStrain") || typeName.startsWith("TStress"))
          typeName.remove(0, 1);

        if (typeName.startsWith("Stress"))
          typeName = "Stress";
        else if (typeName.startsWith("Strain"))
          typeName = "Strain";
        else if (typeName.startsWith("Displacement"))
          typeName = "Displacement";
        else if (typeName.startsWith("Aperture") && typeName != "ApertureParameter")
          typeName.replace("Aperture", "V0");
        else if (typeName.startsWith("Temperature"))
          typeName = "Temperature";
        else if (typeName.startsWith("Pore pressure") || typeName == "Consolidation")
          typeName = "Pressure";
        else if (typeName.startsWith("VolumetricStrain"))
          typeName = "VolumetricStrain";
        else if (typeName.startsWith("Porosity"))
          typeName = "Porosity";
        else if (typeName.startsWith("Gamma"))
          typeName = "Gamma";

        meta.typeName = typeName.toStdString();

        int componentSize = it->second->Parent().ComponentSize();

        switch (componentSize)
        {
        case 1:
          meta.typeType = PropertyMetaData::SCALAR;
          meta.componentIndex = 0;
          break;
        case 2:
          meta.typeType = PropertyMetaData::DUAL_SCALAR;
          meta.componentIndex = 0;
          break;
        case 3:
        case 4:
        case 5:
        {
          meta.typeType = PropertyMetaData::VECTOR;
          char component = meta.propertyName.back();

          switch (component)
          {
          case 'N':
            meta.componentIndex = 0;
            break;
          case 'E':
            meta.componentIndex = 1;
            break;
          case 'V':
            meta.componentIndex = 2;
            break;
          default:
            break;
          }

          if (meta.componentIndex >= 0)
            meta.propertyName = meta.propertyName.substr(0, meta.propertyName.size() - 1);
        }
        break;
        case 6:
        case 7:
        case 8:
        {
          meta.typeType = PropertyMetaData::TENSOR;

          std::string component = meta.propertyName.substr(meta.propertyName.size() - 2, 2);

          if (component == "NN")
            meta.componentIndex = 0;
          else if (component == "EE")
            meta.componentIndex = 1;
          else if (component == "VV")
            meta.componentIndex = 2;
          else if (component == "NE")
            meta.componentIndex = 3;
          else if (component == "EV")
            meta.componentIndex = 4;
          else if (component == "NV")
            meta.componentIndex = 5;

          if (meta.componentIndex >= 0)
            meta.propertyName = meta.propertyName.substr(0, meta.propertyName.size() - 2);
        }
        break;
        default:
          break;
        }
      }
    }

    metaData.insert(std::make_pair(it->first, meta));
  }
}

}

