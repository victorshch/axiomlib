#ifndef MARKUPEDITOR_H
#define MARKUPEDITOR_H

#include <QWidget>
#include <vector>

namespace Ui {
    class MarkupEditor;
}

class MarkupEditor : public QWidget {
    Q_OBJECT
public:
    MarkupEditor(const std::vector<int> &markup, int maxAxiomIndex, QWidget *parent = 0);
    ~MarkupEditor();
signals:
	void markupChanged(const std::vector<int> &newMarkup);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MarkupEditor *ui;
	
	int maxAxiomIndex;
	
	void displayErrorMessage() const;
	
	static QString markupToString(const std::vector<int> markup);
private slots:
	void saveMarkup();
};

#endif // MARKUPEDITOR_H
