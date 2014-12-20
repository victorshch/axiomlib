/****************************************************************************
*					Функции класса AxiomSetBrowser.h
*
*	Description:	Функции окна с описанием систем аксиом - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/
#include "AxiomSetBrowser.h"

// Окончания основных файлов
#define str_axiomSet_ending	".as"
#define str_axiomSet_stat_ending	".stat"

// Обозначения для таблицы с описанием систем аксиом
#define str_no_data		"n/a"
#define int_column_with_axiomSet	1


/****************************************************************************
*					AxiomSetBrowser::AxiomSetBrowser
*
*	Description:	Конструктор класса, задает необходимые начальные параметры
*					и настраивает внешний вид
*	Parameters:		pathToAxiomSets - строка с путем к директории, в которой 
*						содержатся описания систем аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
AxiomSetBrowser::AxiomSetBrowser(const std::string pathToAxiomSets) {
	assVec.clear();
	axiomSetsPath = pathToAxiomSets;

	setupUi(this);

	refreshForm();

	connect( tableWidget, SIGNAL( cellActivated(int , int ) ), this, SLOT( cellCalled(int , int ) ) );
	connect( pushButton_refresh, SIGNAL( clicked() ), this, SLOT( refreshForm() ) );
}


/****************************************************************************
*					AxiomSetBrowser::~AxiomSetBrowser
*
*	Description:	Пустой деструктор класса
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
AxiomSetBrowser::~AxiomSetBrowser() {	

}


/****************************************************************************
*					AxiomSetBrowser::refreshForm
*
*	Description:	Функция вызывает обновление таблицы с описанием систем аксиом
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::refreshForm() {
	clearTable();
	findAxiomSets (axiomSetsPath);
}


/****************************************************************************
*					AxiomSetBrowser::cellCalled
*
*	Description:	Функция вычисляет номер аксиомы в системе аксиом по ее 
*					положению в таблице с описанием систем аксиом и вызывает 
*					функцию, которая содержит вызов окна отображения аксиомы.
*	Parameters:		row - ряд, содержащий выбранную аксиому, в таблице с описанием систем аксиом
*					column - столбец, содержащий выбранную аксиому, в таблице с описанием систем аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::cellCalled (int row, int /*column*/) {
	int curCol = tableWidget->currentColumn();
	int curRow = tableWidget->currentRow();
	if (curCol == int_column_with_axiomSet) {
		if ((curRow >= 0) && (curRow < (int) assVec.size()))
			showAxiomDescription (assVec[curRow], row);
	}
}


