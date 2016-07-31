/****************************************************************************
*					Функции класса FullFuzzyDataSet
*
*	Description:	Класс FullFuzzyDataSet - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/

#include "FullFuzzyDataSet.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

// Определения, используемые при именовании файлов
#define reference_str				"reference"
#define testing_str					"testing"
#define file_name_first_half_str	"ref"
#define csv_file_ending_str			".csv"

using namespace AxiomLib;

/****************************************************************************
*				FullFuzzyDataSet::FullFuzzyDataSet
*
*	Description:	Пустой конструктор с инициализацией переменных класса по умолчанию
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
FullFuzzyDataSet::FullFuzzyDataSet (void) {
	// Задание внутренних параметров класса по умолчанию
	this->normal.assign ("normal");
}


/****************************************************************************
*					FullFuzzyDataSet::setNormalStr
*
*	Description:	Функция устанавливает новое обозначение под именем к
*					отороого в обучающей выборке находятся траектории нормального 
*					поведения
*	Parameters:		env - класс с данными из конфигурационного файла
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
signed int FullFuzzyDataSet::setNormalStr (EnvDataSet &envDataSet) {
	int ret;
	std::string normalStr;
	ret = envDataSet.getNormalStr(normalStr);
	if (ret == 0) {
		normal = normalStr;
	}
	return 0;
}


/****************************************************************************
*				FullFuzzyDataSet::readNormalClassTS
*
*	Description:	Читает эталонные ряды для класса неисправности, за которым 
*					понимается нормальное поведение системы.
*					Цикл, ходит по всем каталогам в $path/reference/. 
*					Для каждого каталога, ходим по всем лежащим в нем 
*					.csv-файлам.
*	Parameters:		path - строка содержащая адрес базоваой директории набора данных
*					normalTS - заполняемое значение набора рядов
*	Returns:		0 - если все было считанно нормально (иначе throw)
*	Throws:			AxiomLibException  - если директория $path/reference/ - не существует 
*										или этот адрес существует, но это файл
*	Author:			dk
*	History:
*
****************************************************************************/
signed int FullFuzzyDataSet::readNormalClassTS (std::string &path, ClassTS &normalTS) {
	// Проверка - задано ли обозначение для траекторий нормального поведения
	if (normal.size() < 1)
		throw AxiomLibException ("Error in FullFuzzyDataSet::readNormalClassTS: string for normal class is not set.");

	// Очищаем содержимое заполняемой переменной
	normalTS.clear();

	// Задание путей для работы с директориями и файлами 
	std::string nPath = path;
	nPath.append ("/");
	nPath.append (normal);
	boost::filesystem::path classPath( nPath ); // указывает на директорию с траекториями нормального поведения
	boost::filesystem::path filePath( nPath ); // будет использоваться чтобы ходить по внутренностям директории с именем класса несиправности "normal"
	
	// шаблон - которому должны удовлеворять имена файлов с эталонными рядами для классов неисправностей
	std::string fileNameFirstHalf (file_name_first_half_str);
	std::string fileNameLastHalf (csv_file_ending_str);
	std::string aa; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	// Проверка - существует ли указаный путь к файлу
	if ( !boost::filesystem::exists( classPath ) )
		throw AxiomLibException ("Error in FullFuzzyDataSet::readNormalClassTS: path for normal behavior does not exists.");
	
	// Проверяем - директория ли данный путь - как предполагается к основному какталогу набора данных
	if ( boost::filesystem::is_directory( classPath ) ) {
		// цикл по файлам выбранной директории
		
		readClassTS(nPath, normalTS);
		
//		boost::filesystem::directory_iterator end_iter_int;
//		for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
//			filePath = *dir_itr_int;
//			// переводим названия файла и шаблона в char* - чтобы сравнить на соответствие (см ./formats.x)
//			aa = dir_itr_int->leaf();
//			if ((checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 5, (int) aa.size() - 4, fileNameLastHalf))) {
//				//осталось тока считать файл и записать в специально описанный класс multiTS
//				MultiTS multiTStemp;
//				aa = (*dir_itr_int).string();
//				this->readFromCSV (aa, multiTStemp);
//				/*// Test Output
//				std::vector<double> vecTemp;
//				std::cout << "\n MultiTS \n";
//				for (int nR = 0; nR < multiTStemp.size(); nR++) {
//					multiTStemp.getTSByIndex(vecTemp, nR);
//					std::cout << "\n";
//					for (int fR = 0; fR < vecTemp.size(); fR++) {
//						std::cout << "  " << vecTemp[fR];
//					}
//				}*/
//				// Сохраняем считанный multiTStemp в переменную normalTS
//				normalTS.push_back(multiTStemp);
//			}
//		}
	
	} else {// Указанный путь не является каталогом - поэтому считывание эталонных рядов не произведено
		throw AxiomLibException ("Error in FullFuzzyDataSet::readNormalClassTS: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					FullFuzzyDataSet::ReadDataSet
*
*	Description:	Функция чтения всех данных из набора данных, находящегося
*					в указанном каталоге
*	Parameters:		const char* path - путь к каталогу, содержащему набор данных
*	Returns:		0
*	Throws:			AxiomLibExeption - если каталог отсутствует, или есть ошибки
*					в формате набора данных
*	Author:			dk
*	History:
*
****************************************************************************/
signed int FullFuzzyDataSet::readDataSet (std::string dataSetDir, std::string dataSetName) {
  // Общая схема работы функции:
  // 0. Проверяем корректность пути
  // 1. Читаем файл с названиями классов и заполняем массив названий классов;
  //    этот порядок классов объявляется каноническим
  // 2. Читаем файл с названиями параметров и заполняем массив названий параметров
  //    этот порядок параметров объявляется каноническим
  // 3. Читаем эталонные ряды
  //    Цикл, ходит по всем каталогам в $path/reference/. Имя каталога
  //    задает класс, к которому относятся ряды из этого каталога.
  //        Для каждого каталога, ходим по всем лежащим в нем .csv-файлам,
  //        читаем их, не забывая приводить к каноническому виду.
  //	Читаем траектории нормального поведения.
  // 4. Читаем тестовую выборку
  //    Цикл, ходит по всем каталогам в $path/testing/. Имя каталога
  //    задает класс, к которому относятся ряды из этого каталога.
  //        Для каждого каталога, ходим по всем лежащим в нем .csv-файлам,
  //        читаем их, не забывая приводить к каноническому виду.
  //	Читаем траектории нормального поведения.

  //0 - 1 - 2 пункты реализуются в следующей функции: 
	
	std::string curPath;
	std::string baseDataSetDir = dataSetDir;
	baseDataSetDir.append("/");
	baseDataSetDir.append(dataSetName);
	
	this->readParams(baseDataSetDir);
  
  //3 пункт

	// Читаем траектории, относящиеся к нормальному поведению
	curPath = baseDataSetDir;
	curPath.append ("/");
	curPath.append (reference_str);
	readNormalClassTS (curPath, referenceNormalTS);
	
	// Читаем траектории, содержащие участки нештатного поведения
	readReferenceTS (curPath, referenceClassesTS);
	// Проверка на корректность заполенния:
	// - для каждого класса неисправностей проверяем задан ли хотя бы один эталонный ряд
	// - проверяем все ли классы неисправностей учтены
	int countMultiTS = 0;
	for (unsigned int h = 0; h < referenceClassesTS.size(); h++) {
		if ((referenceClassesTS[h]).size() == 0) {
			countMultiTS++;
		}
	}
	if ((referenceClassesTS.size() != classNames.size()) || (countMultiTS > 0)) {
		throw AxiomLibException ("Error in FullFuzzyDataSet::readDataSet: cannot find all Classes TSs in reference dataset directory.");
	}
	
  //4 пункт

	// Читаем траектории, относящиеся к нормальному поведению
	curPath = baseDataSetDir;
	curPath.append ("/");
	curPath.append (testing_str);
	readNormalClassTS (curPath, testingNormalTS);

	// Читаем траектории, содержащие участки нештатного поведения
	readReferenceTS(curPath, testingClassesTS);
	// Проверка на корректность заполенния:
	// - для каждого класса неисправностей проверяем задан ли хотя бы один эталонный ряд
	// - проверяем все ли классы неисправностей учтены
	countMultiTS = 0;
	for (unsigned int h = 0; h < testingClassesTS.size(); h++) {
		if ((testingClassesTS[h]).size() == 0) {
			countMultiTS++;
		}
	}
	if ((testingClassesTS.size() != classNames.size()) || (countMultiTS > 0)) {
		throw AxiomLibException ("Error in FullFuzzyDataSet::readDataSet: cannot find all Classes TSs in testing dataset directory.");
	}

	return 0;
}


/****************************************************************************
*				FullFuzzyDataSet::getNormalTSFromClass
*
*	Description:	Функция возвращает временной ряд, соответствующий нормальному 
*					поведению, из обучающей выборки по определенным - номеру 
*					мультиряда, номеру ряда в мультиряде.
*	Parameters:		vec - заполняемый временной ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getNormalTSFromClass (std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	if ((indexMultiTS < (int) (referenceNormalTS.size())) && (indexMultiTS >=0)) {
		return (referenceNormalTS[indexMultiTS]).getTSByIndex (vec, indexTS);
	}
	return false;
}


/****************************************************************************
*					FullFuzzyDataSet::getNormalTSFromTest
*
*	Description:	Функция возвращает временной ряд, соответствующий нормальному 
*					поведению, из контрольной выборки по определенным - номеру 
*					мультиряда, номеру ряда в мультиряде.
*	Parameters:		vec - заполняемый временной ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getNormalTSFromTest (std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	if ((indexMultiTS < (int) (testingNormalTS.size())) && (indexMultiTS >=0)) {
		return (testingNormalTS[indexMultiTS]).getTSByIndex (vec, indexTS);
	}
	return false;
}


/****************************************************************************
*					FullFuzzyDataSet::getNormalClassSize
*
*	Description:	Функция выдает информацию о числе временных рядов 
*					соответстующих нормальному поведению в обучающей выборке
*	Parameters:		numOfMultiTS - число мультирядов
*					numOfTS - вектор чисел рядов в мультирядах
*	Returns:		true
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getNormalClassSize (int &numOfMultiTS, std::vector<int> &numOfTS) const {
	numOfMultiTS = referenceNormalTS.size();
	numOfTS.resize (numOfMultiTS);
	for (int i = 0; i < numOfMultiTS; i++) {
		numOfTS[i] = (int) referenceNormalTS[i].size();
	}
	return true;
}


/****************************************************************************
*					FullFuzzyDataSet::getNormalTestSize
*
*	Description:	Функция выдает информацию о числе временных рядов 
*					соответстующих нормальному поведению в контрольной выборке
*	Parameters:		numOfMultiTS - число мультирядов
*					numOfTS - вектор чисел рядов в мультирядах
*	Returns:		true
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getNormalTestSize (int &numOfMultiTS, std::vector<int> &numOfTS) const {
	numOfMultiTS = testingNormalTS.size();
	numOfTS.resize (numOfMultiTS);
	for (int i = 0; i < numOfMultiTS; i++) {
		numOfTS[i] = (int) testingNormalTS[i].size();
	}
	return true;
}


/****************************************************************************
*					FullFuzzyDataSet::getTSByIndexFromTest
*
*	Description:	Функция возвращает временной ряд из обучающей выборки по 
*					определенным - номеру класса, номеру мультиряда в классе, 
*					номеру ряда в мультиряде
*	Parameters:		vec - заполняемый временной ряд
*					indexClass - номер класса неисправностей во внутреннем 
*						представлении данного класса, откуда считывать ряд
*					indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*					indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*					false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getTSByIndexFromTest (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const {
	if ((indexClass < (int) testingClassesTS.size()) && (indexClass >=0)) {
		if ((indexMultiTS < (int) (testingClassesTS[indexClass]).size()) && (indexMultiTS >=0)) {
			return (testingClassesTS[indexClass][indexMultiTS]).getTSByIndex (vec, indexTS);
		}
	}
	return false;
}


/****************************************************************************
*					FullFuzzyDataSet::getTestSize
*
*	Description:	Функция выдает информацию о числе временных рядов 
*					в переменной класса testingClassesTS.
*	Parameters:		numOfClasses - заполняемое число классов неисправностей
*					numOfMultiTS - вектор числа мультирядов в каждом из классов
*					numOfTS - вектор векторов числа рядов в классах
*	Returns:		true
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FullFuzzyDataSet::getTestSize (int &numOfClasses, std::vector<int> &numOfMultiTS, std::vector<std::vector<int> > &numOfTS) const {
	numOfClasses = (int) testingClassesTS.size();
	numOfMultiTS.resize(numOfClasses);
	numOfTS.resize(numOfClasses);
	for (int i = 0; i < numOfClasses; i++) {
		numOfMultiTS[i] = (int) (testingClassesTS[i]).size();
		(numOfTS[i]).resize(numOfMultiTS[i]);
		for (int j = 0; j < numOfMultiTS[i]; j++) {
			numOfTS[i][j] = (testingClassesTS[i][j]).size();
		}
	}
	return true;
}
