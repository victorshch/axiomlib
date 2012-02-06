/****************************************************************************
*					Функции класса FuzzyDataSet
*
*	Description:	Класс FuzzyDataSet - реализация
*	Author:			dk
*	History:	
*
****************************************************************************/

#include "FuzzyDataSet.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

using namespace AxiomLib;

/****************************************************************************
*				FuzzyDataSet::FuzzyDataSet
*
*	Description:	Пустой конструктор с инициализацией переменных класса
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
FuzzyDataSet::FuzzyDataSet (void) {
	// Задание внутренних параметров класса по умолчанию
	std::string nr ("normal");
	this->normal = nr;
}


/****************************************************************************
*					FuzzyDataSet::setNormalStr
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
signed int FuzzyDataSet::setNormalStr (EnvDataSet &envDataSet) {
	int ret;
	std::string normalStr;
	ret = envDataSet.getNormalStr(normalStr);
	if (ret == 0) {
		normal = normalStr;
	}
	return 0;
}


/****************************************************************************
*				FuzzyDataSet::getNormalTS
*
*	Description:	Функция возвращает временной ряд, соответствующий нормальному 
*					поведению, из переменных класса по определенным - номеру 
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
bool FuzzyDataSet::getNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	if ((indexMultiTS < (int) (normalTS.size())) && (indexMultiTS >=0)) {
		return (normalTS[indexMultiTS]).getTSByIndex (vec, indexTS);
	}
	return false;
}


/****************************************************************************
*					FuzzyDataSet::setNormalTS
*
*	Description:	Функция устанавливает новое значение ряду обучающей выборки, 
*					соответстующему нормальному поведению
*	Parameters:		vec - новый временной ряд
*					indexMultiTS - номер мультиряда
*					indexTS - номер ряда в мультиряде
*	Returns:		true - если ряд успешно установлен
*					false - если ряд по каким-то причинам не установлен
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet::setNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS) {
	if ((indexMultiTS < (int) (normalTS.size())) && (indexMultiTS >=0)) {
		return (normalTS[indexMultiTS]).setTSByIndex (vec, indexTS);
	}
	return false;
}

 
/****************************************************************************
*					FuzzyDataSet::getNormalClassSize
*
*	Description:	Функция выдает информацию о числе временных рядов 
*					соответстующих нормальному поведению
*	Parameters:		numOfMultiTS - число мультирядов
*					numOfTS - вектор чисел рядов в мультирядах
*	Returns:		true
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet::getNormalClassSize (int &numOfMultiTS, std::vector<int> &numOfTS) const {
	numOfMultiTS = normalTS.size();
	numOfTS.resize (numOfMultiTS);
	for (int i = 0; i < numOfMultiTS; i++) {
		numOfTS[i] = (int) normalTS[i].size();
	}
	return true;
}


/****************************************************************************
*				FuzzyDataSet::readNormalClassTS
*
*	Description:	Читает эталонные ряды для класса неисправности, за которым 
*					понимается нормальное поведение системы.
*					Цикл, ходит по всем каталогам в $path/reference/. 
*					Для каждого каталога, ходим по всем лежащим в нем 
*					.csv-файлам.
*	Parameters:		path - строка содержащая адрес базоваой директории набора данных
*	Returns:		0 - если все было считанно нормально (иначе throw)
*	Throws:			AxiomLibException  - если директория $path/reference/ - не существует 
*										или этот адрес существует, но это файл
*	Author:			dk
*	History:
*
****************************************************************************/
signed int FuzzyDataSet::readNormalClassTS (std::string &path) {
	if (normal.size() < 1)
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: string for normal class is not set.");

	// Задание путей для работы с директориями и файлами 
	std::string nPath = path;
	nPath.append ("/reference/");
	nPath.append (normal);
	boost::filesystem::path classPath( nPath ); // указывает на директорию с траекториями нормального поведения
	boost::filesystem::path filePath( nPath ); // будет использоваться чтобы ходить по внутренностям директории с именем класса несиправности "normal"
	
	// шаблон - которому должны удовлеворять имена файлов с эталонными рядами для классов неисправностей
	std::string fileNameFirstHalf ("ref");
	std::string fileNameLastHalf (".csv");
	std::string aa; // локальная переменная, хранящая имя текущего открываемого и считываемого файла 
	
	// Проверка - существует ли указаный путь к файлу
	if ( !boost::filesystem::exists( classPath ) )
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: path for normal behavior does not exists.");
	
	// Проверяем - директория ли данный путь - как предполагается к основному какталогу набора данных
	if ( boost::filesystem::is_directory( classPath ) ) {
		// цикл по файлам выбранной директории
		boost::filesystem::directory_iterator end_iter_int;
		for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
			filePath = *dir_itr_int;
			// переводим названия файла и шаблона в char* - чтобы сравнить на соответствие (см ./formats.x)
			aa = dir_itr_int->leaf();
			if ((checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 5, (int) aa.size() - 4, fileNameLastHalf))) {
				//осталось тока считать файл и записать в специально описанный класс multiTS
				MultiTS multiTStemp;
				aa = (*dir_itr_int).string();
				this->readFromCSV (aa, multiTStemp);
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
				// Сохраняем считанный multiTStemp в переменную класса
				normalTS.push_back(multiTStemp);
			}
		}
	
	} else {// Указанный путь не является каталогом - поэтому считывание эталонных рядов не произведено
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					FuzzyDataSet::ReadDataSet
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
signed int FuzzyDataSet::readDataSet (std::string dataSetDir, std::string dataSetName) {
	// Читаем основные данные набора траекторий
	DataSet::readDataSet (dataSetDir, dataSetName);
	// Читаем траектории, относящиеся к нормальному поведению
	std::string baseDataSetDir = dataSetDir;
	baseDataSetDir.append("/");
	baseDataSetDir.append(dataSetName);
	readNormalClassTS (baseDataSetDir);
	return 0;
}
