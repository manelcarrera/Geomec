#include "ExportResultDataXML.h"
#include "BaseEntryTypes.h"
#include "ElementSet.h"
#include "ExportResultData.h"
#include "FormationBase.h"
#include "FormationPlane.h"
#include "HexaEntryTypes.h"
#include "HexaFormation.h"
#include "HorizonBase.h"
#include "HotSpot.h"
#include "IColorScaleEntry.h"
#include "MaterialResultTree.h"
#include "MeshResultTree.h"
#include "ModelBase.h"
#include "NewWellPath.h"
#include "NonMeshedSurface.h"
#include "PointSet.h"
#include "ResultTree.h"
#include <QFile>
#include <QTextStream>
#include <QtXml/QDomElement>
#include <vector>

namespace erd_xml {

QString AttributeStringValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));

  return domAttrName.value();
}

double AttributeDoubleValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));
  bool bOk;
  double dValue = domAttrName.value().toDouble(&bOk);
  if (!bOk)
    throw CException(QObject::tr("Value of attribute '%1' should be floating point").arg(sName));

  return dValue;
}

QRgb AttributeColorValue(QDomElement &domElement, const QString &sName) {
  QDomAttr domAttrName = domElement.attributeNode(sName);
  if (domAttrName.isNull())
    throw CException(QObject::tr("No '%1' attribute found in '%2'").arg(sName).arg(domElement.tagName()));
  QRgb dRgb = QColor(domAttrName.value()).rgb();

  return dRgb;
}

QString AttributeColorValue(QRgb color) {
  char buffer[8];
  sprintf(buffer, "#%02x%02x%02x", qRed(color), qGreen(color), qBlue(color));

  return buffer;
}

bool isTrue(const QString &value) {
  if (!value.isEmpty()) {
    QString lower = value.toLower();
    if ("y" == lower || "yes" == lower || "true" == lower || "check" == lower)
      return true;

    bool isInt;
    int i = lower.toInt(&isInt);

    if (isInt && i != 0)
      return true;
  }

  return false;
}

