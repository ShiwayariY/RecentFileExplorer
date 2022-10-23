#include <set>
#include <vector>
#include <filesystem>

#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>
#include <QtCore/QMimeData>

#ifndef INCLUDE_RECENTFILEMODEL__
#define INCLUDE_RECENTFILEMODEL__

class RecentFileModel : public QAbstractListModel {
	Q_OBJECT

	std::vector<std::filesystem::path> m_files;

public:
	explicit RecentFileModel(const std::filesystem::path& root, QObject* parent = nullptr);

	void load_files(const std::filesystem::path& root);
	void recursively_add_all_files(const std::filesystem::path& root);
	void remove_files_with_same_name_and_content();
	void sort_by_write_time();
	int rowCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indices) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
};

#endif