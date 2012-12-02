/****************************************************************************
*
*			Функции класса ElemCondition
*
****************************************************************************/
#include "ElemCondition.h"
#include "../AxiomLibException.h"
#include "../FuzzyDataSet.h"

using namespace AxiomLib;

/****************************************************************************
*                       ElemCondition::crossEC
*
*       Description:    Безовая (хотя и весьма медленная и не универсальная)
*			функция скрещивания двух элементарных условий одного
*			типа. Меняет оба элементарных условия.
*       Parameters:     const ElemCondition& second - второе элементарное условие
*			double crossProb - вероятность скрещивания двух условий
*			double crossParam - параметр скрещивания (=0 - нет, =1 - унификация)
*       Returns:        
*       Throws:         AxiomLibException - если условия разных типов
*       Author:         dk
*       History:
*
****************************************************************************/
void ElemCondition::crossEC(ElemCondition& second, const double crossProb, const double crossParam)
{
  // 1. Определяем, являются ли они условиями одного типа
  if (name() != second.name()) {
    throw AxiomLibException("ElemCondition::crossEC - trying to cross parameters of differenc ECs.");
  }

  // Проверяем другие параметры  
  if (crossParam > 1 || crossParam < 0) {
    throw AxiomLibException("ElemCondition::crossEC - crossParam out of range.");
  }
  if (crossProb > 1 || crossProb < 0) {
    throw AxiomLibException("ElemCondition::crossEC - crossProb out of range.");
  }
  
  // 2. получаем список имен параметров
  std::vector<std::string> ecParamNames;
  getECParamNames (ecParamNames);
  
  // 3. Ходим последовательно по всем параметрам
  for (std::vector<std::string>::const_iterator curParamName = ecParamNames.begin(); curParamName != ecParamNames.end(); curParamName++) {
    // смотрим, будет ли тут мутация
    if ((( (double) rand() / (double) RAND_MAX )) < crossProb) {
      // да, мутация будет

      // получаем значения параметра с именем curParamName
      double p1;
      getParamValue(p1, *curParamName);
      double p2;
      second.getParamValue(p2, *curParamName);
      
      // скрещиваем их
      p1 = p1 * (1 - crossParam) + p2 * crossParam;
      p2 = p2 * (1 - crossParam) + p1 * crossParam;
      
      // сохраняем
      setParamValue(p1, *curParamName);
      second.setParamValue(p2, *curParamName);
    }
  }
}

int ElemCondition::setLimits(FuzzyDataSet *fuzzyDataSet, bool oneVsAllMode, const double reserve, const unsigned int numOfLevels)
{
	// Достаем необходимый для обучения ряд
	std::vector<double> teachRow;
	int numOfMultiTS;
	std::vector <int> numOfTS;
	if(!oneVsAllMode) {
		fuzzyDataSet->getNormalClassSize (numOfMultiTS, numOfTS);
		if ((numOfTS.size() != numOfMultiTS) || (numOfMultiTS < 1))
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : not enough data for the algorithm.");
		for (int i = 0; i < numOfMultiTS; i++) {
			if (numOfTS[i] > dimension) {
				fuzzyDataSet->getNormalTSFromClass (teachRow, i, dimension);
				if (teachRow.size() > 0)
					break;
			}
		}
	} else {
		if(fuzzyDataSet->getClassCount() <= 0) {
			throw AxiomLibException("FuzzyMultiDataExtAlgorithm::selectElemCond() : abnormal classes not found");
		}

		fuzzyDataSet->getTSByIndexFromClass(teachRow, 0, 0, dimension);
	}
	if (teachRow.size() < 1)
		throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::selectElemCond : incorrect normal data set.");

	return setLimits(teachRow, reserve, numOfLevels);
}

