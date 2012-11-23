/***************************************************************************
*					FuzzyDataLearnAlgorithm
*
*	Description:	Класс описывающий алгоритм поиска системы аксиом при 
*					не заданных тракториях аномального поведения.
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef _Fuzzy_Data_Learn_Algorithm_HXX
#define _Fuzzy_Data_Learn_Algorithm_HXX

#include "all_axioms.h"
#include "AxiomLibException.h"
#include "AxiomBase.h"
#include "AxiomExpr.h"
#include "AxiomExprSet.h"
#include "ElemCondPlus.h"
#include "ElemConditionsFactory.h"
#include "Round.h"
#include "Environment.h"
#include "CompareStatistic.h"
#include "CompareClassFactory.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include "PreprocessFactory.h"
#include "FuzzyDataSet_old.h"
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace AxiomLib {

class FuzzyDataLearnAlgorithm {

 private:
	// Стадия, на которой находится алгоритм построения системы аксиом и поиска разметок
	unsigned short stage;
	
	// Исходный набор данных, на котором производится обучение
	FuzzyDataSet_old fuzzyDataSet;
	// Вектор индексов параметров в каноническом представлении по которым вести анализ - поиск нештатных ситуаций
	// Под параметром понимается - название одномерного ряда в множестве рядов описывающих поведение системы
	std::vector<int> dataSetParams;

	// Параметря для сохранения лучших решений
	std::string axiomSetBaseDir;
	std::string axiomName;

	// Параметры алгоритма настройки элементарных условий
	int leftLimit;
	int rightLimit;
	// Величина, на которую ослаблять граничные значения для поиска
	double reserve;
	// Число градаций, на которые разбивать диапазон изменения параметров для поиска (диаметр сетки)
	int numOfLevels;
	// Стоимости различных раскладов, необходимые для подсчета целевой фукнции элементов
	double costEthIn;
	double costEthOut;
	double costNormalIn;
	double costNormalOut;

	// Параметры алгоритма настройки аксиом
	// Максимальное число аксиом в популяции 
	int maxAxiomPopSize;
	// Максимальное число итераций алгоритма поиска аксиом
	int maxIterNum;
	// Максимальное значение целевой функции для аксиомы, если оно превышается, то поиск новых аксиом останавливается
	double maxAxiomRes;
	// Процент лучших аксиом популяции, которые сохраняются при сокращении популяции
	double percentBestAxioms;

	// Стоимость для оценивания вероятности существования опорных точек
	double costStrongPoint;
	// Стоимость близости отношения минимальной длине к максимальной для участков где аксиома выполняется на тестовой выборке
	double costMaxMinLenRatio;
	// Стоимость близости к 0 отношения числа точек, где аксиома срабатывает к общему число на тестовой выборке.
	double costLenRatio;
	// Стоимость отсутствия областей срабатывания аксиомы на траекториях нормального поведения
	double costHiAreasNum;
	// Гибкость, с которой учитывать costHiAreasNum
	double hiAreasNumEffect;

	// Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
	double shiftErrIn;

	// Степень расширения разметки (неточность при поиске разметок во время их подбора)
	double extendMarkUpLensLevel;

 protected:

	// Внешная переменная класса, по которой определяется выводить ли комментарии в выходной поток
	bool comments;

	// Класс подсчета числа ошибок I и II рода на тестовой выборке
	CompareStatistic* compareStatistic;

	// Стратегия подсчета целевой функции по ошибкам I и II рода
	GoalStrategy* goalStrategy;

	// Вектор элементарных условий, заполняемый в результате их обучения
	std::vector <std::vector <ElemCondPlus> > bestECs;
	std::vector <std::vector <double> > bestECres; // целевая функция для выбранных условий

	// Вектор аксиом, создаваемый по мере формирования аксиом из элементраных условий
	std::vector <AxiomExpr> bestAxioms;
	std::vector <double> bestAxiomsRes;

	// Вектор систем аксиом, формируемых из созданных аксиом
	std::vector <AxiomExprSet> bestAxiomSets;
	std::vector <double> bestAxiomSetsRes;


	// Функции класса

	// Функция подбора и настройки элементарных условий
	int selectElemCond (const int param);

	// Функция формирования аксиом из элементарных условий
	int formAxioms (const int param);

	// функция формирования системы аксиом из аксиом
	int createAxiomSet (const int param);

	// Функция сохранения лучших элементарных условий и соответствующих значений целевой функции
	inline int storeBestECs (ElemCondPlus &ec, double &curFun, std::vector <ElemCondPlus> &bestECs, std::vector <double> &bestECres);

	// Подсчет числа точек в ряду, на которых элементарное условие выполняется
	inline int numOfCarriedOutItems (ElemCondPlus &ec, std::vector <double> &row) const;

	// Подсчет целевой функции для элементарного условия
	double matterECFunc (ElemCondPlus &ec, const int param);
	
	// Подсчет целевой функции для аксиомы
	double matterAxiomFunc (AxiomExpr &ax, const int param);

	// Функция сокращает популяцию существующих аксиом до определенного в параметрах значения
	int cutDownBestAxioms (void);

	// Вспомогательная функция сортировки массива по убыванию с указанием индексов результата в отдельном векторе
	inline int getPriority (std::vector <double> &vec, std::vector <unsigned int> &pos) const;

	// Объединение двух аксиом и вычисление результата для полученных новых аксиом
	int combineAxioms (const unsigned int i, const unsigned int j, AxiomExpr &axiomExpr, double &axiomExprRes, const int param);

	// Анализа разметки аксиомой axiomExpr ряда curTS и сбор статистики (hiAreasNum - число областей связности, hiAreasLen - общая длина таких областей)
	inline int axiomMarkUpStat (AxiomExpr &axiomExpr, std::vector <double> &row, int &hiAreasNum, int &hiAreasMinLen, int &hiAreasMaxLen) const;

	// Разметка ряда curTS аксиомой axiom с записью результата в resMarkUp
	inline int createMarkUp (std::vector <int> &resMarkUp, std::vector <double> &curTS, AxiomExpr &axiom, const int axNum) const;

	// Cдвиг основных разметок и формирование обобщенной разметки
	inline int createBaseMarkUp (std::vector <int> &sens, std::vector < std::vector <int> > &markUps) const;

	// Учет разметки markUp как не вошедшей в сдвиг и вычисление ошибки, которую она (разметка) дает
	inline double shiftErrors (std::vector <int> &markUp, const int from, const int upTo) const;

	// Учет несовпадений разметки row1 и row2 - подсчет числа несовпадений областей разметки и умножение на соответствующий коэффициент
	inline double shiftErrors (std::vector <int> &row1, const int from1, std::vector <int> &row2, const int from2, const int len) const;

	// Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens из вектора markUp с указанием границ областей обновления.
	inline int moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &markUp, const int from, const int upTo) const;

	// Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens из векторов base, sens с указанием границ областей обновления.
	inline int moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &base, std::vector <int> &sens, const int from, const int upTo) const;

	// Вспомогательная функция обновдения векторов newMarkUp и newMarkUpSens из вектора markUp и вектора base с указанием границ областей обновления.
	inline int moveVal (std::vector <int> &newMarkUp, std::vector <int> &newMarkUpSens, const int newFrom, std::vector <int> &base, std::vector <int> &sens, const int fromBase, std::vector <int> &markUp, const int fromMarkUp, const int len) const;

	// Формирование обобщенных разметок для заданного типа аномального поведения и заданной аксиомы
	inline int formGeneralizedMarkUps (std::vector <int> &baseMarkUp, std::vector < std::vector <int> > &resMarkUps, int axNum, int i, int numOfMultiTS, const int param);

	// Разметка траекторий контрольной выборки всеми различными аксиомами
	int getExtraMarkUps (std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &correctMarkUps, int axNum, int i, const int param) const;

	// Упрощение разметки и приведение ее в сокращенное описание
	int simplifyMarkUp (std::vector <int> &simp, std::vector <int> &len, std::vector <int> &row, const int axNum) const;

	// Функция проверки содержания одной из разметок в другой
	int subcheck (std::vector<int> &rowAxiomTypes, std::vector<int> &rowAxiomLen, int upTo, std::vector<int> &axiomTypes, std::vector<int> &axiomMinLen, std::vector<int> &axiomMaxLen, int curEnd) const;

	// Объединение расширенных разметок
	int splitMarkUps (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <int> &set, std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &markUpsMinLen, std::vector < std::vector <int> > &markUpsMaxLen) const;

	// Подсчет эффективности разметки на траекториях обучающей выборки
	int countMarkUpEfficiency (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <std::vector <int> > &localMarkUps, std::vector <bool> &localMarkUpsEngaged) const;

	// Обновление принимаемых во внимание разметок траекторий обучающей выборки
	bool setEngagedMarkUps (std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <std::vector <int> > &localMarkUps, std::vector <bool> &localMarkUpsEngaged) const;

	// Преобразование расширенной разметки в обычную разметку
	int expandMarkUp (std::vector <int> &simpleMarkUp, std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen) const;

	// Расширяет элементы вектора на заданный коэффициент
	inline int extendMarkUpLen (std::vector <int> &stdLen, std::vector <int> &minLen, std::vector <int> &maxLen) const;

	// Функция формирует ряд результатов сопоставления эталонной разметки и разметки анализируемого ряда
	int addResults (std::vector <int> &result, std::vector <int> &markUp, std::vector <int> &markUpMinLen, std::vector <int> &markUpMaxLen, std::vector <int> &wholeRow, int indicator) const;

	// Функция подсчета статистики для вектора разметок на контрольной выборке и на разметках траекторий аномального поведения
	inline int countExtraStat (std::vector <int> &curSet, std::vector < std::vector <int> > &simpMarkUps, std::vector < std::vector <int> > &simpMarkUpsMinLens, std::vector < std::vector <int> > &simpMarkUpsMaxLens, std::vector < std::vector <std::vector <int> > > &extraMarkUps, std::vector <std::vector <int> > &correctExtraMarkUps, const int indicator) const;

	// Формирование кандидатов в разметки для заданного типа аномального поведения  
	inline int createCandidates (std::vector < std::vector <int> > &candMarkUps, std::vector < std::vector <int> > &candMarkUpsMinLens, std::vector < std::vector <int> > &candMarkUpsMaxLens, std::vector < std::vector <int> > &baseMarkUps, std::vector < std::vector <std::vector <int> > > &localMarkUps, std::vector < std::vector <std::vector <int> > > &extraMarkUps, std::vector <std::vector <int> > &correctExtraMarkUps, const int indicator) const; 

	// Отбор разметок для заданного типа аномального поведения из сформированных кандидатов
	int sortCandidateMarkUps (std::vector < std::vector <int> > &markUps, std::vector < std::vector <int> > &candMarkUps, const int  param) const;

	// Создание систем аксиом на основе разметок и всего набора аксиом
	int formAxiomSets (std::vector < std::vector <std::vector <int> > > &markUps, std::vector < std::vector <std::vector <int> > > &markUpsMinLens, std::vector < std::vector <std::vector <int> > > &markUpsMaxLens);

	// Сохранение сформированных систем аксиом и разметок траекторий аномального поведения
	int saveResults (void);

 public:

	// Пустой конструктор: 
	FuzzyDataLearnAlgorithm (void);  
  
	// Декструктор, уничтожающий все экземпляры объектов внутри данного
	~FuzzyDataLearnAlgorithm (void);

	// Функция устанавливает параметры класса по данным, полученным из класса окружения
	int initFromEnv (const Environment& env);
	
	// Основная функция класса - запуск подбора системы аксиом
	int run (void);

	// Устанавливает новое значение переменной класса comments
	int setComments (const bool newVal);

}; // end of class

}; // end of namespace

#endif /* _Fuzzy_Data_Learn_Algorithm_HXX */
