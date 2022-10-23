#include <filesystem>
#include <fstream>

#include <windows.h>

#include <QtCore/QString>

#ifndef INCLUDE_UTIL
#define INCLUDE_UTIL

inline void default_run(const QString& path) {
	ShellExecute(0, 0, path.toLocal8Bit(), 0, 0, SW_SHOW);
}

inline bool identical_file(const std::filesystem::path& f1, const std::filesystem::path& f2) {
	using namespace std::filesystem;
	if (!is_regular_file(f1) || !is_regular_file(f2)) return false;
	std::ifstream ifs1{ f1, std::ios::binary | std::ios::ate };
	std::ifstream ifs2{ f2, std::ios::binary | std::ios::ate };
	if (ifs1.tellg() != ifs2.tellg() || ifs1.fail() || ifs2.fail()) return false;

	ifs1.seekg(0, std::ios::beg);
	ifs2.seekg(0, std::ios::beg);
	return std::equal(
	  std::istreambuf_iterator<char>{ ifs1 }, std::istreambuf_iterator<char>{},
	  std::istreambuf_iterator<char>{ ifs2 });
}

#endif