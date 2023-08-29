#include <QtWidgets/QListView>
#include <QtGui/QCloseEvent>

#ifndef INCLUDE_QLISTVIEWWITHCLEANUP
#define INCLUDE_QLISTVIEWWITHCLEANUP

class QListViewWithCleanup : public QListView {
	Q_OBJECT

signals:
	void do_cleanup();

private:
	inline void closeEvent(QCloseEvent*) override {
		emit do_cleanup();
	}
};

#endif