#include <windows.h>

#include <QtCore/QString>

#ifndef INCLUDE_UTIL
#define INCLUDE_UTIL

inline void default_run(const QString& path) {
	ShellExecute(0, 0, path.toLocal8Bit(), 0, 0, SW_SHOW);
}

#endif