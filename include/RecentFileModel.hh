#include <set>
#include <vector>
#include <filesystem>

#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>
#include <QtCore/QMimeData>
#include <QtCore/QThread>

#ifndef INCLUDE_RECENTFILEMODEL__
#define INCLUDE_RECENTFILEMODEL__

class RecentFileModel : public QAbstractListModel {
	Q_OBJECT

	std::vector<std::filesystem::path> m_files;

public:
	explicit RecentFileModel(const std::filesystem::path& root, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indices) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

public slots:
	void receive_files(std::vector<std::filesystem::path>);
};

// ##################################################

Q_DECLARE_METATYPE(std::vector<std::filesystem::path>)

class FileListLoader : public QObject {
	Q_OBJECT

public:
	FileListLoader(std::filesystem::path root);

public slots:
	void load_files();

signals:
	void finished();
	void send_files(std::vector<std::filesystem::path>);

private:
	std::filesystem::path m_root;
};

#endif