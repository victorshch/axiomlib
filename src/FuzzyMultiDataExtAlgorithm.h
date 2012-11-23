/****************************************************************************
*					FuzzyMultiDataExtAlgorithm
*
*	Description:	Класс, реализующий конструктивный алгоритм с эвристиками
*					и допускающий экспорт/импорт параметров и результатов
*					каждого этапа (используется в GUI)
*	Author:			wictor
*	History:	
*
****************************************************************************/
#ifndef FUZZYMULTIDATAEXT_H
#define FUZZYMULTIDATAEXT_H

#include <boost/tuple/tuple.hpp>

#include "Environment.h"

#include "ElemCondPlusStat.h"
#include "AxiomExprPlus.h"
#include "AxiomExprSetPlus.h"
#include "FuzzyDataSet.h"

#include "ReducedRecognizer.h"
#include "GoalStrategy.h"

#include "FuzzyMultiDataExt/ForwardDeclarations.h"

#include "Logger.h"

#include "StringPlus.h"

namespace AxiomLib {
	
class FuzzyMultiDataExtAlgorithm
{
public:	
    FuzzyMultiDataExtAlgorithm();
	~FuzzyMultiDataExtAlgorithm();
	
	// Устанавливает новое значение переменной класса comments
	int setComments (const bool newVal);
	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);

	// Функция устанавливает параметры класса только для возможности распознавания и подсчета целевой функции для системы аксиом
	//int initFromEnvRecognitionOnly (const Environment& env);
	
	// Доступ к датасету
	FuzzyDataSet &getDataSet();
	const FuzzyDataSet& getDataSet() const;
	
	// Основная функция класса - запуск подбора системы аксиом
	void run (void);

	// Функции доступа к набору типов ЭУ
	void setECTypes(const std::vector<ElemCondPlus>& ecTypes);
	const ElemCondPlus& getECType(int n) const;
	bool isECTypeSelected(int n) const;
	void setECTypeSelected(int n, bool value);
	int getECTypeSize() const;
	
	// Запуск эвристик уровня типов ЭУ
	void runZeroLevelHeuristics();
	
	// Запуск только первой из 3-х стадий алгорима
	void runFirstLevel (void);
	
	// Эвристики отбора ЭУ
	void runFirstLevelHeuristics();
	
	// Функции доступа к набору ЭУ
	void setECs(const std::vector<std::vector<std::vector<std::vector<ElemCondPlusStat> > > >& ecs);
	const ElemCondPlusStat & getEC(int aType, int dimension, int type, int n) const;
	bool isECSelected(int aType, int dimension, int type, int n) const;
	void setECSelected(int aType, int dimension, int type, int n, bool value);
	void getECSize(std::vector<std::vector<std::vector<int> > > &result) const;
	
	int getECSize() const ;
	int getECSize(int aType) const ;
	int getECSize(int aType, int dimension) const;
	int getECSize(int aType, int dimension, int ecType) const;

	struct ECIndex {
		int abType;
		int dimension;
		int ecType;
		int n;
		ECIndex(int abType, int dimension, int ecType, int n);
	};

	const ElemCondPlusStat & getEC(const ECIndex& index) const;
	bool isECSelected(const ECIndex& index);
	void setECSelected(const ECIndex& index, bool value);

	static ECIndex makeECIndex(int abType, int dimension, int ecType, int n);
	
	void saveECs(const std::string& dir, const std::string& listFileName, bool selectedOnly = false) const;
	void loadECs(const std::string& dir, const std::string& listFileName);
	
	// Запуск только второй из 3-х стадий алгорима
	void runSecondLevel();
	
	// Эвристики отбора аксиом
	void runSecondLevelHeuristics();
	
	// Функции доступа к набору аксиом
	void setAxioms(const std::vector<std::vector<AxiomExprPlus> >& axioms);
	const AxiomExprPlus &getAX(int aType, int n) const;
	bool isAXSelected(int aType, int n) const;
	void setAXSelected(int aType, int n, bool value);
	void getAXSize(std::vector<int> &result) const;
	
	int getAXSize() const;
	int getAXSize(int aType) const;
	
	void saveAXs(const std::string& dir, const std::string& listFileName, bool selectedOnly = false) const;
	void loadAXs(const std::string& dir, const std::string& listFileName);
	
	// Запуск только третьей из 3-х стадий алгорима
	void runThirdLevel();
	
	// Функции доступа к набору систем аксиом
	void setAxiomSets(const std::vector<AxiomExprSetPlus>& axiomSets);
	AxiomExprSetPlus &getAS(int n);
	const AxiomExprSetPlus& getAS(int n) const;
	int getASSize() const;
	
	void saveASs(const std::string& dir) const;
	void saveASs() const;
	void loadASs(const std::string& dir, const std::vector<std::string>& asNames);
	
	void recalculateMatterECFunc(ElemCondPlusStat& ec, int abType);
	void recalculateMatterAxiomFunc(AxiomExprPlus& ax, int abType);
	void recalculateMatterAxiomSetFunc(AxiomExprSetPlus& as);

	bool oneVsAllMode() const { return mOneVsAllMode; }
	
private:
	
	// Исходный набор данных, на котором производится обучение
	FuzzyDataSet fuzzyDataSet;

	// Режим "один против всех". Если true, то
	// при подборе ЭУ и аксиом для некоторого класса нештатного поведения
	// траектории других классов нештатного поведения учитываются в целевых функциях 1-го и 2-го этапов
	// как траектории нормального поведения
	bool mOneVsAllMode;

	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> dataSetParams;

	// Параметры для сохранения лучших аксиом
	std::string axiomBaseDir;
	
	// Имя файла, в который сохранять списки имен сохраненных файлов
	std::string fileNameECList;
	// Имя файла, в который сохранять списки имен сохраненных файлов
	std::string fileNameAxiomList;
	// Параметры для сохранения лучших решений
	std::string axiomSetBaseDir;
	std::string axiomSetName;

 protected:
	Logger* logger;
	
	FuzzyMultiDataExt::HeuristicsSet* heuristicsSet;
	
	FuzzyMultiDataExt::ECTypeStage* stage0;
	FuzzyMultiDataExt::ECStage* stage1;
	FuzzyMultiDataExt::AXStage* stage2;
    FuzzyMultiDataExt::ASStage* stage3;

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

	// Вектор с именами сохраненных файлов с описаниями лучших систем аксиом и статистика по ним
	std::vector <StringPlus> bestAxiomSetsNamesPlus;

};

};

#endif // FUZZYMULTIDATAEXT_H