/****************************************************************************
*					AxiomSetBrowser::showAxiomDescription
*
*	Description:	Функция вызывает окно с отображением структуры и параметров
*					выбранной аксиомы 
*	Parameters:		ass - структура системы аксиом, в которой содержится выбранная аксиома
*					axiomNum - номер выбранной аксиомы в системе аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::showAxiomDescription (AxiomSetStructure &ass, const int axiomNum) const {
	if ((axiomNum < 0) || (axiomNum >= (int) ass.axioms.size()))
		return;
	AxiomBrowser* axBrowser;
	axBrowser = new AxiomBrowser(ass.axioms[axiomNum]);
	axBrowser->show();
}


/****************************************************************************
*					AxiomSetBrowser::clearTable
*
*	Description:	Функция очищает содержимое таблицы с описаниями систем аксиом
*	Parameters:		-
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::clearTable () {
	tableWidget->clearContents();
	int size = tableWidget->rowCount();
	for (int j = 0; j < size; j++)
		tableWidget->removeRow (0);
	assVec.clear();
}


/****************************************************************************
*					AxiomSetBrowser::findAxiomSets
*
*	Description:	Функция обхода директории в поисках файлов с описанием 
*					систем аксиом
*	Parameters:		pathToAxiomSets - путь к директории, содержащей отображаемые 
*						системы аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::findAxiomSets(const std::string pathToAxiomSets) {
	// Задание пути для поиска систем аксиом
	boost::filesystem::path fullPath( pathToAxiomSets );
	
	std::string fileNameLastHalf (str_axiomSet_ending); // шаблон - которому должны удовлеворять имена файлов с эталонными рядами для классов неисправностей
	std::string curFileName; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	// Проверка - существует ли указаный путь к каталогу
	if ( !boost::filesystem::exists( fullPath ) ) {
		return;
	}
	
	// Проверяем - директория ли данный путь
	if ( boost::filesystem::is_directory( fullPath ) ) {
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( !boost::filesystem::is_directory( *dir_itr ) ) {
								curFileName = dir_itr->path().filename().c_str();
				if (curFileName.find(fileNameLastHalf) == (curFileName.size() - fileNameLastHalf.size())) {
					// Значит нашли необходимый файл - обрабатываем его содержимое
					curFileName = (*dir_itr).path().string();
					addAxiomSet (curFileName);
				}
        	}
		}
	}

}


/****************************************************************************
*					AxiomSetBrowser::addAxiomSet
*
*	Description:	Функция добавляет описание системы аксиом в соответствующую 
*					таблицу в данном окне. При этом при необходимости - считывается 
*					файл с описанием статистики по данной системе аксиом.
*	Parameters:		path - путь к файлу с описанием добавляемой системы аксиом
*	Returns:		-
*	Author:			dk
*	History:		
*
****************************************************************************/
void AxiomSetBrowser::addAxiomSet (std::string &path) {
	AxiomSetStructure ass;
	// считываем содержимое файла и отображаем считанные значения
	if (readFromAS (path, ass) >= 0) {
		int curRow = tableWidget->rowCount();
		if (tableWidget->columnCount() < 2)
			return; // таблица с неверным заголовком
		// Сохраняем считанное описание во внутреннюю переменную
		assVec.push_back(ass);
		// Заводим новую строку для отображения считанной системы аксиом
		tableWidget->insertRow(curRow);
		tableWidget->setCellWidget (curRow,0,new QLabel(ass.axiomSetName.c_str()));

		QTableWidget *newTab = new QTableWidget (ass.axioms.size(), 1);
		QStringList names;
		names.clear();
		names << "Axiom Name";
		newTab->setHorizontalHeaderLabels(names);
		for(unsigned int i = 0; i < ass.axioms.size(); i++) {
			newTab->setCellWidget (i,0,new QLabel (ass.axioms[i].axiomName.c_str()));
		}
		newTab->adjustSize();
		connect( newTab, SIGNAL( cellActivated(int , int ) ), this, SLOT( cellCalled(int , int ) ) );
		tableWidget->setCellWidget (curRow, 1, newTab);
		// Отображение статистики по системе аксиом
		if (tableWidget->columnCount() >= 6) {
			// Считывание файла со статистикой
			std::string asEnding (str_axiomSet_ending);
			std::string statFile = path.substr (0, path.size() - asEnding.size());
			statFile.append(str_axiomSet_stat_ending);
			Environment env;
			env.readFile(statFile);
			// Отображение значений
			double dbl;
			QString qstr;
			if ((env.getDoubleParamValue(dbl, "axiomSet", "firstError") >= 0) && (dbl >= 0.0)) {
				qstr.clear();
				QTextStream(&qstr) << dbl;
				tableWidget->setCellWidget (curRow,3,new QLabel (qstr));
			} else {
				QLabel *label = new QLabel (str_no_data);
				label->setEnabled (false);
				tableWidget->setCellWidget (curRow,3,label);
			}
			if ((env.getDoubleParamValue(dbl, "axiomSet", "secondError") >= 0) && (dbl >= 0.0)) {
				qstr.clear();
				QTextStream(&qstr) << dbl;
				tableWidget->setCellWidget (curRow,2,new QLabel (qstr));
			} else {
				QLabel *label = new QLabel (str_no_data);
				label->setEnabled (false);
				tableWidget->setCellWidget (curRow,2,label);
			}
			if ((env.getDoubleParamValue(dbl, "axiomSet", "anomCount") >= 0) && (dbl >= 0.0)) {
				qstr.clear();
				QTextStream(&qstr) << dbl;
				tableWidget->setCellWidget (curRow,4,new QLabel (qstr));
			} else {
				QLabel *label = new QLabel (str_no_data);
				label->setEnabled (false);
				tableWidget->setCellWidget (curRow,4,label);
			}
			if ((env.getDoubleParamValue(dbl, "axiomSet", "normCount") >= 0) && (dbl >= 0.0)) {
				qstr.clear();
				QTextStream(&qstr) << dbl;
				tableWidget->setCellWidget (curRow,5,new QLabel (qstr));
			} else {
				QLabel *label = new QLabel (str_no_data);
				label->setEnabled (false);
				tableWidget->setCellWidget (curRow,5,label);
			}
		}
	}
}


