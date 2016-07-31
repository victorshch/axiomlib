/****************************************************************************
*			RefMarkupSet
*
*	Description:	Набор эталонных разметок, соответствующих одному
*				классу неисправностей
*	Author:		gevor
*	History:	
*
****************************************************************************/

#ifndef __AXIOMS_LIB_REF_MARKUP_SET_HXX
#define __AXIOMS_LIB_REF_MARKUP_SET_HXX

#include "./elem_conditions/ElemCondition.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "all_axioms.h"
#include "Axiom.h"
#include "AxiomSet.h"
#include "RefMarkup.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

namespace AxiomLib {

class RefMarkupSet {

 private:

 protected:

    // Точность задания расширенной разметки - то есть максимальное и минимально число вхожений аксиомы домнажаются на (1+-precision)
    double precision;

 public:
 
    // Расширенные эталонные  разметки 
    std::vector<RefMarkup> refMarkups;
  
    // Пустой конструктор
    RefMarkupSet(void);  

    // Деструктор удаляющий все созданные в рамках данного класса динамические объекты
    ~RefMarkupSet(void);
    
    // Функция задания новых значений внутренних переменных
    int setPrecision (const double newPrecision);

    // Добавление новой эталонной разметки данной системой аксиом
    void addRefMarkup(std::vector<double>& refPattern, AxiomSet& as, const bool comments);
    
    // Добавление новой эталонной разметки данной системой аксиом
    void addRefMarkup(std::vector<int> &resRef, const bool comments);
    
    // Вспомогательная функция - преобразование полученной разметки до расширенной
    signed int processRes (std::vector<int> &resRef, std::vector<int> &axiomTypes, std::vector<int> &axiomMinLen, std::vector<int> &axiomMaxLen);
    
    // Функция проверки - подходит ли какая либо из разметок класса к вектору расшиеренной разметки во вхоном параметре
    signed int check (std::vector<int> &axiomTypes, std::vector<int> &axiomLen, long int upTo);
        
}; // end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_REF_MARKUP_SET_HXX */
