#include <QtWidgets/QListView>
#include <QtGui/QKeyEvent>

#include <KeyPressHandler.hh>
#include <util.hh>

bool KeyPressHandler::eventFilter(QObject* o, QEvent* e) {
	const QListView* view = dynamic_cast<QListView*>(o);
	if(!view) return false;
	if(e->type() != QEvent::KeyPress) return false;

	auto* ke = static_cast<QKeyEvent*>(e);
	if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
		for(const auto& i : view->selectionModel()->selectedIndexes())
			default_run(i.data(Qt::UserRole).toString());
		return true;
	}
	return false;
}
