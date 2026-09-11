#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
#if defined(MODGMBUS_LIB)
#define MODGMBUS_EXPORT Q_DECL_EXPORT
#else
#define MODGMBUS_EXPORT Q_DECL_IMPORT
#endif
#else
#define MODGMBUS_EXPORT
#endif
