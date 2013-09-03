#ifndef BOOK_GLOBAL_H
#define BOOK_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BOOK_LIBRARY)
#  define BOOKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BOOKSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BOOK_GLOBAL_H
