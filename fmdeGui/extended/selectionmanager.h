#ifndef SELECTIONMANAGER_H
#define SELECTIONMANAGER_H

#include <QWidget>
#include <QMap>
#include <QList>

#include "Index.h"

namespace Ui {
    class SelectionManager;
}

class SelectionManager : public QWidget {
    Q_OBJECT
public:
    SelectionManager(QWidget *parent = 0);
    ~SelectionManager();

	QString currentSelectionName() const;
	
	int indexCount();
	Index index(int number);
	
	QList<Index> currentIndexList() const;
	void addIndex(Index index);
	
	void clear();
signals:
	void saveSelectionRequested();
	void loadSelectionRequested();
	void selectAllRequested();
	void deselectAllRequested();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::SelectionManager *ui;
	
	QMap<QString, QList<Index> > indexMap;
	
	QList<Index> &currentIndexList();
	
	void selectionChanged(const QString &newSelection);
private slots:
	void saveSelection();
	void loadSelection();
};

#endif // SELECTIONMANAGER_H
