/****************************************************************************
*				AxiomBase.cxx
*
*	Description:	Функции класса AxiomBase - Реализация
*	Author:		dk
*	History:		
*
****************************************************************************/
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "AxiomBase.h"

using namespace AxiomLib;

// Ключевые слова для записи файла с описанием системы аксиом: 
#define str_not_symbol			"not"
#define str_or_symbol			"or"
#define str_ax_file_ending		".ax"
#define str_ec_file_ending		".ec"
#define str_list_file_ending	".list"


// Пустой конструктор
AxiomBase::AxiomBase (void) {
}


// Функция возвращает число  аксиом в считанном множестве
int AxiomBase::size (void) const {
	return (int) axioms.size();
}


/****************************************************************************
*					AxiomBase::compareStringAndPath
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
bool AxiomBase::compareStringAndPath (std::string fileName, std::string path, const char* n3) const {
	
	   std::string n1("/");
	   const char* n2 = fileName.c_str();
	   n1.append(n2);
	   n1.append(n3);
	   return checkName (path, (int) (path.size() - n1.size() - 1), (int) (path.size()  - n1.size()), n1);

}


/****************************************************************************
*					AxiomBase::checkName
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
#define maxAddrLen 1000
bool AxiomBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) const {
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
*					AxiomBase::readFromAX
*
*	Description:	Функция обработки .ax файлов
*				(Вспомогательная функция к readAxioms)
*				Функция считывает информацию из файла описания аксиомы
*				и записывает полчученную информацию в специально 
*				определенную структуру - отображение по имени аксиомы в 
*				вектор элементарных условий. составляющих данную аксиому
*	Parameters:	filePath - путь к обрабатываему файлу описания аксиомы
*				vec - заполняемый вектор элементарных условий составляющих 
*					данную аксиому
*	Returns:	0 - если файл был считан и корректно разобран
*				-1 - если файл не был считан
*	Throws:		AxiomLibException  - если файл с заданным путем не найден 
*								если по этому пути находится каталог
*								если формат файла не соответствует 
*								описанию форматов (см. formats.x)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (std::string filePath, std::vector<std::string> &vec) {
	std::string str;
	int i = 0;
	int flagAND = 0;
	
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromAX: cannot find given .ax file.");
		return 1;
	}
	
	vec.resize(0);
	
	// Открываем файл
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" .,:;	");
	
	if (i = std::getline(file, str) && i != EOF) {
		// читаем имя аксиомы и сравниваем с названием файла
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (this->compareStringAndPath(*tok_name, filePath, ".ax") == false) {
			// имя аксиомы расходится - в названии файла и в его содержимом - выдаем ошибку
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		
		// итеративано считываем записанные имена элементарных условий в файле 
		while (i = std::getline(file, str) && i != EOF)  {
			if (!str.empty()) {
				token_separator tokens(str, sep);
				for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
					if (flagAND == 0) { // ждем очередное элементарое условие
						vec.push_back(*tok_iter);
						flagAND = 1;
					}
					else { // ждем AND - и если нет - то оишбка в формате файла аксиомы
						if (*tok_iter != "AND") {
							// файл содержит неверное или более сложное описание аксиомы - поэтому пропускаем его
							file.close();
							vec.clear();
							return -1;
							//throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format in axiom file .");
						}
						flagAND = 0;
					}
				}
			}
 		}
	}
	
	file.close();
	
	if ((flagAND == 0) && (vec.size() > 0)) { // была считано несколько элементарных условий - но на конце стоит AND - это ошибка формата
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format in axiom file .");
	}
	
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromX
*
*	Description:	Функция обработки .x файла
*				Функция считывает имя банка аксиом и сравнивает с именем данного файла
*				(Вспомогательная функция к readAxioms)
*	Parameters:	filePath - путь к обрабатываему .x файлу
*				strName - заполняемый параметр - именем банка аксиом - считывается из файла
*	Returns:		0
*	Throws:		AxiomLibException  - если обрабатываемго файла нет - то есть путь не верный
*								если файл пуст
*								если имя банка аксиом, записанное в файле отчличается 
*								от имени этого файла
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromX (std::string filePath, std::string &strName) {
	std::string str;
	int i = 0;
	
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromX: cannot find given .x file.");
	}
	
	// Открываем файл
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" .,:;	");
	while (i = std::getline(file, str) && i != EOF) {
		// просто читаем файл пока не найдем первую не пустую сторку
		if ( str != "\n" ) {
			break;
		}
	}
	if ((str.empty()) || (str == " ")) {
		throw AxiomLibException ("Error in AxiomBase::readFromX: name of axiom_base should be in .x file  .");
	}
	if (i != EOF) {
		// читаем имя базы аксиом и сравниваем с названием файла
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if (this->compareStringAndPath(*tok_name, filePath, ".x") == false) {
			// имя базы аксиом расходится - в названии файла и в его содержимом - выдаем ошибку
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromX: different file's names in the file and in the name of the file .");
		}
		strName = *tok_name;
	}
	else {
		file.close();
		throw AxiomLibException ("Error in AxiomBase::readFromX: given .x file is empty - there should be an axiom_base_name.");	
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readAxioms
*
*	Description:	Функция чтения базы аксиом.
*				Функция получает из входного параметра информацию 
*				о месте (пути к каталогу) хранения базы аксиом. Далее функция 
*				в цикле ходит файлам каталога - и сравнивает имя файлов 
*				на удолвлетворение шаблонам. Псоле отбора файлов - считыватся
*				аксиомы, описанные в файлах.
*	Parameters:	axiomBaseDir - строка с путем к каталогу базы аксиом
*	Returns:		0
*	Throws:		AxiomLiException  - 	если не нашлось каталога, указанного в 
*								конфигурационном файле как банк аксиом
*								если заданный путь к этому каталогу - путь к файлу
*								если в каталоге не содержится файла .x с указанием
*								имени банка аксиом (либо таких файлов более одного)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readAxioms (const char* axiomBaseDir) {
	std::string axiomDir (axiomBaseDir);
	boost::filesystem::path fullPath( axiomDir );
	boost::filesystem::path filePath;
	
	std::string templateAxiomFile (".ax"); // шаблон1 расширение файла с описание аксиомы
	std::string templateBankFile (".x"); // шаблон2 расширение файла с именем банка аксиом
	
	std::string aa; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	std::string str;
	int i = 0;
	
	// Проверка - существует ли каталог с банком аксиом
	if ( !boost::filesystem::exists( fullPath  ) ) {
		throw AxiomLibException("Error in AxiomBase::readAxioms: cannot find base axiom directory "
								"'" + axiomDir + "'");
		return 1;
	}
	
	// Проверяем - директория ли данный путь - как предполагается к основному какталогу банка аксиом
	if ( boost::filesystem::is_directory( fullPath ) ) {
		int numOfXFiles = 0;
		this->axioms.clear();
		// итеративано проходим директорию - обрабатываем все водходящие под шаблоны файлы
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( boost::filesystem::is_directory( *dir_itr ) ) { // внутри заданного каталога с банком аксиом - нашелся новый каталог - 
												// это не предуспотрено форматом - просто пропускаем этот каталог				
        		}
			else {
				filePath = fullPath;
				filePath /= *dir_itr;
				aa = dir_itr->leaf();
				if ( checkName (aa, (int) (aa.size() - 4), (int) (aa.size() - 3), templateAxiomFile) ) {
					// то есть в dir_itr сейчас имя файла описания аксиомы
					std::vector<std::string> vec;
					aa = (*dir_itr).string();
					this->readFromAX (aa, vec);
					if (vec.size() > 0) {
						aa = dir_itr->leaf();
						this->axioms[aa.substr(0, aa.size() - 3)] = vec;
					}
				}
				else {
					if ( checkName (aa, (int) aa.size() - 3, (int) aa.size() - 2, templateBankFile) ) {
						// то есть в dir_itr_int сейчас имя файла с названием банка аксиом
						if (numOfXFiles == 0) {
							std::string strName;
							aa = (*dir_itr).string();
							this->readFromX (aa, strName);
							this->name = strName;
							numOfXFiles++;
						}
						else {
							throw AxiomLibException("Error in AxiomBase::readAxioms: to many .x files.");
						}
					}
					else {
						// то есть в директории банка аксиом нашелся файл с не подходящим под шаблоный именем - его просто пропускаем
					}
				}
			}
		}
		if (numOfXFiles == 0) {// не было считано название базы аксиом - поэтому выход с ошибкой - нет нужного файла
			throw AxiomLibException ("Error in AxiomBase::readAxioms: cannot find a file with a name of the AxiomBase.");
		}
	}
	else {// Указанный путь не является каталогом - поэтому считывание эталонных рядов не произведено
		throw AxiomLibException ("Error in AxiomBase::readAxioms: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					AxiomBase::getVectorOfEC
*
*	Description:	Фунция выдает вектор элементарных условий соответсвующих 
*				запрошенной аксиоме, либо пустой вектор если не нашлось аксиомы 
*				с заданным именем
*	Parameters:	toReturn - возвращаемый вектор
*			axiomName - имя аксиомы, информацию о которой ищем
*	Returns:		0
*	Throws:		AxiomLiException  - 	если не нашлось каталога, указанного в 
*								конфигурационном файле как банк аксиом
*								если заданный путь к этому каталогу - путь к файлу
*								если в каталоге не содержится файла .x с указанием
*								имени банка аксиом (либо таких файлов более одного)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::getVectorOfEC (std::vector<std::string>& toReturn, std::string axiomName) {
	toReturn.resize(0);
	if ((this->axioms).count(axiomName)) {
		toReturn = this->axioms[axiomName];
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	Функция чтения информации об аксиом из одного .ax файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы элементарных условий; 
*					(Имена не должны содержать разделяющих символов " ,';:[]" )
*					Далее в функцие считываются входящие в аксиом условия и их параметры.
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. AxiomExprStrucutre.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (const std::string & filePath, AxiomExprStructure &aes) const {
	std::vector <std::string> tmpParamNames;
	tmpParamNames.clear();
	return readFromAX(filePath, aes, tmpParamNames);
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	Функция чтения информации об аксиом из одного .ax файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы элементарных условий; 
*					(Имена не должны содержать разделяющих символов " ,';:[]" )
*					Далее в функцие считываются входящие в аксиом условия и их параметры.
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. AxiomExprStrucutre.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (const std::string &filePath, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	// дописываем разрешение к имени файла
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_ax_file_ending);
	
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromAX: cannot find given file with axiom-description.");
		return 1;
	}

	// Очищаем содержимое структуры с описанием аксиомы
	aes.clear();

	// Открываем файл
	boost::filesystem::ifstream file( fullFilePath );

	boost::char_separator<char> sep(" :;\t[],");
	std::string str;
	int i = 0;

	// Читаем имя аксиомы и сравниваем с названием файла
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (compareStringAndPath(*tok_name, fullFilePath, str_ax_file_ending) == false) {
			// имя аксиомы расходится - в названии файла и в его содержимом - выдаем ошибку
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		// запоминаем имя файла
		aes.axiomName = *tok_name;
	} else {
		// если файл пуст  -  то ошибка - потому что файл пуст
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - it is empty.");
	}
	
	// Читаем имя банка элементарных условий, используемых в аксиоме
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		// запоминаем имя банка аксиом
		aes.elemCondBankName = *tok_name;;
	} else {
		// формат неверный - нет имени используемого банка элементарных условий
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - it has no axioms.");
	}
	
	// Читаем описание аксиомы
	readFromAX (file, aes, paramNames);

	// Проверяем - все ли было считано из файла
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong fowmat - there is something wrong at the end og the file.");
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	Функция чтения информации об аксиом из одного .ax файла
*					Формат файла описан в .formats.x
*					В данной функцие считывается и сравнивается с инменем файла,
*					имя записанное в первой строке файла; Считывается и сохраняется 
*					имя используемой базы элементарных условий; 
*					(Имена не должны содержать разделяющих символов " ,';:[]" )
*					Далее в функцие считываются входящие в аксиом условия и их параметры.
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. AxiomExprStrucutre.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					aes - заполняемая по файлу струтура типа AxiomExprStrucutre
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*					file - открытый файл из которого будет производиться считывание аксиомы
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*							если имя файла не согласовано с именем в первой строке файла
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (boost::filesystem::ifstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {	
	boost::char_separator<char> sep(" :;\t[],");
	std::string currentParam;
	std::string currentEC, curStr, currentDimen;
	std::vector <ElemCondPlusStruture> andECs;
	ElemCondPlusStruture ecPlusStruture;
	int toSave, condition;
	std::string str, tempStr;
	int i = 0;
	// Итеративано считываем записанные имена элементарных условий и их параметры
	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF) {
		// структуру, которую будем заполнять и копировать в aes
		ecPlusStruture.clear();
		// временные параметры для хранения имени аксиомы и имени параметра аксиомы
		andECs.clear();
		// условие выхода из цикла обработки строки
		condition = 0;
		toSave = 0;
		while (condition == 0) {
			// разбираем строку на токены
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// вариант - считывание пустой строки
			if ( tok_iter == tokens.end() ) {
				// пустая строка  - возможно разделитель между аксиомами - 
				// а возможно просто одна из множетсва пустых строк
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			// вариант - считываение имени аксиомы
			//const char* tempNumber = (*tok_iter).c_str();
			tempStr = *tok_iter;
			if (atof(tempStr.c_str()) > 0) {
				// Читаем название аксиомы
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAX: wrong format of axiom-description - cannot find axiom name.");
				}
				if (!aes.axiomName.empty()) {
					// закончили считывать аксиому - и перешли к чтению следующей - возвращаем указатель и выходим из функции
					file.seekg(pos);
					condition = 1;
					break;
				}
				aes.axiomName = *tok_iter;
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) { // после названия аксиомы что-то - есть - идем на новую и терацию цикла
					continue;
				} else { // после названия аксиомы ничего нет
					condition = 1;
					break;
				}
			}
			// вариант - считываем символ дизъюнкции в описании структуры аксиомы
			if ( (*tok_iter) == str_or_symbol) {
				// сохраняем описание данного ЭУ и переходим к считыванию следующего
				if (!andECs.empty()) {
					aes.elemConds.push_back (andECs);
					andECs.clear();
				}
				toSave = 0; // указываем, что текущий набор элементарных условий сохранили в аксиоме
				++tok_iter;
				if ( tok_iter != tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - after there is some else in the string, beginning with 'or'.");
				}
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					// после - что-то - есть - идем на новую и терацию цикла
					continue;
				} else {
					throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - there is nothing after the string, beginning with 'or'.");
				}
			} 
			// вариант - читаем элементарное условие, его параметры и значения
			file.seekg(pos);
			readFromEC(file, ecPlusStruture, paramNames);
			// Сохраняем считанную аксиому
			andECs.push_back (ecPlusStruture);
			ecPlusStruture.clear();
			toSave = 1;
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
		// сохраняем- если есть не сораненные считанные элементарные условия
		if (toSave > 0) {
			aes.elemConds.push_back (andECs);
		}
	}
	
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	Функция сохранения аксиомы в файл
*	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
*					AxiomName - имя аксиомы
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (const std::string & baseDir, AxiomExprStructure &aes) const {
	std::vector <std::string> tmpParamNames;
	tmpParamNames.clear();
	return saveToAX (baseDir, aes, tmpParamNames);
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	Функция сохранения аксиомы в файл
*	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
*					AxiomName - имя аксиомы
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (const std::string & baseDir, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(aes.axiomName);
	filePath.append(str_ax_file_ending);
	// Открываем поток
	std::ofstream w (filePath.c_str());
	// Собстенно записываем данные об аксиоме в файл
	// Записываем имя аксиомы
	w << aes.axiomName << "\n";
	// Записываем имя базы систем аксиом
	w << aes.elemCondBankName << "\n";
	// Записываем элементарные условия
	saveToAX (w, aes, paramNames);
	// Закрываем поток
	w.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	Функция сохранения аксиомы в файл
*	Parameters:		baseDir - директория, где находится записываемый файл описания аксиомы
*					AxiomName - имя аксиомы
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*					file - открытый файл, в который дописывается описание аксиомы
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файла не нашлось
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (std::ofstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	std::string emptyStr;
	emptyStr.clear();
	// Записываем элементарные условия
	for (unsigned int iterExprI = 0; iterExprI < aes.elemConds.size(); iterExprI++) {
		if (iterExprI > 0) {
			// Значит данная аксиома содержит знак дизъюнкции в своем выражении элементарных условий
			file << "\t" << str_or_symbol << "\n";
		}
		for (unsigned int iterExprII = 0; iterExprII < aes.elemConds[iterExprI].size(); iterExprII++) {
			if ((aes.elemConds[iterExprI][iterExprII].dimension >=0) && (aes.elemConds[iterExprI][iterExprII].dimension < (int) paramNames.size()))
				saveToEC(file, aes.elemConds[iterExprI][iterExprII], paramNames[aes.elemConds[iterExprI][iterExprII].dimension]);
			else
				saveToEC(file, aes.elemConds[iterExprI][iterExprII], emptyStr);			
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	Функция чтения параметров элементарного условия из одного .ec файла
*					Формат файла описан в .formats.x
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. ElemCondPlusStruture.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					ecps - заполняемая по файлу струтура типа ElemCondPlusStruture
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (const std::string & filePath, ElemCondPlusStruture &ecps) const {
	std::vector<std::string> paramNames;
	return readFromEC(filePath, ecps, paramNames);
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	Функция чтения параметров элементарного условия из одного .ec файла
*					Формат файла описан в .formats.x
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. ElemCondPlusStruture.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					ecps - заполняемая по файлу струтура типа ElemCondDataDimensionStruture
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (const std::string &filePath, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const {
	// Дописываем к файлу расширение
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_ec_file_ending);

	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromEC: cannot find given .ec file.");
		return 1;
	}

	// Очищаем содержимое структуры с описанием аксиомы
	ecps.clear();
	
	// Открываем файл
	boost::filesystem::ifstream file( fullFilePath );

	try {
	// Читаем содержимое файла
	readFromEC (file, ecps, paramNames);
	} catch(AxiomLibException e) {
		// Таким образом в информацию об ошибке добавляется имя файла
		throw AxiomLibException(e.error() + std::string(" Filename : '") + fullFilePath + "'");
	}

	// Проверяем - все ли было считано из файла
	std::string str;
	int i = 0;
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong fowmat - there is something wrong at the end of the file.");
	}

	// закрываем обработанный файл
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	Функция чтения параметров элементарного условия из одного .ec файла
*					Формат файла описан в .formats.x
*					Все считанный сущности записываются в специально определенную 
*					структуру, см. ElemCondPlusStruture.h.
*	Parameters:		filePath - имя файла для обработки (путь к фйлу)
*					ecps - заполняемая по файлу струтура типа ElemCondDataDimensionStruture
*					paramNames - список параметров набора данных, т.е. названия размерностей набора данных
*					file - открытый файл из которого будет производиться считывание описания элементарного условия
*	Returns:		0
*	Throws:			AxiomLibException  - если файл с заданным путем не существует
*							если формат файла не соглаосван описанием формата (см. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (boost::filesystem::ifstream &file, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const {
	boost::char_separator<char> sep(" :;\t[],");
	std::string currentParam, currentDimen, curStr;
	currentDimen.clear();
	int condition;
	bool curSign;
	std::string str;
	int i = 0;
	// Итеративано считываем записанные имена элементарных условий и их параметры
	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF)  {
		// временные параметры для хранения имени аксиомы и имени параметра аксиомы
		currentParam.clear();
		// условие выхода из цикла обработки строки
		condition = 0;
		while (condition == 0) {
			// разбираем строку на токены
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// вариант - считывание пустой строки
			if ( tok_iter == tokens.end() ) {
				// пустая строка  - возможно разделитель между аксиомами, а возможно просто одна из множетсва пустых строк
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				}
				else {
					condition = 1;
					break;
				}
			}
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			// вариант - считываем символ дизъюнкции в описании структуры аксиомы
			if ((*tok_iter) == str_or_symbol) {
				file.seekg(pos);
				condition = 1;
				break;
			}
			// вариант - читаем элементарное условие, его параметры и значения
			curSign = true;
			if (*tok_iter == str_not_symbol) {
				
				//std::cout<<"Current token is negation"<<std::endl;
				
				//значит перед название ЭУ идет отрицание - запоминаем это
				curSign = false;
				do {
					++tok_iter;
				} while(*tok_iter == str_not_symbol);
				
				//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
				
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find name of elementary condition.");
				}
			}
			if (*tok_iter == ecps.ecName) {
				//значит продолжаем заполнения параметров данного элементарного условия
			} else {
				if (!ecps.ecName.empty()) {
					file.seekg(pos);
					condition = 1;
					break;
				}
				ecps.ecName = *tok_iter;
				ecps.sign = curSign;
			}
			++tok_iter;
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find name of parameter.");
			}
			// Считали название размерности множества данных или название параметра элементарного условия
			curStr = *tok_iter;
			++tok_iter;
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find -> in .ec file while setting parameters.");
			}
			if (*tok_iter == "->") {
				// значит считанная ранее строка - была параметром элементарного условия
				currentParam = curStr;
			} else {
				// значит ранее считали размерность набора данных
				if (curStr == currentDimen) {
					//значит продолжаем заполнения параметров данного элементарного условия
				} else {
					if (!currentDimen.empty()) {
						// Значит началось изложение нового условия, которое имеет тоже название, но определено для другой размерности данных
						file.seekg(pos);
						condition = 1;
						break;
						// ошибка чтения из файла - неверная структура
						//throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong format of elementary condition - different dimension.");
					}
					currentDimen = curStr;
					for (unsigned int u = 0; u < paramNames.size(); u++) {
						if (currentDimen == paramNames[u]) {
							ecps.dimension = u;
							break;
						}
					}
				}
				// сохраняем название параметра элементарного условия
				currentParam = *tok_iter;
				// считываем знаки '->'
				++tok_iter;
				
				//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
				
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find -> in .ec file while setting parameters.");
				}
				// считали символы '->'
				if (*tok_iter != "->") {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong format -- expected '->', got '"+std::string(*tok_iter)+"'");
				}				
			}
			// считываем значение текущего параметра
			++tok_iter;
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find a value of parameter.");
			}
			// считали значение текущего параметра
			ecps.ecParams[currentParam] = *tok_iter;
			// если есть что-то еще - идем на новую и итерацию цикла
			pos = file.tellg();
			if (i = std::getline(file, str) && i != EOF) {
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
*					AxiomBase::saveToEC
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		filePath - полный путь к записываемому файлу с параметрами 
*					ecps - структура типа ElemCondPlusStruture с содержанием 
*						   параметров элементарного условия
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файл не удалось записать
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps) const {
	std::string paramName;
	paramName.clear();
	return saveToEC (baseDir, fileName, ecps, paramName);
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		filePath - полный путь к записываемому файлу с параметрами 
*					ecps - структура типа ElemCondPlusStruture с содержанием 
*						   параметров элементарного условия
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файл не удалось записать
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string & baseDir, const std::string & fileName, ElemCondPlusStruture &ecps, const std::vector <std::string> &paramNames) const {
	if ((ecps.dimension >= 0) && (ecps.dimension < (int) paramNames.size()))
		return saveToEC (baseDir, fileName, ecps, paramNames[ecps.dimension]);
	std::string paramName;
	paramName.clear();
	return saveToEC (baseDir, fileName, ecps, paramName);
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		filePath - полный путь к записываемому файлу с параметрами 
*					ecps - структура типа ElemCondPlusStruture с содержанием 
*						   параметров элементарного условия
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файл не удалось записать
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string & baseDir, const std::string & fileName, ElemCondPlusStruture &ecps, const std::string &paramName) const {
	// Создаем путь и дописываем расширение к файлу
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_ec_file_ending);
	// Открываем поток
	std::ofstream w (filePath.c_str());
	// Собстенно записываем данные об аксиоме в файл
	saveToEC (w, ecps, paramName);
	// Закрываем поток
	w.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	Функция сохранения параметров элементарного условия в файл
*	Parameters:		filePath - полный путь к записываемому файлу с параметрами 
*					ecps - структура типа ElemCondPlusStruture с содержанием 
*						   параметров элементарного условия
*					file - открытый файл, в который дописывается описание элементарного условия
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			AxiomLibExeption - если файл не удалось записать
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (std::ofstream &file, ElemCondPlusStruture &ecps, const std::string &paramName) const {
	// Собстенно записываем данные об аксиоме в файл
	std::map <std::string, std::string>::iterator iterParams;
	// Вывод информации о параметрах элементарного условия
	for (iterParams = ecps.ecParams.begin(); iterParams != ecps.ecParams.end(); ++iterParams) {
		// Подписываем обозначение отрицания элементарного условия
		if (!(ecps.sign))
			file << "\t" << str_not_symbol << ": \t";
		else
			file << "\t\t\t";
		if (paramName.size() > 0)
			file << "[" << ecps.ecName << ", " << paramName << "]:\t\t";
		else
			file << ecps.ecName << ":\t\t";
		file << iterParams->first << " -> " << iterParams->second << "\n";
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToList
*
*	Description:	Функция записи в файл списка с названиями файлов
*	Parameters:		baseDir - директория, где находится записываемый файл
*					fileName - имя файла
*					strList - структура с сохраняемымим данными
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToList (const std::string & baseDir, const std::string & fileName, const std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const {
	// Создаем путь и дописываем расширение к файлу
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_list_file_ending);
	// Открываем поток
	std::ofstream file (filePath.c_str());
	// Собстенно записываем данные об аксиоме в файл
	for (unsigned int i = 0; i < strList.size(); i++) {
		for (unsigned int j = 0; j < strList[i].size(); j++) {
			for (unsigned int k = 0; k < strList[i][j].size(); k++) {
				for (unsigned int l = 0; l < strList[i][j][k].size(); l++) {
					file << i+1 << ".\t" << j+1 << ".\t" << k+1 << ".\t" << l+1 << ".\t";
					file << strList[i][j][k][l] << "\n";
				}
				file << "\n";
			}
			file << "\n";
		}
		file << "\n";
	}
	// Закрываем поток
	file.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToList
*
*	Description:	Функция записи в файл списка с названиями файлов
*	Parameters:		baseDir - директория, где находится записываемый файл
*					fileName - имя файла
*					strList - структура с сохраняемымим данными
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToList (const std::string & baseDir, const std::string & fileName, const std::vector <std::vector <std::string> > &strList) const {
	// Создаем путь и дописываем расширение к файлу
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_list_file_ending);
	// Открываем поток
	std::ofstream file (filePath.c_str());
	// Собстенно записываем данные об аксиоме в файл
	for (unsigned int i = 0; i < strList.size(); i++) {
		for (unsigned int j = 0; j < strList[i].size(); j++) {
			file << i+1 << ".\t" << j+1 << ".\t";
			file << strList[i][j] << "\n";
		}
		file << "\n";
	}
	// Закрываем поток
	file.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromList
*
*	Description:	Чтение из файла со списком названий файлов
*	Parameters:		baseDir - директория, где находится записываемый файл
*					fileName - имя файла
*					strList - структура, в которую считываются данные
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			-
*	Author:			dk
*	History:
*

****************************************************************************/
signed int AxiomBase::readFromList (const std::string & filePath, std::vector <std::vector <std::string> > &strList) const {
	// Дописываем к файлу расширение
	std::string fullFilePath;
	fullFilePath.assign (filePath);
	checkedAddExtension(fullFilePath, str_list_file_ending);
	//fullFilePath.append (str_list_file_ending);
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromList: cannot find file at '" + fullFilePath + "'");
		return 1;
	}
	// Очищаем содержимое структуры с описанием аксиомы
	strList.clear();
	// Открываем файл
	boost::filesystem::ifstream file( fullFilePath );
	// Читаем содержимое файла

	std::string curStr, str;
	int i = 0;
	int curLev1, curLev2, lev1, lev2, ret;
	char s[1001]; // максимальная допускаемая длина имени файла с описанием структуры
	// Итеративано считываем записанные имена элементарных условий и их параметры
	// Подготавливаем структуру для записи в нее имен
	strList.resize(1);
	lev1 = 0;
	strList[0].clear();
	lev2 = 0;
	// Читаем из файла по одной строке
	while (i = std::getline(file, str) && i != EOF)  {
		// Разбираем строку на составляющие
		ret = sscanf (str.c_str(), "%d.\t%d.\t%1000s", &curLev1, &curLev2, s);
		// Проверяем сколько элементов удалось выделить
		if (ret < 1) {
			continue;
		}
		if ((ret > 0) && (ret != 3)) {
			std::cout << "\n\tWarning: string '" << str << "' was not correctly recognized.\n";
			continue;
		}
		// Проверяем последовательность нумерации строк в файле
		if (curLev1 == (lev1 + 2)) {
			strList.resize(lev1 + 2);
			lev1++;
			strList[lev1].clear();
			lev2 = 0;
		}
		if (curLev1 != (lev1+1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev2 != (lev2+1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		lev2++;
		// Сохраняем считнную строку
		strList[lev1].push_back ((std::string) s);
	}
	// Очищаем содержимое последнего добавленного измерения, так как оно может быть пусто
	if (strList[lev1].empty()) {
		strList.pop_back();
	}
	
	// закрываем обработанный файл
	file.close();	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromList
*
*	Description:	Чтение из файла со списком названий файлов
*	Parameters:		baseDir - директория, где находится записываемый файл
*					fileName - имя файла
*					strList - структура, в которую считываются данные
*	Returns:		0 - если все нормально, в противном случае выход по throw
*	Throws:			-
*	Author:			dk
*	History:
*

****************************************************************************/
signed int AxiomBase::readFromList (const std::string & filePath, std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const {
	// Дописываем к файлу расширение
	std::string fullFilePath;
	fullFilePath.assign (filePath);
	//fullFilePath.append (str_list_file_ending);
	checkedAddExtension(fullFilePath, str_list_file_ending);
	// Проверяем - существует ли файл
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromList: cannot find file at '"+fullFilePath+"'");
		return 1;
	}
	// Очищаем содержимое структуры с описанием аксиомы
	strList.clear();
	// Открываем файл
	boost::filesystem::ifstream file( fullFilePath );
	// Читаем содержимое файла

	std::string curStr, str;
	int i = 0;
	int curLev1, curLev2, curLev3, curLev4, lev1, lev2, lev3, lev4, ret;
	char s[1001]; // максимальная допускаемая длина имени файла с описанием структуры
	// Итеративано считываем записанные имена элементарных условий и их параметры
	// Подготавливаем структуру для записи в нее имен
	strList.resize(1);
	lev1 = 0;
	strList[0].resize(1);
	lev2 = 0;
	strList[0][0].resize(1);
	lev3 = 0;
	strList[0][0][0].clear();
	lev4 = 0;
	// Читаем из файла по одной строке
	while (i = std::getline(file, str) && i != EOF)  {
		// Разбираем строку на составляющие
		ret = sscanf (str.c_str(), "%d.\t%d.\t%d.\t%d.\t%1000s", &curLev1, &curLev2, &curLev3, &curLev4, s);
		// Проверяем сколько элементов удалось выделить
		if (ret < 1) {
			continue;
		}
		if (ret != 5) {
			std::cout << "\n\tWarning: string '" << str << "' was not correctly recognized.\n";
			continue;
		}
		// Проверяем последовательность нумерации строк в файле
		if (curLev1 == (lev1 + 2)) {
			strList.resize(lev1 + 2);
			lev1++;
			strList[lev1].resize(1);
			lev2 = 0;
			strList[lev1][0].resize(1);
			lev3 = 0;
			strList[lev1][0][0].clear();
			lev4 = 0;
		}
		if (curLev1 != (lev1 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev2 == (lev2 + 2)) {
			strList[lev1].resize(lev2 + 2);
			lev2++;
			strList[lev1][lev2].resize(1);
			lev3 = 0;
			strList[lev1][lev2][0].clear();
			lev4 = 0;
		}
		if (curLev2 != (lev2 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev3 == (lev3 + 2)) {
			strList[lev1][lev2].resize(lev3 + 2);
			lev3++;
			strList[lev1][lev2][lev3].clear();
			lev4 = 0;
		}
		if (curLev3 != (lev3 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev4 != (lev4 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		lev4++;
		// Сохраняем считнную строку
		strList[lev1][lev2][lev3].push_back ((std::string) s);
	}
	// Очищаем содержимое последнего добавленного измерения, так как оно может быть пусто
	if (strList[lev1][lev2][lev3].empty()) {
		strList[lev1][lev2].pop_back();
	}
	if (strList[lev1][lev2].empty()) {
		strList[lev1].pop_back();
	}
	if (strList[lev1].empty()) {
		strList.pop_back();
	}
	
	// закрываем обработанный файл
	file.close();	
	return 0;
}

bool AxiomBase::checkExtension(const std::string &fileName, const std::string &extension) {
	return boost::algorithm::iends_with(fileName, extension);
}

void AxiomBase::checkedAddExtension(std::string &fileName, const std::string &extension) {
	if(!checkExtension(fileName, extension)) {
		fileName.append(extension);
	}
}
