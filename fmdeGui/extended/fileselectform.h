#ifndef FILESELECTFORM_H
#define FILESELECTFORM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

/// A form for selecting an existing file or folder
class FileSelectForm : public QWidget
{
	Q_OBJECT
public:
	enum Type {
		File = 0x1,
		Directory = 0x2
	};

	FileSelectForm(Type type, QString label, QWidget *parent = 0);

	QString value() const;
	void setValue(QString newValue);

	bool isValid() const;

public slots:
	void onButtonClick();

signals:
	void changed();

private:
	QLineEdit* mLineEdit;
	QPushButton* mButton;
	QString mText;

	Type mType;
};

#endif // FILESELECTFORM_H
