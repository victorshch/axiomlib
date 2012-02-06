/****************************************************************************
*			RefMarkupSetSimple
*
*	Description:	Набор эталонных разметок, соответствующих одному
*				классу неисправностей
*				Разметки представлены в простом виде для ипользования 
*				распознавателей на метрике.
*	Author:		dk
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_REF_MARKUP_SET_Simple_HXX
#define __AXIOMS_LIB_REF_MARKUP_SET_Simple_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "all_axioms.h"
#include "Axiom.h"
#include "AxiomSet.h"
#include "RefMarkupSimple.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>

namespace AxiomLib {

class RefMarkupSetSimple {

 private:

 protected:
 
    // Базовый класс на основе которого (путем копирования) создаются классы разметок эталонных траекторий
    RefMarkupSimple rmu;

 public:
 
    // Эталонные  разметки  // ПЕРЕНЕСТИ В БУДУЕЩМ В PRIVATE
    std::vector<RefMarkupSimple> refMarkups;
  
    // Пустой конструктор
    RefMarkupSetSimple(void);  

    // Деструктор удаляющий все созданные в рамках данного класса динамические объекты
    ~RefMarkupSetSimple(void);
    
    // Установка параметров из конфигурационного файла
    int setParamsFromEnv (const Environment& env);
    
    // Добавление новой эталонной разметки данной системой аксиом
    void addRefMarkup(std::vector<double>& refPattern, AxiomSet& as, const bool comments);
    
    // Добавление новой эталонной разметки данной системой аксиом
    void addRefMarkup(std::vector<int> &axiomTypes, const bool comments);
    
    // Удаление всех разметок в данном классе
    signed int clear();
    
    // Функция проверки - подходит ли какая либо из разметок класса к вектору расшиеренной разметки во вхоном параметре
    signed int check (const double newMaxDistance, std::vector<int> &axiomTypes, long int upTo);

	// Оператор присваивания
    RefMarkupSetSimple& operator= (const RefMarkupSetSimple& second);
        
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_REF_MARKUP_SET_Simple_HXX */
