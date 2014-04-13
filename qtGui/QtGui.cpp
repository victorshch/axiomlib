/****************************************************************************
*					Функции класса QtGui.h
*
*	Description:	Функции основного окна ГПИ - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include <stdexcept>

#include <QtDebug>

#ifdef Q_OS_WIN32
	#include <windows.h>
#endif

#include "QtGui.h"

#ifndef max
#define max(a,b)	(((a) >= (b)) ? (a) : (b))
#endif

// Programs to use in Linux
#define str_defWordProc	"konqueror"
//#define str_defCsvProc	"oocalc"

// Algorithms' labels
#define str_alg_test	"_TestInputOutput"
#define str_alg_bg		"Base Genetics"
#define str_alg_exec	"Execute"
#define str_alg_eg		"Ext Genetics"
#define str_alg_fdt		"Fuzzy Data"
#define str_alg_prec	"Precision"
#define str_alg_tpl		"Template"
#define str_alg_managed	"ManagedFuzzyData"

// Algorithms' names
#define str_alg_nm_test	"TestInputOutput"
#define str_alg_nm_bg	"Base_genetics"
#define str_alg_nm_exec	"Execute"
#define str_alg_nm_eg	"Ext_genetics"
#define str_alg_nm_fdt	"Fuzzy_data"
#define str_alg_nm_prec	"Precision"
#define str_alg_nm_tpl	"Template"
#define str_alg_nm_managed	"fmdeGui"
// Algorithm's endings
#define str_alg_win_end	".exe"

// System messages
#define	str_systemMsg_prog	"CurrentProgress: "
#define	str_systemMsg_stop	"CurrentProcess has stopped."

// Parameters' labels
#define str_configFile	"ConfigFile"
#define str_dataSetDir	"BaseDataSetDir"
#define str_dataSetName	"DataSet"
#define str_resultsDir	"PathToSaveResults"

// Text
#define str_curVersion	"Current Version of AxiomLib GUI - v 0.1 alpha."
#define str_help		"This is AxiomLib GUI. Using this program you can create recognizers of abnormal behavior."
#define str_about		"The text shown in wide text area is the standard output of the chosen algorithm."
// Buttons
#define str_stopped		"stopped"
#define str_working		"working"
#define str_pause		"Pause"
#define str_paused		"paused"
#define str_continue	"Continue"
// Errors
#define str_dataSetEmpty	"Please, fill in which DataSet to use."
#define	str_paramsEmpty		"Please, fill in which config-file to use."
#define	str_whereToSaveErr	"Please, fill in where to save results of the algorithm."
#define str_workingDirEmpty	"Please, fill in working directory field.\n Default value is './'"
#define str_pathToMpiErr	"Please, fill in the location of mpiexec."
// FileNames:
#define str_logFileName		"log.txt"


/****************************************************************************
*					QtGui::QtGui
*
*	Description:	Конструктор класса, задает необходимые начальные параметры
*					и настраивает внешний вид
*	Parameters:		parent - Виджет, из которого данный класс был инициирован
*					flags - параметры запуска основоного окна, как виджета QT
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
QtGui::QtGui(QWidget *parent, Qt::WFlags flags) {
	// Default values for private members
	process = NULL;
	writeToLog = true;
	logFile = NULL;
	curSysIsWindows = true;
	procNum = 1;
	//isExtendedGUI = false;

	// Setuping view of the mainWindow
	setupUi(this);

	comboBox_algorithm->clear();
	comboBox_algorithm->insertItems(0, QStringList()
	//	 << QApplication::translate("mainWindow", str_alg_test, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_bg, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_exec, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_eg, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_fdt, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_prec, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_tpl, 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("mainWindow", str_alg_managed, 0, QApplication::UnicodeUTF8)
	);

	// Connecting signals/slots
	connect( browseButton_workingDir, SIGNAL( clicked() ), this, SLOT( getPathWorkingDir() ) );
	connect( browseButton_dataSet, SIGNAL( clicked() ), this, SLOT( getPathDataSet() ) );
	connect( browseButton_params, SIGNAL( clicked() ), this, SLOT( getPathParams() ) );
	connect( editButton_config, SIGNAL( clicked() ), this, SLOT( editConfig() ) );
	connect( browseButton_whereToSave, SIGNAL( clicked() ), this, SLOT( getPathWhereToSave() ) );
	connect( browseButton_mpi, SIGNAL( clicked() ), this, SLOT( getPathMpiexec() ) );
    connect( clearButton, SIGNAL( clicked() ), this, SLOT( clear() ) );
    connect( aboutButton, SIGNAL( clicked() ), this, SLOT( about() ) );
	connect( checkBox_saveResults, SIGNAL( clicked() ), this, SLOT( checkBoxFunc() ) );
	connect( checkBox_useMpi, SIGNAL( clicked() ), this, SLOT( checkBoxUseMpi() ) );
	connect( startButton, SIGNAL( clicked() ), this, SLOT( start() ) );
	connect( stopButton, SIGNAL( clicked() ), this, SLOT( stop() ) );
	connect( actionHelp, SIGNAL( triggered() ), this, SLOT( showHelp() ) );
	connect( actionVersion, SIGNAL( triggered() ), this, SLOT( showVersion() ) );
	connect( actionClearAll, SIGNAL( triggered() ), this, SLOT( clearAllFields() ) );
	connect( actionExit, SIGNAL( triggered() ), this, SLOT( exit() ) );
	connect( pushButton_asBrowser, SIGNAL( clicked() ), this, SLOT( runASBrowser() ) );
	connect( pushButton_resBrowser, SIGNAL( clicked() ), this, SLOT( runResBrowser() ) );
	
	lineEdit_workingDir->setText(QDir::currentPath());

	connect(lineEdit_dataSet, SIGNAL(textChanged(QString)), this, SLOT(updateCommand()));
	connect(lineEdit_mpiOps, SIGNAL(textChanged(QString)), this, SLOT(updateCommand()));
	connect(lineEdit_params, SIGNAL(textChanged(QString)), this, SLOT(updateCommand()));
	connect(lineEdit_pathToMpi, SIGNAL(textChanged(QString)), this, SLOT(updateCommand()));
	connect(lineEdit_workingDir, SIGNAL(textChanged(QString)), this, SLOT(updateCommand()));
	connect(checkBox_saveResults, SIGNAL(toggled(bool)), this, SLOT(updateCommand()));
	connect(checkBox_saveResults, SIGNAL(toggled(bool)), this, SLOT(updateCommand()));
	connect(comboBox_algorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCommand()));
	connect(spinBox_numProc, SIGNAL(valueChanged(int)), this, SLOT(updateCommand()));

	connect(pushButton_copyCommand, SIGNAL(clicked()), this, SLOT(copyCommand()));

	updateCommand();
}

/****************************************************************************
*					QtGui::~QtGui
*
*	Description:	Деструктор класса, щакрывает если необходимо лог-файл
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
QtGui::~QtGui() {
	if (logFile)
		fclose (logFile);
}


/****************************************************************************
*					QtGui::getAlgPath
*
*	Description:	Функция, которая возвращает путь к исполняемому файлу
*					внешнего процесса для выбранного типа алгоритма
*	Parameters:		algLabel - обозначение выбранного алгоритма
*					workDir - путь к рабочей директории
*					algName - заполняемое поле со значением выбранного алгоритма
*	Returns:		QString - путь к исполняемому файлу выбранного внешнего процесса
*	Author:			dk
*	History:		
*
****************************************************************************/
QString QtGui::getAlgPath(QString algLabel, QString workDir, QString &algName) {
	QString toRet;
	toRet = workDir;
	if (!toRet.endsWith ("//"))
		toRet.append ("//");
	bool flag = true;

	if (algLabel == str_alg_test) {
		toRet.append (str_alg_nm_test);
		algName = str_alg_nm_test;
		flag = false;
	}
	if (algLabel == str_alg_bg) {
		toRet.append (str_alg_nm_bg);
		algName = str_alg_nm_bg;
		flag = false;
	}
	if (algLabel == str_alg_exec) {
		toRet.append (str_alg_nm_exec);
		algName = str_alg_nm_exec;
		flag = false;
	}
	if (algLabel == str_alg_eg) {
		toRet.append (str_alg_nm_eg);
		algName = str_alg_nm_eg;
		flag = false;
	}
	if (algLabel == str_alg_fdt) {
		toRet.append (str_alg_nm_fdt);
		algName = str_alg_nm_fdt;
		flag = false;
	}
	if (algLabel == str_alg_prec) {
		toRet.append (str_alg_nm_prec);
		algName = str_alg_nm_prec;
		flag = false;
	}
	if (algLabel == str_alg_tpl) {
		toRet.append (str_alg_nm_tpl);
		algName = str_alg_nm_tpl;
		flag = false;
	}
	if(algLabel == str_alg_managed) {
		toRet.append (str_alg_nm_managed);
		algName = str_alg_nm_managed;
		flag = false;
	}

	if (flag) {
		toRet.clear();
		algName.clear();
		return toRet;
	}

	#ifdef Q_OS_WIN32
		toRet.append (str_alg_win_end);	
	#endif
		
	return QDir::cleanPath(toRet);
}


