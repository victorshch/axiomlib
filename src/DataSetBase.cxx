#include <set>

#include "Common.h"

#include "DataSetBase.h"

using namespace std;

using namespace AxiomLib;

DataSetBase::DataSetBase()
{
	// Задание внутренних параметров класса по умолчанию
	std::string nm ("name");
	this->name = nm;
	std::string nl ("null");
	this->null = nl;
}

/****************************************************************************
*				DataSetBase::ReadParams
*
*	Description:	Функция чтения параметров класса DataSet - чтение названий 
*				классов нештатных ситуаций, а также названий параметров, 
*				по которым обрабатываются временные ряды.
*				Порядок следоавния названий классов нештаных ситуаций и 
*				названий параметров - важен - он принимается каноническим и 
*				к нему приводятся все считываемые временные ряды.
*	Parameters:	path - строка, содержащая имя освновной директории класса,
*				в которой хранятся файлы с названиями параметров и классов
*				неисправностей
*	Returns:		0
*	Throws:		AxiomLibExcption  - 	если хотя бы один из файлов не существует
*								(файлы содежащие имена классов неисправносте
*								и имена параметров)
*							     - 	если хотя бы один из файлов пустой
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSetBase::readParams(std::string path) {
	// Создаем структуры указания пути к файлам
	boost::filesystem::path classNamesFile(path);
	boost::filesystem::path paramNamesFile(path);
	
	// Файлы по умолчанию содежащие имена классов и рараметров
	classNamesFile /= "./class_names";
	paramNamesFile /= "./param_names";

	// Проверка существования файлов
	if ( !boost::filesystem::exists( classNamesFile ) ) {
		throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot find classNamesFile.");		
	}
	if ( !boost::filesystem::exists( paramNamesFile ) ) {
		throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot find paramNamesFile.");		
	}
	
	// Открываем файлы для обработки
	boost::filesystem::ifstream fileP( paramNamesFile );
	boost::filesystem::ifstream fileC( classNamesFile );
	
	std::string str; // Чтение из файлов производим построчкно и считвать будем в данную переменную
	//int i; // В данной переменной хранится код ошибки чтения из файла

	// Создаем сепаратор слов в строке с указанием разделителей
	// wictor: добавил \r
	boost::char_separator<char> sep(";:,.	\r");

	// Обработка файла, содержащего имена параметров, которым соответсвуют временные ряды
	this->paramNames.resize(0);
	while (!fileP.eof())  {
		std::getline(fileP, str);
		if (!str.empty()) {
			token_separator tokens(str, sep);
			for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				this->paramNames.push_back(*tok_iter);
			}
		}
	 }
	 // Проверка - удалось ли прочитать названия параметров
	 if (this->paramNames.size() == 0) {
	 	throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot read paramNamesFile.");
	 }
 	
	 // Обработка файла, содержащего имена классов неисправностей
	 this->classNames.resize(0);
	 while (!fileC.eof())  {
		getline(fileC, str);
		if (!str.empty()) {
			token_separator tokens(str, sep);
			for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				this->classNames.push_back(*tok_iter);
			}
		}	
	 }
	 // Провека - удалось ли прочитать названия классов
	 if (this->classNames.size() == 0) {
	 	throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot read classNamesFile.");
	 }
	 
	 // Обработка завершена - закрываем файлы и выходим
	fileP.close();
	fileC.close();

	return 0;	
}

/****************************************************************************
*				DataSetBase::ReadDataSet
*
*	Description:	Функция чтения всех данных из набора данных, находящегося
*				в указанном каталоге
*	Parameters:	const char* path - путь к каталогу, содержащему набор данных
*	Returns:		0
*	Throws:		AxiomLibExeption - если каталог отсутствует, или есть ошибки
*				в формате набора данных
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSetBase::readDataSet (const std::string &baseDataSetDir)
{
  // Общая схема работы функции:
  // 0. Проверяем корректность пути
  // 1. Читаем файл с названиями классов и заполняем массив названий классов;
  //    этот порядок классов объявляется каноническим
  // 2. Читаем файл с названиями параметров и заполняем массив названий параметров
  //    этот порядок параметров объявляется каноническим

  //0 - 1 - 2 пункты реализуются в следующей функции: 

	//std::cout << "\n" << "Base file is " << baseDataSetDir << "\n";
	
	this->readParams(baseDataSetDir);
	
	//std::cout << "Configuring params..." << std::endl;
	
	paramNums.clear();
	paramNums.reserve(paramNames.size());
	for (unsigned int i = 0; i < paramNames.size(); i++) {
		if (paramNames[i] != "time" ) {
			paramNums.push_back(i);
		}
	}	
	
  	return 0;
}

/****************************************************************************
*				DataSetBase::placeOfParam
*
*	Description:	Получаем информацию о положении параметра в каноническом 
*				представлении по его имени
*	Parameters:	parameter - имя параметра информация о месте которого в 
*						канонической системе запрашивается
*	Returns:		- 1 - код ошибки - если такого параметра нет
*				неотрицательное целое - номер параметра в каноническом порядке
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
int DataSetBase::placeOfParam(std::string parameter) {
	return isIn (parameter, paramNames);
}

/****************************************************************************
*				DataSetBase::paramName
*
*	Description:	Получаем информацию об имени параметра по его номеру в 
*					каноническом представлении
*	Parameters:		parameter - номер параметра в канонической системе
*	Returns:		"" - пустая строка, если парметра с указанным индеском не нашлось
*					std::string - имя параметра
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
std::string DataSetBase::paramName(int parameter) const {
	if ( (parameter >= 0) && (parameter < (int) paramNames.size()) ) 
		return paramNames[parameter];
	return (std::string) "";
}

/****************************************************************************
*				DataSetBase::className
*
*	Description:	Получаем информацию об имени аномального поведения по его 
*					номеру в каноническом представлении
*	Parameters:		parameter - номер ситуации аномального поведения в канонической системе
*	Returns:		"" - пустая строка, если парметра с указанным индеском не нашлось
*					std::string - имя параметра
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
std::string DataSetBase::className(int parameter) const {
	if ( (parameter >= 0) && (parameter < (int) classNames.size()) ) {
		return classNames[parameter];
	} else if (parameter == -1) {
		return "normal";
	}
	return (std::string) "";
}

std::vector<std::string> DataSetBase::getClassNames() const {
	return classNames;
}

/****************************************************************************
*				DataSetBase::getParamNames
*
*	Description:	Получаем информацию обо всех названиях параметров набора 
*					данных в их каноническом представлении
*	Parameters:		-
*	Returns:		std::vector <std::string> - все имена параметров
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
const std::vector <std::string>& DataSetBase::getParamNames (void) const {
//	std::vector <std::string> toReturn;
//	toReturn = paramNames;
//	return toReturn;
	return paramNames;
}

/****************************************************************************
*				DataSetBase::getParamNums
*
*	Description:	Функция по классам конфигураций создает вектор номеров параметров 
*				в каноническом представлении по которым ведется подбор системы аксиом
*	Parameters:	toReturn - заполняемый вектор
*				env - класс с данными из основного конфига
*				envDataSet - класс с данными из конфига набора данных
*	Returns:		std::vector<int> - заполняемый вектор номеров параметров для анализа
*	Throws:		AxiomLibException - если параметры с заданными в конфигу именами не нашлись в данном наборе данных
*	Author:		dk,gevor
*	History:		-
*
****************************************************************************/
int DataSetBase::getParamNums (std::vector<int>& toReturn, const Environment &env, EnvDataSet &envDataSet) {

	setupParamNums(env, envDataSet);
	
	toReturn = paramNums;
	
	return 0;
}

