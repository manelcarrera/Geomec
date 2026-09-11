#include <QDateTime>
#include <QString>

#include "ProgressBase.h"

CProgressBase::CProgressBase() : IProgressBase() {}

CProgressBase::~CProgressBase() {}

// protected

namespace {

const QString DATE_TIME_FORMAT = "dd-MM-yyyy hh:mm:ss ";

} // anonymous namespace

QString CProgressBase::getDateTimeStamp() {
  QDateTime dateTimeStamp = QDateTime::currentDateTime();

  return dateTimeStamp.toString(DATE_TIME_FORMAT);
}
