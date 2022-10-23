#include <algorithm>

#include <QtGui/QFont>
#include <QtCore/QList>
#include <QtCore/QUrl>

#include <RecentFileModel.hh>

RecentFileModel::RecentFileModel(const std::filesystem::path& root, QObject* parent) :
		QAbstractListModel(parent) {
	load_files(root);
}

void RecentFileModel::load_files(const std::filesystem::path& root) {
	for(const auto& file : std::filesystem::recursive_directory_iterator(root))
		if(file.is_regular_file())
			m_files.push_back(std::filesystem::absolute(file));

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
