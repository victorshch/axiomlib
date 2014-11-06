#include "fileselectform.h"

#include <QHBoxLayout>
#include <QDebug>

FileSelectForm::FileSelectForm(Type type, QString label, QWidget *parent) :
	QWidget(parent), mType(type), mText(label)
{
	QHBoxLayout* layout = new QHBoxLayout(this);
	mLineEdit = new QLineEdit();
	layout->addWidget(mLineEdit);
	mButton = new QPushButton("...");
	layout->addWidget(mButton);

	connect(mButton, SIGNAL(clicked()), this, SLOT(onButtonClick()));
	connect(mLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
}

QString FileSelectForm::value() const
{
	return mLineEdit->text();
}

void FileSelectForm::setValue(QString newValue)
{
	mLineEdit->setText(newValue);
}

bool FileSelectForm::isValid() const
{
	bool exists = false;

	switch(mType) {
	case File: exists = QFileInfo(value()).exists(); break;
	case Directory: exists = !(value().isEmpty()) && QDir(value()).exists(); break;
	}

	qDebug() << "'" << value() << "' exists: " << exists;
	return exists;
}

void FileSelectForm::onButtonClick()
{
	QString newValue;
	if(!(mType & File)) {
		newValue = QFileDialog::getExistingDirectory(0, mText, value());
	} else {
		QString dir = QDir(value()).dirName();
		if(!QDir(dir).exists()) {
			dir = QString();
		}
		newValue = QFileDialog::getOpenFileName(0, mText, dir);
	}
	setValue(newValue);
}
