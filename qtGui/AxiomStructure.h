/****************************************************************************
*					AxiomStructure
*
*	Description:	Класс, содержащий описание структуры аксиомы и ее параметры
*	Author:			dk
*	History:     	-
*
****************************************************************************/
#ifndef __qtGui_AxiomStructure_HXX
#define __qtGui_AxiomStructure_HXX

#include <stdlib.h>
#include <map>

// Сокращение для названия
typedef std::map<std::string, std::map<std::string, std::string> >::const_iterator	c_iter;

class AxiomStructure {

public:
	
	// Название аксиомы
	std::string axiomName;
	// Набор названий элементарных условий, составляющих аксиому
	std::vector <std::string> ecNames;
	// Параметры элементарных условий, входящих в систему аксиом
	std::map<std::string, std::map<std::string, std::string> > axiomParameters;
	
}; //end of class

#endif /* __qtGui_AxiomStructure_HXX */