/****************************************************************************
*					QtGui::addArgsToList
*
*	Description:	Функция добавляет аргументы, записанные в строке, в список аргуметов
*	Parameters:		str - строка, содержащая добавляемые аргументы
*					args - обновляемый список аргуметов
*	Returns:		0
*	Author:			dk
*	History:		
*
****************************************************************************/
int QtGui::addArgsToList (const std::string str, QStringList &args) {
	boost::char_separator<char> sep(" \"	");
	token_separator tokens(str, sep);
	for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
		std::string strTemp = *tok_iter;
		if ( (strTemp.size() < 1) || (strTemp[0] == ' ' ) ) { // либо пустой токен, либо начинается с пробела - а значит это не слово
			continue;
		}
		args << strTemp.c_str();
	}
	return 0;
}


/****************************************************************************
*					QtGui::systemMsgHandler
*
*	Description:	Функция обработки специализированных сообщения от внешнего
*					процесса к главному окну ГПИ
*	Parameters:		str - полученная от внешнего процесса обрабатываемая сторка
*	Returns:		0 - если системых сообщений в строке не содержится
*					1 - иначе
*	Author:			dk
*	History:		
*
****************************************************************************/
int QtGui::systemMsgHandler (std::string &str) {
	int toRet = 0;// Важно что значение по умолчанию - 0, т.к. значенеи 1 - указывает на то, что найдена строка, соответствующая остановке процесса
	std::string::size_type cur;

	// Обработка указателя текущего прогресса
	std::string sf(str_systemMsg_prog);
	cur = 0;
	std::string::size_type curPos = str.find (sf, cur);
	while (curPos != std::string::npos) {
		cur = str.find_first_not_of ("0123456789%", curPos + sf.size());
		moveProgressBar (str.substr (curPos + sf.size(), cur - curPos - sf.size()));
		if (cur != std::string::npos)
			str.erase (curPos, cur - curPos);
		else
			str.erase (curPos);
		curPos = str.find (sf, curPos);
	}

	// Обработка сообщения об остановке процесса
	std::string ss(str_systemMsg_stop);
	curPos = str.find (ss, 0);
	while (curPos != std::string::npos) {
		//stop();
		toRet = 1;
		str.erase (curPos, ss.size());
		curPos = str.find (ss, curPos);
	}

	return toRet;
}


