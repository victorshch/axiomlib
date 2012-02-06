/***************************************************************************
*					QtGui
*
*	Description:	Класс, отражающий функциональность основного окна ГПИ.
*	Author:			dk
*	History:
*
****************************************************************************/
#ifndef QTGUI_H
#define QTGUI_H

//#include <QObjectCleanupHandler>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <iostream>

#include <QtGui>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/tokenizer.hpp>

#include "ui_QtGui.h"
#include "AxiomSetBrowser.h"
#include "ResBrowser.h"

class QtGui :public QMainWindow, public Ui::mainWindow {

	Q_OBJECT

 private:

	QProcess *process; // Ссылка на запускаемый из ГПИ внешний процесс
	FILE *logFile; // Ссылка на лог файл для текущего исполняемого процесса
	bool writeToLog; // Булево значение отражающее - сохранять ли лог-файл или нет
	bool curSysIsWindows; // Булево значение для отображение - исполняется ли ГПИ по операционной системой Windows
	bool isExtendedGUI; // Флаг того, что используется расширенный GUI для FuzzyData
	unsigned int procNum; // Число процессов при использовании MPI
	struct tm timeStart; // Время запуска внешнего процесса
	

	// Обозначние для сокращения понимания и лучшего понимания
	typedef boost::tokenizer<boost::char_separator<char> > token_separator;

	//QObjectCleanupHandler cleanupHandler;

	void checkExtendedGUI();
	void getDataSetNamePath(const QString &dataSetFullPath, QString &dataSetName, QString &dataSetPath);
 public:
	
	// Конструктор класса
	QtGui(QWidget *parent = 0, Qt::WFlags flags = 0);
	// Деструктор класса
	~QtGui();

	// Возвращает путь к исполняемому файлу внешнего процесса для выбранного алгоритма
	QString getAlgPath(QString algLabel, QString workDir, QString &algName);
	// Проверка все ли необходимые поля перед запуском быди заполнены
	bool checkFieldsEntered ();
	// Обработчик сообщений от внешних процессов
	int systemMsgHandler (std::string &str);
	// Функция устанавливает значение индикатора выполнения процесса
	void moveProgressBar (std::string str);
	// Функция удаления лишних слешей из строки, содержащей путь к файлу или папке
	const char* removeSplashes (std::string &str);
	// Функция удвоения слешей в строке
	void addSplashes (QString &str);
	// Функция заключает строку в ковычки
	void addQuotes (QString &str);
	// Функция добавляет строку в список аргументов
	int addArgsToList (const std::string str, QStringList &args);
	// Функция создает новый лог-файл поверх предыдущего, если он существует
	void createLogFile ();
	// Функция создает директорию по указанному адресу
	int createDir (std::string dir);
	// Функция записывает в лог-файл содержание строки
	void toLogFile (std::string &str);
	// Функция останвливает запись в лог-файл и закрывает его
	void closeLogFile ();

 public slots:
	
	// Функции, вызываемые внешними воздействиями, например нажатием кнопки в ГПИ:
    void getPathParams(); // Соответствует выбору пути для конфигурационного файла
	void getPathDataSet(); // Соответствует выбору пути для набора данных
	void getPathWhereToSave(); // Соответствует выбору пути, по которому сохранять результаты
	void getPathWorkingDir(); // Соответствует выбору пути рабочей директории
	void getPathMpiexec(); // Соответствует выбору пути к исполняемому файлу mpiexec
	void editConfig(); // Соответствует кнопке редактирования конфигурационного файла
    void clear();  // Соответствует кнопке очищения основного окна отображения результатов
    void startLog(); // Вызывается при начале записи в лог-файл
	void about(); // Вызов информации о ГПИ
	void checkBoxFunc(); // Вызывается при клике на чекбокс сохранения результатов
	void checkBoxUseMpi(); // Вызывается при клике на чекбокс использования mpi
	void startChgState (); // Изменяет внешний вид ГПИ при нажатии на кнопку старта
	void start(); // Вызывается при нажатии на кнопку старта, вызывает запуск внешнего процесса
	void stop(); // Вызывается при нажатии на стоп, останавливает выполнение внешнего процесса
	void showVersion(); // Показывает текущую версию ГПИ
	void showHelp(); // Показывает необходимую инфомрацию по использованию ГПИ
	void runASBrowser(); // Вызывает окно с отображением систем аксиом
	void runResBrowser();  // Вызывает окно с отображением результатов распознавания
	void clearAllFields(); // Очищает содержимое всех полей ГПИ
	void exit(); // Закрывает основное окно ГПИ
	void processOutput(); // Запускается при поступлении данных от внешнего процесса, обрабатывает их.

}; // end of class

#endif // QTGUI_H
