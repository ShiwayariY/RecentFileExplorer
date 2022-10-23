#include <algorithm>

#include <QtGui/QFont>
#include <QtCore/QList>
#include <QtCore/QUrl>

#include <util.hh>

#include <RecentFileModel.hh>

RecentFileModel::RecentFileModel(const std::filesystem::path& root, QObject* parent) :
		QAbstractListModel(parent) {
	load_files(root);
}

void RecentFileModel::load_files(const std::filesystem::path& root) {
	recursively_add_all_files(root);
	remove_files_with_same_name_and_content();
	sort_by_write_time();
}

void RecentFileModel::recursively_add_all_files(const std::filesystem::path& root) {
	for (const auto& file : std::filesystem::recursive_directory_iterator(root)) {
		if (file.is_regular_file())
			m_files.push_back(std::filesystem::absolute(file));
	}
}

void RecentFileModel::remove_files_with_same_name_and_content() {
	std::sort(m_files.begin(), m_files.end(),
	  [](const auto& lhs, const auto& rhs) {
		  return lhs.filename() < rhs.filename();
	  });
	auto last = std::unique(m_files.begin(), m_files.end(),
	  [](const auto& lhs, const auto& rhs) {
		  return (lhs.filename() == rhs.filename()) && identical_file(lhs, rhs);
	  });
	m_files.erase(last, m_files.end());
}

void RecentFileModel::sort_by_write_time() {
	std::sort(std::begin(m_files), std::end(m_files),
	  [](const std::filesystem::path& lhs, const std::filesystem::path& rhs) {
		  return std::filesystem::last_write_time(lhs) > std::filesystem::last_write_time(rhs);
	  });
}

int RecentFileModel::rowCount(const QModelIndex& parent) const {
	if (!parent.isValid()) return m_files.size();
	return 0;
}

QVariant RecentFileModel::data(const QModelIndex& index, int role) const {
	const auto& path = m_files[index.row()];
	switch (role) {
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
	for (const auto& i : indices)
		if (i.isValid())
			urls.append(QUrl::fromLocalFile(
			  data(i, Qt::UserRole).toString()));
	mime->setUrls(urls);
	return mime;
}

Qt::ItemFlags RecentFileModel::flags(const QModelIndex& index) const {
	return QAbstractListModel::flags(index) | Qt::ItemIsDragEnabled;
}