/****************************************************************************
*					QtGui::moveProgressBar
*
*	Description:	Функция устанавливает новое значение указателя состояния
*					текущего процесса по данным, записанным в строке
*	Parameters:		str - сторка с числом от 0 до 100, устанавливаемым значением
*						  степени завершенности процесса
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::moveProgressBar (std::string str) {
	int curVal;
	curVal = atoi (str.c_str());
	if ((curVal > 0) && (curVal <= 100)) {
		if (progressBar->value() >= curVal)
			return;
		progressBar->setValue(curVal);
	}
}


/****************************************************************************
*					QtGui::getPathDataSet
*
*	Description:	Функция вызывает диалог выбора пути к используемому набору
*					данных
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::getPathDataSet() {

    QString path;

	path = QFileDialog::getExistingDirectory(
        this,
        "Choose a folder with DataSet to open",
        lineEdit_workingDir->text(),
        QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);

	lineEdit_dataSet->setText( path );

}


/****************************************************************************
*					QtGui::getPathParams
*
*	Description:	Функция вызывает диалог выбора пути к используемому
*					конфигурационному файлу
*	Parameters:		- 
*	Returns:		- 
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::getPathParams() {

    QString path;
    
    path = QFileDialog::getOpenFileName(
        this,
        "Choose a config-file to open",
        lineEdit_workingDir->text(),
        QString::null);

	lineEdit_params->setText( path );

}


/****************************************************************************
*					QtGui::getPathWhereToSave
*
*	Description:	Функция вызывает диалог выбора пути сохранения результатов
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::getPathWhereToSave() {

    QString path;
    
    path = QFileDialog::getExistingDirectory(
        this,
        "Choose a folder with DataSet to open",
        lineEdit_workingDir->text(),
        QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);

	lineEdit_whereToSave->setText( path );

}


/****************************************************************************
*					QtGui::getPathWorkingDir
*
*	Description:	Функция вызывает диалог выбора рабочей директории
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::getPathWorkingDir() {

    QString path;
    
    path = QFileDialog::getExistingDirectory(
        this,
        "Choose a working directory",
		lineEdit_workingDir->text(),
        QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);

	lineEdit_workingDir->setText( path );

}


/****************************************************************************
*					QtGui::getPathMpiexec
*
*	Description:	Функция вызывает диалог выбора пути к исполняемому файлу mpiexec
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::getPathMpiexec() {

    QString path;
    
    path = QFileDialog::getOpenFileName(
        this,
        "Choose an executable mpiexec file",
        lineEdit_workingDir->text(),
        QString::null);

	lineEdit_pathToMpi->setText( path );

}


/****************************************************************************
*					QtGui::startLog
*
*	Description:	Функция вывод информацию о созданном лог-файле
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::startLog() {
	
	textEdit_output->clear();
 
    textEdit_output->append( "Path to log-file: " + lineEdit_dataSet->text() + "\n" );

}


/****************************************************************************
*					QtGui::clear
*
*	Description:	Функция очищает содержимое окна, отображающего вывод внешенего алгоритма
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::clear() {

    textEdit_output->clear();

}


/****************************************************************************
*					QtGui::about
*
*	Description:	Функция вызывает окно с отображением информации о ГПИ
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::about() {
    QMessageBox::about(this, "About output", str_about);
}


/****************************************************************************
*					QtGui::checkBoxFunc
*
*	Description:	Функция, отражающая реакцию на нажатие checkbox'а -
*					сохранять ли результаты работы внешенего алгоритма
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::checkBoxFunc() {
	
	if (checkBox_saveResults->isChecked()) {
		label_whereToSave->setEnabled(true);
		lineEdit_whereToSave->setEnabled(true);
		browseButton_whereToSave->setEnabled(true);
	} else {
		label_whereToSave->setEnabled(false);
		lineEdit_whereToSave->setEnabled(false);
		browseButton_whereToSave->setEnabled(false);
	}

}


/****************************************************************************
*					QtGui::checkBoxUseMpi
*
*	Description:	Функция, отражающая реакцию на нажатие checkbox'а по использованию mpi
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::checkBoxUseMpi() {
	
	if (checkBox_useMpi->isChecked()) {
		label_pathToMpi->setEnabled(true);
		lineEdit_pathToMpi->setEnabled(true);
		browseButton_mpi->setEnabled(true);
		spinBox_numProc->setEnabled(true);
		label_numProc->setEnabled(true);
		label_mpiOps->setEnabled(true);
		lineEdit_mpiOps->setEnabled(true);
	} else {
		label_pathToMpi->setEnabled(false);
		lineEdit_pathToMpi->setEnabled(false);
		browseButton_mpi->setEnabled(false);
		spinBox_numProc->setEnabled(false);
		label_numProc->setEnabled(false);
		label_mpiOps->setEnabled(false);
		lineEdit_mpiOps->setEnabled(false);
	}

}


/****************************************************************************
*					QtGui::startChgState
*
*	Description:	Изменение внешнего вида окна по нажатию на кнопку старта
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::startChgState () {
	startButton->setEnabled (false);
	stopButton->setEnabled (true);
	progressBar->setEnabled (true);
	progressBar->setValue (0);
	lineEdit_status->setText (str_working);
}

void QtGui::checkExtendedGUI() {
	if(comboBox_algorithm->currentText() == str_alg_managed) {
		isExtendedGUI = true;
	}
}


/****************************************************************************
*					QtGui::start
*
*	Description:	Функция запуска внешнего процесса. Проверяется все ли необходимые
*					поля основного окна заполнены для запуска внешнего процесса.
*					Создается процесс и соответствующим образом изменяет внешний вид
*					основного окна ГПИ.
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::start() {
	// Проверка - заполнены ли необходимые поля
	if (!checkFieldsEntered())
		return;
	
	checkExtendedGUI();

	// Изменение вида интерфейса в соответствии с действиями сопутствующими запуску алгоритма
	startChgState();

	// Установка флага записи в файл
	if (checkBox_saveResults->isChecked()) {
		writeToLog = true;
	} else 
		writeToLog = false;

	// Индикация состояния
	textEdit_output->append("\nStarting the algorithm.");
	QString workDir;
	workDir = lineEdit_workingDir->text();
	// Создание процесса
	process = new QProcess;
	addSplashes (workDir);
	process->setWorkingDirectory (workDir);
	process->setProcessChannelMode(QProcess::MergedChannels);

	// Установка связи сигналов от процесса с действиями интерфейса
	connect (process, SIGNAL (readyRead()), this, SLOT( processOutput() ));
	connect (process, SIGNAL (finished(int, QProcess::ExitStatus)), this, SLOT( stop() ) );
	connect (stopButton, SIGNAL (clicked()), process, SLOT( kill() ));
	
	// Создание log-файла
	if (writeToLog)
		createLogFile();

	// Получение пути запуска файла
	QString path, algName;
	path = getAlgPath (comboBox_algorithm->currentText (), workDir, algName);

	QStringList args;
	args.clear();

	// Если используется MPI - модификация пути запуска
	if (checkBox_useMpi->isChecked()) {
		procNum = spinBox_numProc->value();
		QString mpiPath;
		mpiPath = lineEdit_pathToMpi->text();
		addQuotes (mpiPath);
		addSplashes (mpiPath);
		args << "-n";
		QString numStr;
		QTextStream(&numStr) << procNum;
		args << numStr;
		QString ops (lineEdit_mpiOps->text());
		if (ops.size() > 0) 
			addArgsToList(ops.toStdString(), args);
		args << path;
		path = mpiPath;
		//printf ("\n\t");printf (path.toStdString().c_str());printf ("\n");
	} else {
		procNum = 1;
	}

	// Дополнительные параметры запуска:
	// Обработка специально указанного конфигурационного файла
	if (lineEdit_params->text().size() > 0) {
		QString pathToConfigFile;
		pathToConfigFile = lineEdit_params->text();
		addSplashes (pathToConfigFile);
		args << str_configFile;
		args << pathToConfigFile;
	}
	// Обработка специально указанного внешнего набора данных
	if (lineEdit_dataSet->text().size() > 0) {
		QString dataSetName, dataSetPath;
		getDataSetNamePath(lineEdit_dataSet->text(), dataSetName, dataSetPath);
		args << str_dataSetDir;
		args << dataSetPath;
		args << str_dataSetName;
		args << dataSetName;
	}
	// Обработка параметра - сохранять ли результаты работы внешнего процесса
	if (checkBox_saveResults->isChecked()) {		
		args << str_resultsDir;
		if (lineEdit_whereToSave->text().size() > 0)
			args << lineEdit_whereToSave->text();
		else 
			args << "./";
	}

	// Фиксация времени запуска процесса
	time_t mytime;
	struct tm *mytm;

	mytime=time(NULL);
	mytm=localtime(&mytime);
	timeStart = *mytm;

	// Запуск процесса
	process->start(path, args);

	// Если процесс не удалось запустить - то соответствующее преобразование внешнего вида основного окна ГПИ
	if (!process->waitForStarted()) {
		QString message = QString("Could not start the algorithm, possibly its exe-file '%1' is absent.").arg(path);
		textEdit_output->append("\n " + message);
		stop();
		return;
	}

	// Вывод необходимой информации в окно с вывода результатов внешнего алгоритма
	textEdit_output->append("\nThe process has started successfully.");
	QString tmpStr;
	tmpStr = "Process name: ";
	QTextStream(&tmpStr) << algName;
	textEdit_output->append(tmpStr);
	tmpStr = "Start time:  ";
	QTextStream(&tmpStr) << mytm->tm_hour << ":" << mytm->tm_min << ":" << mytm->tm_sec << ".";
	textEdit_output->append(tmpStr);
	textEdit_output->append("------------------------------------------------------\n");
	//if (!process.waitForFinished()) return;

}


/****************************************************************************
*					QtGui::editConfig
*
*	Description:	Функция вызывает для редактирования выбранный конфигурационный файл
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::editConfig() {
	// Проверка - заполнено ли поле с адресом конфиг-файла
	if (lineEdit_params->text().size() < 1)
		return;

	std::string s;
	s = lineEdit_params->text().toStdString();
	
	#ifdef Q_OS_WIN32
		ShellExecuteA(NULL, "open", (char*) s.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		// Создание процесса
		process = new QProcess;
		
		QStringList args;
		args.clear();
		args << lineEdit_params->text();
		
		// Запуск процесса
		process->start (str_defWordProc, args);
	#endif
}


/****************************************************************************
*					QtGui::processOutput
*
*	Description:	Обработчик поступления сообщения от внешнего алгоритма
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::processOutput() {
	std::string strAll(process->readAll());
	// Обработчик - удаляет системные сообщения
//	int res;
//	res = systemMsgHandler (strAll);
	// Вывод на экран
	textEdit_output->append (strAll.c_str());
	// Запись в log-файл
	if (writeToLog)
		toLogFile (strAll);
	// Если при обработке было найдено служебное сообщение об останове процесса - то вызываем функцию остановки
//	if (res == 1)
	//		stop();
}

void QtGui::updateCommand()
{
	QStringList commandArgs = getCommand();

	for(int i = 1; i < commandArgs.count(); ++i) {
		commandArgs[i] = "'" + commandArgs[i] + "'";
	}

	lineEdit_command->setText(commandArgs.join(" "));
}

void QtGui::copyCommand()
{
	QApplication::clipboard()->setText(lineEdit_command->text());
}


/****************************************************************************
*					QtGui::stop
*
*	Description:	Функция остановки выполнения внешнего алгоритма, изменяет
*					соответствующим образом внешний вид окна и вывод данные о
*					продолжительности выполнения внешнего алгоритма
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::stop() {
	// Проверка - была ли уже эта кнопка нажата или вызвано нажатие
	if (lineEdit_status->text () == str_stopped)
		return;
	// Редактирование интерфейса - внешнего вида
	lineEdit_status->setText (str_stopped);
	startButton->setEnabled (true);
	stopButton->setEnabled (false);
	progressBar->setEnabled (false);
	progressBar->setValue (0);

	// Подсчет времени работы алгоритма и вывод его на экран
	time_t mytime;
    struct tm *mytm, tmDif;

	mytime=time(NULL);
    mytm=localtime(&mytime);

	tmDif.tm_year = mytm->tm_year - timeStart.tm_year;
	tmDif.tm_yday = mytm->tm_yday - timeStart.tm_yday;
	tmDif.tm_hour = mytm->tm_hour - timeStart.tm_hour;
	tmDif.tm_min = mytm->tm_min - timeStart.tm_min;
	tmDif.tm_sec = mytm->tm_sec - timeStart.tm_sec;
	if (tmDif.tm_sec < 0) {tmDif.tm_sec += 60; tmDif.tm_min--;}
	if (tmDif.tm_min < 0) {tmDif.tm_min += 60; tmDif.tm_hour--;}
	if (tmDif.tm_hour < 0) {tmDif.tm_hour += 24; tmDif.tm_yday--;}
	if (tmDif.tm_yday < 0) {tmDif.tm_yday += 365; tmDif.tm_year--;}
	

	QString tmpStr;
	tmpStr = "Algorithm's work time:  ";
	//QTextStream(&tmpStr) << mytm->tm_hour << ":" << mytm->tm_min << ":" << mytm->tm_sec << ".";

	bool toOut = false;
	if ((toOut) || (tmDif.tm_year > 0)) { QTextStream(&tmpStr) << tmDif.tm_year << " year "; toOut = true;}
	if ((toOut) || (tmDif.tm_yday > 0)) { QTextStream(&tmpStr) << tmDif.tm_yday << " days "; toOut = true;}
	if ((toOut) || (tmDif.tm_hour > 0)) { QTextStream(&tmpStr) << tmDif.tm_hour << " hours "; toOut = true;}
	if ((toOut) || (tmDif.tm_min > 0))  { QTextStream(&tmpStr) << tmDif.tm_min  << " min "; toOut = true;}
	QTextStream(&tmpStr) << tmDif.tm_sec << " sec.";

	// Закрытие записи к лог
	closeLogFile();

	// Вывод на экран завершающего сообщения
	textEdit_output->append("\n------------------------------------------------------");
	textEdit_output->append("The process has stopped successfully.");
	textEdit_output->append(tmpStr);
	tmpStr = "Number of processes: ";
	QTextStream(&tmpStr) << procNum << "\n";
	textEdit_output->append(tmpStr);
}


/****************************************************************************
*					QtGui::showVersion
*
*	Description:	Функция отображает текущую версию ГПИ
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::showVersion() {
	 QMessageBox::about(this,"Current Version", str_curVersion);

}


/****************************************************************************
*					QtGui::showHelp
*
*	Description:	функция отображает необходимую информацию по использованию ГПИ
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::showHelp() {
	QMessageBox::about(this, "Help notes", str_help);
}


/****************************************************************************
*					QtGui::clearAllFields
*
*	Description:	Функция очищает все поля основного окна ГПИ
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::clearAllFields() {
	comboBox_algorithm->setCurrentIndex(0);
	lineEdit_dataSet->clear();
	lineEdit_params->clear();
	checkBox_saveResults->setChecked(true);
	lineEdit_whereToSave->clear();
	lineEdit_whereToSave->setEnabled(true);
	browseButton_whereToSave->setEnabled(true);
	textEdit_output->clear();

}


/****************************************************************************
*					QtGui::exit
*
*	Description:	Функция закрывает основное окно ГПИ
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::exit() {
	if (lineEdit_status->text () != str_stopped)
		stop();
	this->close();
}


/****************************************************************************
*					QtGui::checkFieldsEntered
*
*	Description:	Функция проверяет все ли необходимые для запуска внешнего
*					алгоритма поля заполнены
*	Parameters:		-
*	Returns:		true - если все необходимые поля заполнены
*					false - иначе
*	Author:			dk
*	History:		
*
****************************************************************************/
bool QtGui::checkFieldsEntered () {
	if (lineEdit_workingDir->text().size() < 1) {
		QMessageBox::about(this,"Error notes", str_workingDirEmpty);
		lineEdit_workingDir->setFocus();
		return false;
	}
	/*if (lineEdit_dataSet->text().size() < 1) {
		QMessageBox::about(this,"Error notes", str_dataSetEmpty);
		lineEdit_dataSet->setFocus();
		return false;
	}*/
	/*if (lineEdit_params->text().size() < 1) {
		QMessageBox::about(this,"Error notes", str_paramsEmpty);
		lineEdit_params->setFocus();
		return false;
	}*/
	if ((checkBox_saveResults->isChecked()) && (lineEdit_whereToSave->text().size() < 1)) {
		QMessageBox::about(this,"Error notes", str_whereToSaveErr);
		lineEdit_whereToSave->setFocus();
		return false;
	}
	if ((checkBox_useMpi->isChecked()) && (lineEdit_pathToMpi->text().size() < 1)) {
		QMessageBox::about(this,"Error notes", str_pathToMpiErr);
		lineEdit_pathToMpi->setFocus();
		return false;
	}
	return true;
}


