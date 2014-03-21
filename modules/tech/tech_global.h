#ifndef TECH_GLOBAL_H
#define TECH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TECH_LIBRARY)
#  define TECHSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TECHSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TECH_GLOBAL_H