std::vector<int> DataSetBase::getParamNums() const {
	std::vector<int> result;
	getParamNums(result);
	return result;
}

int DataSetBase::getParamNums(std::vector<int> &toReturn) const {
	toReturn = paramNums;
	return 0;
}

void DataSetBase::setupParamNums(const Environment &env, const EnvDataSet &envDataSet) {
	// Попробуем получить все параметры из общего конфига
	set<string> paramSet;
	int ret = env.getStringSetParamValue(paramSet, "analizeParameter");

	// проверяем, нашлись ли они в общем конфиге
	if (ret >= 0) {
		// нашлись
		// заносим все параметры
		paramNums.resize(paramSet.size());
		int j = 0;
		for (set<string>::const_iterator i = paramSet.begin(); i != paramSet.end(); i++,j++) {
			string paramName = *i;
			int paramNum = placeOfParam(paramName);
			if (paramNum < 0) {
				throw AxiomLibException ("Error in DataSetBase::paramNums: given parameter's name do not exists in dataSet.");
			}
			paramNums[j] = paramNum;
		}
	}
	else { // значит не указано ни одного параметра в общем конфиге
		int num = envDataSet.getAnalizeNameSize();
		std::string parameter;
		if (num > 0) { // значит какие-то парметры указаны в параметрах набора данных
			paramNums.resize(num);
			for (int i = 0; i < num; i++) {
				parameter = envDataSet.getAnalizeName(i);
				//std::cout << "\n2) parameter name is " << parameter << "\n";
				paramNums[i] = placeOfParam(parameter); // получаем информацию о положении параметра в каноническом представлении
				if (paramNums[i] < 0) {
					throw AxiomLibException ("Error in DataSetBase::paramNums: given parameter's name do not exists in dataSet*.");
				}
			}
		}
		else {// нигде не задан ни один параметр - значит прогоняем алгоритм по всем рядам
			paramNums.clear();
			paramNums.reserve(paramNames.size());
			for (unsigned int i = 0; i < paramNames.size(); i++) {
				if (paramNames[i] != "time" ) {
					//std::cout << "\n3) parameter num is " << paramNames[i] << "\n";
					paramNums.push_back(i);
				}
			}
		}
	}
	// сортируем, чтобы индексы параметров были в каноническом порядке
	std::sort(paramNums.begin(), paramNums.end());
}

