#pragma once

#ifdef _WIN32
#define MAKESTRING2(str)  #str
#define MAKESTRING(str) MAKESTRING2(str)
#pragma message("WARNING: disable C4702 (unreachable code in Qt) in " __FILE__ "[" MAKESTRING(__LINE__) "]")
#pragma warning(push)
#pragma warning(disable: 4702)
#endif

#include <QSharedPointer>

#ifdef _WIN32
#pragma warning(pop)
#endif
