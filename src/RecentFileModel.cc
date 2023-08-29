#include <algorithm>
#include <iostream>
#include <fstream>

#include <QtGui/QFont>
#include <QtGui/QBrush>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtCore/QStandardPaths>

#include <util.hh>

#include <RecentFileModel.hh>

RecentFileModel::RecentFileModel(const std::filesystem::path& root, QObject* parent) :
		QAbstractListModel(parent) {

	auto* loader_thread = new QThread;
	auto* loader = new FileListLoader{ root };
	loader->moveToThread(loader_thread);

	qRegisterMetaType<std::vector<std::filesystem::path>>();

	connect(loader, &FileListLoader::send_files, this, &RecentFileModel::receive_files);
	connect(loader_thread, &QThread::started, loader, &FileListLoader::load_files);
	connect(loader, &FileListLoader::finished, loader_thread, &QThread::quit);
	connect(loader, &FileListLoader::finished, loader, &FileListLoader::deleteLater); // as per qt doc,
	connect(loader_thread, &QThread::finished, loader_thread, &QThread::deleteLater); // but it has issues ..?
	loader_thread->start();
}

int RecentFileModel::rowCount(const QModelIndex& parent) const {
	if(!parent.isValid()) return m_items.size();
	return 0;
}

QVariant RecentFileModel::data(const QModelIndex& index, int role) const {
	const auto& item = m_items[index.row()];
	switch(role) {
		case Qt::DisplayRole:
			return QString::fromStdString(item.path.filename().string());
		case Qt::UserRole:
			return QString::fromStdString(item.path.string());
		case Qt::FontRole: {
			QFont font;
			font.setPointSize(16);
			return font;
		}
		case Qt::BackgroundRole:
			switch(item.tag) {
				case 1:
					return QBrush{ Qt::darkGreen };
				case 2:
					return QBrush{ Qt::darkRed };
			}
	}
	return {};
}

QStringList RecentFileModel::mimeTypes() const {
	QStringList types;
	types << "text/uri-list";
	return types;
}

QMimeData* RecentFileModel::mimeData(const QModelIndexList& indices) const {
	QMimeData* mime = new QMimeData;
	QList<QUrl> urls;
	for(const auto& i : indices)
		if(i.isValid())
			urls.append(QUrl::fromLocalFile(
			  data(i, Qt::UserRole).toString()));
	mime->setUrls(urls);
	return mime;
}

Qt::ItemFlags RecentFileModel::flags(const QModelIndex& index) const {
	return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;
}

bool RecentFileModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	if(!hasIndex(index.row(), index.column(), index.parent()) || !value.isValid())
		return false;

	auto& item = m_items[index.row()];
	if(role == Qt::BackgroundRole)
		item.tag = value.toInt();
	else
		return false;

	emit dataChanged(index, index, { role });

	return true;
}

void RecentFileModel::receive_files(std::vector<std::filesystem::path> files) {
	beginResetModel();
	for(auto& path : files)
		m_items.emplace_back(std::move(path), 0);
	load_tags();
	endResetModel();
}

std::map<std::filesystem::path, int> RecentFileModel::read_tag_file() {
	std::map<std::filesystem::path, int> items;
	std::filesystem::path tagfile_path{ QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation).toStdString() };
	std::filesystem::create_directories(tagfile_path);
	tagfile_path /= "tags";

	std::ifstream ifs{ tagfile_path };
	while(!ifs.eof()) {
		std::string path, tag_str;
		if(!std::getline(ifs, path) || !std::getline(ifs, tag_str)) break;
		try {
			items.try_emplace(std::filesystem::u8path(path), std::stoi(tag_str));
		} catch(const std::invalid_argument&) {
		} catch(const std::out_of_range&) {
		}
	}
	return items;
}

void RecentFileModel::update_tag_file() const {
	auto items = read_tag_file();
	for(const auto& item : m_items) {
		if(item.tag == 0)
			items.erase(item.path);
		else
			items[item.path] = item.tag;
	}

	std::filesystem::path tagfile_path{ QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation).toStdString() };
	tagfile_path /= "tags";
	std::ofstream ofs{ tagfile_path };
	for(const auto& [path, tag] : items) {
		ofs << path.string() << "\n"
			<< tag << "\n";
	}
}

void RecentFileModel::load_tags() {
	// only call from within a beginResetModel - endResetModel block
	auto items = read_tag_file();
	for(auto& item : m_items)
		if(items.count(item.path)) item.tag = items[item.path];
}

// ##################################################

FileListLoader::FileListLoader(std::filesystem::path root) :
		m_root{ std::move(root) } {}

void FileListLoader::load_files() {
	std::cout << "listing files .." << std::endl;
	auto files = recursively_list_all_files(m_root);
	std::cout << "removing dupes .." << std::endl;
	remove_files_with_same_name_and_content(files);
	std::cout << "sorting by write time .." << std::endl;
	sort_by_write_time(files);
	std::cout << "finished loading" << std::endl;
	emit send_files(std::move(files));
	emit finished();
}