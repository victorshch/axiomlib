/****************************************************************************
*			Функции класса DataSet
*
*	Description:	Класс работы с DataSet - чтение и выдача наборов данных
*	Author:		dk
*	History:	
*
****************************************************************************/

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

#include <set>

#include "Common.h"

#include "DataSet.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*				DataSet::DataSet
*
*	Description:	Пустой конструктор с инициализацией переменных класса
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
DataSet::DataSet (void) : DataSetBase() {

}

/****************************************************************************
*				DataSet::ReadDataSet
*
*	Description:	Функция чтения всех данных из набора данных, находящегося
*				в указанном каталоге
*	Parameters:	string baseDataSetDir - путь к каталогу, содержащему набор данных
*	Returns:		0
*	Throws:		AxiomLibExeption - если каталог отсутствует, или есть ошибки
*				в формате набора данных
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readDataSet(const std::string &baseDataSetDir) {
	
	// 0. Проверяем корректность пути, читаем названия классов и параметров
	// 1. Читаем эталонные ряды
	//    Цикл, ходит по всем каталогам в $path/reference/. Имя каталога
	//    задает класс, к которому относятся ряды из этого каталога.
	//        Для каждого каталога, ходим по всем лежащим в нем .csv-файлам,
	//        читаем их, не забывая приводить к каноническому виду.
	// 2. Читаем тестовую выборку
	//    Цикл, ходит по всем файлам xxx.csv, ищет для них парные файлы xxx-ref.csv,
	//    читает эту пару.
	
	
	// 0
	DataSetBase::readDataSet(baseDataSetDir);
	
  //1. Чтение эталонных траекторий
	
	this->readClassesTS(baseDataSetDir);
	// Проверка на корректность заполенния:
	// - для каждого класса неисправностей проверяем задан ли хотя бы один эталонный ряд
	// - проверяем все ли классы неисправностей учтены
	int countMultiTS = 0;
	for (unsigned int h = 0; h < this->referenceClassesTS.size(); h++) {
		if ((this->referenceClassesTS[h]).size() == 0) {
			countMultiTS++;
		}
	}
	if ((this->referenceClassesTS.size() != this->classNames.size()) || (countMultiTS > 0)) {
		//throw AxiomLibException ("Error in DataSetBase::readDataSet: cannot find all Classes TSs in dataset directory.");
	}
	
  //2. Чтение тестовых траекторий и корректных разметок
	
	this->readTestesTS (baseDataSetDir);
	// проверка на корректность заполнения:
	// - для каждого ли тествого файла заданна разметка и для каждого ли файла с разметкой есть временной ряд
	int length1 = 0;
	int length2 = 0;
	for (unsigned int g = 0; g < this->testSets.size(); g++) {
		length1 = 0;
		length2 = 0;
		//std::cout << "\n Num of rows is " << (((this->testSets[g]).data).data).size() << "\n";
		for (unsigned int j = 0; j < (((this->testSets[g]).data).data).size(); j++) {
			length1 = (int) (((this->testSets[g]).data).data[j]).size();
			if (length1 > 0){ 
				//std::cout<< "\n break \n";
				break;
			}
		}
		length2 = (int) (((this->testSets[g]).correct).data[((this->testSets[g]).correct).validParams.size() - 1]).size();
		//std::cout << "\n len1 is " << length1 << "\n";
		//std::cout << "\n len2 is " << length2 << "\n";
		//std::cout << "\n";
		if (length1 != length2) {
			//std::cout << "\n len1 is " << length1 << "\n";
			//std::cout << "\n len2 is " << length2 << "\n";
			throw AxiomLibException ("Error in DataSetBase::readDataSet: there is no correction by length of rows in a pair of csv files.");
		}
	}
}

/****************************************************************************
*				DataSet::ReadDataSet
*
*	Description:	Функция чтения всех данных из набора данных, находящегося
*				в указанном каталоге
*	Parameters:	dataSetDir - директория датасета, dataSetName - имя датасета
*
*	Returns:		0
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
signed int DataSet::readDataSet (const std::string &dataSetDir, const std::string &dataSetName)
{
	return readDataSet(dataSetDir + "/" + dataSetName);
}

/****************************************************************************
*				DataSet::getTSByIndexFromClass
*
*	Description:	Функция возвращает временной ряд из переменных класса 
*				по определенным - номеру класса, номеру мультиряда в 
*				классе, номеру ряда в мультиряде. 
*				Функция необходима для получения ряда из переменных класса.
*				Используется при передаче ряда в класс разметки.
*	Parameters:	vec - заполняемый временной ряд
*				indexClass - номер класса неисправностей во внутреннем 
*						представлении данного класса, откуда считывать ряд
*				indexMultiTS - номер мультиряда откуда считывать требуемый ряд
*				indexTS - номер требуемого ряда в мультиряде
*	Returns:		true - если ряд успешно считан
*				false - если ряд по каким-то причинам не считан (т.е. если его нет)
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const {
	if ((indexClass < (int) referenceClassesTS.size()) && (indexClass >=0)) {
		if ((indexMultiTS < (int) (referenceClassesTS[indexClass]).size()) && (indexMultiTS >=0)) {
			return (referenceClassesTS[indexClass][indexMultiTS]).getTSByIndex (vec, indexTS);
		}
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromClass
*
*	Description:	Функция устанавливает новый временной ряд по
*					определенным - номеру класса, номеру мультиряда в 
*					классе, номеру ряда в мультиряде. 
*					Функция необходима для работы алгоритрмов предобработки.
*	Parameters:		vec - новый временной ряд
*					indexClass - номер класса неисправностей во внутреннем 
*						представлении данного класса
*					indexMultiTS - номер мультиряда
*					indexTS - номер ряда в мультиряде
*	Returns:		true - если ряд успешно установлен
*					false - если ряд по каким-то причинам не установлен
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) {
	if ((indexClass < (int) referenceClassesTS.size()) && (indexClass >=0)) {
		if ((indexMultiTS < (int) (referenceClassesTS[indexClass]).size()) && (indexMultiTS >=0)) {
			return (referenceClassesTS[indexClass][indexMultiTS]).setTSByIndex (vec, indexTS);
		}
	}
	return false;
}

/****************************************************************************
*				DataSet::getNumberOfClasses
*
*	Description:	Функция возвращает число классов нештатного поведения
*	Parameters:	-
*	Returns:	Число классов нештатного поведения
*	Throws:		-
*	Author:		wictor
*	History:		-
*
****************************************************************************/
int DataSet::getNumberOfClasses() const {
	return (int) referenceClassesTS.size();
}

 
/****************************************************************************
*				DataSet::getClassSize
*
*	Description:	Функция выдает информацию о числе временных рядов 
*				во внутреннем представлении данного класса
*	Parameters:	numOfClasses - заполняемое число классов неисправностей
*				numOfMultiTS - вектор числа мультирядов в каждом из классов
*				numOfTS - вектор векторов числа рядов в классах
*	Returns:		true - если переменные были успешно записаны
*				false - если переменные записать не удалось
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getClassSize (int &numOfClasses, std::vector<int> &numOfMultiTS, std::vector<std::vector<int> > &numOfTS) const {
	if (referenceClassesTS.size() > 0) {
		numOfClasses = (int) referenceClassesTS.size();
		numOfMultiTS.resize(numOfClasses);
		numOfTS.resize(numOfClasses);
		for (int i = 0; i < numOfClasses; i++) {
			numOfMultiTS[i] = (int) (referenceClassesTS[i]).size();
			(numOfTS[i]).resize(numOfMultiTS[i]);
			for (int j = 0; j < numOfMultiTS[i]; j++) {
				numOfTS[i][j] = (referenceClassesTS[i][j]).size();
			}
		}
		return true;
	}
	return false;
}


/****************************************************************************
*				DataSet::getTestSize
*
*	Description:	Функция возвращает информацию о числе тестовых
*				классов в данном экземпляре класса
*	Parameters:	numOfTests - заполняемое число тестовых классов
*				numOfTS - заполняемый вектор числа временных рядов в 
*						тестовых данных
*	Returns:		true - если записать информацию удалось
*				false - если запись переменных не удалась
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTestSize (int &numOfTests, std::vector<int> &numOfTS) const {
	if (testSets.size() > 0) {
		numOfTests = (int) testSets.size();
		numOfTS.resize(numOfTests);
		for (int i = 0; i < numOfTests; i++) {
			numOfTS[i] = (testSets[i]).size();
		}
		return true;
	}
	return false;
}


/****************************************************************************
*				DataSet::getTSByIndexFromTests
*
*	Description:	Функция возвращает временной ряд по номеру теста, 
*				по номеру ряда в мультиряде в тесте
*	Parameters:	vec - заполняемый временной ряд
*				indexTest - внутренний номер тестового класса в данном классе
*				indexTS - внутренний номер временного ряда в тестовом классе
*						(ВАЖНО: Если укзать это параметр равным '-1' - то будет
*						записан ряд корректоной разметки)
*	Returns:		true - если записать переменные удалось
*				false - если записать переменные не удалось
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS) const {
	
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).getTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromTests
*
*	Description:	Функция устанавливает новый временной ряд по номеру теста, 
*					по номеру ряда в мультиряде в тесте
*	Parameters:		vec - новый временной ряд
*					indexTest - внутренний номер тестового класса в данном классе
*					indexTS - внутренний номер временного ряда в тестовом классе
*						(ВАЖНО: Если укзать это параметр равным '-1' - то будет
*						записан ряд корректоной разметки)
*	Returns:		true - если записать новое значение удалось
*					false - если записать не удалось
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS) {
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).setTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::getTSByIndexFromTests
*
*	Description:	Функция возвращает временной ряд по номеру теста, 
*				по номеру ряда в мультиряде в тесте
*	Parameters:	vec - заполняемый временной ряд
*				indexTest - внутренний номер тестового класса в данном классе
*				indexTS - внутренний номер временного ряда в тестовом классе
*						(ВАЖНО: Если укзать это параметр равным '-1' - то будет
*						записан ряд корректоной разметки)
*	Returns:		true - если записать переменные удалось
*				false - если записать переменные не удалось
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS) const {
	
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).getTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromTests
*
*	Description:	Функция устанавливает новый ряд по номеру теста, 
*					по номеру ряда в мультиряде в тесте
*	Parameters:		vec - новый временной ряд
*					indexTest - внутренний номер тестового класса в данном классе
*					indexTS - внутренний номер временного ряда в тестовом классе
*						(ВАЖНО: Если укзать это параметр равным '-1' - то будет
*						записан ряд корректоной разметки)
*	Returns:		true - если записать новое значение ряда удалось
*					false - если записать не удалось
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS) {
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).setTSByIndex(vec, indexTS);
	}
	return false;
}

/****************************************************************************
*				DataSet::readClassTS
*
*	Description:	Читает эталонные ряды для каждого из классов неисправностей
*				Цикл, ходит по всем каталогам в $path/reference/. 
*				Имя каталога задает класс, к которому относятся ряды из 
*				этого каталога. Для каждого каталога, ходим по всем лежащим в нем 
*				.csv-файлам, читаем их, не забывая приводить к каноническому виду.
*	Parameters:	path - строка содержащая адрес базоваой директории набора данных
*	Returns:		0 - если все было считанно нормально (иначе throw)
*	Throws:		AxiomLibException  - если директория $path/reference/ - не существует 
*								или этот адрес существует, но это файл
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readClassesTS (const std::string &path) {	
	std::string nPath = path;
	nPath.append ("/");
	nPath.append ("reference");
	return readReferenceTS (nPath, referenceClassesTS);
}

/****************************************************************************
*				DataSet::readTestesTS
*
*	Description:	Функция чтения тестовой выборки.
*				Цикл, ходит по всем файлам .csv заданного по умолчанию 
*				каталога $path/testing, считывает их в особый 
*				формат TestTS - причем каждому файлу находится пара
*				то есть в этот класс записыватся данные из пары файлов 
*				с названиями: runX.cvs и runX-ref.csv. При этом в первом 
*				файле содежатся тестовые временные ряды, а во втором -
*				корректная разметка приведенных рядов.(то есть указание 
*				где встречаются нештатные поведения и к какому классу они
*				относятся)
*	Parameters:	path - строка содержащая имя основного каталога набора данных
*	Returns:		0
*	Throws:		AxiomLibException  - если не существует каталога $path/testing (в
*								котором по умолчанию хранятся тестовые выборки)
*								либо если такой путь существует, но это файл
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readTestesTS (const std::string &path) {
	// создаем путь к директории по умолчанию содержащую тестовые ряды
	boost::filesystem::path fullPath( path );
	fullPath /= "./testing";
	boost::filesystem::path filePath( fullPath ); // будет использоваться чтобы ходить по внутренностям директории
	// создаем шаблоны для имен файлов которым должны удовлетворять имена файлов с тестовыми временными рядами
	std::string fileNameFirstHalf ("run");
	std::string fileNameLastHalf1 ("-ref.csv");
	std::string fileNameLastHalf2 (".csv");
	std::string aa; // в данной переменной будут храниться имена обрабатываемых в некоторый момент файлов
	std::vector<int> readSeq; // заводим отдельный массив в который заносим информацию о том, файлы с какими номерами занесены в память из файлов
						  // этот массив требуется для согласования - к примеру если runX.csv был почитан, а runX-ref.csv - но мы алгоритм подошел 
						  // к обработке этого файла - то для него не создается отдельной новой структуры TestTS - а используется уже созданная 
						  // для runX.csv
	int isin = -1; // переменная для хранения результатов работы функции isIn
	
	// Проверяем существует ли заданный путь - то есть $path/testing
	if ( !boost::filesystem::exists( fullPath ) ) {
		throw AxiomLibException ("Error in DataSet::readTestesTS: wrong path - path do not exists.");
		return 1;
	}
	
	// Если класс уже был заполнен - то при новом чтении тестовых данных заполянем его заново
	this->testSets.resize(0);
	
	// Проверяем директория ли путь $path/testing
	if ( boost::filesystem::is_directory( fullPath ) ) {
		int intByName = -1; // переменная для хранения числа составленного по имени текущего обрабатываемого файла
		// Запускаем цикл по файлам каталога $path/testing
        	boost::filesystem::directory_iterator end_itr;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_itr; ++dir_itr ) {
			filePath = fullPath;
			filePath /= *dir_itr;
			// Переводим имя текущего обрабатываемого файла в строку
			aa = dir_itr->leaf();
			// Составляем число по имени файла
			intByName = this->getIntByName(aa);
			// Проверяем - по полученному числу - удовлетворяет ли имя файла формату имен тестовых рядов
			if (intByName >= 0) {
				// Проверяем какого типа данный файл - с временными рядами или с описанием вхождений траекторий во временные ряды
				if ( (checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 9, (int) aa.size() - 8, fileNameLastHalf1)) ) {
					// значит это ref - файл
					// Читаем файл с временными рядами
					MultiTS correctTS;
					aa = (*dir_itr).string();
					this->readFromRefCSV (aa, correctTS);
					// Проверяем прочитана ли пара для данного файла (см. описание данной функции)
					isin = isIn (intByName, readSeq);
					if (isin >= 0) {
						// Пара описана - только добавляем новые данные
						(testSets[isin]).correct = correctTS;
						// Вызов функции для копирования корректной разметки в вектор целых чисел
						(testSets[isin]).makeIntClone();
					}
					else {
						// Пара не описана  - создаем новый экземпляр класса TestTS
						TestTS testTS;
						testTS.correct = correctTS;
						// Вызов функции для копирования корректной разметки в вектор целых чисел
						testTS.makeIntClone();
						this->testSets.push_back(testTS);
						readSeq.push_back(intByName);
					}
				}
				else {
					// Проверяем какого типа обрабатываемый файл
					if ( (checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 5, (int) aa.size() - 4, fileNameLastHalf2)) ) {
						// Значит это просто cvs - файл с одежанием тестового ряда
						// Производим чтение данного файла
						MultiTS multiTStemp;
						aa = (*dir_itr).string();
						this->readFromCSV (aa, multiTStemp);
						// Определяем считана ли пара для данного файла
						isin = isIn (intByName, readSeq);
						if (isin >= 0) {
							// Пара считана - просто добавляем новые данные к уже созданному экземпляру класса TestTS
							(testSets[isin]).data = multiTStemp;
						}
						else {
							// Пара не создана - создаем новый экземпляр класса TestTS
							TestTS testTS;
							testTS.data = multiTStemp;
							this->testSets.push_back(testTS);
							readSeq.push_back(intByName);
						}
					}
					else {
						//файл не удовлетворяет ни одному шаблону - его просто не обрабатываем 
					}
				}
			}
		}
	}
	else {// По указанному пути $path/testing - расположен не каталог, а файл - 
		  //соответсвенно произвести чтение тестовых данных не представляетя возможным (смотри ./formats.x)
		throw AxiomLibException ("Error in DataSet::readClassRows: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}

