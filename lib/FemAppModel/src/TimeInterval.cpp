
#include "TimeInterval.h"

TTimeInterval calculateTimeInterval(const QDateTime &begin, const QDateTime &end) {
  TTimeInterval timeInterval;
  QTime time(0, 0);

  timeInterval.days = begin.daysTo(end);
  time = time.addSecs(begin.addDays(timeInterval.days).secsTo(end));
  timeInterval.hours = time.hour();
  timeInterval.minutes = time.minute();
  timeInterval.seconds = time.second();

  return timeInterval;
}
