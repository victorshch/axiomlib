/***************************************************************************
*					Check
*
*	Description:	Класс для проверок
*					(В частности для проверки совместимсти двух элементраных условий
*					по их положению в классах аксиом)
*	Author:			dk
*	History:
*
****************************************************************************/

#ifndef __CHECK_LIB_AXIOM_HXX
#define __CHECK_LIB_AXIOM_HXX

#include "./elem_conditions/ElemCondition.h"
#include "Axiom.h"
#include "AxiomLibException.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace AxiomLib {

class Check {

 private:
 
 protected:
 
 public:
 
  // Пустой конструктор
    Check(void);
  
  // Пустой декструктор
    ~Check(void);
    
  // Функция проверки на совсестимость по "и" двух элементарных условий из двух аксиом
    signed int checkAndCompatibility (Axiom* ax1, int ecNum1, Axiom* ax2, int ecNum2);
    
  // Функция обмена местами двух элементаных условий из двух аксиом
    signed int change (Axiom* ax1, int ecNum1, Axiom* ax2, int ecNum2);
    
};// end of class

}; //  end of namespace

#endif /* __CHECK_LIB_AXIOM_HXX */
