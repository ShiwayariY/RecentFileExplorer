#include <fstream>
#include <algorithm>
#include <regex>
#include <cctype>

#include <util.hh>

void to_uppercase(std::string& s) {
	std::for_each(s.begin(), s.end(), [](char& c) {
		c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
	});
}

std::vector<std::filesystem::path> recursively_list_all_files(const std::filesystem::path& root) {
	std::vector<std::filesystem::path> files;
	for (const auto& file : std::filesystem::recursive_directory_iterator(root)) {
		if (file.is_regular_file())
			files.push_back(std::filesystem::absolute(file));
	}
	return files;
}

std::unordered_map<std::filesystem::path, std::unordered_set<std::filesystem::path>>
extract_filename_groups(std::vector<std::filesystem::path>& files) {
	std::unordered_map<std::filesystem::path, std::unordered_set<std::filesystem::path>> filename_map;
	for (auto& file : files)
		filename_map[file.filename()].insert(std::move(file));
	files.clear();
	return filename_map;
}

void remove_identical_files(std::unordered_set<std::filesystem::path>& files) {
	for (auto i = files.begin(); i != files.end(); ++i) {
		for (auto j = std::next(i); j != files.end();) {
			if (identical_file(*i, *j))
				j = files.erase(j);
			else
				++j;
		}
	}
}

void extract_into(std::unordered_set<std::filesystem::path>& source, std::vector<std::filesystem::path>& destination) {
	for (auto i = source.begin(); i != source.end();)
		destination.push_back(std::move(source.extract(i++).value())); //MUST increment i here, because extract invalidates
}

void remove_files_with_same_name_and_content(std::vector<std::filesystem::path>& files) {
	auto filename_map = extract_filename_groups(files);

	for (auto& [filename, same_filename_paths] : filename_map) {
		(void)filename;
		remove_identical_files(same_filename_paths);
	}

	for (auto& [filename, paths] : filename_map) {
		(void)filename;
		extract_into(paths, files);
	}
}

void sort_by_write_time(std::vector<std::filesystem::path>& files) {
	std::sort(std::begin(files), std::end(files),
	  [](const std::filesystem::path& lhs, const std::filesystem::path& rhs) {
		  return std::filesystem::last_write_time(lhs) > std::filesystem::last_write_time(rhs);
	  });
}

std::string find_crc32_hash(const std::string& filename) {
	static const std::regex crc32_rgx{ "\\[([0-9a-fA-F]{8})\\]" };

	std::sregex_iterator it{ filename.begin(), filename.end(), crc32_rgx };
	if (it == std::sregex_iterator{}) return "";

	auto crc32 = it->str();
	to_uppercase(crc32);
	return crc32;
}

bool identical_file(const std::filesystem::path& f1, const std::filesystem::path& f2) {
	using namespace std::filesystem;
	if (!is_regular_file(f1) || !is_regular_file(f2)) return false;

	std::ifstream ifs1{ f1, std::ios::binary | std::ios::ate };
	std::ifstream ifs2{ f2, std::ios::binary | std::ios::ate };
	if (ifs1.tellg() != ifs2.tellg() || ifs1.fail() || ifs2.fail()) return false;

	// avoid expensive file comparison if hashes can be found
	if (const auto hash1 = find_crc32_hash(f1.filename().string()); !hash1.empty()) {
		if (const auto hash2 = find_crc32_hash(f2.filename().string()); !hash2.empty()) {
			if (hash1 == hash2)
				return true;
			else
				return false;
		}
	}

	ifs1.seekg(0, std::ios::beg);
	ifs2.seekg(0, std::ios::beg);
	return std::equal(
	  std::istreambuf_iterator<char>{ ifs1 }, std::istreambuf_iterator<char>{},
	  std::istreambuf_iterator<char>{ ifs2 });
}