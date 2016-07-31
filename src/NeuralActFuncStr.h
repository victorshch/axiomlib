/****************************************************************************
*			NeuralActFuncStr.h
*
*	Description: Объявление функций, нужных для преобразования enum'ов для
*				функций активации и обучающих алгоритмов в символьное представ-
*				ление и обратно
*	Author:		wictor
*	History:	
*
****************************************************************************/
#ifndef NEURALACTFUNCSTR_H_
#define NEURALACTFUNCSTR_H_

#include <string>

namespace AxiomLib {

// Активационная функция по строке (возвращает -1 в случае некорректного параметра,
//  иначе можно кастануть в FANN:activation_function_enum)
int actFuncFromStr(const std::string &val);

// Строка по активационной функции (возвращает "" в случае некорректного параметра)
const std::string &strFromActFunc(int val);

// Обучающий алгоритм по строке (возвращает -1 в случае некорректного параметра, 
// иначе можно кастануть в FANN:training_algorithm_enum)
int trainFromStr(const std::string &val);

// Строка по обучающему алгоритму (возвращает "" в случае некорректного параметра)
const std::string &strFromTrain(int val);

};

#endif /* ACTFUNCSTR_H_ */
