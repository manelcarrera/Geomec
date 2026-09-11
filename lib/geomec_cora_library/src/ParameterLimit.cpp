#include "ParameterLimit.h"
#include "GetValueTypeInfo.h"
#include "ModelBase.h"
#include "Object.h"
#include "ValueTypes.h"

namespace cora {

CParameterLimit::CParameterLimit() : CParameterBase() {}

CParameterLimit::~CParameterLimit() {}

namespace {

const char *EMPTY_STRING = "";

const QString DOUBLE2STRING = "%1";

const QString LOWER_LIMIT = "-1e20";
const QString UPPER_LIMIT = "1e20";

const QString LOWER_POISSON_RATIO = "0.0001";
const QString UPPER_POISSON_RATIO = "0.4999";

const QString LOWER_RATIO = "1e-20";
const QString UPPER_RATIO = "1.0";

const QString LOWER_STIFFNESS = "1e-20";

const QString LOWER_COMPRESSIBILITY = "1e-20";

} // anonymous namespace

const QString CParameterLimit::lowerLimit() const {
  QString lowerLimit = NAN_STRING;

  if (getRange(object(), valueTypeID()).first.Valid()) {
    lowerLimit = QString(DOUBLE2STRING).arg(getRange(object(), valueTypeID()).first.Value());
  }

  if (lowerLimit == NAN_STRING) {
    lowerLimit = LOWER_LIMIT;
  } else if ((valueTypeID() == IDT_VALUETYPE_POISSONS_RATIO) || (valueTypeID() == IDT_VALUETYPE_UNDRAINEDPOISSON) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_NORM) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_TRANS) || (valueTypeID() == IDT_VALUETYPE_POISSON_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_PIPEPOISSONRATIO)) {
    lowerLimit = LOWER_POISSON_RATIO;
  } else if ((valueTypeID() == IDT_VALUETYPE_LATRATIO_MAX) || (valueTypeID() == IDT_VALUETYPE_LATRATIO_MIN)) {
    lowerLimit = LOWER_RATIO;
  } else if ((valueTypeID() == IDT_VALUETYPE_BULKSTIFFNESS) || (valueTypeID() == IDT_VALUETYPE_DYNUNISTIFFNESS) ||
             (valueTypeID() == IDT_VALUETYPE_DYNSHEARSTIFFNESS) ||
             (valueTypeID() == IDT_VALUETYPE_FRACT_STIFF_NORMAL) ||
             (valueTypeID() == IDT_VALUETYPE_FRACT_STIFF_SHEAR) || (valueTypeID() == IDT_VALUETYPE_GRAINSTIFFNESS) ||
             (valueTypeID() == IDT_VALUETYPE_NORMALSTIFFNESS) || (valueTypeID() == IDT_VALUETYPE_SHEARSTIFFNESS)) {
    lowerLimit = LOWER_STIFFNESS;
  } else if ((valueTypeID() == IDT_VALUETYPE_ELASCOMPRES) || (valueTypeID() == IDT_VALUETYPE_PLASCOMPRES)) {
    lowerLimit = LOWER_COMPRESSIBILITY;
  }

  return lowerLimit;
}

const QString CParameterLimit::upperLimit() const {
  QString upperLimit = NAN_STRING;

  if (getRange(object(), valueTypeID()).second.Valid()) {
    upperLimit = QString(DOUBLE2STRING).arg(getRange(object(), valueTypeID()).second.Value());
  }

  if (upperLimit == NAN_STRING) {
    upperLimit = UPPER_LIMIT;
  } else if ((valueTypeID() == IDT_VALUETYPE_POISSONS_RATIO) || (valueTypeID() == IDT_VALUETYPE_UNDRAINEDPOISSON) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_NORM) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_TRANS) || (valueTypeID() == IDT_VALUETYPE_POISSON_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_NORM_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_POISSONRATIO_TRANS_DECOMP) ||
             (valueTypeID() == IDT_VALUETYPE_PIPEPOISSONRATIO)) {
    upperLimit = UPPER_POISSON_RATIO;
  } else if ((valueTypeID() == IDT_VALUETYPE_LATRATIO_MAX) || (valueTypeID() == IDT_VALUETYPE_LATRATIO_MIN)) {
    upperLimit = UPPER_RATIO;
  }

  return upperLimit;
}

// protected

// static

const double CParameterLimit::NAN_DOUBLE = nan(EMPTY_STRING);
const QString CParameterLimit::NAN_STRING = QString(DOUBLE2STRING).arg(NAN_DOUBLE);

// private

// static

std::pair<geo::CValue, geo::CValue> CParameterLimit::getRange(CGraphNode *object, unsigned int valueTypeID) {
  CGetValueTypeInfo &getValueTypeInfo(
      CGetValueTypeInfo::instance(dynamic_cast<CModelBase *>(&(dynamic_cast<CStorageNode *>(object)->Model()))));
  QString valueTypeName = getStringTableEntry(CValueTypeFactory::instance()->NameIndex(valueTypeID));

  return getValueTypeInfo.getRange(valueTypeID, valueTypeName);
}

} // namespace cora
