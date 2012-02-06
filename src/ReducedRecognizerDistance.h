/****************************************************************************
*					ReducedRecognizerDistance
*
*	Description:	Промежуточный класс в иерархии ReducedRecognizer.
*                   Реализует вариант функции run, формирующий маркировку через
*                   функцию, вычисляющую расстояния от участков наблюдаемой
*                   траектории до эталонных траекторий
*                   (реализация этой функции нужна для обратной совместимости).
*	Author:			wictor
*	History:
*
****************************************************************************/
#ifndef REDUCEDRECOGNIZERIMPLBASE_H
#define REDUCEDRECOGNIZERIMPLBASE_H

#include "ReducedRecognizer.h"

namespace AxiomLib {

class ReducedRecognizerDistance : public ReducedRecognizer
{
public:
	ReducedRecognizerDistance();
	
	// Функция задания значений параметров класса
    virtual int setParamsFromEnv (const Environment& env);
    
    // Функция поиска разметки etalon в markUp и запись результатов поиска в result
    virtual int run (const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <int> &result);
	
	// Функция поиска разметки etalon в markUp -- в result пишутся получающиеся расстояния
	virtual int run(const std::vector <int> &markUp, const std::vector<int> &etalon, std::vector <double> &result) = 0;
	
protected:
	
    //void setPrecision(double newValue);
	
	double precision() const;
	
private:
	
	double m_precision;	
};

}

#endif // REDUCEDRECOGNIZERIMPLBASE_H