/****************************************************************************
*					QtGui::createLogFile
*
*	Description:	Функция открывает на запись лог-файл
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::createLogFile () {
	std::string fileName (lineEdit_whereToSave->text().toStdString());
	createDir (fileName);
	fileName.append ("//");
	fileName.append (str_logFileName);
	if( (logFile  = fopen( fileName.c_str(), "w" )) == NULL ) {
		textEdit_output->append("\n Warning:");
		textEdit_output->append("\n Cannot create log-file '");
		textEdit_output->append(fileName.c_str());
		textEdit_output->append("'. \n");
		return;
	}

	textEdit_output->append("\nLog-file created with the path: ");
	textEdit_output->append(removeSplashes (fileName));

	fprintf (logFile, "<Log file created by AxiomLib QT GUI>\n");

}


/****************************************************************************
*					QtGui::removeSplashes
*
*	Description:	Функция удаляет вхождения двойных слешей в строку
*	Parameters:		str - строка, в которой двойные слеши приводятся к одинарным
*	Returns:		char* - преобразованная строка в виде массива char'ов
*	Author:			dk
*	History:		
*
****************************************************************************/
const char* QtGui::removeSplashes (std::string &str) {
	unsigned int i = 1;
	while ( i < str.size() ) {
		if ((str[i] == '/') && (str[i-1] == str[i])) {
			str.erase (i, 1);
			continue;
		}
		i++;
	}
	return str.c_str();
}


