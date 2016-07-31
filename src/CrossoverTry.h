/****************************************************************************
*				CrossoverTry
*
*	Description:	Класс скрещивания систем аксиом - попытка создания
*				класса скрещивания на всех уровнях:
*					- уровень элементарных условий
*					- уровень аксиом
*					- уровень систем аксиом
*				Параметры скрещивания для каждого из уровней 
*				считываются из конфигурациионного файла или 
*				задаются специальной функцией
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_CROSSOVER_TRY_HXX
#define __AXIOMS_LIB_CROSSOVER_TRY_HXX

#include "CrossoverStrategy.h"
#include "Check.h"
#include <iostream>

namespace AxiomLib {

class CrossoverTry : public CrossoverStrategy {

   private:

	// Степерь скрещивания на каждом из уровней скрещивания
	// Каждой из велечин может принимать занчения 
	// от 0,0 (отсутсвтие скрещивания на данном уровне) до 1,0 (максимально возможная степень скрещивания).
	double elemCondCrossLevel;
	double axiomCrossLevel;
	double axiomSetCrossLevel;

   protected:
   
	// Вспомогательные функции для скрещивания :
	// реализация выбора пар систем аксиом для скрещивания 
	// и непосредественно скрещивание на различных уровнях
	
	// Функция выбора пар систем аксиом для скрещивания на уровне систем аксиом
	int choosePairs (std::vector <Pair> &asPairs);
	
	// Функция скрещивания двух выбранных систем аксиом на уровне систем аксиом
	int crossAxiomSets (std::vector<Axiom *>* axSet1, std::vector<Axiom *>* axSet2, AxiomSetWithStats &axiomSet);
	
	// Функция выбора пар аксиом для скрещивания 
	int chooseAxQuads (std::vector <Quad> &axQuads);
	
	// Функция скрещивания двух выбранных аксиом на уровне аксиом
	int crossAxioms (Axiom* ax1, Axiom* ax2, AxiomSetWithStats &newAxiomSet);
	
	// Функция выбора пар элементарных условий
	int chooseEcQuads (std::vector <QuadPlus> &ecQuads);
	
	// Скрещивание двух выбранных пар элементарных условий на уровне параметров элементаррных условий
	int crossEcs (Axiom* ax1, Axiom* ax2, std::string &ecName, AxiomSetWithStats &newAxiomSet);

   public:

	// Конструктор - с заданием параметров класса по умочанию
	CrossoverTry (void);

	// Деструктор - уничтожаются все созданные в рамках данного экземпляра класса динамические объекты
	~CrossoverTry () {};

	// Возвращает имя данного класса кодсчета целевой функции
	std::string name (void) const;

	// Функции задания значений параметров данного класса
	int setParamsFromEnv (const Environment& env);
	int setParams (const double newCrossLevel);
	int setParams (const double newElemCondCrossLevel, const double newAxiomCrossLevel, const double newAxiomSetCrossLevel);
	int setParams (std::map<std::string, std::string> &params);

	// Функция скрещивания некоторых систем аксиом из вектора систем
	int crossover (AxiomSetPop &axiomSetPop);

}; // of class

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_CROSSOVER_TRY_HXX */
