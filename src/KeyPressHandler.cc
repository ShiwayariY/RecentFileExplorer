#include <QtWidgets/QListView>
#include <QtWidgets/QInputDialog>
#include <QtGui/QKeyEvent>

#include <KeyPressHandler.hh>
#include <util.hh>

bool KeyPressHandler::eventFilter(QObject* o, QEvent* e) {
	QListView* view = dynamic_cast<QListView*>(o);
	if(!view) return false;
	if(e->type() != QEvent::KeyPress) return false;

	const auto* ke = static_cast<QKeyEvent*>(e);
	if(ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
		open_selected(view);
		return true;
	} else if(ke->matches(QKeySequence::Find)) {
		find_item_dialog(view);
		return true;
	} else if(ke->key() == Qt::Key_0) {
		set_tag(view, 0);
		return true;
	} else if(ke->key() == Qt::Key_1) {
		set_tag(view, 1);
		return true;
	} else if(ke->key() == Qt::Key_2) {
		set_tag(view, 2);
		return true;
	}
	return false;
}

void KeyPressHandler::open_selected(const QListView* view) const {
	for(const auto& i : view->selectionModel()->selectedIndexes())
		default_run(i.data(Qt::UserRole).toString());
}

void KeyPressHandler::find_item_dialog(QListView* view) const {
	auto s = QInputDialog::getText(view, "Search", "");
	if(s.isEmpty()) return;

	auto selected_indexes = view->selectionModel()->selectedIndexes(); // this is not sorted..
	int last_selected_row = 0;
	for(const auto& index : selected_indexes) {
		if(index.row() > last_selected_row) last_selected_row = index.row();
	}

	auto* model = view->model();
	int rows = model->rowCount();
	for(auto row = last_selected_row + 1; row < rows; ++row) {
		auto index = model->index(row, 0);
		if(model->data(index, Qt::DisplayRole).toString().contains(s)) {
			view->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectionFlag::ClearAndSelect);
			view->scrollTo(index);
			break;
		}
	}
}

void KeyPressHandler::set_tag(QListView* view, int tag) {
	for(const auto& index : view->selectionModel()->selectedIndexes())
		view->model()->setData(index, tag, Qt::BackgroundRole);
}
