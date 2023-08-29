#include <set>
#include <vector>
#include <map>
#include <filesystem>

#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>
#include <QtCore/QMimeData>
#include <QtCore/QThread>

#ifndef INCLUDE_RECENTFILEMODEL__
#define INCLUDE_RECENTFILEMODEL__

struct RecentFileItem {
	std::filesystem::path path;
	int tag = 0;
	RecentFileItem(std::filesystem::path p, int t) :
			path(std::move(p)),
			tag(t) {}
};

class RecentFileModel : public QAbstractListModel {
	Q_OBJECT

	std::vector<RecentFileItem> m_items;

public:
	explicit RecentFileModel(const std::filesystem::path& root, QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indices) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	bool setData(const QModelIndex&, const QVariant& value, int role) override;

public slots:
	void receive_files(std::vector<std::filesystem::path>);
	void update_tag_file() const;

private:
	static std::map<std::filesystem::path, int> read_tag_file();
	void load_tags();
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