#include <algorithm>
#include <iostream>

#include <QtGui/QFont>
#include <QtCore/QList>
#include <QtCore/QUrl>

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
	const auto& path = m_files[index.row()];
	switch(role) {
		case Qt::DisplayRole:
			return QString::fromStdString(path.filename().string());
		case Qt::UserRole:
			return QString::fromStdString(path.string());
		case Qt::FontRole:
			QFont font;
			font.setPointSize(16);
			return font;
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

void RecentFileModel::receive_files(std::vector<std::filesystem::path> files) {
	beginResetModel();
	m_files = std::move(files);
	endResetModel();
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