/****************************************************************************
*				DataSetBase::paramNamesSize
*
*	Description:	Получение информации о числе элементов в векторе названий параметров
*	Parameters:	-
*	Returns:		положительное целое - число элементов в векторе названий параметров
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
 int DataSetBase::paramNamesSize (void) const {
 	return (int) (this->paramNames).size();
 }

 /****************************************************************************
 *				DataSetBase::setNullStr
 *
 *	Description:	Функция устанавливает новое обозначение для отсутсвия какой либо траектории во временном ряду
 *	Parameters:	newNullStr - новое имя для обозначения null
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNullStr (const std::string newNullStr) {
	 //std::cout << "\n newNullStr is " << newNullStr << "\n";
	 this->null = newNullStr;
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::setNullStr
 *
 *	Description:	Функция устанавливает новое обозначение для отсутсвия какой либо траектории во временном ряду
 *	Parameters:	env - класс с данными из конфигурационного файла
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNullStr (EnvDataSet &envDataSet) {
	 int ret;
	 std::string nullStr;
	 ret = envDataSet.getNullStr(nullStr);
	 //std::cout << "\n nullStr is " << nullStr << "\n";
	 if (ret == 0) {
		 null = nullStr;
	 }
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::setNameStr
 *
 *	Description:	Функция устанавливает новое обозначение для имени столбца с разметкой траекторий
 *	Parameters:	newNameStr - новое имя для обозначения name
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNameStr (const std::string newNameStr) {
	 this->name = newNameStr;
	 return 0;
 }
 
 /****************************************************************************
 *					DataSetBase::checkName
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
 bool DataSetBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) {
	 const std::basic_string <char>::size_type maxAddrLen = 1000;
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
 *				DataSetBase::ReadFromCSV
 *
 *	Description:	Функция чтения данных из csv-файла. Читаются все данные файла и 
 *				записываются по заданному адресу в специально разработанную структуру MultiTS
 *	Parameters:	fileName - текстовая строка с именем файла из которого требуется считать данные
 *				multiTS - класс для записи считанных данных
 *	Returns:		0  - если все считалось нормально
 *				-1 - если в файле не содержится рядов описывающих поведение какого то из параметров, 
 *				указанного в конфигурационно файле.
 *	Throws:		AxiomLibException -  если указанного параметрах файла не содержится в текущей директории
 *								если файл задан не корректно, то есть к какой-то строке содержится 
 *								больше или меньше значений, чем описываемых параметров
 *								(формат файла временных рядов смотри в ./ formats.x)
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readFromCSV (std::string fileName, MultiTS &multiTS) {
	 // открываем файл для обраотки
	 boost::filesystem::ifstream file( fileName );
	 
	 //Проверка - существует ли файл
	 if ( !boost::filesystem::exists( fileName ) ) {
		 // Раз файла не нашлось - происходит throw AxiomLibException
		 throw AxiomLibException ("Error in DataSetBase::ReadFromCSV: given file does not  exists.");
	 }

	 multiTS.trajectoryName = fileName;
	 
	 // теперь знаем что файл существует
	 // Читаем первую строку файла, содержащую названия параметров, к которым относятся временные ряды
	 
	 std::string str; // чтение из файла будем производить построчно и читать будем в данную переменную
	 std::vector<int> mask; 	// запись в класс multiTS временных рядов произодтся в каноническом порядке
						 // чтобы поддержать этот порядок - заводим вектор чисел - соответсующий порядковый номер 
						 // параметра в файле в порядковый номер параметра в каноническом порядке
	 //int i; // в данную переменную будет записываться код ошибки при чтении строки из файла
	 int numToMask; // в данную переменную будет записываться результат поиска параметра с заданным именем в векторе 
				  // строк соответсвующем каноническому порядку параметров
	 
	 // создаем сепаратор - в параметрах задаем символы разделяющие в обрабатываемом файле разные слова и числа между собой
	 // стандарт csv допускает использование как ;, так и , в качестве разделителя
     boost::char_separator<char> sep(",;:	\t\r\n");
 
	 //считываем и обрабатываем первую строку файла - содежащуюю параметры
	 if (!file.eof())  {
		 getline(file, str);
		 int res = (int) str.size();
		 //std::cout << "\n str size is" << str.size() << "\n";
		 if (res) {
			 //подготавливаем класс для записи параметров и значений ряда
			 multiTS.validParams.resize ( this->paramNames.size() );
			 multiTS.data.resize( this->paramNames.size() );
			 mask.resize( this->paramNames.size() );
			 //разделяем первую прочитанную строку на слова
			 token_separator tokens(str, sep);
			 // Изначально заполняем вектор присутсвующих параметров пустым
			 for (unsigned int t = 0; t < multiTS.validParams.size(); t++) {
				 multiTS.validParams[t] = false;
			 }
			 int pointer = 0;
			 //заполняем вектор mask
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 numToMask = isIn(*tok_iter, paramNames); 
				 if (numToMask != -1) {
					 multiTS.validParams[numToMask] = true;
					 mask[pointer] = numToMask;
				 }
				 else {
					 mask[pointer] = -1;
				 }
				 pointer++;
			 }
		 }
	 }
	 else {
		 // обрабатываемый файл существует, но он пуст - при этом просто выходим из функции
		 //throw AxiomLibException ("Error in DataSetBase::readFromCSV : given .csv file is empty.");
		 file.close();
		 return 0;
	 }
 
	 // теперь знаем какие параметры отражены в данном файле - какие временные ряды даны - то есть заполнена переменная mask
	 // проверяем - есть ли среди рядов в файле описывающие параметры, содержащиеся в канонической системе параметров из конфигурационного файла
	 int numOfRows = 0;
	 for (unsigned int j = 0; j < multiTS.validParams.size(); j++) {
		 if (multiTS.validParams[j] == true) {
			 numOfRows++;
		 }
	 }
	 // если так получилось, что файл не относится к данной системе - к данному набору данных - то файл не считывается и идет возврат с кодом -1
	 if (numOfRows == 0) {//нечего читать из файла - поэтому выходим - и не забываем говорить - что ничего не прочитали - то есть код "-1"
		 //std::cout << "\n exit by zero num of rows \n";
		 file.close();
		 return -1;
	 }
	 //std::cout << "\n num of row is " << numOfRows << "\n";
	 // теперь все готово - есть маска чтения, есть файл откуда читать - и выяснено что считывать в multiTS
	 // тогда производим считываение: 
	 signed int lenOfTS = 0; // в данной переменной будет хранитсья число элементов в считываемых временных рядах
	 signed int pointer; // вспомогательная переменная, чтобы бегать по вектору mask
	 // Начинаем считывание рядов
	 while (!file.eof())  {
		 getline(file, str);
		 //std::cout << "\n i is " << i <<  "\n";
		 if (!str.empty()) {
			 lenOfTS++; 
			 // разбиваем считанную строку на отдельные элементы
			 token_separator tokens(str, sep);
			 pointer = 0;
			 // производим запись элементов относящихся к считываемым рядам в класс multiTS
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 while ((pointer  < numOfRows) && (mask[pointer] == -1)) {
					 pointer++;
				 }
				 //проверка на число элементов в строке - не больше ли их чем параметров
				 if (pointer < numOfRows) {
					 //std::cout << "	1 \n";
					 const char * curF = tok_iter->c_str();
					 (multiTS.data[mask[pointer]]).push_back (atof(curF));
				 }
				 else {
					 //std::cout << "	2 \n";
					 file.close();
					 throw AxiomLibException ("Error in DataSetBase::readFromCSV: too many values in one string in cvs file -  more then parameters");
				 }
				 pointer++;
			 }
			 // проверка на число элементов в строке - не меньше ли их чем параметров
			 if (pointer < numOfRows) {
				 file.close();
				 throw AxiomLibException ("Error in DataSetBase::readFromCSV: too small amount of values in cvs file -  less then parameters");
			 }
		 }
	 }
	 
	 // закрытие обработанного файла и выход из функции
	 file.close();
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::ReadFromRefCSV
 *
 *	Description:	Функция чтения данных из *-ref.csv файла. Читаются все данные файла и 
 *				записываются по заданному адресу в вектор корректной разметки
 *	Parameters:	fileName - текстовая строка с именем файла из которого требуется считать данные
 *				multiTS - вектор для считываемого ряда
 *	Returns:		0  - если все считалось нормально
 *				-1 - если в файле не содержится рядов описывающих поведение какого то из параметров, 
 *				указанного в конфигурационно файле.
 *	Throws:		AxiomLibException -  если указанного параметрах файла не содержится в текущей директории
 *								если файл задан не корректно, то есть к какой-то строке содержится 
 *								больше или меньше значений, чем описываемых параметров
 *								(формат файла временных рядов смотри в ./ formats.x)
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readFromRefCSV (std::string fileName, MultiTS &multiTS) {
	 // открываем файл для обраотки
	 boost::filesystem::ifstream file( fileName );
	 
	 //Проверка - существует ли файл
	 if ( !boost::filesystem::exists( fileName ) ) {
		 // Раз файла не нашлось - происходит throw AxiomLibException
		 throw AxiomLibException ("Error in DataSetBase::ReadFromRefCSV: given file do not  exists.");
	 }
	 
	 // теперь знаем что файл существует
	 // Читаем первую строку файла, содержащую названия параметров, к которым относятся временные ряды
	 
	 std::string str; // чтение из файла будем производить построчно и читать будем в данную переменную
	 std::vector<int> mask; 	// запись в класс multiTS временных рядов произодтся в каноническом порядке
						 // чтобы поддержать этот порядок - заводим вектор чисел - соответсующий порядковый номер 
						 // параметра в файле в порядковый номер параметра в каноническом порядке
	 //int i; // в данную переменную будет записываться код ошибки при чтении строки из файла
	 int numToMask; // в данную переменную будет записываться результат поиска параметра с заданным именем в векторе 
				  // строк соответсвующем каноническому порядку параметров
	 
	 // создаем сепаратор - в параметрах задаем символы разделяющие в обрабатываемом файле разные слова и числа между собой
     boost::char_separator<char> sep(";:	\r\n");
 
	 //считываем и обрабатываем первую строку файла - содежащуюю параметры
	 if (!file.eof())  {
		 getline(file, str);
		 int res = (int) str.size();
		 if (res) {
			 //подготавливаем класс для записи параметров и значений ряда
			 multiTS.validParams.resize ( this->paramNames.size() + 1);
			 multiTS.data.resize( this->paramNames.size() + 1);
			 mask.resize( this->paramNames.size() + 1);
			 //разделяем первую прочитанную строку на слова
			 token_separator tokens(str, sep);
			 // Изначально заполняем вектор присутсвующих параметров пустым
			 for (unsigned int t = 0; t < multiTS.validParams.size(); t++) {
				 multiTS.validParams[t] = false;
			 }
			 //заполняем вектор mask
			 int pointer = 0;
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 numToMask = isIn(*tok_iter, paramNames);
				 if (numToMask != -1) {
					 // Считанное название - название параметра ( в частности time )
					 multiTS.validParams[numToMask] = true;
					 mask[pointer] = numToMask;
				 }
				 else {
					 if ( *tok_iter == this->name ) {
						 // колонка с названием класса неисправностей на каждом из отсчетов
						 multiTS.validParams[this->paramNames.size()] = true;
						 mask[pointer] = (int) this->paramNames.size();
					 }
					 else {
						 // некоторая колонка - с неизвестными данными - ее считывание не происходит
						 mask[pointer] = -1;
					 }
				 }
				 pointer++;
			 }
		 }
	 }
	 else {
		 // обрабатываемый файл существует, но он пуст - при этом просто выходим из функции
		 //throw AxiomLibException ("Error in DataSetBase::readFromCSV : given .csv file is empty.");
		 file.close();
		 return 0;
	 }
 
	 // теперь знаем какие параметры отражены в данном файле - какие временные ряды даны - то есть заполнена переменная mask
	 // проверяем - есть ли среди рядов в файле описывающие параметры, содержащиеся в канонической системе параметров из конфигурационного файла
	 int numOfRows = 0;
	 for (unsigned int j = 0; j < multiTS.validParams.size(); j++) {
		 if (multiTS.validParams[j] == true) {
			 numOfRows++;
		 }
	 }
	 // если так получилось, что файл не относится к данной системе - к данному набору данных - то файл не считывается и идет возврат с кодом -1
	 if (numOfRows == 0) {//нечего читать из файла - поэтому выходим - и не забываем говорить - что ничего не прочитали - то есть код "-1"
		 file.close();
		 return -1;
	 }
 
	 // теперь все готово - есть маска чтения, есть файл откуда читать - и выяснено что считывать в multiTS
	 // тогда производим считываение: 
	 signed int lenOfTS = 0; // в данной переменной будет хранитсья число элементов в считываемых временных рядах
	 signed int pointer; // вспомогательная переменная, чтобы бегать по вектору mask
	 // Начинаем считывание рядов
	 int isin = -1;
	 while (!file.eof())  {
		 getline(file, str);
		 if (!str.empty()) {
			 lenOfTS++; 
			 // разбиваем считанную строку на отдельные элементы
			 token_separator tokens(str, sep);
			 pointer = 0;
			 // производим запись элементов относящихся к считываемым рядам в класс multiTS
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 while ((pointer  < numOfRows) && (mask[pointer] == -1)) {
					 pointer++;
				 }
				 //проверка на число элементов в строке - не больше ли их чем параметров
				 if (pointer < numOfRows) {
					 // Запись считанного значения
					 if (mask[pointer] == this->paramNames.size()) {
						 // записываем значение с именем класса неисправности на текущем отсчете
						 isin = isIn(*tok_iter, classNames);
						 if ((isin != -1) || (*tok_iter == this->null)) {
							 // все правильно - записываем номер указанного класса неисправностей в каноническом порядке ряда 
							 (multiTS.data[mask[pointer]]).push_back (isin + 1);
						 }
						 else {
							 // вместо строки со строкой имени класса неисправности - нечто другое - выходим по throw
							 std::cout << "\n tok_iter is '" << *tok_iter << "'\n";
							 std::cout << " null is '" << this->null << "'\n";
							 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: wrong name of class");
						 }
					 }
					 else {
						 // записываем некторое числовое значение - возможно параметр ( к примеру time )
						 const char * curF = tok_iter->c_str();
						 (multiTS.data[mask[pointer]]).push_back (atof(curF));
					 }
				 }
				 else {
					 file.close();
					 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: too many values in one string in cvs file -  more then parameters");
				 }
				 pointer++;
			 }
			 // проверка на число элементов в строке - не меньше ли их чем параметров
			 if (pointer < numOfRows) {
				 file.close();
				 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: too small amount of values in cvs file -  less then parameters");
			 }
		 }
	 }
	 
	 // закрытие обработанного файла и выход из функции
	 file.close();
	 return 0;
 }
   
 /****************************************************************************
 *					DataSetBase::getIntByName
 *
 *	Description:	Функция составляет число из входящих в строку цифр.
 *				Применяется при соглаосвании файлов содержащих 
 *				временной ряд и корректную разметку.
 *	Parameters:	str - строка - по которой составляем число
 *	Returns:		- 1 - если нет цифр в строке или
 *				положительно целое число или 0 - построенное по строке
 *	Throws:		-
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::getIntByName (std::string str) {
	 // Проверяем - не пуста ли анализируемая строка
	 if (!str.empty()) {
		 // Определяем входные параметры к итеративному алгоритму составления числа
		 int toReturn = 0;
		 bool flag = 0;
		 char zero = '0';
		 char nine = '9';
		 // Составляем число по строке
		 for (unsigned int i = 0; i < str.size(); i++) {
			 if ((zero <= str[i]) && (str[i] <= nine)) {
				 flag = 1;
				 toReturn = toReturn * 10 + (int) (str[i] - zero);
			 }
		 }
		 if (flag != 0) {
			 // Если была хотя бы одна цифра в строке - то возвращаем составленное число
			 return toReturn;
		 }
		 else {
			 // Если не было ни одной цифры в строке
			 return -1;
		 }
	 }
	 return -1;
 }
 
 bool DataSetBase::checkRefCSVName(const std::string& currentFileName) {
	 // шаблон - которому должны удовлеворять имена файлов с эталонными рядами для классов неисправностей

     if(currentFileName.size() <= 4 || currentFileName[currentFileName.size() - 4] != '.') {
         return false;
     }

     std::string extension = currentFileName.substr(currentFileName.size() - 3);
     return extension == "csv";

//	 static const std::string fileNameFirstHalf ("ref");
//	 static const std::string fileNameLastHalf (".csv");
	 
//	 return (checkName (currentFileName, -1, 0, fileNameFirstHalf))
//			 && (checkName (currentFileName, (int) currentFileName.size() - 5, (int) currentFileName.size() - 4, fileNameLastHalf));
 }
 
 /****************************************************************************
 *				DataSetBase::readClassTS
 *
 *	Description:	Читает эталонные ряды для заданного класса 
 *	Parameters:	dirName - строка, содержащая адрес директории с классом
 *				classTs - переменная, в которую считываются данные
 *	Returns:		0 - если все было считанно нормально (иначе throw)
 *	Throws:		-
 *	Author:		wictor
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readClassTS(const std::string& dirName, ClassTS &classTS) {
	 

	 boost::filesystem::path classPath (dirName);
	 
	 // цикл по файлам выбранной директории соответсвующей некоторому классу неисправностей
	 boost::filesystem::directory_iterator end_iter_int;
	 int fileCount = 0;
	 for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
		 if(checkRefCSVName((*dir_itr_int).string())) {
			 fileCount++;
		 }
	 }
	 
	 classTS.clear();
	 // Выделение памяти под fileCount временных рядов
	 classTS.reserve(fileCount);
	 
	 for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
		 //std::cout << "\n	EPT 2 \n";
		 //filePath = *dir_itr_int;
		 // переводим названия файла и шаблона в char* - чтобы сравнить на соответствие (см ./formats.x)
		 std::string currentFileName = dir_itr_int->leaf();
		 if (checkRefCSVName(currentFileName)) {
			 //осталось тока считать файл и записать в специально описанный класс multiTS
			 //std::cout << "\n	EPT 3 \n";
			 MultiTS multiTStemp;
			 currentFileName = (*dir_itr_int).string();
			 this->readFromCSV (currentFileName, multiTStemp);
			 /*// Test Output
			 std::vector<double> vecTemp;
			 std::cout << "\n MultiTS \n";
			 for (int nR = 0; nR < multiTStemp.size(); nR++) {
				 multiTStemp.getTSByIndex(vecTemp, nR);
				 std::cout << "\n";
				 for (int fR = 0; fR < vecTemp.size(); fR++) {
					 std::cout << "  " << vecTemp[fR];
				 }
			 }*/
			 // Storing multiTS
			 classTS.push_back(multiTStemp);
		 }
	 }
	 
	 
	 return 0;
 }
 
 /****************************************************************************
 *				DataSetBase::readReferenceTS
 *
 *	Description:	Читает эталонные ряды для каждого из классов неисправностей
 *				Цикл, ходит по всем каталогам в $path/reference/. 
 *				Имя каталога задает класс, к которому относятся ряды из 
 *				этого каталога. Для каждого каталога, ходим по всем лежащим в нем 
 *				.csv-файлам, читаем их, не забывая приводить к каноническому виду.
 *	Parameters:	path - строка содержащая адрес базоваой директории набора данных
 *				refClassesTS - переменная, в которую считываются данные
 *	Returns:		0 - если все было считанно нормально (иначе throw)
 *	Throws:		AxiomLibException  - если директория $path/reference/ - не существует 
 *								или этот адрес существует, но это файл
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readReferenceTS (const std::string &path, ReferenceClassesTS &refClassesTS) {
	 // Задание путей для работы с директориями и файлами 
	 boost::filesystem::path fullPath( path );
	 //fullPath /= "reference";
	 boost::filesystem::path classPath( fullPath ); // будет использоваться чтобы ходить по внутренностям директории reference
	 
	 // Проверка - существует ли указаный путь к файлу
	 if ( !boost::filesystem::exists( fullPath ) ) {
		 throw AxiomLibException ("Error in DataSet::browseFiles: wrong path - path do not exists.");
		 return 1;
	 }
	 
	 // Если поданный на заполнение класс не пуст - начинаем заполнять заново
	 refClassesTS.resize(this->classNames.size());
	 
	 // Проверяем - директория ли данный путь - как предполагается к основному какталогу набора данных
	 if ( boost::filesystem::is_directory( fullPath ) ) {
		 int isin = -1; // переменная для хранения результата работы функции isIn
		 // запускаем цикл - по внутренним папкам директории $path/referenes - 
		 // папки соответсвуют классам неисправностей - содержат соответствующие эталонные ряды
		 boost::filesystem::directory_iterator end_iter;
		 for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			 if ( boost::filesystem::is_directory( *dir_itr ) ) {
				 // проверяем - учтен ли класс неисправностей с именем текущей обрабатываемой директории в конфиг. файле
					 isin = isIn(dir_itr->leaf(), classNames);
				 //std::cout<< "\n WARN isin is " << isin << "\n";
				 //std::cout << "\n	EPT 0 \n";
				 if ( isin != -1) {//Имя класса есть в векторе DataSet - то есть теперь работаем с содежимым директории
					 //std::cout << "\n	EPT 1 \n";
					 classPath = fullPath;
					 classPath /= classNames[isin];
					 
					 readClassTS(classPath.string(), refClassesTS[isin]);
				 }
			} else { // в директории $path/reference нашелся некоторый файл  - по условию форматов - он смысловой нагрузки не несет - пропускаем
				 //std::cout << "Выводим название файла: " << dir_itr->leaf() << "\n";
			 }
		 }
	 
	 }
	 else {// Указанный путь не является каталогом - поэтому считывание эталонных рядов не произведено
		 throw AxiomLibException ("Error in DataSet::readClassesTS: wrong path - path is not a dir, it is a file.");
	 }
	 
	 return 0;
 }
 
  
// /****************************************************************************
// *				DataSet::IsIn
// *
// *	Description:	Функция проверяет - находится ли данная строка в данном векторе строк
// *	Parameters:	whatToFind - строка, которую ищем, whereToFind - вектор строк. среди которых ищем
// *	Returns:		- 1 - если строки нет в списке или
// *				Положительное целое - номер строки в списке
// *	Throws:		-
// *	Author:		dk
// *	History:		-
// *
// ****************************************************************************/
// signed int DataSet::isIn (std::string whatToFind, std::vector<std::string> &whereToFind) {
//	 for (signed int i = 0; i < (int) whereToFind.size(); i++) {
//		 if (whatToFind == whereToFind[i]) {
//			 return i;
//		 }
//	 }
//	 return -1;
// }
 
 
// /****************************************************************************
// *				DataSet::IsIn
// *
// *	Description:	Функция возвращает - находится ли данное число в данном векторе чисел
// *	Parameters:	whatToFind - число, которое ищем, whereToFind - вектор чисел. среди которых ищем
// *	Returns:		- 1 - если числа нет в списке или
// *				Положительное целое - номер числа в списке
// *	Throws:		-
// *	Author:		dk
// *	History:		-
// *
// ****************************************************************************/
// signed int DataSet::isIn (int whatToFind, std::vector<int> &whereToFind) {
//	 for (signed int i = 0; i < (int) whereToFind.size(); i++) {
//		 if (whatToFind == whereToFind[i]) {
//			 return i;
//		 }
//	 }
//	 return -1;
// }
