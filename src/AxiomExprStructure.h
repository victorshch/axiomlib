/****************************************************************************
*						AxiomExprStructure
*
*	Description:	Описание структуры аксиомы
*	Author:			dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AxiomExprStructure_HXX
#define __AXIOMS_LIB_AxiomExprStructure_HXX

#include "ElemCondPlusStruture.h"
#include <vector>
#include <string>

namespace AxiomLib {

class AxiomExprStructure {

public:
	
	// Название данной системы аксиом
	std::string axiomName;
	// Название используемого банка элементарных условий (может быть не задано)
	std::string elemCondBankName;
	// Описание выражения над элементарными условиями, составляющими данную аксиому. 
	// В столбцах - между ЭУ предполагается знак and, между столбцами - знак or. Отрицания включены в структуру ElemCondPlusStruture
	std::vector <std::vector <ElemCondPlusStruture> > elemConds;

		// Функции класса: //

	// Конструктор класса - создает все поля пустыми
	AxiomExprStructure() {
		axiomName.clear();
		elemCondBankName.clear();
		elemConds.clear();
	}

	// Функция очищает содержимое полей класса
	void clear() {
		axiomName.clear();
		elemCondBankName.clear();
		for (unsigned int i = 0; i < elemConds.size(); i++) {
			for (unsigned int j = 0; j < elemConds[i].size(); j++)
				elemConds[i][j].clear();
			elemConds[i].clear();
		}
		elemConds.clear();
	}
	
}; //end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AxiomExprStructure_HXX */
