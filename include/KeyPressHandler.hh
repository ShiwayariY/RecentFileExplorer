#include <QtWidgets/QListView>
#include <QtCore/QEvent>

#ifndef INCLUDE_KEYPRESSHANDLER
#define INCLUDE_KEYPRESSHANDLER

class KeyPressHandler : public QObject {
	Q_OBJECT

protected:
	bool eventFilter(QObject*, QEvent*) override;

private:
	void open_selected(const QListView*) const;
	void find_item_dialog(QListView*) const;

};

#endif