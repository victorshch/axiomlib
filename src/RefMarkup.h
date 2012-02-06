/****************************************************************************
*			RefMarkup
*
*	Description:	Одна эталонная разметка
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_REF_MARKUP_HXX
#define __AXIOMS_LIB_REF_MARKUP_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "all_axioms.h"
#include "Axiom.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

// если максимальный размер установлен в -1, он не органичивается
#define MARKUPINF = -1

class RefMarkup {

 private:

 protected:

    // Одна расширенная разметка
    std::vector<int> axiomTypes;
    std::vector<int> axiomMinLen;
    std::vector<int> axiomMaxLen;
    
    // Хранение значения целефой функции сравнения, от которого считать сравниваемые методом использования метрики участки рядов различными
    int errorLimit;

 public:
 
    // Пустой конструктор
    RefMarkup(void);

    // Деструктор удаляющий все созданные в рамках данного класса динамические объекты
    ~RefMarkup(void);

    // Добавление новой эталонной разметки данной системой аксиом
    void create(std::vector<signed int> &newAxiomTypes, std::vector<signed int> &newMinLen, std::vector<signed int> &newMaxLen);
    
    // Это функция для использования с уже заполненными переменными класса - может увеличить лишь разметку - если основные ряды совпадут
    signed int addLens (std::vector<signed int> &newAxiomTypes, std::vector<signed int> &newMinLen, std::vector<signed int> &newMaxLen);
    
    // Функция проверки - подходит ли разметка данного класса к вектору расшиеренной разметки во вхоном параметре
    signed int check (std::vector<int> &axiomTypes, std::vector<int> &axiomLen, long int upTo);

	// Выдает текущее состояние разметки
	signed int getCopy (std::vector<int> &axTypes, std::vector<int> &axMinLen, std::vector<int> &axMaxLen) const;
    
    // Вспомогательная функция сравенения с эталонной разметкой для check
    signed int subcheck (std::vector<int> &rowAxiomTypes, std::vector<int> &rowAxiomLen, long int upTo, int curEnd);

    // Вспомогательная функция - для организации хранения разметок в виде set'a, а не в виде vector'a
    bool operator < (const RefMarkup second) const {return 0;};
        
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_REF_MARKUP_HXX */