/****************************************************************************
*					QtGui::addQuotes
*
*	Description:	Функция заключает строку в ковычки
*	Parameters:		str - строка, которая заключается в ковычки
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::addQuotes (QString &str) {
	#ifdef Q_OS_WIN32
		if (str.size() > 0){
			QString newStr;
			newStr = "\"";
			newStr.append (str);
			newStr.append ("\"");
			str = newStr;
		}
	#endif
}


/****************************************************************************
*					QtGui::addSplashes
*
*	Description:	Функция удваивает слеши, входящие в строку
*	Parameters:		str - строка, в которой ищутся и удваиваются слеши
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::addSplashes (QString &str) {
	unsigned int i = 0;
	while ( i < (unsigned int) str.size() ) {
		if (str[i] == '\\') {
			str.insert (i, '\\');
			i++;
		}
		/*if (str[i] == '/') {
			str.insert (i, '/');
			i++;
		}
		*/
		i++;
	}
}


/****************************************************************************
*					QtGui::toLogFile
*
*	Description:	Функция записывает в текущий открытый лог-файл содержание
*					укзанной в параметрах строки
*	Parameters:		str - записываемая строка
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::toLogFile (std::string &str) {
	if (logFile)
		fprintf (logFile, str.c_str());

}


/****************************************************************************
*					QtGui::createDir
*
*	Description:	Функция создает директорию по указанному адресу
*	Parameters:		dir - строка с путем, по которому необхоидимо создать папку
*	Returns:		< 0 - если при создании возникла ошибка
*					0 - если директория существует или ее удалось создать
*	Author:			dk
*	History:		
*
****************************************************************************/
int QtGui::createDir (std::string dir) {
	// Проверка входного параметра
	if (dir.size() < 1)
		return -1;

	boost::filesystem::path path (dir);
	// Проверка - существует ли указанная директория
	if ( boost::filesystem::exists(path) ) {
		if ( !boost::filesystem::is_directory(path) ) {
			boost::filesystem::create_directory(path);
		}
		return 0;
	}

	// Если директория с указанным именем не существует
	int index;
	index = (int) dir.find_last_of ("/\\");
	if (index > 0) {
		if (index == (int) (dir.size() - 1)) {
			dir.erase(index, 1);
			return createDir (dir);
		}
		if (createDir (dir.substr(0, index)) < 0)
			return -1;
		boost::filesystem::create_directory(path);
		return 0;
	} else {
		// значит слешей в пути не осталось - такая директория, считаем, должна всегда существовать
		return -1;
	}

	// Сюда дойти не должны - поэтому возвращаем ошибку
	return -2;
}


