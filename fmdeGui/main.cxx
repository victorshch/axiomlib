#include <QtGui/QApplication>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include <QSettings>

#include "AxiomLibException.h"

#include "extended/MainWindow.h"
#include "extended/MainWindowContent.h"
#include "extended/signalproxy.h"

#include "extended/configselectwidget.h"

#include <clocale>

// Наследник класса QApplication, в котором реализована поимка
// и отображение исключений при обработке событий
class AxiomLibGuiApplication : public QApplication {
public:
	AxiomLibGuiApplication(int &c, char **v): QApplication(c, v) {}
	bool notify(QObject *rec, QEvent *ev) {
		try {
			return QApplication::notify(rec, ev);
		}
		catch (AxiomLib::AxiomLibException &exception) {
			QMessageBox::critical(0, "AxiomLibException", QString::fromStdString(exception.error()));
			return false;
		}
		catch (QtGuiException &exception) {
			QMessageBox::critical(0, "QtGuiException", exception.what());
			return false;
		}
		catch (std::range_error &exception) {
			QMessageBox::critical(0, "range_error", exception.what());
			return false;			
		}
		catch (std::logic_error &exception) {
			QMessageBox::critical(0, "logic_error", exception.what());
			return false;						
		}
		catch (std::runtime_error& exception) {
			QMessageBox::critical(0, "runtime_error", exception.what());
		}
		catch (std::exception &exception) {
			QMessageBox::critical(0, "exception", exception.what());
		}
		catch (...) {
			QMessageBox::critical(0, "Unknown exception", "Unknown exception");
			return false;
		}
	}
};

void getDirectoryAndFile(QString src, QString& dir, QString& file) {
	src = QDir::fromNativeSeparators(src);
	QStringList list = src.split(QDir::separator());
	file = list.value(list.size() - 1);
	dir = QString();
	if(list.size() > 1) {
		list.removeAt(list.size() - 1);
		dir = list.join(QDir::separator());
	}
}

int main(int argc, char *argv[])
{
    std::setlocale(LC_ALL,"C");
	AxiomLibGuiApplication a(argc, argv);
//	QApplication a(argc, argv);

	try {		
		QStringList args = QApplication::arguments();
		args.removeAt(0);

		QSettings settings("fmdegui_settings.ini", QSettings::IniFormat);

		ConfigSelectWidget configSelect;
		// TODO set default values for workdir, dataset, config file name
		QString workDir = settings.value("WorkingDirectory", QDir::currentPath()).toString();
		configSelect.setWorkdir(workDir);
		configSelect.setDataset(settings.value("Dataset", QString()).toString());
		configSelect.setConfigFile(settings.value("ConfigFile", QString()).toString());
		configSelect.exec();

		if(configSelect.result() == QDialog::Rejected) {
			return 0;
		}

		settings.setValue("WorkingDirectory", configSelect.workdir());
		settings.setValue("Dataset", configSelect.dataset());
		settings.setValue("ConfigFile", configSelect.configFile());

		args << "ConfigFile" << configSelect.configFile();
		QString dataSetDir;
		QString dataSetName;
		getDirectoryAndFile(configSelect.dataset(), dataSetDir, dataSetName);
		args << "Dataset" << dataSetName;
		args << "BaseDataSetDir" << dataSetDir;

		MainWindowContent* mainWindowContent = new MainWindowContent(0, 
																	 args,
																	 configSelect.workdir());

		MainWindow* mainWindow = new MainWindow(0, (mainWindowContent->controller));
		mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);
		//mainWindow->setWindowModality(Qt::WindowModal);
		SignalProxy *signalProxy = new SignalProxy(mainWindow);
		mainWindowContent->wireUp(signalProxy);
		mainWindow->wireUp(signalProxy);			
		
		mainWindow->setCentralWidget(mainWindowContent);
		mainWindow->refreshStages();
		
		//connect(mainWindow, SIGNAL(destroyed()), this, SLOT(close()));
		
		mainWindow->showMaximized();
	} catch (std::exception exception) {
		QMessageBox::critical(0, "std::exception", exception.what());
	} catch (AxiomLib::AxiomLibException exception) {
		QMessageBox::critical(0, "AxiomLibException", exception.error().c_str());
	} catch (...) {
		QMessageBox::critical(0, "Exception", "General failure");
	}
	
	
	return a.exec();
}
