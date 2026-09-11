
#include <stdexcept>

#include "ElementValueSet.h"
#include "FailureTypeParameterBase.h"
#include "ModelBase.h"
#include "SummaryResultFile.h"
#include "ValueTypeFactory.h"

namespace cora {

CFailureTypeParameterBase::CFailureTypeParameterBase(CSummaryResultFile &summaryResultFile, const std::string &object,
                                                     const std::string &option, const std::string &parameter,
                                                     std::vector<double> &value, const TObjects &objects,
                                                     const QString &objectType)
    : m_actualObject(findActualObject(summaryResultFile, object, objects, objectType)),
      m_actualParameter(findActualParameter(summaryResultFile, object, parameter, m_actualObject->getParameters())),
      m_object(object.c_str()), m_parameter(parameter.c_str()),
      m_parameterModifier(TParameterModifier(new CParameterModifier(summaryResultFile, option.c_str(), value))),
      m_summaryResultFile(summaryResultFile), m_objectType(objectType) {}

CFailureTypeParameterBase::~CFailureTypeParameterBase() {}

namespace {

const QString PARAMETER_FILE_PROCESSING = "The parameter file cannot (yet) be processed for objects of type %1";

} // anonymous namespace

void CFailureTypeParameterBase::modify(CModelBase * /*modelBase*/) {
  if (m_actualParameter) {
    m_summaryResultFile.addAdditionalInformation(QString(PARAMETER_FILE_PROCESSING).arg(m_objectType));
  }
}

const TParameter CFailureTypeParameterBase::getActualParameter() const { return m_actualParameter; }

namespace {

const QString OBJECT_DOES_NOT_EXIST = "%1 '%2' does not exist";
const QString SEPARATOR = ":";

} // anonymous namespace

TObject CFailureTypeParameterBase::findActualObject(CSummaryResultFile & /*summaryResultFile*/,
                                                    const std::string &object, const TObjects &objects,
                                                    const QString &objectType) {
  for (TObjects::const_iterator o = objects.begin(); o != objects.end(); ++o) {
    if (((*o)->name() == object.c_str()) || ((objectType + SEPARATOR + (*o)->name()) == object.c_str())) {
      return *o;
    }
  }

  const QString message = QString(OBJECT_DOES_NOT_EXIST).arg(objectType).arg(object.c_str());

  throw std::runtime_error(message.toStdString());
}

namespace {

const QString PARAMETER = "parameter";
const QString PARAMETER_DOES_NOT_EXIST = "%1 '%2' '%3' does not exist";

} // anonymous namespace

TParameter CFailureTypeParameterBase::findActualParameter(CSummaryResultFile & /*summaryResultFile*/,
                                                          const std::string &object, const std::string &parameter,
                                                          const TParameters &parameters) {
  for (TParameters::const_iterator p = parameters.begin(); p != parameters.end(); ++p) {
    if (((*p)->name() == parameter.c_str()) || ((PARAMETER + SEPARATOR + (*p)->name()) == parameter.c_str())) {
      return *p;
    }
  }

  const QString message = QString(PARAMETER_DOES_NOT_EXIST).arg(PARAMETER).arg(parameter.c_str()).arg(object.c_str());

  throw std::runtime_error(message.toStdString());
}

} // namespace cora