/****************************************************************************
*					QtGui::closeLogFile
*
*	Description:	Функция закрывает лог-файл на запись, если тот был открыт ранее
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::closeLogFile () {
	if (logFile)
		fclose (logFile);
}

QStringList QtGui::getCommand()
{
	QString workDir;
	workDir = lineEdit_workingDir->text();
	addSplashes (workDir);

	// Получение пути запуска файла
	QString path, algName;
	path = getAlgPath (comboBox_algorithm->currentText (), workDir, algName);

	QStringList args;

	int procNum;

	// Если используется MPI - модификация пути запуска
	if (checkBox_useMpi->isChecked()) {
		procNum = spinBox_numProc->value();
		QString mpiPath;
		mpiPath = lineEdit_pathToMpi->text();
		addQuotes (mpiPath);
		addSplashes (mpiPath);
		args << "-n";
		QString numStr;
		QTextStream(&numStr) << procNum;
		args << numStr;
		QString ops (lineEdit_mpiOps->text());
		if (ops.size() > 0)
			addArgsToList(ops.toStdString(), args);
		args << path;
		path = mpiPath;
		//printf ("\n\t");printf (path.toStdString().c_str());printf ("\n");
	} else {
		procNum = 1;
	}

	// Дополнительные параметры запуска:
	// Обработка специально указанного конфигурационного файла
	if (lineEdit_params->text().size() > 0) {
		QString pathToConfigFile;
		pathToConfigFile = lineEdit_params->text();
		addSplashes (pathToConfigFile);
		args << str_configFile;
		args << pathToConfigFile;
	}
	// Обработка специально указанного внешнего набора данных
	if (lineEdit_dataSet->text().size() > 0) {
		QString dataSetName, dataSetPath;
		getDataSetNamePath(lineEdit_dataSet->text(), dataSetName, dataSetPath);
		args << str_dataSetDir;
		args << dataSetPath;
		args << str_dataSetName;
		args << dataSetName;
	}
	// Обработка параметра - сохранять ли результаты работы внешнего процесса
	if (checkBox_saveResults->isChecked()) {
		args << str_resultsDir;
		if (lineEdit_whereToSave->text().size() > 0)
			args << lineEdit_whereToSave->text();
		else
			args << "./";
	}

	args.prepend(path);
	return args;
}


/****************************************************************************
*					QtGui::runASBrowser
*
*	Description:	Функция вызывает окно с описанием системы аксиом
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::runASBrowser() {
	AxiomSetBrowser* asBrowser;
	asBrowser = new AxiomSetBrowser(lineEdit_whereToSave->text().toStdString());
	asBrowser->show();
}


/****************************************************************************
*					QtGui::runResBrowser
*
*	Description:	Функция вызывает окно с описанием результатов распознавания
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void QtGui::runResBrowser() {
	ResBrowser* resBrowser;
	resBrowser = new ResBrowser(lineEdit_whereToSave->text().toStdString());
	resBrowser->show();
}

void QtGui::getDataSetNamePath(const QString &dataSetFullPath, QString &dataSetName, QString &dataSetPath) {
	int nameFrom, tmpInt;
	dataSetPath = dataSetFullPath;
	nameFrom = dataSetPath.lastIndexOf('\\');
	tmpInt = dataSetPath.lastIndexOf('/');
	nameFrom = max (nameFrom, tmpInt);
	dataSetName = dataSetPath;
	if (nameFrom < 0) {
		dataSetPath = "./";
	} else {
		dataSetName.remove (0, nameFrom+1);
		dataSetPath.remove (nameFrom, dataSetPath.size() - nameFrom);
		addSplashes (dataSetPath);
	}
}
