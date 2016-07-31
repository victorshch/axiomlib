#include <stdexcept>
#include <QtGui/QApplication>
#include <QMessageBox>
#include "QtGui.h"

// Наследник класса QApplication, в котором реализована поимка
// и отображение исключений при обработке событий
class AxiomLibGuiApplication : public QApplication {
public:
	AxiomLibGuiApplication(int &c, char **v): QApplication(c, v) {}
	bool notify(QObject *rec, QEvent *ev) {
		try {
			return QApplication::notify(rec, ev);
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

int main(int argc, char *argv[]) {
	AxiomLibGuiApplication app(argc, argv);
	QtGui w;
	w.show();
	return app.exec();
}
