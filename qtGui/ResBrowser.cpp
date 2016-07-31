/****************************************************************************
*					Функции класса ResBrowser.h
*
*	Description:	Функции окна с отображением результатов распознавания - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include "ResBrowser.h"

// Окончания файлов и начала служебных файлов
#define str_axiomSet_ending		".as"
#define str_dataFile_ending		".csv"
#define str_markUpFile_start	"mu_"
#define str_no_table_data		"n/a"

// Приложение для просмотра рядов под Unix
#define str_defCsvProc		"oocalc"

// Название исполняемого файла, вызываемого для графического отображения данных
#define str_showRowProc		"show_rows"

// Окончания исполняемых файлов
#define str_alg_win_end		".exe"
#define str_alg_unix_end	".sh"

// Управляющее определение - Если > 0 - то для каждой папки ищется соответствущий файл с описанием системы аксиом.
#define	check_axiomSet_file_presence 1


/****************************************************************************
*					ResBrowser::ResBrowser
*
*	Description:	Конструктор класса, задает необходимые начальные параметры
*					и настраивает внешний вид
*	Parameters:		parent - виджет, из которого данный класс был инициирован
*					flags - параметры запуска окна, как виджета QT
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
ResBrowser::ResBrowser(const std::string pathToDir) {
	axiomSetDirs.clear();
	pathToResults = pathToDir;

	setupUi(this);

	refreshForm ();	

	connect( comboBox_axiomSets, SIGNAL( activated( int ) ), this, SLOT( loadContents( int ) ) );
	connect( tableWidget_markUps, SIGNAL( cellActivated(int , int ) ), this, SLOT( muCellCalled(int , int ) ) );
	connect( tableWidget_results, SIGNAL( cellActivated(int , int ) ), this, SLOT( resCellCalled(int , int ) ) );
	connect( pushButton_refresh, SIGNAL( clicked() ), this, SLOT( refreshForm() ) );
	connect( pushButton_showRow, SIGNAL( clicked() ), this, SLOT( showRow() ) );
	connect( pushButton_showResults, SIGNAL( clicked() ), this, SLOT( showResults() ) );

}


/****************************************************************************
*					ResBrowser::~ResBrowser
*
*	Description:	Пустой деструктор класса
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
ResBrowser::~ResBrowser() {

}


/****************************************************************************
*					ResBrowser::refreshForm
*
*	Description:	Функция, вызывающая обновление содержимого окна путем 
*					нового прохода по содержимому директории с результатами
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::refreshForm () {
	clearForm();
	findAxiomSets (pathToResults);
	loadContents (0);
}


/****************************************************************************
*					ResBrowser::muCellCalled
*
*	Description:	Функция вызывает открытие файла с разметкой некоторой 
*					нештатной ситуации выбранной системой аксиом
*	Parameters:		row - номер ряда выбранного файла в таблице с файлами разметок 
*					column - номер столбца выбранного файла
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::muCellCalled (int row, int column) {
	std::map <int, std::string>::const_iterator c_iter;
	c_iter = axiomSetDirs.find (comboBox_axiomSets->currentIndex());
	if (c_iter == axiomSetDirs.end())
		return;
	std::string str;
	str = c_iter->second;
	str.append ("/");
	QLabel *item;
	item = (QLabel *) tableWidget_markUps->cellWidget(row, column);
	str.append (item->text().toStdString());
	openFile (str);
}


/****************************************************************************
*					ResBrowser::resCellCalled
*
*	Description:	Функция вызывает открытие файла с описанием результатов
*					распознавания конкретного ряда
*	Parameters:		row - ряд выбранного для открытия файла в таблице с файлами результатов
*					column - столбец выбранного для открытия файла
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::resCellCalled (int row, int column) {
	std::map <int, std::string>::const_iterator c_iter;
	c_iter = axiomSetDirs.find (comboBox_axiomSets->currentIndex());
	if (c_iter == axiomSetDirs.end())
		return;
	std::string str;
	str = c_iter->second;
	str.append ("/");
	QLabel *item;
	item = (QLabel *) tableWidget_results->cellWidget(row, column);
	str.append (item->text().toStdString());
	openFile (str);	
}


/****************************************************************************
*					ResBrowser::openFile
*
*	Description:	Функция вызывает открытие выбранного файла с траекторией или разметкой
*	Parameters:		path - путь к открываемому файлу
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::openFile (std::string &path) {
	#ifdef Q_OS_WIN32
		ShellExecuteA(NULL, "open", (char*) path.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		// Создание процесса
		QProcess *process = new QProcess;
		
		QStringList args;
		args.clear();
		args << (QString) path.c_str();
		
		// Запуск процесса
		process->start (str_defCsvProc, args);
	#endif
}


/****************************************************************************
*					ResBrowser::clearForm
*
*	Description:	Функция очищает содержимое всего окна
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::clearForm () {
	axiomSetDirs.clear();
	comboBox_axiomSets->clear();
	clearTableFields();
	setFormEnabledOrNot();
}


/****************************************************************************
*					ResBrowser::clearTableFields
*
*	Description:	Функция очищает содержимое полей таблиц с файлами разметок и траекторий
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::clearTableFields() {
	tableWidget_markUps->clearContents();
	int size = tableWidget_markUps->rowCount();
	for (int j = 0; j < size; j++)
		tableWidget_markUps->removeRow (0);
	
	tableWidget_results->clearContents();
	int size2 = tableWidget_results->rowCount();
	for (int j = 0; j < size2; j++)
		tableWidget_results->removeRow (0);

}


/****************************************************************************
*					ResBrowser::setFormEnabledOrNot
*
*	Description:	Функция делает окно затемненным, если в нем не содержится 
*					результатов ни по одной системе аксиом
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::setFormEnabledOrNot() {
	if (comboBox_axiomSets->count() > 0)
		comboBox_axiomSets->setEnabled(true);
	else
		comboBox_axiomSets->setEnabled(false);
	if (tableWidget_markUps->rowCount() > 0)
		tableWidget_markUps->setEnabled(true);
	else
		tableWidget_markUps->setEnabled(false);
	if (tableWidget_results->rowCount() > 0)
		tableWidget_results->setEnabled(true);
	else
		tableWidget_results->setEnabled(false);
}


/****************************************************************************
*					ResBrowser::findAxiomSets
*
*	Description:	Функция обзора директории в поисках описаний результатов 
*					систем аксиом.
*	Parameters:		pathToDir - путь к директории, в которой искать результаты
*						по системам аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::findAxiomSets(const std::string pathToDir) {
	// Задание пути для поиска систем аксиом
	boost::filesystem::path fullPath( pathToDir );
	
	std::string fileNameLastHalf (str_axiomSet_ending); // шаблон - которому должны удовлеворять имена файлов с эталонными рядами для классов неисправностей
	std::string curFileName, curDirName; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	// Проверка - существует ли указаный путь к каталогу
	if ( !boost::filesystem::exists( fullPath ) ) {
		clearForm ();
		return;
	}
		
	// Проверяем - директория ли данный путь
	if ( boost::filesystem::is_directory( fullPath ) ) {
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( boost::filesystem::is_directory( *dir_itr ) ) {
				curDirName = dir_itr->path().filename().c_str();
				/*std::string::size_type ind = curDirName.find_first_not_of ("0123456789");
				if ((ind != std::string::npos) && (ind != 0))
					ind += 1;
				else 
					continue;
				curDirName = curDirName.substr (ind); // теперь в curDirName содержится предполагаемое имя системы аксиом
				*/
				// ищем файл с описанием системы аксиом
				#if check_axiomSet_file_presence
				for ( boost::filesystem::directory_iterator file_itr( fullPath ); file_itr != end_iter; ++file_itr ) {
					curFileName = file_itr->path().filename().c_str();
					if (curFileName.find(fileNameLastHalf) == (curFileName.size() - fileNameLastHalf.size())) {
						// Значит текущий файл - это файл с описанием системы аксиом 
						curFileName = curFileName.substr(0, curFileName.size() - fileNameLastHalf.size());
						if (curDirName == curFileName) {
							addAxiomSetResults (dir_itr->path().string(), curDirName);
						}
					}
				}
				#else
				addAxiomSetResults (dir_itr->string(), curDirName);
				#endif
        	}
		}
	} else {
		clearForm ();
	}
	setFormEnabledOrNot();
}


