#ifndef AXSTAGESIMPLE_H
#define AXSTAGESIMPLE_H

#include "../AxiomExprPlus.h"
#include "../FuzzyMultiDataExtAlgorithm.h"

namespace AxiomLib {

namespace FuzzyMultiDataExt {

class AXStageSimple
{
public:

	AXStageSimple(FuzzyDataSet* fuzzyDataSet,
						 ECStage* stage1);

	void initFromEnv(const Environment& env);

	// Функции доступа к набору аксиом
	void setAxioms(const std::vector<std::vector<AxiomExprPlus> > &initial);

	const AxiomExprPlus &getAX(int aType, int n) const;
	bool isAXSelected(int aType, int n) const;
	void setAXSelected(int aType, int n, bool value);
	void getAXSize(std::vector<int> &result) const;
	int getAXSize() const;
	int getAXSize(int aType) const;

	void recalculateMatterAxiomFunc(AxiomExprPlus& ax, int abType);

	void run();

	FuzzyMultiDataExtAlgorithm* getParent() const { return parent; }

private:

	// Набор точек выполнения на нештатном и нормальном поведении
	//typedef boost::tuples::tuple<PSatPointSet, PSatPointSet> ReducedSatPointSet;

	FuzzyMultiDataExtAlgorithm* parent;
	const FuzzyDataSet* fuzzyDataSet;
	const ECStage* stage1;

	// Параметры алгоритма настройки аксиом
	// Максимальное число аксиом в популяции
	int maxAxiomPopSize;
	// Максимальное число итераций алгоритма поиска аксиом
	int maxIterNum;
	// Максимальное значение целевой функции для аксиомы, если оно превышается, то поиск новых аксиом останавливается
	double maxAxiomRes;
	// Процент лучших аксиом популяции, которые сохраняются при сокращении популяции
	double percentBestAxioms;
	// Вес встречаемости в целевой функции
	double goalOccurenceWeight;

	std::string axiomNameTemplate;

	Logger* logger;


};

}

}

#endif // AXSTAGESIMPLE_H
