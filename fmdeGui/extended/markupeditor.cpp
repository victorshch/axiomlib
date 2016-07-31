#include <QStringList>
#include <QMessageBox>

#include "utilities.h"

#include "markupeditor.h"
#include "ui_markupeditor.h"

MarkupEditor::MarkupEditor(const std::vector<int> &markup, int maxAxiomIndex, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkupEditor),
	maxAxiomIndex(maxAxiomIndex)
{
    ui->setupUi(this);
	
	setAttribute(Qt::WA_DeleteOnClose, true);
	
	ui->plainTextEdit_markup->setPlainText(
			markupToString(markup)
			);
	
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(saveMarkup()));
}

QString MarkupEditor::markupToString(std::vector<int>markup) {
	QStringList stringList;
	for(unsigned int i = 0; i < markup.size(); i++) {
		stringList.push_back(QString::number(markup[i]));
	}
	return stringList.join(" ");
}

MarkupEditor::~MarkupEditor()
{
    delete ui;
}

void MarkupEditor::saveMarkup() {
	QString source = ui->plainTextEdit_markup->toPlainText().simplified();
	QStringList stringList = source.split(" ");

	std::vector<int> result;
	result.reserve(stringList.size());
	for(int i = 0; i < stringList.size(); i++) {
		bool ok = true;
		int axNum = stringList[i].toInt(&ok);
		if(!ok) {
			displayErrorMessage();
			return;
		} else {
			if(axNum < 0 || axNum > maxAxiomIndex) {
				displayErrorMessage();
				return;
			}
			result.push_back(axNum);
		}
	}
	
	emit markupChanged(result);
	
	close();
}

void MarkupEditor::displayErrorMessage() const {
	QMessageBox::critical(0,
						  encode("Ошибка"),
						  encode("Неверный формат разметки"));
}

void MarkupEditor::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
