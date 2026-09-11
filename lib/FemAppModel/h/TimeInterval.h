#ifndef _TimeInterval_h_
#define _TimeInterval_h_

#include <QDateTime>

typedef struct _TimeInterval {
  int days;
  int hours;
  int minutes;
  int seconds;
} TTimeInterval;

TTimeInterval calculateTimeInterval(const QDateTime &begin, const QDateTime &end);

#endif // _TimeInterval_h_