/****************************************************************************
*					ResBrowser::addAxiomSetResults
*
*	Description:	Функция добавляет во внутреннее представление систему аксиом
*	Parameters:		axiomSetPath - найденный путь к результатам по системе аксиом
*					axiomSetName - имя обрабатываемой системы аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::addAxiomSetResults (std::string axiomSetPath, std::string &axiomSetName) {
	QString str("");
	QTextStream(&str) << comboBox_axiomSets->count() + 1 << ". ";
	str.append(axiomSetName.c_str());
	comboBox_axiomSets->addItem (str);
	axiomSetDirs[comboBox_axiomSets->count() - 1] = axiomSetPath;
}


/****************************************************************************
*					ResBrowser::loadContents
*
*	Description:	Функция отображает все файлы в директории с результатами 
*					для выбранной системы аксиом
*	Parameters:		index - номер системы аксиом в axiomSetDirs
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::loadContents (int index) {
	// Очищение содержимого полей для других записей
	clearTableFields();
	
	// поиск адреса где хранятся результаты по выбранной системе аксиом
	std::map <int, std::string>::const_iterator c_iter;
	c_iter = axiomSetDirs.find(index);
	if (c_iter == axiomSetDirs.end())
		return;

	// считывание и отображение результатов в форме
	boost::filesystem::path path( c_iter->second );

	std::string fileLastHalf (str_dataFile_ending);
	std::string muFileFirstHalf (str_markUpFile_start);
	std::string curFileName; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	// Проверка - существует ли указаный путь к каталогу
	if ( !boost::filesystem::exists( path ) )
		return;
	// Проверяем - директория ли данный путь
	if ( !boost::filesystem::is_directory( path ) )
		return;

	// Проходим по директории и анализируем содержимое
	int curRow = 0;
	boost::filesystem::directory_iterator end_iter;
	for ( boost::filesystem::directory_iterator file_itr( path ); file_itr != end_iter; ++file_itr ) {
		if ( boost::filesystem::is_directory (*file_itr) )
			continue;
		curFileName = file_itr->path().filename().c_str();
		if (curFileName.find(fileLastHalf) == (curFileName.size() - fileLastHalf.size())) {
			// значит файл оканчивается как должен файл с рядами
			if (curFileName.find(muFileFirstHalf) == 0) {
				// значит это файл с разметкой эталонных траекторий
				curRow = tableWidget_markUps->rowCount();
				tableWidget_markUps->insertRow(curRow);
				tableWidget_markUps->setCellWidget (curRow,0,new QLabel(curFileName.c_str()));
			} else {
				// значит это файл с разметкой тестовых рядов
				curRow = tableWidget_results->rowCount();
				tableWidget_results->insertRow(curRow);
				tableWidget_results->setCellWidget (curRow,0,new QLabel(curFileName.c_str()));
			}
		}
	}
	setFormEnabledOrNot();
}


/****************************************************************************
*					ResBrowser::showRow
*
*	Description:	Функция вызывает графическое отображение выбранной траектории
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::showRow () {
	int curCol = -1;
	int curRow = -1;
	curCol = tableWidget_results->currentColumn();
	curRow = tableWidget_results->currentRow();
	if ((curCol < 0)|| (curRow < 0))
		return;
	std::map <int, std::string>::const_iterator c_iter;
	c_iter = axiomSetDirs.find (comboBox_axiomSets->currentIndex());
	if (c_iter == axiomSetDirs.end())
		return;
	std::string str = c_iter->second;
	str.append ("/");
	QLabel *item;
	item = (QLabel *) tableWidget_results->cellWidget(curRow, curCol);
	str.append (item->text().toStdString());
	// Теперь в str содержится полный путь к csv-файлу, данные из которого необходимо отобразить
	showFile (str, 0);
}


/****************************************************************************
*					ResBrowser::showResults
*
*	Description:	Функция вызывает графическое отображение результатов распознавания
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::showResults () {
	int curCol = -1;
	int curRow = -1;
	curCol = tableWidget_results->currentColumn();
	curRow = tableWidget_results->currentRow();
	if ((curCol < 0)|| (curRow < 0))
		return;
	std::map <int, std::string>::const_iterator c_iter;
	c_iter = axiomSetDirs.find (comboBox_axiomSets->currentIndex());
	if (c_iter == axiomSetDirs.end())
		return;
	std::string str = c_iter->second;
	str.append ("/");
	QLabel *item;
	item = (QLabel *) tableWidget_results->cellWidget(curRow, curCol);
	str.append (item->text().toStdString());
	// Теперь в str содержится полный путь к csv-файлу, данные из которого необходимо отобразить
	showFile (str, 1);
}


/****************************************************************************
*					ResBrowser::showFile
*
*	Description:	Функция запускает графическое отображение данных из файла.
*	Parameters:		path - путь к файлу, данные из которого необходимо отображать
*					whatToShow - если 0, то отображается сама траектория,
*								 если 1, то отображаются результаты распознавания
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void ResBrowser::showFile (std::string &path, int whatToShow) {
	QString tmpStr;
	QTextStream(&tmpStr) << whatToShow;
	#ifdef Q_OS_WIN32
		std::string exeFile (str_showRowProc);
		exeFile.append (str_alg_win_end);
		path.append (" ");
		path.append (tmpStr.toStdString());
		ShellExecuteA(NULL, "open", (char*) exeFile.c_str(), (char*) path.c_str(), NULL, SW_SHOWNORMAL);
	#else
		// Создание процесса
		QProcess *process = new QProcess;
		
		QStringList args;
		args.clear();
		args << (QString) "arspds";
		args << (QString) path.c_str();
		args << tmpStr;
		
		std::string exeFile ("./");
		exeFile.append (str_showRowProc);
		exeFile.append (str_alg_unix_end);
		// Запуск процесса
		process->start (exeFile.c_str(), args);
	#endif
}
