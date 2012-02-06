/****************************************************************************
*					CrossoverTpl
*
*	Description:	Класс скрещивания систем аксиом для операции генетики для шаблонов
*	Author:			dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_CROSSOVER_TPL_HXX
#define __AXIOMS_LIB_CROSSOVER_TPL_HXX

#include "Environment.h"
#include "Axiom.h"
#include "AxiomSet.h"
#include "AxiomSetPlus.h"
#include "AxiomLibException.h"
#include "CrossoverStrategy.h"
#include "Check.h"
#include "Round.h"
#include <string>
#include <vector>

namespace AxiomLib {

/*struct Pair {
	unsigned short first;
	unsigned short second;
};

struct Quad {
	unsigned short first;
	unsigned short fax;
	unsigned short second;
	unsigned short sax;
};

struct QuadPlus {
	unsigned short first;
	unsigned short fax;
	unsigned short second;
	unsigned short sax;
	std::string ecName;
};*/

class CrossoverTpl {

 private:

	std::vector < std::vector <Axiom *>* > axioms;

	std::vector < std::vector <double> > axiomsProfit;

	std::vector < std::vector <unsigned short> > axiomsMask;

	std::vector < std::vector <double> > axiomsMaskProb;

 protected:

	// Вспомогательные функции для скрещивания :
	// реализация выбора пар систем аксиом для скрещивания 
	// и непосредественно скрещивание на различных уровнях

	// Функция выбора пар систем аксиом для скрещивания на уровне систем аксиом
	int choosePairs (std::vector <Pair> &asPairs, const std::vector <AxiomSetPlus> &population);

	// Функция скрещивания двух выбранных систем аксиом на уровне систем аксиом
	int crossAxiomSets (unsigned short as1, unsigned short as2, AxiomSetPlus &axiomSetPlus);

	// Функция выбора пар аксиом для скрещивания 
	int chooseAxQuads (std::vector <Quad> &axQuads, const std::vector <AxiomSetPlus> &population);

	// Функция скрещивания двух выбранных аксиом на уровне аксиом
	int crossAxioms (unsigned short as1, unsigned short ax1, unsigned short as2, unsigned short ax2, AxiomSetPlus &axiomSetPlus);

	// Функция выбора пар элементарных условий
	int chooseECQuads (std::vector <QuadPlus> &ecQuads, const std::vector <AxiomSetPlus> &population);

	// Скрещивание двух выбранных пар элементарных условий на уровне параметров элементаррных условий
	int crossECs (unsigned short as1, unsigned short ax1, unsigned short as2, unsigned short ax2, std::string &ecName, AxiomSetPlus &axiomSetPlus);

	// Функция добавления в популяцию созданных систем аксиом
	inline void addSets (std::vector <AxiomSetPlus> &newPop, const std::vector <AxiomSetPlus> &population, std::vector < std::vector <AxiomSetPlus> > &newSets, std::vector < std::vector <int> > &setsMask) const;

	// Сортирует элементы вектора vec и заносит отсортированную последовательность индексов в mask
	inline void sort (std::vector <double> &vec, std::vector <unsigned short> &mask);

 public:

	// Конструктор - с заданием параметров класса по умочанию
	CrossoverTpl ();

	// Деструктор - уничтожаются все созданные в рамках данного экземпляра класса динамические объекты
	~CrossoverTpl ();

	// Возвращает имя данного класса кодсчета целевой функции
	std::string name (void) const;

	// Функции задания значений параметров данного класса
	int setParamsFromEnv (const Environment& env);

	// Функция инициализации класса стратегии скрещивания - Получение вектора ссылок на наборы аксиом
	void initCrossover (const std::vector <AxiomSetPlus> &population);
	
	// Функция скрещивания некоторых систем аксиом из вектора систем
	int crossover (const std::vector <AxiomSetPlus> &population, std::vector <AxiomSetPlus> &toRet);

}; // of class

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_CROSSOVER_TPL_HXX */
