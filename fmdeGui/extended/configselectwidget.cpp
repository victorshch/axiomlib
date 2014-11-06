#include "configselectwidget.h"

#include <QHBoxLayout>
#include <QFormLayout>

#include <QSignalMapper>

ConfigSelectWidget::ConfigSelectWidget(QWidget *parent) :
	QDialog(parent)
{
	QFormLayout* mainLayout = new QFormLayout(this);

	mWorkdirSelect = new FileSelectForm(FileSelectForm::Directory, tr("Working directory"));

	mainLayout->addRow(tr("Working directory:"), mWorkdirSelect);

	mDatasetSelect = new FileSelectForm(FileSelectForm::Directory, tr("Dataset"));

	mainLayout->addRow(tr("Dataset:"), mDatasetSelect);

	mConfigSelect = new FileSelectForm(FileSelectForm::File, tr("Config file"));

	mainLayout->addRow(tr("Config file:"), mConfigSelect);

	QHBoxLayout* buttonLayout = new QHBoxLayout();

	mainLayout->addRow(buttonLayout);

	buttonLayout->addStretch(1);

	mOk = new QPushButton(tr("OK"));
	mCancel = new QPushButton(tr("Cancel"));

	buttonLayout->addWidget(mOk);
	buttonLayout->addWidget(mCancel);

	connect(mWorkdirSelect, SIGNAL(changed()), this, SLOT(onChanged()));
	connect(mDatasetSelect, SIGNAL(changed()), this, SLOT(onChanged()));
	connect(mConfigSelect, SIGNAL(changed()), this, SLOT(onChanged()));

	connect(mOk, SIGNAL(clicked()), this, SLOT(accept()));
	connect(mCancel, SIGNAL(clicked()), this, SLOT(reject()));

	onChanged();
}

QString ConfigSelectWidget::workdir() const
{
	return mWorkdirSelect->value();
}

void ConfigSelectWidget::setWorkdir(QString value)
{
	mWorkdirSelect->setValue(value);
}

QString ConfigSelectWidget::dataset() const
{
	return mDatasetSelect->value();
}

void ConfigSelectWidget::setDataset(QString value)
{
	mDatasetSelect->setValue(value);
}

QString ConfigSelectWidget::configFile() const
{
	return mConfigSelect->value();
}

void ConfigSelectWidget::setConfigFile(QString value)
{
	mConfigSelect->setValue(value);
}

void ConfigSelectWidget::onChanged()
{
	mOk->setEnabled(
				mWorkdirSelect->isValid()
				&& mDatasetSelect->isValid()
				&& mConfigSelect->isValid()
				);
}
