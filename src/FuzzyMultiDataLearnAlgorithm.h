/***************************************************************************
*					FuzzyMultiDataLearnAlgorithm
*
*	Description:	Класс описывающий алгоритм поиска системы аксиом при
*					не заданных тракториях аномального поведения по обучающей
*					выборке, содержащей многомерные траектории.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Fuzzy_Multi_Data_Learn_Algorithm_HXX
#define _Fuzzy_Multi_Data_Learn_Algorithm_HXX

#include "all_axioms.h"
#include "AxiomLibException.h"
#include "AxiomBase.h"
#include "AxiomExpr.h"
#include "AxiomExprPlus.h"
#include "AxiomExprSet.h"
#include "AxiomExprSetPlus.h"
#include "ElemCondPlus.h"
#include "ElemCondPlusStat.h"
#include "ElemConditionsFactory.h"
#include "Round.h"
#include "Environment.h"
#include "ReducedRecognizer.h"
#include "ReducedRecognizerFactory.h"
//#include "CompareStatistic.h"
#include "CompareClassFactory.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include "PreprocessFactory.h"
//#include "FullFuzzyDataSet.h"
#include "ClippedFullFuzzyDataSet.h"
#include "StringPlus.h"
//#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <omp.h>

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

namespace AxiomLib {

class FuzzyMultiDataLearnAlgorithm {
	friend class FuzzyMultiDataAccessor;
 private:
	// Стадия, на которой находится алгоритм построения системы аксиом и поиска разметок
	unsigned short stage;
	
	// Исходный набор данных, на котором производится обучение
	ClippedFullFuzzyDataSet fuzzyDataSet;
	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> dataSetParams;

	// Параметры для сохранения лучших аксиом
	std::string axiomBaseDir;
	std::string axiomNameTemplate;
	// Шаблон для названий файлов, под которыми сохрянять элементарные условия, после первого этапа алгоритма
	std::string ecNameTemplate;
	// Имя файла, в который сохранять списки имен сохраненных файлов
	std::string fileNameECList;
	// Имя файла, в который сохранять списки имен сохраненных файлов
	std::string fileNameAxiomList;
	// Параметры для сохранения лучших решений
	std::string axiomSetBaseDir;
	std::string axiomSetName;

	// Параметры алгоритма настройки элементарных условий
	int leftLimit;
	int rightLimit;
	// Величина, на которую ослаблять граничные значения для поиска
	double reserve;
	// Число градаций, на которые разбивать диапазон изменения параметров для поиска (диаметр сетки)
	int numOfLevels;
	// Число лучших экземпляров элементарных условий каждого типа, которые сохраняются после первого этапа работы алгоритма
	int numBestECs;
	// Максимальное число, до которого можно по требованию расширить numBestECs
	int numBestECsMax;

	// Параметры алгоритма настройки аксиом
	// Максимальное число аксиом в популяции
	int maxAxiomPopSize;
	// Максимальное число итераций алгоритма поиска аксиом
	int maxIterNum;
	// Максимальное значение целевой функции для аксиомы, если оно превышается, то поиск новых аксиом останавливается
	double maxAxiomRes;
	// Процент лучших аксиом популяции, которые сохраняются при сокращении популяции
	double percentBestAxioms;

	// Параметры алгоритма построения систем аксиом
	// Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
	double shiftErrIn;
	// Степень расширения разметки (неточность при поиске разметок во время их подбора)
	double extendMarkUpLensLevel;
	// Максимальное число аксиом в популяции
	int maxAxiomSetPopSize;
	// Максимальное число шагов в алгоритме построения систем аксиом
	int maxNumberOfSteps;
	// Критерий остановки для алгоритма построения системы аксиом - требуемое значение целевой функции
	double bestAxiomSetGoal;
	// Чило лучших сохраняемых систем аксиом для каждого процесса
	int numberOfBestAxiomSets;
	// Процент лучших систем аксиом в популяции, которые сохраняются при переходе на следующий шаг - (1 - percentBestAxiomSets) - выбираются случайно
	double percentBestAxiomSets;
	// Параметры алгоритма формирования разметок эталонных траекторий
	// Число траекторий для каждого из классов нештатного поведения из набора Reference, которые будут использоваться для формирования разметок эталонных траекторий
	int numberOfUsedClassTraj;

 protected:

	// Внешная переменная класса, по которой определяется выводить ли комментарии в выходной поток
	bool comments;

	bool managed;

	// Класс для распознавания эталонных траекторий в анализируемом ряду
	ReducedRecognizer* recognizer;

	// Класс подсчета числа ошибок I и II рода на тестовой выборке
	//CompareStatistic* compareStatistic;
	int ccNumPoints; // Число точек в области, после которого начислять ошибку за каждую лишнюю точку в этой области (используется при подсчете статистики)

	// Стратегия подсчета целевой функции по ошибкам I и II рода
	GoalStrategy* goalStrategy;

	//Исходные типы элементарных условий
	std::vector<ElemCondPlusStat> initialECTypes;

	// Вектор элементарных условий, заполняемый в результате их обучения
	//std::vector <std::vector <ElemCondPlus> > bestECs;
	//std::vector <std::vector <double> > bestECres; // целевая функция для выбранных условий
	// вектор с именами файлов, содержащих описания элементарных условий, которые будут использоваться на втором шаге алгоритма
	std::vector <std::vector <std::vector <std::vector <std::string> > > > bestECForAllAbnormalTypes;
	// множество имен файлов, содержащих описания элементарных условий, которые включаются во второй этапы работы, так как заданы в конфигурационном файле
	std::set <std::string> predefinedECFileName;

	// Вектор названий файлов с описаниями лучших аксиом - для каждого типа нештатного поведения
	std::vector <std::vector <std::string> > bestAxiomsFileNames;
	// множество имен файлов, содержащих описания аксиом, которые включаются в третий этап работы, так как заданы в конфигурационном файле
	std::set <std::string> predefinedAxiomsFileName;

	// Вектор систем аксиом, формируемых из созданных аксиом
	std::vector <AxiomExprSetPlus> bestAxiomSets;
	// Вектор с именами сохраненных файлов с описаниями лучших систем аксиом и статистика по ним
	std::vector <StringPlus> bestAxiomSetsNamesPlus;

		// Функции класса

	// Функция подбора и настройки элементарных условий - основная
	int selectElemCond (void);

	// Функция подбора и настройки элементарных условий для заданного типа нештатного поведения
	int selectElemCond (const int abnormalBehaviourType, const int rank, const int groupRankFrom, const int groupRankUpTo, std::vector <std::vector <std::vector <std::string> > > &bestECFileNames);

	// Функция подбора и настройки элементарных условий для заданного типа нештатного поведения и для заданной размерности исходных данных
	int selectElemCond (const int abnormalBehaviourType, const int dimension, std::vector <std::vector <ElemCondPlusStat> > &bestECs) const;
	int selectElemCond (const int abnormalBehaviourType, const int dimension, std::vector <std::vector <ElemCondPlus> > &bestECs, std::vector <std::vector <double> > &bestECres);

	// Функция формирования аксиом из элементарных условий
	int formAxioms (void);

	// Формирование аксиом для заданного типа нештатного поведения
	int formAxioms (const int abType, std::vector <std::vector <std::vector <std::string> > > &ecFileNames, std::vector <AxiomExprPlus> &axioms, std::vector <std::string> &axiomFileNames) const;
	int formAxioms (const int abType, std::vector <std::vector <std::vector <std::string> > > &ecFileNames, std::vector <AxiomExpr> &axioms, std::vector <double> &axiomRes, std::vector <std::string> &axiomFileNames) const;

	// функция формирования системы аксиом из аксиом
	int createAxiomSet (void);

	// сохранение всех имен файлов с описаниями элементарных условий в файл
	std::string saveBestECFileNames (const int rank) const;

	// сохранение всех имен файлов с описаниями лучших аксиом в файл
	std::string saveBestAxiomFileNames (void) const;

	// считывание имен файлов с описаниями элементарных условий из файла
	int restoreBestECFileNames (void);

	// считывание имен файлов с описаниями элементарных условий из файла c указанным в параметрах путем/именем
	int restoreBestECFileNames (const std::string filePath);

	// считывание имен файлов с описаниями аксиом из файла
	int restoreBestAxiomFileNames (void);

	// считывание имен файлов с описаниями аксиом из файла c указанным в параметрах путем/именем
	int restoreBestAxiomFileNames (const std::string filePath);

	// Функция собирает имена файлов с описаниями лучших систем аксиом с разных процессов, на которые был распараллелен основной алгоритм
	int gatherBestAxiomsSets (const int rank, const int size);

	// Функция отображает на экране значения целевой функции для лучших систем аксиом
	int displayBestResult(void) const;

	// Функция сохраняет систему аксиом в векторе лучших систем
	int addToBestAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets);

	// Функция добавляет системы аксиом из одного вектора в другой в соответствии с перечисленным множеством индексов
	int addAxiomSets(std::vector <AxiomExprSetPlus> &nextStepAxiomSets, std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets) const;

	// Функция сортирует элементы лучшего набора систем аксиом и сокращает число элементов данного вектора до заданного значения
	int sortBestAxiomSets (void);

	// Функция сортирует по неубыванию целевой функции элементы входного вектора axiomSets
	int sortAxiomSets (const std::vector <AxiomExprSetPlus> &axiomSets, std::vector <int> &indecies) const;

	// Функция отбирает заданный набор лучших систем аксиом, для которых значение целевой функции меньше заданного параметра
	int chooseIndicesOfBestAxiomSets (std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets, double &goal) const;

	// Функция возвращает индекс первого вхождения элемента в вектор или -1, если такого элемента нет
	inline int isIn (const unsigned int elem, const std::vector <unsigned int> &vec) const;

	// Функция сохраняет в файлы лучшие сформированные на втором этапе файлы
	int saveBestAxiomsInFiles (const int abnormalBehaviourType, std::vector <AxiomExprPlus> &axioms, std::vector <std::string> &axiomFileNames) const;

	// Функция собирает имена файлов с описаниями лучших аксиом с разных процессов, на которые был распараллелен основной алгоритм
	int gatherBestAxioms (void);

	// Функция обновляет список имен файлов с описаниями лучших аксиом - именами из конфигурационного файла
	int updateBestAxiomsListFromEnv(void);

	// Функция вытаскивает из имени файла два возможных числовых параметра
	int disintegrateAxiomFileName(const std::string fileName, int &at, int &index) const;

	// Функция распределяет диапазон значений [rangeMin, rangeMax] по size классам (процессам) и выдает диапазоны для текущего класса (процесса)
	int splitRange (const int rank, const int size, const int rangeMin, const int rangeMax, int &from, int &upTo, int &rankFrom, int &rankUpTo) const;

	// Функция разбивает множество типов нештатного поведения на подмножества для независимой обработки в рамках разных процессов
	int splitAbnormalTypeRange (int rank, int size, int &from, int &upTo, int &rankFrom, int &rankUpTo) const;

	// Функция сбора полученной информации со всех процессов после первого этапа работы алгоритма
	//int gatherBestECfromProcesses (int rank, int size);
	int gatherBestECNames (const int rankFrom, const int rankUpTo, std::vector <std::vector <std::vector <std::string> > > &ecNames, std::vector < std::vector < std::vector <std::string> > > &allNames) const;

	// функция  обновляет список имен файлов с описанием параметров элементарных условий списков файлов из конфигурационного файла
	int updateBestECListFromEnv(void);

	// функция выдает ближайшее число в строке str начиная с указанного индекса
	int getNearestNumber (const std::string str, std::string::size_type &index) const;

	// Функция разбирает название файла на части - и вытаскивает определенным образом индексы
	int disintegrateECFileName(const std::string fileName, int &at, int &dim, int &vecI, int &vecJ);

	// функция сохраняет описания элементарных условий из текущего набора лучших условий в файлы
	int saveBestECinFile (const int abnormalBehaviourType, const int  dimen, std::vector <std::vector <ElemCondPlusStat> > &bestECs, std::vector <std::vector <std::string> > &ecNames) const;

	// Функция сохранения лучших элементарных условий и соответствующих значений целевой функции
	inline int storeBestECs (std::vector <ElemCondPlusStat> &bestECs, ElemCondPlusStat &ec, double &goal, double &statNorm, double &statAbnorm) const;

	// Подсчет числа точек в ряду, на которых элементарное условие выполняется
	inline int numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const;

	// Подсчет числа точек в ряду, на которых аксиома выполняется
	inline int numOfCarriedOutItems (AxiomExpr &ax, std::vector < std::vector <double> > &row, int &rowLen) const;

	// Подсчет целевой функции для системы аксиом
	double matterAxiomSetFunc (AxiomExprSetPlus &as) const;

	// Подсчет целевой функции для системы аксиом для заданного типа нештатного поведения и варианта разметки эталонной траектории этого типа
	double matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, std::vector <int> &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) const;

	// Функция считает число ненулевых областей в ряду
	inline int getStatistic (std::vector <int> &row) const;

	// Функция сокращает число систем аксиом во входном векторе до заданного в параметрах класса значения
	int cutDownAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) const;

	// Функция сокращает популяцию существующих аксиом до определенного в параметрах значения
	int cutDownBestAxioms (std::vector <AxiomExprPlus> &axioms) const;

	// Вспомогательная функция сортировки массива по убыванию с указанием индексов результата в отдельном векторе
	inline int getPriority (std::vector <AxiomExprPlus> &vec, std::vector <unsigned int> &pos) const;

	// Объединение двух аксиом и вычисление результата для полученных новых аксиом
	int combineAxioms (AxiomExprPlus &axiomFirst, AxiomExprPlus &axiomSec, AxiomExprPlus &axiomExprPlus, const int abType) const;

	// Разметка многомерного ряда системой аксиом as
	inline int createRefMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, const int abType, const int multiTSNumber, std::vector < std::vector <int> > &numOfTS) const;

	// Разметка многомерного ряда системой аксиом as 
	inline int createTestMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, std::vector <bool> &dims, int &abType, int &indexTest, int &numOfTestTS) const;

	// Разметка многомерного ряда системой аксиом as 
	inline int createTestMarkUp (std::vector <int> &markUp, AxiomExprSetPlus &as, std::vector <bool> &dims, int &indexTest, int &numOfTestTS) const;

	// Функция создает вариант разметок траекторий нештатного поведения по разметкам траекторий, содержащих участки нештатного поведения
	inline int createMarkUpVariants (std::vector < std::vector <int> > &genMarkUps, std::vector < std::vector <int> > &resMarkUps) const;

	// Функция создает варианты разметок, состоящих только их одной аксиомы
	inline int createSimpleMarkUpVariants (std::vector < std::vector <int> > &genMarkUps, const int numOfAxioms) const;

	// Функция выбирает из набора кандидатов в разметки эталонной траектории заданного типа лучшую
	int chooseBestMarkUp (AxiomExprSetPlus &as, int abType, std::vector <int> &markUp, std::vector <std::vector <int> > &genMarkUps, int &errFirstVal, int &errSecondVal) const;

	// Упрощение разметки и приведение ее в сокращенное описание
	inline int simplifyMarkUps (std::vector <std::vector <int> > &markUps) const;
	//  старый вариант
	//int simplifyMarkUp (std::vector <int> &markUp, std::vector <int> &markUpLen, std::vector <int> &genMarkUp, std::vector <int> &genMarkUpSens) const;
	//  другой старый вариант реализации
	//int simplifyMarkUp2 (std::vector <int> &markUp, std::vector <int> &markUpLen, std::vector <int> &genMarkUp, std::vector <int> &genMarkUpSens) const;
	
	// Функция выдает взвешенную сумму входных параметров
	//inline double countCutCriteria (int &len, int &val) const;

	// Функция вычиления матрицы соответствия элементов двух строк
	int getSquare (const std::vector <int> &str1, const std::vector <int> &str2, std::vector < std::vector <int> > &square) const;

	// Функция ищет все максимальные общие подпоследовательности двух строк
	int getSubsequences (const std::vector <int> &str1, const std::vector <int> &str2, int i, int j, const std::vector < std::vector <int> > &square, std::vector < std::vector <int> > &res) const;

	// Расширяет элементы вектора на заданный коэффициент
	//inline int extendMarkUpLen (std::vector <int> &row, std::vector <int> &minLen, std::vector <int> &maxLen, std::vector <int> &stdLen) const;

	// Сохранение сформированных систем аксиом
	int saveBestAxiomSets (const int rank, std::vector <std::string> &dataSetParams);

 public:

	// Пустой конструктор:
	FuzzyMultiDataLearnAlgorithm (void);  
  
	// Декструктор, уничтожающий все экземпляры объектов внутри данного
	~FuzzyMultiDataLearnAlgorithm (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция устанавливает параметры класса только для возможности распознавания и подсчета целевой функции для системы аксиом
	int initFromEnvRecognitionOnly (const Environment& env);
	
	// Основная функция класса - запуск подбора системы аксиом
	int run (void);

	// Запуск только первой из 3-х стадий алгорима
	std::string runFirstLevel (void);

	// Запуск только второй из 3-х стадий алгорима (для нескольких процессов)
	std::string runSecondLevel (std::vector <std::string> &ecListFileVecs);
	// Запуск только второй из 3-х стадий алгорима
	std::string runSecondLevel (const std::string ecListFilePath);

	// Запуск только третьей из 3-х стадий алгорима
	std::string runThirdLevel (const std::string axiomListFilePath);

	// Подсчет целевой функции для системы аксиом
	double matterAxiomSetFunc (const std::string baseDir, const std::string fileName, int &errFirst, int &errSecond) const;
	
	// Подсчет целевой функции для элементарного условия
	double matterECFunc (ElemCondPlusStat &ec, const int param, const int abnormalBehaviourType) const;
	
	// Подсчет целевой функции для аксиомы
	double matterAxiomFunc (AxiomExprPlus &ax, const int abType) const;

	// Подсчет целевой функции для системы аксиом с заданным набором разметок эталонных траекторий
	double matterAxiomSetFunc (AxiomExprSetPlus &as, std::vector <std::vector <int> > &markUps) const;	

	// Устанавливает новое значение переменной класса comments
	int setComments (const bool newVal);

}; // end of class

}; // end of namespace

#endif /* _Fuzzy_Multi_Data_Learn_Algorithm_HXX */