class CExportResultMacroSettings {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroSettings(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {
    QDomElement child = domElement.firstChildElement("Setting");
    while (!child.isNull()) {
      QString name = AttributeStringValue(child, "Name");
      QString value = AttributeStringValue(child, "Value");

      CExportResultDataSettings &settings = m_exportResultData.m_settings;

      if ("AutoExport" == name) {
        settings.m_export = isTrue(value);
      } else if ("ExportFolder" == name) {
        settings.m_folder = value;
      } else if ("ExportFile" == name) {
        settings.m_file = value;
      } else if ("Calculation" == name) {
        QStringList vList = value.split(",");

        for (QStringList::iterator v = vList.begin(); v != vList.end(); ++v) {
          if ("All" == *v)
            settings.m_bLinear = settings.m_bNonLinear = settings.m_bHeat = settings.m_bMixture =
                settings.m_bMixtureContainment = true;
          else if ("Linear" == *v)
            settings.m_bLinear = true;
          else if ("Nonlinear" == *v)
            settings.m_bNonLinear = true;
          else if ("Heat" == *v)
            settings.m_bHeat = true;
          else if ("Mixture" == *v)
            settings.m_bMixture = true;
          else if ("MixtureContainment" == *v)
            settings.m_bMixtureContainment = true;
        }
      } else if ("Unit" == name) {
        if ("SI" == value)
          settings.m_nUnit = 0;
        else if ("FIELD" == value)
          settings.m_nUnit = 1;
      } else if ("Mapping" == name) {
        if ("InterpolatedPoints" == value)
          settings.m_nWeightingType = 0;
        else if ("None" == value)
          settings.m_nWeightingType = 1;
        else if ("Voight" == value)
          settings.m_nWeightingType = 2;
        else if ("Reuss" == value)
          settings.m_nWeightingType = 3;
      } else if ("Geography" == name) {
        if ("Element" == value)
          settings.m_nCenterPoints = 0;
        else if ("Center" == value)
          settings.m_nCenterPoints = 1;
        else if ("WellpathCenter" == value)
          settings.m_nCenterPoints = 2;
      } else if ("FormationNames" == name) {
        settings.m_nFormationNames = isTrue(value) ? 1 : 0;
      } else if ("WellpathMeshIntersection" == name) {
        settings.m_nWellPoints = isTrue(value) ? 1 : 0;
      }

      child = child.nextSiblingElement("Setting");
    }
  }
  void Save(QDomElement &domElement) {
    QDomElement child;

    domElement.appendChild(domElement.ownerDocument().createComment(" AutoExport = yes | no "));
    domElement.appendChild(domElement.ownerDocument().createComment(" ExportFolder = folder location "));
    domElement.appendChild(domElement.ownerDocument().createComment(" ExportFile = file name "));
    domElement.appendChild(domElement.ownerDocument().createComment(" AutoExport = yes | no "));
    domElement.appendChild(
        domElement.ownerDocument().createComment(" Calculation = All | Linear | Nonlinear | Heat | Mixture | "
                                                 "MixtureContainment or a comma-separated list thereof "));
    domElement.appendChild(domElement.ownerDocument().createComment(" Unit = SI | FIELD "));
    domElement.appendChild(
        domElement.ownerDocument().createComment(" Mapping = InterpolatedPoints | None | Voight | Reuss "));
    domElement.appendChild(domElement.ownerDocument().createComment(" Geography = Element | Center | WellpathCenter "));
    domElement.appendChild(domElement.ownerDocument().createComment(" FormationNames = yes | no "));
    domElement.appendChild(domElement.ownerDocument().createComment(" WellpathMeshIntersection = yes | no "));

    CExportResultDataSettings &settings = m_exportResultData.m_settings;

    if (settings.m_export) {
      child = domElement.ownerDocument().createElement("Setting");
      child.setAttribute("Value", "yes");
      child.setAttribute("Name", "AutoExport");
      domElement.appendChild(child);
    }
    if (!settings.m_folder.isEmpty()) {
      child = domElement.ownerDocument().createElement("Setting");
      child.setAttribute("Value", settings.m_folder);
      child.setAttribute("Name", "ExportFolder");
      domElement.appendChild(child);
    }
    if (!settings.m_file.isEmpty()) {
      child = domElement.ownerDocument().createElement("Setting");
      child.setAttribute("Value", settings.m_file);
      child.setAttribute("Name", "ExportFile");
      domElement.appendChild(child);
    }

    if (settings.m_bLinear || settings.m_bNonLinear || settings.m_bHeat || settings.m_bMixture ||
        settings.m_bMixtureContainment) {

      if (settings.m_bLinear && settings.m_bNonLinear && settings.m_bHeat && settings.m_bMixture &&
          settings.m_bMixtureContainment) {
        child = domElement.ownerDocument().createElement("Setting");
        child.setAttribute("Value", "All");
        child.setAttribute("Name", "Calculation");
        domElement.appendChild(child);
      } else {
        QStringList vList;

        if (settings.m_bLinear)
          vList.append("Linear");
        if (settings.m_bNonLinear)
          vList.append("Nonlinear");
        if (settings.m_bHeat)
          vList.append("Heat");
        if (settings.m_bMixture)
          vList.append("Mixture");
        if (settings.m_bMixtureContainment)
          vList.append("MixtureContainment");

        if (vList.size() > 0) {
          child = domElement.ownerDocument().createElement("Setting");
          child.setAttribute("Value", vList.join(","));
          child.setAttribute("Name", "Calculation");
          domElement.appendChild(child);
        }
      }
    }

    child = domElement.ownerDocument().createElement("Setting");
    child.setAttribute("Value", settings.m_nUnit == 0 ? "SI" : "FIELD");
    child.setAttribute("Name", "Unit");
    domElement.appendChild(child);

    child = domElement.ownerDocument().createElement("Setting");
    switch (settings.m_nWeightingType) {
    case 0:
      child.setAttribute("Value", "InterpolatedPoints");
      break;
    case 1:
      child.setAttribute("Value", "None");
      break;
    case 2:
      child.setAttribute("Value", "Voight");
      break;
    case 3:
      child.setAttribute("Value", "Reuss");
      break;
    }
    child.setAttribute("Name", "Mapping");
    domElement.appendChild(child);

    child = domElement.ownerDocument().createElement("Setting");
    switch (settings.m_nCenterPoints) {
    case 0:
      child.setAttribute("Value", "Element");
      break;
    case 1:
      child.setAttribute("Value", "Center");
      break;
    case 2:
      child.setAttribute("Value", "WellpathCenter");
      break;
    }
    child.setAttribute("Name", "Geography");
    domElement.appendChild(child);

    if (settings.m_nFormationNames) {
      child = domElement.ownerDocument().createElement("Setting");
      child.setAttribute("Value", "yes");
      child.setAttribute("Name", "FormationNames");
      domElement.appendChild(child);
    }

    if (settings.m_nWellPoints) {
      child = domElement.ownerDocument().createElement("Setting");
      child.setAttribute("Value", "yes");
      child.setAttribute("Name", "WellpathMeshIntersection");
      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroDeformation {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroDeformation(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {
    QDomElement child = domElement.firstChildElement("Deformation");
    while (!child.isNull()) {
      QString timeStep = AttributeStringValue(child, "TimeStep");
      QString type = AttributeStringValue(child, "Type");
      QString calc = AttributeStringValue(child, "Calculation");

      if ("Set" == type) {
        m_exportResultData.m_deformation.m_nType = IExportResultDataFilter::SET;
        if ("Initial" == timeStep)
          m_exportResultData.m_deformation.m_nSetValue = CExportResultDataDeformation::INITIAL;
        else if ("Last" == timeStep)
          m_exportResultData.m_deformation.m_nSetValue = CExportResultDataDeformation::LAST;
      } else if ("ByName" == type) {
        m_exportResultData.m_deformation.m_nType = IExportResultDataFilter::BYNAME;
        m_exportResultData.m_deformation.m_sName = timeStep;
      }

      if ("Linear" == calc)
        m_exportResultData.m_deformation.m_nCalculation = CExportResultDataDeformation::LINEAR;
      else if ("Nonlinear" == calc)
        m_exportResultData.m_deformation.m_nCalculation = CExportResultDataDeformation::NONLIN;
      else if ("Mixture" == calc)
        m_exportResultData.m_deformation.m_nCalculation = CExportResultDataDeformation::MIXTURE;

      child = child.nextSiblingElement("Deformation");
    }
  }

  void Save(QDomElement &domElement) {
    domElement.appendChild(domElement.ownerDocument().createComment(
        " Deformation: TimeStep = Initial | Last when Type = Set || TimeStep = name when Type = ByName; Calculation = "
        "Linear | Nonlinear | Mixture "));

    if (m_exportResultData.m_deformation.m_nType != IExportResultDataFilter::NONE) {
      QDomElement child = domElement.ownerDocument().createElement("Deformation");

      if (m_exportResultData.m_deformation.m_nType == IExportResultDataFilter::SET) {
        child.setAttribute("Type", "Set");
        if (m_exportResultData.m_deformation.m_nSetValue == CExportResultDataDeformation::INITIAL)
          child.setAttribute("TimeStep", "Initial");
        else if (m_exportResultData.m_deformation.m_nSetValue == CExportResultDataDeformation::LAST)
          child.setAttribute("TimeStep", "Last");
      } else if (m_exportResultData.m_deformation.m_nType == IExportResultDataFilter::BYNAME) {
        child.setAttribute("Type", "ByName");
        child.setAttribute("TimeStep", m_exportResultData.m_deformation.m_sName);
      }
      switch (m_exportResultData.m_deformation.m_nCalculation) {
      case CExportResultDataDeformation::LINEAR:
        child.setAttribute("Calculation", "Linear");
        break;
      case CExportResultDataDeformation::NONLIN:
        child.setAttribute("Calculation", "Nonlinear");
        break;
      case CExportResultDataDeformation::MIXTURE:
        child.setAttribute("Calculation", "Mixture");
        break;
      }
      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroHotSpot {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroHotSpot(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {
    QDomElement child = domElement.firstChildElement("HotSpot");
    while (!child.isNull()) {
      CExportResultDataHotSpot &hotspot = m_exportResultData.m_hotspot;

      hotspot.m_sName = AttributeStringValue(child, "Name");

      hotspot.m_bCenterIsTransparent = isTrue(AttributeStringValue(child, "CenterIsTransparent"));

      hotspot.m_nMinColor = AttributeColorValue(child, "MinColor");
      hotspot.m_nMaxColor = AttributeColorValue(child, "MaxColor");

      hotspot.m_dMinMaxValue.first = AttributeDoubleValue(child, "MinValue");
      hotspot.m_dMinMaxValue.second = AttributeDoubleValue(child, "MaxValue");

      child = child.nextSiblingElement("HotSpot");
    }
  }

  void Save(QDomElement &domElement) {
    domElement.appendChild(
        domElement.ownerDocument().createComment(" HotSpot: Name = name; CenterIsTransparent = yes | no ; MinColor = "
                                                 "#000000; MaxColor = #ffffff; MinValue = double; MaxValue = double "));
    if (m_exportResultData.m_hotspot.m_nType != IExportResultDataFilter::NONE) {
      QDomElement child = domElement.ownerDocument().createElement("HotSpot");

      CExportResultDataHotSpot &hotspot = m_exportResultData.m_hotspot;

      child.setAttribute("Name", hotspot.m_sName);
      child.setAttribute("CenterIsTransparent", hotspot.m_bCenterIsTransparent ? "yes" : "no");
      child.setAttribute("MinColor", AttributeColorValue(hotspot.m_nMinColor));
      child.setAttribute("MaxColor", AttributeColorValue(hotspot.m_nMaxColor));
      child.setAttribute("MinValue", hotspot.m_dMinMaxValue.first);
      child.setAttribute("MaxValue", hotspot.m_dMinMaxValue.second);

      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroDepletionStageFilter {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroDepletionStageFilter(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {

    QDomElement child = domElement.firstChildElement("DepletionStageFilter");
    while (!child.isNull()) {
      QString type = AttributeStringValue(child, "Type");
      QString value = AttributeStringValue(child, "Value");

      CExportResultDataDepletionStageFilter filter;

      if ("Set" == type) {
        filter.m_nType = IExportResultDataFilter::SET;

        if ("Initial" == value)
          filter.m_nSetValue = CExportResultDataDepletionStageFilter::INITIAL;
        else if ("Last" == value)
          filter.m_nSetValue = CExportResultDataDepletionStageFilter::LAST;
        else if ("All" == value)
          filter.m_nSetValue = CExportResultDataDepletionStageFilter::ALL;
        else
          filter.m_nType = IExportResultDataFilter::NONE; // unknown
      } else if ("ByType" == type) {
        filter.m_nType = IExportResultDataFilter::BYTYPE;

        filter.m_nTypeValue = CDepletionStage::NONE;
        if ("Results" == value)
          filter.m_nTypeValue = CDepletionStage::RESULTS;
        else if ("Branch" == value)
          filter.m_nTypeValue = CDepletionStage::BRANCH;
        else if ("Phase" == value)
          filter.m_nTypeValue = CDepletionStage::PHASE;
      } else if ("ByName" == type) {
        filter.m_nType = IExportResultDataFilter::BYNAME;
        filter.m_sName = value;
      }

      if (filter.m_nType != IExportResultDataFilter::NONE)
        m_exportResultData.m_depletionStageFilterVector.m_depletionStageFilters.push_back(filter);

      child = child.nextSiblingElement("DepletionStageFilter");
    }
  }
  void Save(QDomElement &domElement) {
    domElement.appendChild(domElement.ownerDocument().createComment(
        " Value = All | Initial | Last when Type = Set || Value = name when Type = ByName || Value = Results | Branch "
        "| Phase when Type = ByType "));

    for (size_t i = 0; i < m_exportResultData.m_depletionStageFilterVector.m_depletionStageFilters.size(); ++i) {
      QDomElement child;

      const CExportResultDataDepletionStageFilter &filter =
          m_exportResultData.m_depletionStageFilterVector.m_depletionStageFilters[i];

      if (filter.m_nType == IExportResultDataFilter::NONE) // the only check we do (for now)
        continue;

      switch (filter.m_nType) {
      case IExportResultDataFilter::SET:
        child.setAttribute("Type", "Set");
        switch (filter.m_nSetValue) {
        case CExportResultDataDepletionStageFilter::INITIAL:
          child.setAttribute("Value", "Initial");
          break;
        case CExportResultDataDepletionStageFilter::LAST:
          child.setAttribute("Value", "Last");
          break;
        case CExportResultDataDepletionStageFilter::ALL:
          child.setAttribute("Value", "All");
          break;
        }
        break;
      case IExportResultDataFilter::BYTYPE:
        child.setAttribute("Type", "ByType");
        switch (filter.m_nTypeValue) {
        case CDepletionStage::RESULTS:
          child.setAttribute("Value", "Results");
          break;
        case CDepletionStage::BRANCH:
          child.setAttribute("Value", "Branch");
          break;
        case CDepletionStage::PHASE:
          child.setAttribute("Value", "Phase");
          break;
        }
        break;
      case IExportResultDataFilter::BYNAME:
        child.setAttribute("Type", "ByName");
        child.setAttribute("Value", filter.m_sName);
        break;
      }

      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroModelFilter {
  CExportResultData &m_exportResultData;

  typedef std::vector<CFormationPlane::ePlaneType> TFormationPlaneTypes;
  typedef std::map<const CHexaFormation *, TFormationPlaneTypes> TFormationPlaneTypeSet;

public:
  CExportResultMacroModelFilter(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {
    QDomElement child = domElement.firstChildElement("ModelFilter");
    while (!child.isNull()) {
      QString filterName = AttributeStringValue(child, "Filter");
      QString type = AttributeStringValue(child, "Type");
      QString value = AttributeStringValue(child, "Value");

      CExportResultDataModelFilter filter;

      if ("Faults" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::FAULTS;
      else if ("Horizons" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::HORIZONS;
      else if ("Formations" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::FORMATIONS;
      else if ("FormationPlanes" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::FORMATIONPLANES;
      else if ("Wellpaths" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::WELLPATHS;
      else if ("Pointsets" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::POINTSETS;
      else if ("Elementsets" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::ELEMENTSETS;
      else if ("NonMeshedSurfaces" == filterName)
        filter.m_nFilter = CExportResultDataModelFilter::NONMESHEDSURFACES;
      else
        continue;

      if ("Set" == type) {
        filter.m_nType = IExportResultDataFilter::SET;
        if ("All" == value)
          filter.m_nSetValue = CExportResultDataModelFilter::ALL;
        else
          continue;
      } else if ("ByName" == type) {
        filter.m_nType = IExportResultDataFilter::BYNAME;

        if (filter.m_nFilter == CExportResultDataModelFilter::FORMATIONPLANES) {
          QStringList nv = value.split(':', QString::SkipEmptyParts);
          if (nv.size() == 2) {
            filter.m_sName = nv[0];

            QStringList l = nv[1].split(',', QString::SkipEmptyParts);

            for (int i = 0; i < l.size(); ++i) {
              if ("All" == l[i] || "Top" == l[i])
                filter.m_nPlaneMask |= CExportResultDataModelFilter::PLANE_TOP;
              if ("All" == l[i] || "Middle" == l[i])
                filter.m_nPlaneMask |= CExportResultDataModelFilter::PLANE_MIDDLE;
              if ("All" == l[i] || "Bottom" == l[i])
                filter.m_nPlaneMask |= CExportResultDataModelFilter::PLANE_BOTTOM;
            }
          }
        } else
          filter.m_sName = value;
      } else
        continue;

      m_exportResultData.m_modelFilterVector.m_modelFilters.push_back(filter);

      child = child.nextSiblingElement("ModelFilter");
    }
  }

  void Save(QDomElement &domElement) {
    domElement.appendChild(
        domElement.ownerDocument().createComment(" Filter = Faults | Horizons | Formations | FormationPlanes | "
                                                 "Wellpaths | Pointsets | Elementsets | NonMeshedSurfaces "));
    domElement.appendChild(
        domElement.ownerDocument().createComment(" Value = All when Type = Set || Value = name when Type = ByName "));
    domElement.appendChild(domElement.ownerDocument().createComment(
        " for FormationPlanes: Value = formationName:All | formationName:Top,Middle,Bottom or a selection thereof "));

    for (size_t i = 0; i < m_exportResultData.m_modelFilterVector.m_modelFilters.size(); ++i) {
      QDomElement child;

      CExportResultDataModelFilter &filter = m_exportResultData.m_modelFilterVector.m_modelFilters[i];

      if (filter.m_nType == IExportResultDataFilter::NONE)
        continue;

      QString name;

      switch (filter.m_nType) {
      case IExportResultDataFilter::SET:
        child.setAttribute("Type", "Set");
        child.setAttribute("Value", "All"); // only one supported atm
        break;
      case IExportResultDataFilter::BYNAME:
        child.setAttribute("Type", "ByName");
        name = filter.m_sName;
        break;
      }

      switch (filter.m_nFilter) {
      case CExportResultDataModelFilter::FAULTS:
        child.setAttribute("Filter", "Faults");
        break;
      case CExportResultDataModelFilter::HORIZONS:
        child.setAttribute("Filter", "Horizons");
        break;
      case CExportResultDataModelFilter::FORMATIONS:
        child.setAttribute("Filter", "Formations");
        break;
      case CExportResultDataModelFilter::FORMATIONPLANES:
        child.setAttribute("Filter", "FormationPlanes");
        {
          QStringList l;
          if (filter.m_nPlaneMask & CExportResultDataModelFilter::PLANE_TOP)
            l.append("Top");
          if (filter.m_nPlaneMask & CExportResultDataModelFilter::PLANE_MIDDLE)
            l.append("Middle");
          if (filter.m_nPlaneMask & CExportResultDataModelFilter::PLANE_BOTTOM)
            l.append("Bottom");

          if (l.size() > 0) {
            name += ":";

            if (l.size() == 3)
              name += "All";
            else
              name += l.join(",");
          }
        }
        break;
      case CExportResultDataModelFilter::WELLPATHS:
        child.setAttribute("Filter", "Wellpaths");
        break;
      case CExportResultDataModelFilter::POINTSETS:
        child.setAttribute("Filter", "Pointsets");
        break;
      case CExportResultDataModelFilter::ELEMENTSETS:
        child.setAttribute("Filter", "Elementsets");
        break;
      case CExportResultDataModelFilter::NONMESHEDSURFACES:
        child.setAttribute("Filter", "NonMeshedSurfaces");
        break;
      }

      if (!name.isEmpty())
        child.setAttribute("Value", name);

      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroResultFilter {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroResultFilter(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {

    QDomElement child = domElement.firstChildElement("ResultFilter");
    while (!child.isNull()) {
      QString filterName = AttributeStringValue(child, "Filter");
      QString type = AttributeStringValue(child, "Type");
      QString value = AttributeStringValue(child, "Value");

      CExportResultDataResultFilter filter;

      if ("Materials" == filterName)
        filter.m_nFilter = CExportResultDataResultFilter::MATERIALS;
      else if ("Mesh" == filterName)
        filter.m_nFilter = CExportResultDataResultFilter::MESH;
      else if ("Results" == filterName)
        filter.m_nFilter = CExportResultDataResultFilter::RESULTS;
      else if ("GVTResults" == filterName)
        filter.m_nFilter = CExportResultDataResultFilter::GVTRESULTS;
      else
        continue;

      if ("Set" == type) {
        filter.m_nType = IExportResultDataFilter::SET;

        if ("All" == value)
          filter.m_nSetValue = CExportResultDataResultFilter::ALL;
        else if (filter.m_nFilter == CExportResultDataResultFilter::MATERIALS) {
          if ("Normal" == value)
            filter.m_nSetValue = CExportResultDataResultFilter::NORMAL;
          else if ("FractureIntensity" == value)
            filter.m_nSetValue = CExportResultDataResultFilter::FRACTUREINTENSITY;
          else
            continue;
        } else
          continue;
      } else if ("ByName" == type) {
        filter.m_nType = IExportResultDataFilter::BYNAME;
        filter.m_sName = value;
      }

      m_exportResultData.m_resultFilterVector.m_resultFilters.push_back(filter);

      child = child.nextSiblingElement("ResultFilter");
    }
  }
  void Save(QDomElement &domElement) {
    domElement.appendChild(
        domElement.ownerDocument().createComment(" Filter = Materials | Results | Mesh | GVTResults"));
    domElement.appendChild(domElement.ownerDocument().createComment(
        " Value = All when Type = Set || Value = mainExportLabel when Type = ByName "));
    domElement.appendChild(domElement.ownerDocument().createComment(
        " additional for Materials: Value = Normal | FractureIntensity when Type = Set "));
    domElement.appendChild(
        domElement.ownerDocument().createComment(" for Derived and User Defined results always use Type = ByName "));

    for (size_t i = 0; i < m_exportResultData.m_resultFilterVector.m_resultFilters.size(); ++i) {
      CExportResultDataResultFilter &filter = m_exportResultData.m_resultFilterVector.m_resultFilters[i];

      QDomElement child = domElement.ownerDocument().createElement("ResultFilter");

      switch (filter.m_nFilter) {
      case CExportResultDataResultFilter::MATERIALS:
        child.setAttribute("Filter", "Materials");
        break;
      case CExportResultDataResultFilter::MESH:
        child.setAttribute("Filter", "Mesh");
        break;
      case CExportResultDataResultFilter::RESULTS:
        child.setAttribute("Filter", "Results");
        break;
      case CExportResultDataResultFilter::GVTRESULTS:
        child.setAttribute("Filter", "GVTResults");
        break;
      }

      switch (filter.m_nType) {
      case IExportResultDataFilter::SET:
        child.setAttribute("Type", "Set");

        switch (filter.m_nSetValue) {
        case CExportResultDataResultFilter::ALL:
          child.setAttribute("Value", "All");
          break;
        case CExportResultDataResultFilter::NORMAL:
          child.setAttribute("Value", "Normal");
          break;
        case CExportResultDataResultFilter::FRACTUREINTENSITY:
          child.setAttribute("Value", "FractureIntensity");
          break;
        }
        break;
      case IExportResultDataFilter::BYNAME:
        child.setAttribute("Type", "ByName");
        child.setAttribute("Value", filter.m_sName);
        break;
      }

      domElement.appendChild(child);
    }
  }
};

class CExportResultMacroXML {
  CExportResultData &m_exportResultData;

public:
  CExportResultMacroXML(CExportResultData &exportResultData) : m_exportResultData(exportResultData) {}

  void Load(QDomElement &domElement) {
    m_exportResultData.Name(AttributeStringValue(domElement, "Name"));

    QDomElement child = domElement.firstChildElement("Settings");
    while (!child.isNull()) {
      CExportResultMacroSettings settings(m_exportResultData);
      settings.Load(child);

      child = child.nextSiblingElement("Settings");
    }

    child = domElement.firstChildElement("DepletionStageFilters");
    while (!child.isNull()) {
      CExportResultMacroDepletionStageFilter depStageFilter(m_exportResultData);
      depStageFilter.Load(child);

      child = child.nextSiblingElement("DepletionStageFilters");
    }

    child = domElement.firstChildElement("ModelFilters");
    while (!child.isNull()) {
      CExportResultMacroModelFilter modelFilter(m_exportResultData);
      modelFilter.Load(child);

      child = child.nextSiblingElement("ModelFilters");
    }

    child = domElement.firstChildElement("ResultFilters");
    while (!child.isNull()) {
      CExportResultMacroResultFilter resultFilter(m_exportResultData);
      resultFilter.Load(child);

      child = child.nextSiblingElement("ResultFilters");
    }

    CExportResultMacroDeformation deformation(m_exportResultData);
    deformation.Load(domElement);

    CExportResultMacroHotSpot hotspot(m_exportResultData);
    hotspot.Load(domElement);
  }

  void Save(QDomElement &domElement) {
    m_exportResultData.bind();

    domElement.setAttribute("Name", m_exportResultData.Name());

    QDomElement child = domElement.ownerDocument().createElement("Settings");
    domElement.appendChild(child);

    CExportResultMacroSettings settings(m_exportResultData);
    settings.Save(child);

    if (!m_exportResultData.m_stTimeStep.empty()) {
      child = domElement.ownerDocument().createElement("DepletionStageFilters");
      domElement.appendChild(child);

      CExportResultMacroDepletionStageFilter depStageFilter(m_exportResultData);
      depStageFilter.Save(child);
    }

    if (!m_exportResultData.m_selection.empty()) {
      child = domElement.ownerDocument().createElement("ModelFilters");
      domElement.appendChild(child);

      CExportResultMacroModelFilter modelFilter(m_exportResultData);
      modelFilter.Save(child);
    }

    if (!m_exportResultData.m_RGObserverData.empty() || !m_exportResultData.m_MatRGObserverData.empty() ||
        !m_exportResultData.m_GVTRGObserverData.empty()) {
      child = domElement.ownerDocument().createElement("ResultFilters");
      domElement.appendChild(child);

      CExportResultMacroResultFilter resultFilter(m_exportResultData);
      resultFilter.Save(child);
    }

    CExportResultMacroDeformation deformation(m_exportResultData);
    deformation.Save(domElement);

    CExportResultMacroHotSpot hotspot(m_exportResultData);
    hotspot.Save(domElement);

    m_exportResultData.unbind(false);
  }
};

CException::CException(const QString &strError) : m_strError(strError) {}

const QString &CException::error() const { return m_strError; }

CExportResultDataXML::CExportResultDataXML(CModelBase &model, TExportResultDataVector &vcExportResultData)
    : m_model(model), m_vcExportResultData(vcExportResultData) {}

void CExportResultDataXML::LoadFile(const QString &sFileName) {
  QFile file(sFileName);
  if (!file.open(QIODevice::ReadOnly))
    throw CException(QObject::tr("Unable to open file '%1' for reading").arg(sFileName));

  QDomDocument doc;
  if (!doc.setContent(&file)) {
    file.close();
    throw CException(QObject::tr("Unable to parse the content of file '%1'").arg(sFileName));
  }

  QDomElement docElement = doc.documentElement();
  if (docElement.tagName() != "ExportResultMacros")
    throw CException(QObject::tr("The root element of the XML file must be 'ExportResultMacros'"));

  Load(docElement);
}

void CExportResultDataXML::SaveFile(const QString &sFileName) {
  QFile file(sFileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    throw CException(QObject::tr("Unable to open file '%1' for writing").arg(sFileName));

  // deterministic order of attributes
  qSetGlobalQHashSeed(0);

  QDomDocument doc("ExportResultMacros");
  QDomElement root = doc.createElement("ExportResultMacros");
  doc.appendChild(root);

  Save(root);

  QTextStream stream(&file);
  stream << doc.toString();

  qSetGlobalQHashSeed(-1);
}

void CExportResultDataXML::Load(QDomElement &domElement) {
  try {
    QDomElement child = domElement.firstChildElement("ExportResultMacro");
    while (!child.isNull()) {
      m_vcExportResultData.push_back(new CExportResultData(m_model));

      CExportResultMacroXML macroXML(*m_vcExportResultData.back());
      macroXML.Load(child);

      m_vcExportResultData.back()->reParent(m_model.GraphEntry(MD_BASE_EXPORT_MACROS));

      child = child.nextSiblingElement("ExportResultMacro");
    }
  } catch (CException e) {
    m_vcExportResultData.clear();
    throw;
  }
}

void CExportResultDataXML::Save(QDomElement &domElement) {
  for (TExportResultDataVector::iterator macro = m_vcExportResultData.begin(); macro != m_vcExportResultData.end();
       ++macro) {
    QDomElement child = domElement.ownerDocument().createElement("ExportResultMacro");
    domElement.appendChild(child);

    CExportResultMacroXML macroXML(**macro);
    macroXML.Save(child);
  }
}

} // namespace erd_xml