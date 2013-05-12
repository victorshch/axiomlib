#include <QtGui/QApplication>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

#include "AxiomLibException.h"

#include "extended/MainWindow.h"
#include "extended/MainWindowContent.h"
#include "extended/signalproxy.h"

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


int main(int argc, char *argv[])
{
	AxiomLibGuiApplication a(argc, argv);
	
	try {		
		QStringList args = QApplication::arguments();
		args.removeAt(0);

		MainWindowContent* mainWindowContent = new MainWindowContent(0, 
																	 args,
																	 QDir::currentPath());
		
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
