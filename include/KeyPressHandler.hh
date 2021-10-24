#include <QtWidgets/QListView>
#include <QtCore/QEvent>

#ifndef INCLUDE_KEYPRESSHANDLER
#define INCLUDE_KEYPRESSHANDLER

class KeyPressHandler : public QObject {
	Q_OBJECT

protected:
	bool eventFilter(QObject*, QEvent*) override;

};

#endif