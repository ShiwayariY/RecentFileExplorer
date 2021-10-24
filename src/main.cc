#include <filesystem>

#include <windows.h>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListView>
#include <QtGui/QScreen>
#include <QtGui/QWindow>
#include <QtGui/QIcon>

#include <RecentFileModel.hh>

QScreen* get_active_screen(const QWidget* pWidget) {
	QScreen* pActive = nullptr;
	while (pWidget) {
		auto w = pWidget->windowHandle();
		if (w != nullptr) {
			pActive = w->screen();
			break;
		} else
			pWidget = pWidget->parentWidget();
	}
	return pActive;
}

QListView* create_recent_file_view(const std::filesystem::path& root) {
	auto* view = new QListView;
	auto* model = new RecentFileModel{root};
	view->setModel(model);
	view->setDragDropMode(QAbstractItemView::DragOnly);
	view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QObject::connect(view, &QListView::doubleClicked, [](const QModelIndex& i){
		ShellExecute(0, 0,
			i.data(Qt::UserRole).toString().toLocal8Bit(),
		0, 0, SW_SHOW);
	});
	return view;
}

void load_stylesheet(QApplication& app) {
	QFile ssfile(":/dark/stylesheet.qss");
	ssfile.open(QFile::ReadOnly | QFile::Text);
	QTextStream ssstream(&ssfile);
	app.setStyleSheet(ssstream.readAll());
}

int main(int argc, char** argv) {
	if (argc != 2 || !std::filesystem::is_directory(argv[1]))
		return 1;

	QApplication::setApplicationName("RecentFileExplorer");
	QApplication app{argc, argv};
	QApplication::setWindowIcon(QIcon{":/icon.png"});
	load_stylesheet(app);

	auto* view = create_recent_file_view(argv[1]);
	view->show();
	if(const auto* screen = get_active_screen(view); screen)
		view->resize(screen->availableSize() / 3.0);

	app.exec();
}