/****************************************************************************
*					AxiomSetBrowser::readFromAS
*
*	Description:	Функция считывает из файла описание системы аксиом во 
*					внутреннее представление
*	Parameters:		filePath - путь к файлу с описанием системы аксиом
*					ass - заполняемая по файлу структура системы аксиом
*	Returns:		<0 - если в ходе считывания структуры произошли ошибки
*					0 - если структура была считана нормально
*	Author:			dk
*	History:		
*
****************************************************************************/
int AxiomSetBrowser::readFromAS (std::string &filePath, AxiomSetStructure &ass) {
	std::string str, curFile;
	int i = 0;

	ass.axioms.clear();
	
	// Открываем файл
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" :;	");
	
	if (i = std::getline(file, str) && i != EOF) {
		// читаем имя системы аксиом и сравниваем с названием файла
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		curFile.assign(*tok_name);
		if (filePath.find (*tok_name) == std::string::npos) {
			// имя аксиомы расходится - в названии файла и в его содержимом - выдаем ошибку
			file.close();
			return -1;
		}
		// запоминаем имя файла
		ass.axiomSetName = *tok_name;
	} else {
		// файл пуст
		file.close();
		return -2;
	}
	
	if (i = std::getline(file, str) && i != EOF) {
		// читаем имя банка аксиом используемых в данной системе
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if ((*tok_name).empty()) {
			// нет имени банка аксиом
			file.close();
			return -3;
		}		
		ass.axiomBankDir = *tok_name;
	}
	else {
		// файл неожиданно закончился
		file.close();
		return -4;
	}
	
	// итеративано считываем записанные имена аксиом и параметры элементарны условий
	int currentAxiomNumber = 0;
	if (i = std::getline(file, str) && i != EOF)  {
		// структуру, которую будем заполнять и копировать в ass
		AxiomStructure as;
		as.axiomParameters.clear();
		as.ecNames.clear();
		// временные параметры для хранения имени аксиомы и имени параметра аксиомы
		std::string currentParam;
		std::string currentEC;
		std::map<std::string, std::string> readParams;
		
		int toSave = 0; // флаг необходимости сохранить считанной аксиомы
		int condition = 0;
		while (condition == 0) {
			// разбираем строку на токены
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			if ( tok_iter == tokens.end()) {
				// пустая строка
				if (i = std::getline(file, str) && i != EOF) {
					// обрабатываем следующую строку
					continue;
				} else {
					// файл пуст
					condition = 1;
					break;
				}
			}
			const char* tempNumber = (*tok_iter).c_str();
			// вариант - считываение имени аксиомы
			if (atof(tempNumber) > 0) {
				// Читаем название аксиомы
				if (toSave > 0) {
					if (!currentEC.empty()) {
						as.axiomParameters[currentEC] = readParams;
					}
					// Сохраняем ранее сйчитанную аксиому 
					(ass.axioms).push_back(as);
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					// формат файла неподходящий
					file.close();
					return -5;
				}
				currentAxiomNumber++;
				toSave = 1;
				as.axiomName = *tok_iter;
				as.axiomParameters.clear();
				as.ecNames.clear();
				currentParam = "";
				currentEC = "";
				if (i = std::getline(file, str) && i != EOF) { 
					// после названия аксиомы что-то есть - идем на новую и терацию цикла
					continue;
				}
				else { 
					// после названия аксиомы ничего нет - всего 1 аксиома
					condition = 1;
					break;
				}
			} else {
				// вариант - читаем элементарное условие, его параметры и значения
				if (*tok_iter == currentEC) {
					//значит продолжаем заполнения параметров данного элементарного условия
				} else {
					if (!currentEC.empty()) {
						as.axiomParameters[currentEC] = readParams;
					}
					currentEC = *tok_iter;
					readParams.clear();
					as.ecNames.push_back (*tok_iter);
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					// формат файла неподходящий
					file.close();
					return -6;
				}
				currentParam = *tok_iter;
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					// формат файла неподходящий
					file.close();
					return -7;
				}
				if (*tok_iter != "->") {
					// формат файла неподходящий
					file.close();
					return -8;
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					// формат файла неподходящий
					file.close();
					return -9;
				}
				readParams[currentParam] = *tok_iter;
				if (i = std::getline(file, str) && i != EOF) {
					// после - что-то - есть - идем на новую и терацию цикла
					continue;
				} else { 
					condition = 1;
					break;
				}
			}
		}
		// сохраняем - если есть не сораненные считанные аксимы
		if (toSave > 0) {
			if (!currentEC.empty()) {
				as.axiomParameters[currentEC] = readParams;
			}
			//сохраняем ранее считанную аксиому 
			(ass.axioms).push_back(as);
		}
	}
	
	file.close();
	
	return 0;
}
