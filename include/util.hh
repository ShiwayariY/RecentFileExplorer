#include <filesystem>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <windows.h>

#include <QtCore/QString>

#ifndef INCLUDE_UTIL
#define INCLUDE_UTIL

inline void default_run(const QString& path) {
	ShellExecute(0, 0, path.toLocal8Bit(), 0, 0, SW_SHOW);
}

void to_uppercase(std::string&);

std::vector<std::filesystem::path> recursively_list_all_files(const std::filesystem::path& root);
std::unordered_map<std::filesystem::path, std::unordered_set<std::filesystem::path>> extract_filename_groups(std::vector<std::filesystem::path>& files);
void remove_identical_files(std::unordered_set<std::filesystem::path>& files);
void extract_into(std::unordered_set<std::filesystem::path>& source, std::vector<std::filesystem::path>& destination);
void remove_files_with_same_name_and_content(std::vector<std::filesystem::path>& files);
void sort_by_write_time(std::vector<std::filesystem::path>& files);
std::string find_crc32_hash(const std::string& filename);
bool identical_file(const std::filesystem::path& f1, const std::filesystem::path& f2);

#endif