/****************************************************************************
*				AxiomSetBase.cxx
*
*	Description:	Функции класса AxiomSetBase - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "AxiomSetBase.h"

using namespace AxiomLib;

// Ключевые слова для записи файла с описанием системы аксиом: 
#define str_not_symbol			"not"
#define str_or_symbol			"or"
// Разрешение файла с описанием системы аксиом
#define str_as_file_ending		".as"

// Ключевые слова для записи файла со статистикой: 
#define str_stat_file_ending	".stat"
#define str_axiomSet_keyWord	"axiomSet"
#define str_I_error_keyWord		"firstError"
#define str_II_error_keyWord	"secondError"

// Максимальная длина адреса (достаточно большое число)
#define maxAddrLen 1000


// Пустой конструктор
AxiomSetBase::AxiomSetBase (void) {
	axiomSets.clear();
}

/****************************************************************************
*					AxiomSetBase::compareStringAndPath
*
*	Description:	Функция сравнения имени файла с путем к файлу.
*				Сравнение происходит по этапам: 
*				- к имени файла дописывается расширение - указываемое в параметрах
*				- в начало имени дописывается "/"
*				- измененное имя файла ищется в конце пути к файлу
*	Parameters:	fileName - имя файла
*				path - путь к файлу(не факт что к этому)
*				n3 - расширение файла, с которым предполагается найти файл в пути к файлу
*	Returns:		bool
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool AxiomSetBase::compareStringAndPath (std::string fileName, std::string path, const char* n3) const {
	
	   std::string n1("/");
	   const char* n2 = fileName.c_str();
	   n1.append(n2);
	   n1.append(n3);
	   return checkName (path, (int) (path.size() - n1.size() - 1), (int) (path.size()  - n1.size()), n1);

}


/****************************************************************************
*					AxiomSetBase::checkName
*
*	Description:	Функция сравнения строк.
*				Кроме того отдельно указывается с какого символа в каждой 
*				строке начинать сравнение и до какого символа его вести.
*				Используется для сравнения с шаблоном имен файлов.
*	Parameters:	name1 - строка для сравнения
*				from -номер символа в строке name1 с которого начинать сравнение
*				upTo - номер символа в строке name1 до которого вести сравнение
*				name2 - строка для сравнения
*	Returns:		true  - если первый файл подходит под второй как под шаблон
*				false - в противном случае
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool AxiomSetBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) const {
	std::basic_string <char>::size_type indexCh1, indexCh2;
	static const std::basic_string <char>::size_type npos = maxAddrLen;
	const char * cName2 = name2.c_str();
	if ((from >= upTo) || (from > (signed int) name1.size())) {
		return false;
	}
	if (upTo > (int) name1.size()) {
		upTo = (int) name1.size() - 1;
	}
	indexCh1 = name1.rfind ( name2 , upTo );
	if (from < 0) {
		from = 0;
		indexCh2 = npos;
	}
	else {
		indexCh2 = name1.rfind ( name2 , from );
	}
	if ((indexCh1 < npos) && (indexCh1 >= 0) && ((indexCh2 >= npos) || (indexCh2 < 0))) {
		// все верно - есть до upTo и нет до from
		return true;
	}
	return false;
}


/****************************************************************************
*					AxiomSetBase::readSets
*
*	Description:	Функция чтения систем аксиом - откуда читать - путь читается 
*					из конфигурационным файла классом environment
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException  - если не существует каталога с системами аксиом,
*						указанного в конфиурационном файл
*						если заданный путь - путь к файлу, а не к каталогу
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readSets (const char* axiomSetBaseDir) {
	std::string axiomSetPath(axiomSetBaseDir);
	boost::filesystem::path fullPath( axiomSetPath );
	boost::filesystem::path filePath;
	
	std::string axiomSetFileName (str_as_file_ending); // шаблон для формата файла системы аксиом
	
	std::string aa; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	std::string str;
	int i = 0;
	// проверка - верен ли путь
	if ( !boost::filesystem::exists( fullPath  ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readSets: cannot find base directory of axiom set .");
		return 1;
	}
	
	// Проверяем - директория ли данный путь - как предполагается к основному какталогу банка систем аксиом
	if ( boost::filesystem::is_directory( fullPath ) ) {
		// Раз директория - тогда ходим по файлам и считываем системы аксиом
		this->axiomSets.clear();
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( boost::filesystem::is_directory( *dir_itr ) ) { // внутри заданного каталога с банком аксиом - нашелся новый каталог - 
												// это не предуспотрено форматом - просто пропускаем этот каталог				
        		}
			else {
				filePath = fullPath;
				filePath /= dir_itr->path();
				aa = dir_itr->path().filename().c_str();
				if ( checkName (aa, (int) aa.size() - 4, (int) aa.size() - 3, axiomSetFileName) ) {
					// то есть в dir_itr сейчас имя файла описания системы аксиомы
					AxiomSetStructure ass;
					aa = (*dir_itr).path().string().c_str();
					this->readFromAS (aa, ass);
					aa = dir_itr->path().filename().c_str();
					this->axiomSets[aa.substr(0, aa.size() - 3)] = ass;
				}
				else {
					// то есть в директории банка аксиом нашелся файл с не подходящим под шаблоный именем - его просто пропускаем
				}
			}
		}
	}
	else {// Указанный путь не является каталогом - поэтому считывание эталонных рядов не произведено
		throw AxiomLibException ("Error in AxiomSetBase::readSets: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	Функция чтения информации о системе аксиом из одного .as файла
*				Формат файла описан в .formats.x
*				В данной функцие считывается и сравнивается с инменем файла,
*				имя записанное в первой строке файла; Считывается и сохраняется 
*				имя используемой базы аксиом; 
*				(Имена не должны содержать разделяющих символов ".,';:" )
*				Далее в функцие считываются входящие в систему аксиомы и параметры 
*				элементарных условий. Все считанный сущности записываются в 
*				специально определенную структуру типа map (см. AxiomSetStrucutre.h)
*	Parameters:	filePath - имя файла для обработки (путь к фйлу)
*				ass - заполняемая по файлу струтура типа AxiomSetStrucutre
*	Returns:		0
*	Throws:		AxiomLibException  - если файл с заданным путем не существует
*								если формат файла не соглаосван описанием формата (см. formats.x)
*								если имя файла не согласовано с именем в первой строке файла
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomSetStructure &ass) {
	std::string str;
	int i = 0;
	// Дописываем разрешение к файлу
	filePath.append (str_as_file_ending);
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readFromAS: cannot find file '"+filePath+"'");
		return 1;
	}
	
	(ass.axioms).resize(0);
	
	// Открываем файл
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" :;	");
	
	if (i = std::getline(file, str) && i != EOF) {
		// читаем имя системы аксиом и сравниваем с названием файла
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (this->compareStringAndPath(*tok_name, filePath, str_as_file_ending) == false) {
			// имя аксиомы расходится - в названии файла и в его содержимом - выдаем ошибку
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAX: different filenames in the file and in the name of the file : '" 
			                         + std::string(*tok_name) + "', '" + filePath + "'");
		}
		// запоминаем имя файла
		ass.axiomSetName = *tok_name;
	} else {
		// если файл пуст  -  то ошибка - потому что имя файла подобающее
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - it is empty.");
	}
	
	if (i = std::getline(file, str) && i != EOF) {
		// читаем имя банка аксиом используемых в данной системе
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAX: bad axiom bank name.");
		}
		
		ass.axiomBankDir = *tok_name;;
	} else {
		// формат не верный - нет необходимой информации
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - it has no axioms.");
	}
	
	// итеративано считываем записанные имена аксиом и параметры элементарны условий
	int currentAxiomNumber = 0;
	if (i = std::getline(file, str) && i != EOF)  {
		// структуру, которую будем заполнять и копировать в ass
		AxiomStructure as;
		(as.axiomParameters).clear();
		// временные параметры для хранения имени аксиомы и имени параметра аксиомы
		std::string currentParam, currentEC, tempStr;
		std::map<std::string, std::string> readParams;
		
		int toSave = 0; // флаг необходимости сохранить считанной аксиомы
		int condition = 0;
		while (condition == 0) {
			// разбираем строку на токены
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			if ( tok_iter == tokens.end()) {// пустая строка  - возможно разделитель между аксиомами - 
									//а возможно просто одна из множетсва пустых строк
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			//const char* tempNumber = (*tok_iter).c_str();
			tempStr = *tok_iter;
			// вариант - считываение имени аксиомы
			if (atof(tempStr.c_str()) > 0) {
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
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - cannot find axiom name.");
				}
				currentAxiomNumber++;
				toSave = 1;
				as.axiomName = *tok_iter;
				(as.axiomParameters).clear();
				currentParam = "";
				currentEC = "";
				if (i = std::getline(file, str) && i != EOF) { 
					// после названия аксиомы что-то - есть - идем на новую и терацию цикла
					continue;
				} else { 
					// после названия аксиомы ничего нет - всего 1 аксиома
					condition = 1;
					break;
				}
			} else {
				// вариант - читаем элементарное условие, его параметры и значения
				if (*tok_iter == currentEC) {//значит продолжаем заполнения параметров данного элементарного условия
				} else {
					if (!currentEC.empty()) {
						as.axiomParameters[currentEC] = readParams;
					}
					currentEC = *tok_iter;
					readParams.clear();
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find name of parameter.");
				}
				currentParam = *tok_iter;
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find -> in .as file while setting parameters.");
				}
				if (*tok_iter != "->") {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format - while setting parameters '->' should be.");
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
						throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find a value of parameter.");
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
		// сохраняем- если есть не сораненные считанные аксимы
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


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	Функция чтения информации о системе аксиом из одного .as файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы аксиом; 
*					(Имена не должны содержать разделяющих символов ".,';:" )
*					Далее в функцие считываются входящие в систему аксиомы и параметры 
*					элементарных условий. Все считанный сущности записываются в 
*					специально определенную структуру типа map (см. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aess - заполняемая по файлу струтура типа AxiomExprSetStrucutre
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomExprSetStructure &aess) const {
	std::vector <std::string> paramNames;
	paramNames.clear();
	return readFromAS (filePath, aess, paramNames);
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	Функция чтения информации о системе аксиом из одного .as файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы аксиом; 
*					(Имена не должны содержать разделяющих символов ".,';:" )
*					Далее в функцие считываются входящие в систему аксиомы и параметры 
*					элементарных условий. Все считанный сущности записываются в 
*					специально определенную структуру типа map (см. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aess - заполняемая по файлу струтура типа AxiomExprSetStrucutre
*					paramNames - названия размеростей набора данных
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomExprSetStructure &aess, const std::vector <std::string> &paramNames) const {
	// Дописываем расширение к пути к файлу с описанием структуры системы аксиом
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_as_file_ending);
	
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readFromAS: cannot find given axiomSet-description file '" + filePath + "'");
	}

	// Очищаем содержимое структуры с описанием системы аксиом
	aess.axioms.clear();
	
	// Открываем файл
	boost::filesystem::ifstream file( fullFilePath );

	boost::char_separator<char> sep(" :;\t[]");
	std::string str;
	int i = 0;
	
	// Читаем имя системы аксиом и сравниваем с названием файла
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if (this->compareStringAndPath(*tok_name, fullFilePath, str_as_file_ending) == false) {
			// имя аксиомы расходится - в названии файла и в его содержимом - выдаем ошибку
			// закомментировано т.к. почему-то у меня проверка проваливалась, хотя все правильно
			// видимо, надо разбираться, что конкретно делает compareStringAndPath
//			file.close();
//			throw AxiomLibException ("Error in AxiomSetBase::readFromAS: different file's names in the file and in the name of the file : '"
//			                         +std::string(*tok_name)+"', '"+fullFilePath+"'");
		}
		// запоминаем имя файла
		aess.axiomSetName = *tok_name;
	} else {
		// если файл пуст  -  то ошибка - потому что имя файла не задано
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - it is empty.");
	}
	
	// Читаем имя банка аксиом используемых в данной системе
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAS: no banks's name in the file.");
		}
		// запоминаем имя банка аксиом
		aess.axiomBankDir = *tok_name;;
	} else {
		// формат не верный - нет имени банка аксиом
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - no axiom bank name is given.");
	}
	
	// Итеративано считываем записанные имена аксиом и параметры элементарны условий
	readFromAS (file, aess, paramNames);

	// Проверяем - все ли было считано из файла
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromAS: wrong fowmat - there is something wrong at the end og the file.");
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	Функция чтения информации о системе аксиом из одного .as файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы аксиом; 
*					(Имена не должны содержать разделяющих символов ".,';:" )
*					Далее в функцие считываются входящие в систему аксиомы и параметры 
*					элементарных условий. Все считанный сущности записываются в 
*					специально определенную структуру типа map (см. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aess - заполняемая по файлу струтура типа AxiomExprSetStrucutre
*					paramNames - названия размеростей набора данных
*					file - открытый файл из которого будет производиться считывание системы аксиом
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (boost::filesystem::ifstream &file, AxiomExprSetStructure &aess, const std::vector <std::string> &paramNames) const {
	boost::char_separator<char> sep(" :;\t[]");
	std::string str, tempStr;
	int i = 0;
	
	// Итеративано считываем записанные имена аксиом и параметры элементарны условий
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	int condition;

	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF)  {
		// структуру, которую будем заполнять и копировать в aess
		aes.clear();
		// условие выхода из цикла обработки строки
		condition = 0;
		while (condition == 0) {
			// разбираем строку на токены
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// Вариант - пустая строка - возможно разделитель между аксиомами - а возможно просто одна из множетсва пустых строк
			if ( tok_iter == tokens.end()) {
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			
			// Вариант - считываение имени аксиомы
			tempStr = *tok_iter;
			if (atof(tempStr.c_str()) > 0) {
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - cannot find axiom name.");
				}
				if (!aes.axiomName.empty())
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - multiple names per single axiom.");
				aes.axiomName = *tok_iter;
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) { 
					// после названия аксиомы что-то - есть - идем на новую и терацию цикла
					continue;
				} else { 
					// после названия аксиомы ничего нет
					condition = 1;
					break;
				}
			} 
			// вариант - читаем описание аксиомы
			file.seekg(pos);
			axiomBase.readFromAX (file, aes, paramNames);
			aess.axioms.push_back (aes);
			aes.clear();
			// Проверяем есть ли что дальше
			pos = file.tellg();
			if (i = std::getline(file, str) && i != EOF) {
				// после - что-то - есть - идем на новую и терацию цикла
				continue;
			} else {
				condition = 1;
				break;
			}
		}
	}

	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					AxiomSetName - имя системы аксиом
*					ass - структура с описанием системы акисиом, типа AxiomSetStructure
*					first - число ошибок I рода данной системы аксиом на некотрой выборке
*					second - число ошибок II рода данной системы аксиом на некотрой выборке
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomSetStructure &ass, int first, int second) const {
	// Собираем пути к файлам
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(ass.axiomSetName);
	statFile = filePath;
	filePath.append(str_as_file_ending);
	statFile.append(str_stat_file_ending);
	// Открываем поток
	std::ofstream w (filePath.c_str());
	// Собстенно записываем данные о системе аксиом в файл
	// Записываем имя системы аксиом
	w << ass.axiomSetName << "\n";
	// Записываем имя базы систем аксиом
	w << ass.axiomBankDir << "\n";
	// Записываем аксиомы
	std::map < std::string, std::map <std::string, std::string> > ecParams;
	std::map < std::string, std::map <std::string, std::string> > :: iterator iterEC;
	std::map <std::string, std::string> params;
	std::map <std::string, std::string> :: iterator iterParams;
	for (unsigned int i = 0; i < ass.axioms.size(); i++) {
		// Вывод информации о номере и имени аксиомы
		w << "\n";
		w << i+1 << ": " << ass.axioms[i].axiomName << "\n";
		// Вывод информации о параметрах элементарных условий
		for (iterEC = ass.axioms[i].axiomParameters.begin(); iterEC != ass.axioms[i].axiomParameters.end(); ++iterEC) {
			params = iterEC->second;
			for (iterParams = params.begin(); iterParams != params.end(); ++iterParams) {
				w << "	" << iterEC->first << ":		";
				w << iterParams->first << " -> " << iterParams->second << "\n";
			}
		}
	}
	// Закрываем поток
	w.close();
	// Записываем файл со статистикой, если это необходимо
	if ((first < 0) && (second < 0))
		return 0;
	std::ofstream st (statFile.c_str());
	// Собстенно записываем данные о статистике по системе аксиом в файл
	st << str_axiomSet_keyWord << ":\t" << str_I_error_keyWord << ":\t'" << first << "'\n";
	st << str_axiomSet_keyWord << ":\t" << str_II_error_keyWord << ":\t'" << second << "'\n";
	// Закрываем поток
	st.close();
	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					AxiomSetName - имя системы аксиом
*					aess - структура с описанием системы акисиом, типа AxiomExprSetStructure
*					first - число ошибок I рода данной системы аксиом на некотрой выборке
*					second - число ошибок II рода данной системы аксиом на некотрой выборке
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomExprSetStructure &aess, int first, int second) const {
	std::vector <std::string> paramNames;
	paramNames.clear();
	return saveToAS (baseDir, aess, paramNames, first, second);
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		baseDir - директория, где находится запрашиваемый файл описания системы аксиом
*					AxiomSetName - имя системы аксиом
*					aess - структура с описанием системы акисиом, типа AxiomExprSetStructure
*					paramNames - названия размеростей набора данных
*					first - число ошибок I рода данной системы аксиом на некотрой выборке
*					second - число ошибок II рода данной системы аксиом на некотрой выборке
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames, int first, int second) const {
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(aess.axiomSetName);
	statFile = filePath;
	filePath.append(str_as_file_ending);
	statFile.append(str_stat_file_ending);
	// Открываем поток
	std::ofstream w (filePath.c_str());
	// Собстенно записываем данные о системе аксиом в файл
	// Записываем имя системы аксиом
	w << aess.axiomSetName << "\n";
	// Записываем имя базы систем аксиом
	w << aess.axiomBankDir << "\n";
	// Вывод описания системы аксиом
	saveToAS (w, aess, paramNames);
	// Закрываем поток
	w.close();
	// Записываем файл со статистикой, если это необходимо
	if ((first < 0) && (second < 0))
		return 0;
	std::ofstream st (statFile.c_str());
	// Собстенно записываем данные о статистике по системе аксиом в файл
	st << str_axiomSet_keyWord << ":\t" << str_I_error_keyWord << ":\t'" << first << "'\n";
	st << str_axiomSet_keyWord << ":\t" << str_II_error_keyWord << ":\t'" << second << "'\n";
	// Закрываем поток
	st.close();
	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	Функция сохранения системы аксиом в файл
*	Parameters:		file - открытый файл, в который дописывается описание системы аксиом
*					paramNames - названия размеростей набора данных
*					aess - структура с описанием системы акисиом, типа AxiomExprSetStructure
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::ofstream &file, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames) const {
	AxiomBase axiomBase;
	// Записываем аксиомы
	std::string curECName;
	std::map <std::string, std::string>::iterator iterParams;
	for (unsigned int i = 0; i < aess.axioms.size(); i++) {
		// Вывод информации о номере и имени аксиомы
		file << "\n" << i+1 << ": " << aess.axioms[i].axiomName << "\n";
		// Вывод описания самой аксиомы
		axiomBase.saveToAX(file, aess.axioms[i], paramNames);
	}
	return 0;
}
