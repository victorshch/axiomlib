/****************************************************************************
*			ElemConditionsFactory
*
*	Description:	Порождает элементарные аксиомы по номеру или по имени
*				Класс генерируется автоматически в процессе сборки,
*				чтобы включать в себя все классы - элементарные аксиомы
*				из директории axioms.
*	Author:		Eugene A. Vasin
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_ElemConditionsFactory_HXX
#define __AXIOMS_LIB_ElemConditionsFactory_HXX

#include "Round.h"

#include <string>
#include <vector>
#include <stdlib.h>
#include <math.h>

#include "elem_conditions/ElemCondition.h"
#include "ElemCondPlus.h"

namespace AxiomLib {

class ElemConditionsFactory {

 private:

	 std::vector < std::vector<bool> > compatibilityMatrix;

 protected:
 
  // Вспомогательная функция поиска максимального элемнта в векторе положительных значений
  int findMaxNum (std::vector <int> &vec);

  // Вспомогательная функция поэлементной конъюнкции векторов с проверкой равенства их длин
  int andVector (std::vector <bool>& whereVec, const std::vector <bool>& whatFirst, const std::vector <bool>& whatSecond);

  // Вспомогательная рекурсивная процедура формирования номеров совместимых элементарых условий заданной длины
  int subproc (std::vector < std::vector<int> >& prev, int &prevLast, std::vector <int>& cur, std::vector <bool> &curComp, const int numOfEC);

  // Вспомогательная функция создания векторов совместимых элементарных аксиом заданной длины
  int compatTable (std::vector < std::vector<int> >& prev, const int numOfEC);

  // Вектор, содержит указатели на аксиомы всех типов, с дефолтными параметрами
  std::vector<ElemCondition*> all_axioms;
 
 public:

  // Конструктор, инициализирует фабрику для порождения аксиом, заполняя
  // список всех аксиом
  ElemConditionsFactory(void);
  
  // Декструктор, уничтожающий все экземпляры объектов
  ~ElemConditionsFactory(void);

  // Возвращает ссылку на экземпляр элементарной аксиомы с номером num
  // Аксиома создается дефолтным конструктором без параметров
  ElemCondition* createAxiomByNum(const signed int num);

  // Возвращает вектор указателей на элементарные условия слйчайно выбанные для заполнения аксиомы
  int  createAxiomByRand(std::vector<ElemCondition* >& toReturn, const signed int numOfEC);

  // Возвращает вектор указателей на элементарные условия слйчайно выбанные для заполнения аксиомы
  //int  createAxiomByRand(std::vector< std::vector <ElemCondPlus> >& toReturn, const signed int numOfEC);
  int  createAxiomByRand(std::vector< std::vector <int> > &toReturn, const signed int numOfEC);

  // Возвращает ссылку на экземпляр элементарной аксиомы с именем name
  // Аксиома создается дефолтным конструктором без параметров
  // Было бы неплохо еще вариант с параметрами добавить
  ElemCondition* createAxiomByName(const char* name);
  
  // Записывает имена всех аксиом в итератор вывода BackInserter
  template<class BackInserter>
  int getECNames(BackInserter inserter);

};

template<class BackInserter>
int ElemConditionsFactory::getECNames(BackInserter inserter)
{
	for(int i = 0; i <(int) all_axioms.size(); i++)
	{
		*inserter = all_axioms[i]->name();
		++inserter;
	}
	return 0;
}

}; // namespace AxiomLib

#endif /* __AXIOMS_LIB_ElemConditionsFactory_HXX */
