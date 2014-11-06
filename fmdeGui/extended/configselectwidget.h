#ifndef CONFIGSELECTWIDGET_H
#define CONFIGSELECTWIDGET_H

#include <QDialog>

#include "fileselectform.h"

class ConfigSelectWidget : public QDialog
{
	Q_OBJECT
public:
	explicit ConfigSelectWidget(QWidget *parent = 0);

	QString workdir() const;
	void setWorkdir(QString value);

	QString dataset() const;
	void setDataset(QString value);

	QString configFile() const;
	void setConfigFile(QString value);

signals:

public slots:
	void onChanged();

private:

	FileSelectForm* mWorkdirSelect;
	FileSelectForm* mDatasetSelect;
	FileSelectForm* mConfigSelect;

	QPushButton* mOk;
	QPushButton* mCancel;
};

#endif // CONFIGSELECTWIDGET_H
