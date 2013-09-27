#include "ASStageMultiMarking.h"
#include "../ReducedRecognizerFactory.h"
#include "../GoalStrategyFactory.h"
#include "../Logger.h"

#include "AXStage.h"

#include "ASStage.h"
// Параметр показывает максимальный размер вектора с лучшими системами аксилом, который хранить для всех процессов
//  (должен быть достаточно большой, так как из сохраненных все равно потом выбиарются лучшие)
#define int_max_size_of_vector_axiomExprSet		100
// Малая величина чтобы работать с действительными числами
#define eps			0.0000000001
// Максимальное число систем аксиом, которое может быть внесено на каждом шаге алгоритма для одной исходной системы аксиом
#define int_max_number_of_renew_axiom_set		10

using namespace AxiomLib;
using namespace AxiomLib::FuzzyMultiDataExt;

void debug_common_bool(std::vector<std::vector<std::vector<bool> > > common){

    std::vector<std::vector<bool> > one_line;
    std::vector<bool> one_point;
    std::cout << "Find\t" << common.size() << "common subsequence\n";

    for (unsigned int i = 0; i < common.size(); i++){
        std::cout << "Common subsequence ?" << i << "\n";
        one_line.clear();
        one_line = common[i];

        for (unsigned int j = 0; j < one_line.size(); j++){
            std::cout << "Point " << j << "\n";
            one_point.clear();
            one_point = one_line[j];

            for (unsigned int k = 0; k < one_point.size(); k++){
                std::cout << one_point[k];
            }
        }
        std::cout << "\n\n";
    }
}

void debug_common_int(std::vector<std::vector<int> > common){

    std::vector<int> one_line;
    std::cout << "Find\t" << common.size() << "common subsequence\n";

    for (unsigned int i = 0; i < common.size(); i++){
        std::cout << "Common subsequence ?" << i << "\n";
        one_line.clear();
        one_line = common[i];

        for (unsigned int j = 0; j < one_line.size(); j++){
            std::cout << one_line[j];
        }
        std::cout << "\n\n";
    }
}

// From BaseClass
AxiomExprSetPlus &ASStageMultiMarking::getAS(int n){
    return bestAxiomSets[n];
}

const AxiomExprSetPlus &ASStageMultiMarking::getAS(int n) const{
    return bestAxiomSets[n];
}

int ASStageMultiMarking::getASSize() const{
    return bestAxiomSets.size();
}

void ASStageMultiMarking::recalculateMatterASFunc(AxiomExprSetPlus& as){}

void ASStageMultiMarking::setAxiomSets(const std::vector<AxiomExprSetPlus>& initialAS){
    bestAxiomSets = initialAS;
}

void ASStageMultiMarking::initFromEnv(const Environment& env){
    if (env.getIntParamValue (ccNumPoints, "ccNumPoints") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : ccNumPoints is undefined.");

    // Инициализируем стратегию вычисления целевой функции
    std::string goalStrategyName;
    GoalStrategyFactory gsf;
    if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::initFromEnv: goal-class is undefined.");
    goalStrategy = PGoalStrategy(gsf.create(goalStrategyName));
    goalStrategy->setParamsFromEnv(env);

    // Стоимость ошибки, если при сдвиге разметок осталась область разметки без соответствия в другой разметке
    if (env.getDoubleParamValue (shiftErrIn, "shiftErrIn") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : shiftErrIn is undefined.");
    // Степень расширения длины вхождения разметки (при поиске разметок во время их подбора)
    if (env.getDoubleParamValue (extendMarkUpLensLevel, "extendMarkUpLensLevel") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : extendMarkUpLensLevel is undefined.");
    if (env.getIntParamValue (maxAxiomSetPopSize, "maxAxiomSetPopSize") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : maxAxiomSetPopSize is undefined.");
    if (env.getIntParamValue (maxNumberOfSteps, "maxNumberOfSteps") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : maxNumberOfSteps is undefined.");
    if (env.getDoubleParamValue (bestAxiomSetGoal, "bestAxiomSetGoal") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : bestAxiomSetGoal is undefined.");
    if (env.getIntParamValue (numberOfBestAxiomSets, "numberOfBestAxiomSets") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : numberOfBestAxiomSets is undefined.");
    if (env.getDoubleParamValue (percentBestAxiomSets, "percentBestAxiomSets") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : percentBestAxiomSets is undefined.");
    if ((percentBestAxiomSets <= 0.0) || (percentBestAxiomSets > 1.0))
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : percentBestAxiomSets is out of range.");

    // Метрика для поиска наибольшей общей подпоследовательности
    std::string name_m;
    if (env.getStringParamValue (name_m, "DTWMetric") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : m is undefined.");
    else
        m=m->getMetric(name_m) ;
    // Метрика для распознавателя.
    if (env.getStringParamValue (name_m, "mForRecognize") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : mForRecognize is undefined.");
    else
        mForRecognize=mForRecognize->getMetric(name_m) ;
    //Порог для сравнения при подходе с подмножествами
    if (env.getDoubleParamValue (porog, "porog") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : porog is undefined.");

    if (env.getDoubleParamValue (porogForDTW, "porogForDTW") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : porogForDTW is undefined.");

    if (env.getBoolParamValue (areMultiMark, "areMultiMark") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : areMultiMark is undefined.");

    // Растяжка для нештатных при расплознавании(Размер окна)
    if (env.getDoubleParamValue (mStretch, "mStretch") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : mStretch is undefined.");
    if (env.getIntParamValue (maxNOP, "maxNOP") < 0)
            throw AxiomLibException("FuzzyMultiDataExt_AS::setParamsFromEnv : maxNOP is undefined.");

}


ASStageMultiMarking::ASStageMultiMarking(FuzzyDataSet* fuzzyDataSet,
                                         AXStage* stage2): fuzzyDataSet(fuzzyDataSet), stage2(stage2), logger(Logger::getInstance()){
}

/****************************************************************************
*					FuzzyMultiDataExt_AS::chooseBestMarkUp
*
*	Description:	Функция выбирает из всех вариантов для разметок лучшую
*	Parameters:		as - система аксиом, для которой выбирается разметка траектории нештатного типа
*					abType - тип нештатного поведения, для которого выбирается разметка
*					markUp - результат работы функции, т.е. выбранный вариант разметки эталонной траектории данного типа
*					genMarkUps - сформированные ранее кандидаты в разметки траекторий нештатного поведения
*					errFirstVal - заполняемое число ошибок I рода для лучшей по значению целевой функции эталонной разметки
*					errSecondVal - заполняемое число ошибок II рода для лучшей по значению целевой функции эталонной разметки
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::chooseBestMarkUp (AxiomExprSetPlus &as, int abType, std::vector <std::vector <bool> > &markUp, const std::vector <std::vector <std::vector <bool> > > &genMarkUps, int &errFirstVal, int &errSecondVal) {

    // Проверяем размер вектора кандидатов в разметки эталонной траектории заданного типа
        if (genMarkUps.size() < 1) {
                markUp.clear();
                errFirstVal = 0;
                errSecondVal = 0;
                return 1;
        }
        if (genMarkUps.size() == 1) {
                markUp = genMarkUps[0];
                double tmpGoalVal;
                matterAxiomSetFunc (as, abType, genMarkUps[0], tmpGoalVal, errFirstVal, errSecondVal);
                return 0;
        }
        // Подготавливаем необходимые перменные
        double goalVal, goalTmp;
        int errFirstTmp, errSecondTmp;
        int curBestIndex = 0;
        // Вычисляем число ошибок I и II рода для заданного варианта разметки эталонной траектории заданного типа и Выбираем лучшее значение целевой функции
        matterAxiomSetFunc (as, abType, genMarkUps[0], goalVal, errFirstVal, errSecondVal);
        for (unsigned int i = 1; i < genMarkUps.size(); i++) {
            matterAxiomSetFunc (as, abType, genMarkUps[i], goalTmp, errFirstTmp, errSecondTmp);
                if (goalTmp < goalVal) {
                        goalVal = goalTmp;
                        errFirstVal = errFirstTmp;
                        errSecondVal = errSecondTmp;
                        curBestIndex = i;
                }
        }
        // Сохраняем соотвествующую лучшую разметку
        markUp = genMarkUps[curBestIndex];
        return 0;
}
double minimum (const std::vector<double> &r){
        if(r.empty()) {
                throw AxiomLibException("minimum(): vector is empty");
        }

        double min=r[0];
        for(int i=0;i<r.size();i++){
                if (r[i]<min)
                        min=r[i];
        }
        return min;
}
std::vector<int> ASStageMultiMarking::convert(std::vector<double> resultInDouble){  
    std::vector<int> result;
    int toPush=0;
    for (int i=0;i<resultInDouble.size();i++){
        if (resultInDouble[i] < porogForDTW){
            toPush=1;
        }
        else{
            toPush=0;
        }
        result.push_back(toPush);
    }
return result;
}

// Функция запуска поиска разметки ethalon в markUp и запись результатов поиска в result
void ASStageMultiMarking::recognize (std::vector <std::vector<bool> > &markUp, const std::vector <std::vector<bool> > &genMarkUp, std::vector <int> &result){

    std::vector<double> resultInDouble(markUp.size(), 1.0);
    std::vector<double> temp_result;
    int windowSizeLowerLimit = std::max(int((1.0/mStretch)*genMarkUp.size()), 1);
    int windowSizeUpperLimit = std::max(int((mStretch)*genMarkUp.size()), 1);

        /*std::cerr << "windowSizeLowerLimit = " << windowSizeLowerLimit << ", windowSizeUpperLimit = " << windowSizeUpperLimit
			  << ", markUp.size() = " << markUp.size()
                                 << ", genMarkUp.size() = " << genMarkUp.size() << std::endl;*/
	if(genMarkUp.size() > 0) {
		for (int s=windowSizeUpperLimit;s<markUp.size();s++){
				//Logger::debug("Window : " + boost::lexical_cast<std::string>((1.0/mStretch)*etalon[i].size()) + " " + boost::lexical_cast<std::string>((mStretch)*etalon[i].size()));
                                //Logger::debug("Computing DTW...");
                        temp_result.clear();
			mForRecognize->computeDTWForMetric( markUp, s , windowSizeLowerLimit, windowSizeUpperLimit, genMarkUp, temp_result);
                                //Logger::debug("Setting distance...");
                        resultInDouble[s] = minimum(temp_result);
		}
        }

    result = convert(resultInDouble);
}


/****************************************************************************
*					FuzzyMultiDataExt_AS::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом:
*					1. разметка траекторий обучающей выборки и формирование
*					   расщиренных обобщенных разметок
*					2. разметка траекторий контрольной выборки и подсчет
*					   значения целевой функции
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double ASStageMultiMarking::matterAxiomSetFunc (AxiomExprSetPlus &as)  {
    int numOfClasses;
        std::vector <int> numOfMultiTS;
        std::vector < std::vector <int> > numOfTS;

        // Получение информации об обучающем наборе данных
        fuzzyDataSet->getClassSize (numOfClasses, numOfMultiTS, numOfTS);
        // для каждого класса нештатного поведения формируем расширенные обобщенные разметки
        std::vector < std::vector <std::vector <bool> > > resMarkUps, genMarkUps;
        int errFirstVal, errSecondVal;
        as.errFirst = 0;
        as.errSecond = 0;
        as.multiMarkUps.resize(numOfClasses);
        as.errors.resize(numOfClasses);
        std::vector <std::vector <bool> > razmetka;
        for (int abType = 0; abType < numOfClasses; abType++) {
                genMarkUps.clear();
                resMarkUps.resize (numOfMultiTS[abType]);
                for (int multiTS = 0; multiTS < numOfMultiTS[abType]; multiTS++) {
                        createRefMarkUp (as,FuzzyDataSet::Reference,abType,multiTS,resMarkUps[multiTS]);
                        razmetka=resMarkUps[multiTS];
                }
                // Упрощаем разметки - удаляем 0 в начале и в конце векторов - чтобы общую часть было искать проще
                simplifyMarkUps (resMarkUps);
                // На основании полученных разметок траекторий аномального поведения - производим их сдвиг и формируем варианты обобщенных разметок
                createMarkUpVariants (genMarkUps, resMarkUps);
                // Упрощаем вектора разметок - удаляем 0 в начале и в конце. Плюс возможно внесение некоторых изменений в саму разметку
                simplifyMarkUps (genMarkUps);
                // Добавляем минимальные варианты разметок - если общую часть выделить не удалось
                if (genMarkUps.size() < 1) {
                        logger->writeComment("Warinig: couldn't find common subsequence, creating simple markup");
                        createSimpleMarkUpVariants (genMarkUps, (int) as.axioms.size());
                }
                // Выбираем из всех разметок лучшую для данного класса нештатного поведения и сохраняем ее
                 chooseBestMarkUp (as, abType, as.multiMarkUps[abType], genMarkUps, errFirstVal, errSecondVal);

                as.errors[abType].first = errFirstVal;
                as.errors[abType].second = errSecondVal;

                as.errFirst += errFirstVal;
                as.errSecond += errSecondVal;
        }
        // Вычисление общего значения целевой фунекции для данной системы аксиом с учетом выбранных разметок эталонных траекторий

        as.goal = goalStrategy->compute(as.errFirst, as.errSecond);
        return as.goal;
}

double ASStageMultiMarking::matterAxiomSetFunc(AxiomExprSetPlus &as, const std::vector<std::vector<std::vector<std::vector<bool> > > >& markupVariants) {
    int numOfClasses = fuzzyDataSet->getClassCount();
        int errFirstVal, errSecondVal;
        as.errFirst = 0;
        as.errSecond = 0;
        as.multiMarkUps.resize(numOfClasses);
        as.errors.resize(numOfClasses);
        for (int abType = 0; abType < numOfClasses; abType++) {
                // Выбираем из всех разметок лучшую для данного класса нештатного поведения и сохраняем ее
                chooseBestMarkUp (as, abType, as.multiMarkUps[abType], markupVariants[abType], errFirstVal, errSecondVal);
                as.errors[abType].first = errFirstVal;
                as.errors[abType].second = errSecondVal;
                as.errFirst += errFirstVal;
                as.errSecond += errSecondVal;
        }
        // Вычисление общего значения целевой фунекции для данной системы аксиом с учетом выбранных разметок эталонных траекторий

        as.goal = goalStrategy->compute(as.errFirst, as.errSecond);
        return as.goal;
}

/****************************************************************************
*					FuzzyMultiDataExt_AS::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом с заданным вариантом
*					разметок эталонных траекторий
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*					markUps - разметки эталонных траекторий
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/

double ASStageMultiMarking::matterAxiomSetFunc (AxiomExprSetPlus &as, std::vector <std::vector <std::vector <bool> > > &markUps) {
    int tmpFirst, tmpSecond;
        double tmpGoal;
        as.errFirst = 0;
        as.errSecond = 0;
        as.errors.resize(markUps.size());
        for (int abType = 0; abType < (int) markUps.size(); abType++) {
                matterAxiomSetFunc (as, abType, markUps[abType], tmpGoal, tmpFirst, tmpSecond);
                as.errors[abType] = std::make_pair(tmpFirst, tmpSecond);
                as.errFirst += tmpFirst;
                as.errSecond += tmpSecond;
        }
        // Вычисление значения целевой функции для полученного числа ошибок I и II рода
        as.goal = goalStrategy->compute(as.errFirst, as.errSecond);
        return as.goal;
}


/****************************************************************************
*					FuzzyMultiDataExt_AS::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом для заданного
*					типа нештатного поведения и заданного варианта разметки
*					эталонной траектории этого типа
*	Parameters:		as - система аксиом, для которой считается значение целевой функции
*					abType - тип нештатного поведения, для которого считается значение целевой функции
*					genMarkUp - вариант разметки эталонной траектории заданного типа нештатного поведения
*					goalVal - заполняемое значение целевой функции
*					errFirstVal - заполняемое значение число ошибок I рода
*					errSecondVal - заполняемое значение число ошибок II рода
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/

double ASStageMultiMarking::matterAxiomSetFunc (AxiomExprSetPlus &as, int abType, const std::vector <std::vector<bool> > &genMarkUp, double &goalVal, int &errFirstVal, int &errSecondVal) {
    int numOfClasses;
        std::vector <int> numOfMultiTS;
        std::vector < std::vector <int> > numOfTS;
        // получаем информацию о числе траекторий контрольной выборки
        fuzzyDataSet->getTestSize (numOfClasses, numOfMultiTS, numOfTS);
        if ((abType < 0) || (abType >= numOfClasses))
                throw AxiomLibException("FuzzyMultiDataExt_AS::matterAxiomSetFunc: incorrect input number of abnormal type.");
        // Подготавливаем данные - какие измерения рядов задействованы системой аксиом
        std::vector <bool> dims;
        dims.resize (fuzzyDataSet->paramNamesSize(), false);
        for (unsigned int axNum = 0; axNum < as.axioms.size(); axNum++) {
                for (unsigned int ecNumI = 0; ecNumI < as.axioms[axNum]->expression.size(); ecNumI++) {
                        for (unsigned int ecNumII = 0; ecNumII < as.axioms[axNum]->expression[ecNumI].size(); ecNumII++) {
                                if ((as.axioms[axNum]->expression[ecNumI][ecNumII].dimension < 0) || (as.axioms[axNum]->expression[ecNumI][ecNumII].dimension >= (int) dims.size()))
                                        throw AxiomLibException("FuzzyMultiDataExt_AS::matterAxiomSetFunc : incomplete definition of elementary condition, it's data set dimension is out of range.");
                                dims[as.axioms[axNum]->expression[ecNumI][ecNumII].dimension] = true;
                        }
                }
        }
        // подготавливаем перменные для хранения числа ошибок
        errFirstVal = 0;
        errSecondVal = 0;
        // Выбираем очередную траекторию для заданного типа нештатного поведения и запускаем распознаватель
        std::vector <std::vector<bool> > curMarkUp;
        std::vector <int> result;
        int num;
        for (int t = 0; t < (int) fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Testing, abType); t++) {
                // разметка траектории контрольной выборки системой аксиом as
                createRefMarkUp(as,FuzzyDataSet::Testing,abType, t, curMarkUp);
                // Распознавание нештатного поведения в разметке ряда
                 recognize (curMarkUp, genMarkUp, result);
                // Вычисление числа ошибок первого и второго рода
                num = getStatistic (result);
                // Суммирование числа ошибок
                if (num == 0)
                        errSecondVal++;
                else
                        errFirstVal += num - 1;
        }
        // Пробегаем по всем рядам нормального поведения
        int numNormalMultiTS;
        std::vector <int> numNormalTS;
        fuzzyDataSet->getNormalTestSize (numNormalMultiTS, numNormalTS);
        for (int t = 0; t < (int) fuzzyDataSet->getMutiTSCount(FuzzyDataSet::Testing, -1); t++) {
                // размечаем траекторию нормального поведения
                createRefMarkUp(as,FuzzyDataSet::Testing,t, numNormalTS[t],curMarkUp);
                // Распознавание нештатного поведения в разметке ряда
                recognize (curMarkUp, genMarkUp, result);
                // Вычисление числа ошибок первого и второго рода
                num = getStatistic (result);
                // Суммирование числа ошибокover
                errFirstVal += num;
        }

        // Вычисление значения целевой функции для полученного числа ошибок I и II рода
        goalVal = goalStrategy->compute(errFirstVal, errSecondVal);
        return goalVal;
}


/****************************************************************************
*					FuzzyMultiDataExt_AS::matterAxiomSetFunc
*
*	Description:	Подсчет целевой функции для системы аксиом описанной в файле
*	Parameters:		baseDir - название каталога, где лежит требуемое описание системы аксиом
*					fileName - файл с описанием системы аксиом, для которой считается значение целевой функции
*					errFirst - число ошибок I рода
*					errSecond - число ошибок II рода
*	Returns:		double - значение целевой функции
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
/*
double ASStageMultiMarking::matterAxiomSetFunc (const std::string baseDir, const std::string fileName, int &errFirst, int &errSecond) const {
        // Инициализация системы аксиом из указанного файла
        AxiomExprSetPlus aesp;
        AxiomSetBase axiomSetBase;
        AxiomExprSetStructure aess;
        std::string axiomSetPath = baseDir;
        axiomSetPath.append("/");
        axiomSetPath.append(fileName);
        // Считываем информацию о системе аксиом в aess
        std::vector <std::string> dataSetParams;
        dataSetParams = fuzzyDataSet->getParamNames();
        axiomSetBase.readFromAS (axiomSetPath, aess, dataSetParams);
        aesp.initAxiomSetFromStructure (aess);
        // Вычисление значения целевой фукнции
        matterAxiomSetFunc (aesp);
        // Заполнение результатов и выход из функции
        errFirst = aesp.errFirst;
        errSecond = aesp.errSecond;
        double toReturn = aesp.goal;
        aesp.clear();
        return toReturn;
}

/****************************************************************************
*					FuzzyMultiDataExt_AS::getStatistic
*
*	Description:	Функция считает число вхождейни ненулевых областей
*	Parameters:		row - ряд, в котором ищутся ненулевые образы
*	Returns:		int - число ненулевых областей
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
inline int ASStageMultiMarking::getStatistic (std::vector <int> &row) {
    int num = 0;
        int i = 0;
        int j;
        int len = (int) row.size();
        while (i < len) {
                if (row[i] > 0) {
                        num++;
                        j = i+1;
                        while (j < len) {
                                if (row[j]!=row[j-1])
                                        break;
                                j++;
                        }
                        if ((j - i) > ccNumPoints)
                                num += j - i - ccNumPoints;
                        i = j;
                } else {
                        i++;
                }
        }
        return num;
}


/****************************************************************************
*					ASStageMultiMarking::createSimpleMarkUpVariants
*
*	Description:	Функция создает самый простой вариант разметок траекторий нештатного поведения
*					путем создания их, состоящи ищ одной акиомы.
*	Parameters:		genMarkUps - заполняемый вектор возможных вариантов эталонной разметки
*						для некоторого типа нештатного поведения
*					numOfAxioms - число аксиом в системе аксиом, для которой создаются варианты разметки
*	Returns:		int - число созданных разметок
*	Throws:			-
*	Author:			armkor
*	History:
*
****************************************************************************/

inline int ASStageMultiMarking::createSimpleMarkUpVariants (std::vector<std::vector<std::vector<bool> > > &genMarkUps, const int numOfAxioms) const {

     for (int i = 0; i < numOfAxioms; i++) {
         std::vector<std::vector<bool> > line;
             std::vector<bool> temp;
             for (int j=0;j<numOfAxioms;j++){
                 temp.push_back(false);
             }
             temp[i]=true;
             line.push_back(temp);
             genMarkUps.push_back(line);
     }
     return numOfAxioms;
}

void ASStageMultiMarking::createRefMarkUp ( AxiomExprSetPlus &as, FuzzyDataSet::DataSetDivisionType division, int classNo, int multiTSNo, std::vector <std::vector<bool> >& result){

    result.clear();
    std::vector<std::vector<double> > row;

    int multiTSLen = fuzzyDataSet->getMultiTSLength(division, classNo, multiTSNo);
    result.resize(multiTSLen);

    for (int i=0;i<fuzzyDataSet->getDimensionCount();i++){
        std::vector<double> temp;
        fuzzyDataSet->getTSByIndex(division,temp,classNo,multiTSNo,i);
        row.push_back(temp);
    }
       // Продумать, как делается разметка. Нужно ли рерайзить(изменять размер)
    as.enter (result,row, 0, row[0].size());
}


inline int ASStageMultiMarking::simplifyMarkUps (std::vector <std::vector <std::vector<bool> > > &markUps) const {
    int k, l;
    std::vector <std::vector<bool> > line;
    std::vector<bool> point;
    bool stop=false;

    for (int i =0;i< (int) markUps.size(); i++) {
        line.clear();line=markUps[i];
        // Убираем все 0 в начале вектора - их не учитываем
        k = 0;
        stop=false;
        while (k < (int) line.size()) {
            point.clear();point=line[k];
            for (int j=0;j<point.size();j++){
                if (point[j]){
                    stop=true;
                }
            }
            if (stop){
                break;
            }
                k++;
        }
        stop=false;
        // не учитываем все 0 в конце вектора разметки
        l = (int) line.size() - 1;
        while (l >= k) {
            point.clear();point=line[l];

            for (int j=0;j<point.size();j++){
                if (point[j]){
                    stop=true;
                }
            }
            if (stop){
                break;
            }
            l--;
        }

        if (l < k) {
                // Если были только 0 - то удаляем разметку из набора
                markUps.erase(markUps.begin() + i);
        } else {
                // Удаляем 0 в начале и в конце вектора разметки
                markUps[i].erase(markUps[i].begin() + l + 1, markUps[i].end());
                markUps[i].erase(markUps[i].begin(), markUps[i].begin() + k);
        }
}
return 0;

}



void ASStageMultiMarking::createAllMarkUpVariants(AxiomExprSetPlus &as, std::vector<std::vector<std::vector<std::vector<bool> > > > &markUpVariants) {
    int numOfClasses;
        std::vector <int> numOfMultiTS;
        std::vector < std::vector <int> > numOfTS;
        // Получение информации об обучающем наборе данных
        fuzzyDataSet->getClassSize (numOfClasses, numOfMultiTS, numOfTS);
        // для каждого класса нештатного поведения формируем расширенные обобщенные разметки
        std::vector <std::vector <std::vector<bool> > > resMarkUps;
        markUpVariants.resize(numOfClasses);
        for (int abType = 0; abType < numOfClasses; abType++) {
                markUpVariants[abType].clear();
                resMarkUps.resize (numOfMultiTS[abType]);

                // Создание разметки
                for (int multiTS = 0; multiTS < numOfMultiTS[abType]; multiTS++) {
                        createRefMarkUp (as, FuzzyDataSet::Reference, abType, multiTS, resMarkUps[multiTS]);
                }

                // Упрощаем разметки - удаляем 0 в начале и в конце векторов - чтобы общую часть было искать проще
                simplifyMarkUps (resMarkUps);

                // На основании полученных разметок траекторий аномального поведения - производим их сдвиг и формируем варианты обобщенных разметок
                createMarkUpVariants (markUpVariants[abType], resMarkUps);
                // Упрощаем вектора разметок - удаляем 0 в начале и в конце. Плюс возможно внесение некоторых изменений в саму разметку
                simplifyMarkUps (markUpVariants[abType]);
                // Добавляем минимальные варианты разметок - если общую часть выделить не удалось
                if (markUpVariants[abType].size() < 1) {
                        //logger->writeDebug("Warning: couldn't find common subsequence, creating simple markup");
                        createSimpleMarkUpVariants (markUpVariants[abType], (int) as.axioms.size());
                }
        }
}
/****************************************************************************
*					FuzzyMultiDataExt_AS::run()
*
*	Description: Находи разметки эталонных
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			armkor
*	History:
*
****************************************************************************/
std::vector<std::vector<int > > ASStageMultiMarking::stringIn(std::vector<std::vector<std::vector<bool> > > &in,int &numberOfAxiom){
    std::vector<std::vector<int > > result;
    std::vector<bool> point;
    std::vector<std::vector<bool> > oneLine;
    point.clear();oneLine.clear();
    int number;

    for (int i=0;i<in.size();i++){
        oneLine=in[i];
        std::vector<int > lineOfResult;
        for (int j=0;j<oneLine.size();j++){

            number=0;
            point.clear();
            point=oneLine[j];
            numberOfAxiom=point.size();
            for (int k=0;k<point.size();k++){
                if (point[k]){
                    number++;
                    lineOfResult.push_back(k+1);
                }
            }
            if (number==0){
                //Пустое множество
                lineOfResult.push_back(-1);
                lineOfResult.push_back(-1);
            }
            //Разделитель
            lineOfResult.push_back(-1);
        }
        result.push_back(lineOfResult);
    }
    return result;
}

std::vector<std::vector<std::vector<bool> > > ASStageMultiMarking::stringOut(std::vector<std::vector<int > > &genMarkUps,int& numberOfAxiom){
    std::vector<std::vector<std::vector<bool> > > result;
    // Количество траекторий
    std::vector<int > oneLine;
    for (int i=0;i<genMarkUps.size();i++){
        // Перешли к одной траектории
        std::vector<std::vector<bool> > lineOfResult;

        oneLine.clear();
        oneLine=genMarkUps[i];
        int j=0;

        while (j < oneLine.size()){
            // Подготавливаем точку
            std::vector<bool> pointOfResult;
            pointOfResult.resize(numberOfAxiom);
            for (int k=0;k<pointOfResult.size();k++){
                pointOfResult[k]=false;
            }

            if((j+1)<oneLine.size()){
                //Пустое множество
                if (oneLine[j]==-1 && oneLine[j+1]==-1){
                    lineOfResult.push_back(pointOfResult);
                    j=j+2;
                }
                else{
                    while(oneLine[j]!=-1 && j < oneLine.size()){
                        pointOfResult[oneLine[j]-1]=true;
                        j++;
                    }
                    lineOfResult.push_back(pointOfResult);
                }
            }
            j++;
        }

    result.push_back(lineOfResult);
    }
    return result;
}


inline int ASStageMultiMarking::createMarkUpVariants (std::vector<std::vector<std::vector<bool> > > &genMarkUps,std::vector<std::vector<std::vector<bool> > >  &resMarkUps) {
    if (!areMultiMark){
        std::vector<std::vector<int > > _resMarkUps,_genMarkUps;
        int numberOfAxiom;
        _resMarkUps = stringIn(resMarkUps,numberOfAxiom);

        std::vector<std::vector<int > > temp;
        _genMarkUps.push_back(_resMarkUps[0]);
        temp.push_back(_resMarkUps[0]);

        for (int i=1;i<_resMarkUps.size();i++){
            for (int j=0;j<_genMarkUps.size();j++){
                std::vector<std::vector<int> > common =findCommonSubsequence(_resMarkUps[i],_genMarkUps[j],distanceFunctionForAxiom,choiceFunctionForAxiom,this->porog,this->maxNOP);
                debug_common_int(common);
                if (common.size()>0){
                    for (int k=0;k<common.size();k++){
                        temp.push_back(common[k]);
                    }
                }
            }
            _genMarkUps=temp;
            temp.clear();
        }
        genMarkUps=stringOut(_genMarkUps,numberOfAxiom);
    }
    else {
        std::vector<std::vector<std::vector<bool> > > temp;
        genMarkUps.push_back(resMarkUps[0]);
        temp.push_back(resMarkUps[0]);
        for (int i=1;i<resMarkUps.size();i++){
            for (int j=0;j<genMarkUps.size();j++){
                std::vector<std::vector<std::vector<bool> > > common =findCommonSubsequence(resMarkUps[i],genMarkUps[j],DistanceFunctor(this->m),choiceFunctionForMultiMark,this->porog,this->maxNOP);
                debug_common_bool(common);
                if (common.size()>0){
                    for (int k=0;k<common.size();k++){
                        temp.push_back(common[k]);
                    }
                }
            }
            genMarkUps=temp;
            temp.clear();
        }

    }
        return 0;
}



// From run

/****************************************************************************
*					FuzzyMultiDataExt_AS::sortAxiomSets
*
*	Description:	Функция сортирует вектор систем аксиом по неубыванию целевой фунекции
*	Parameters:		axiomSets - вектор сортируемых систем аксиом
*					indecies - вектор индексов систем аксиом в порядке неубывания значения целевой функции
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::sortAxiomSets (const std::vector <AxiomExprSetPlus> &axiomSets, std::vector <int> &indecies) const {
    indecies.resize (axiomSets.size());
        for (int i = 0; i < (int) axiomSets.size(); i++)
                indecies[i] = i;
        int tmp;
        for (int i = (int) axiomSets.size() - 1; i > 0; i--) {
                for (int j = 0; j < i; j++) {
                        if (axiomSets[indecies[j]].goal > axiomSets[indecies[j+1]].goal) {
                                tmp = indecies[j+1];
                                indecies[j+1] = indecies[j];
                                indecies[j] = tmp;
                        }
                }
        }
        return 0;
}
/****************************************************************************
*					FuzzyMultiDataExt_AS::addAxiomSets
*
*	Description:	Функция добавляет системы аксиом из одного вектора, индексы
*					которых перечислены во входном векторе, в другой вектор.
*	Parameters:		nextStepAxiomSets - вектор систем аксиом, в который добавляются новые системы
*					newAxiomSets - вектор систем, из которого переносятся системы аксиом
*					indicesOfBestSets - вектор индексов переносимых систем аксиом
*	Returns:		0
*	Throws:			AxiomLibException - если вектор, в который сохранять системы аксиом не пуст
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::addAxiomSets(std::vector <AxiomExprSetPlus> &nextStepAxiomSets, std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets) const {
    if (nextStepAxiomSets.size() > 0)
                throw AxiomLibException("Error in FuzzyMultiDataExt_AS::addAxiomSets: input arguments out of consistency.");
        // Определяем сколько систем аксиом добавлять
        unsigned int toAdd = 0;
        for (unsigned int i = 0; i < indicesOfBestSets.size(); i++) {
                if ((indicesOfBestSets[i] >= 0 ) && (indicesOfBestSets[i] < (int) newAxiomSets.size()))
                        toAdd++;
        }
        // Заносим все новые системы аксиом в вектор всех систем в соответствии с указанным индексом
        unsigned int curSize = 0; // nextStepAxiomSets.size();
        nextStepAxiomSets.resize (toAdd);
        for (unsigned int i = 0; i < indicesOfBestSets.size(); i++) {
                if ((indicesOfBestSets[i] >= 0 ) && (indicesOfBestSets[i] < (int) newAxiomSets.size())) {
                        nextStepAxiomSets[curSize] = newAxiomSets[indicesOfBestSets[i]];
                        curSize++;
                }
        }
        return 0;
}
/****************************************************************************
*					FuzzyMultiDataExt_AS::chooseIndicesOfBestAxiomSets
*
*	Description:	Функция выбирает из вектора систем аксиом заданное число
*					лучших, причем их значение целевой функции должно быть
*					меньше заданного значения.
*	Parameters:		newAxiomSets - вектор систем аксиом, из которых выбриаются лучшие
*					indicesOfBestSets - заполняемый вектор индексов лучших систем аксиом
*					goal - значение целевой функции, для критерия выбора лучших систем
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::chooseIndicesOfBestAxiomSets (std::vector <AxiomExprSetPlus> &newAxiomSets, std::vector <int> &indicesOfBestSets, double &goal) const {
    // Сортируем вектор систем аксиом по значению целевой функции
    std::vector <int> indecies;
    sortAxiomSets (newAxiomSets, indecies);
    // Ищем первую содержательную систему аксиом
    int first = 0;
    while (first < (int) indecies.size()) {
            if (newAxiomSets[indecies[first]].goal > -eps)
                    break;
            first++;
    }
    // Ищем крайний индекс, начиная с которого все системы аксиом имеют значение целевой функции больше заданного goal
    int last = first;
    while (last < (int) indecies.size()) {
            if ((newAxiomSets[indecies[last]].goal - goal) > eps)
                    break;
            last++;
    }
    // Если таких не нашлось - то выходим
    if ((first == (int) indecies.size()) || (first == last)) {
            indicesOfBestSets.clear();
            return 0;
    }

    // Определяем максимальное число элементов в результирующем векторе
    //  Выбираем число лучших систем аксиом и случайно выбранных
    int numBest, numRand, num;
    numBest = round ((double) int_max_number_of_renew_axiom_set * percentBestAxiomSets);
    if (numBest > (last - first)) {
            numBest = last - first;
            numRand = round ((double) numBest * (1.0 - percentBestAxiomSets) / percentBestAxiomSets);
            num = numBest + numRand;
    } else {
            numRand = int_max_number_of_renew_axiom_set - numBest;
            num = int_max_number_of_renew_axiom_set;
    }

    // Если доступно меньшее число систем аксиом
    if (((int) newAxiomSets.size() - first) <= num) {
            num = (int) newAxiomSets.size() - first;
            indicesOfBestSets.resize (num);
            for (int i = 0; i < num; i++, first++)
                    indicesOfBestSets[i] = indecies[first];
            return 0;
    }

    // Отбираем случайные системы аксиом
    int chooseDelta;
    chooseDelta = indecies.size() - first - numBest;
    std::vector <int> jRand;
    jRand.resize (numRand);
    int cur;
    int condition;
    for (int t = 0; t < numRand; t++) {
            // Случайно выбираем число из отрезка [0, chooseDelta-1]
            cur = first + numBest + (int) (((double) rand () / ((double) RAND_MAX + 1.0)) * (double) chooseDelta);
            // проверяем - не выбирали ли его раньше - если выбирали, то сдвигаем
            do {
                    condition = false;
                    for (int s = 0; s < t; s++) {
                            if (cur == jRand[s]) {
                                    cur++;
                                    condition = true;
                                    break;
                            }
                    }
            } while (condition);
            // Сохраняем сделанный выбор
            jRand[t] = cur;
            // Уменьшаем диапазон выбора
            chooseDelta--;
    }
    // Сохраняем все выбранные индексы
    indicesOfBestSets.resize (num);
    cur = first;
    for (int i = 0; i < numBest; i++, cur++)
            indicesOfBestSets[i] = indecies[cur];
    cur = 0;
    for (int i = numBest; i < num; i++, cur++)
            indicesOfBestSets[i] = indecies[jRand[cur]];
    return 0;

}
/****************************************************************************
*					FuzzyMultiDataExt_AS::cutDownAxiomSets
*
*	Description:	Функция сокращает число систем аксиом во входном векторе до
*					заданного в параметрах класса значения.
*	Parameters:		axiomSets - вектор систем аксиом, который подлежит сокращению
*					до заданного размера
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::cutDownAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) const {
        // Проверка входных параметров
    if ((maxAxiomSetPopSize < 1) || ( (int) axiomSets.size() <= maxAxiomSetPopSize))
                return 0;

        // Сортировка аксиом по значению целевой функции
        std::vector <int> indecies;
        sortAxiomSets (axiomSets, indecies);

        // Определение числа лучших аксиом, сохраняемых в популяции
        unsigned int numBestToSave, numToChoose, numFrom;
        numBestToSave = (unsigned int) round (percentBestAxiomSets*maxAxiomSetPopSize);
        numToChoose = (unsigned int) maxAxiomSetPopSize - numBestToSave;

        numFrom = axiomSets.size() - numBestToSave;
        std::vector <bool> chosen;
        chosen.resize (numFrom, false);
        int curChoice;
        std::vector <int> toSave;
        toSave.resize (maxAxiomSetPopSize);
        // Заносим в вектор сохраняемых элементов - заданное число лучших
        for (unsigned int i = 0; i < numBestToSave; i++) {
                toSave[i] = indecies[i];
        }
        // Случайный выбор оставшихся
        for (unsigned int i = 0; i < numToChoose; i++) {
                curChoice = round (((double) rand() / (double) RAND_MAX) * (double) (numFrom - i - 1));
                for (unsigned int t = (unsigned int) curChoice; t < numFrom; t++)
                        if (!chosen[t]) {
                                chosen[t] = true;
                                toSave[numBestToSave + i] = indecies[numBestToSave + t];
                                break;
                        }
        }

        // Сортировка массива выбранных для сохранения элементов по убыванию
        unsigned int tmpInt;
        for (unsigned int i = maxAxiomSetPopSize - 1; i > 0; i--) {
                for (unsigned int j = 0; j < i; j++) {
                        if (toSave[j] < toSave[j+1]) {
                                tmpInt = toSave[j+1];
                                toSave[j+1] = toSave[j];
                                toSave[j] = tmpInt;
                        }
                }
        }

        // Удаление невыбранных систем аксиом
        // Очищаем содержимое элементов вектора аксиом
        if ((toSave[0] + 1) < (int) axiomSets.size()) {
                for (unsigned int j = toSave[0] + 1; j < axiomSets.size(); j++)
                        axiomSets[j].clear();
                axiomSets.erase (axiomSets.begin() + toSave[0] + 1, axiomSets.end());
        }
        for (int i = 1; i < maxAxiomSetPopSize; i++) {
                if ((toSave[i] + 1) < toSave[i-1]) {
                        // Очищаем содержимое элементов вектора аксиом
                        for (int j = toSave[i] + 1; j < toSave[i-1]; j++)
                                axiomSets[j].clear();
                        // Удаляем элементы векторов
                        axiomSets.erase (axiomSets.begin() + toSave[i] + 1, axiomSets.begin() + toSave[i-1]);
                }
        }
        // Очищаем содержимое элементов вектора аксиом
        if (toSave[maxAxiomSetPopSize-1] > 0) {
                for (int j = 0; j < toSave[maxAxiomSetPopSize-1]; j++) {
                        axiomSets[j].clear();
                }
                axiomSets.erase (axiomSets.begin(), axiomSets.begin() + toSave[maxAxiomSetPopSize-1]);
        }
        return 0;
}

/****************************************************************************
*					FuzzyMultiDataExt_AS::sortBestAxiomSets
*
*	Description:	Функция сокращает число элементов вектора лучших систем аксиом
*					до заданного по умолчанию или в конфигурационном файле значения
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::sortBestAxiomSets (void) {
        // Если число элементов в векторе лучших систем аксиом меньше заданного параметра - то просто выходим
    if ((int) bestAxiomSets.size() <= numberOfBestAxiomSets)
                return 0;
        // Если число тех, которые должны выжить - меньше 1, то просто удаляем все элементы
        if (numberOfBestAxiomSets < 1) {
                for (unsigned int i = 0; i < bestAxiomSets.size(); i++)
                        bestAxiomSets[i].clear();
                bestAxiomSets.clear();
                return 0;
        }
        // Выбираем из вектора заданный набор худших систем аксиом и удаляем их
        int numToDel;
        numToDel = (int) bestAxiomSets.size() - numberOfBestAxiomSets;
        std::vector <int> indeciesToDel;
        indeciesToDel.resize(numToDel);
        for (int i = 0; i < numToDel; i++) {
                indeciesToDel[i] = i;
        }
        int curIndex;
        double curMin;
        for (int i = numToDel; i < (int) bestAxiomSets.size(); i++) {
                curMin = bestAxiomSets[i].goal;
                curIndex = -1;
                for (int j = 0; j < numToDel; j++) {
                        if (curMin > bestAxiomSets[indeciesToDel[j]].goal) {
                                curMin = bestAxiomSets[indeciesToDel[j]].goal;
                                curIndex = j;
                        }
                }
                if (curIndex >= 0) {
                        indeciesToDel[curIndex] = i;
                }
        }
        // Вектор индексов составлен - Сортируем его по убыванию
        for (int i = numToDel - 1; i > 0; i--) {
                for (int j = 0; j < i; j++) {
                        if (indeciesToDel[j] < indeciesToDel[j+1]) {
                                curIndex = indeciesToDel[j+1];
                                indeciesToDel[j+1] = indeciesToDel[j];
                                indeciesToDel[j] = curIndex;
                        }
                }
        }
        // Удаляем системы аксиом в соответствии с выбранным вектором индексов
        for (int i = 0; i < numToDel; i++) {
                bestAxiomSets[indeciesToDel[i]].clear();
                bestAxiomSets.erase (bestAxiomSets.begin() + indeciesToDel[i]);
        }
        return 0;
}
/****************************************************************************
*					FuzzyMultiDataExt_AS::addToBestAxiomSets
*
*	Description:	Функция сохраняет поданную набор систем аксиом в векторе лучших систем,
*					если там есть еще место или заменяет систему аксиом из вектора на данную
*					систему.
*	Parameters:		axiomExprSetPlus - сохраняемая система аксиом
*	Returns:		-1 - если в ходе выполнения возникла ошибка
*					0  - если система аксиом была сохранена
*					1  - если целевая функция у данной системы аксиом больше, чем у системе
*						 в векторе лучших систем, поэтому данная система не была сохранена
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
int ASStageMultiMarking::addToBestAxiomSets (std::vector <AxiomExprSetPlus> &axiomSets) {
        // Копируем содержимое вектора bestAxiomSets во временную переменную
    std::vector <AxiomExprSetPlus> tmpBest;
        tmpBest.resize (bestAxiomSets.size());
        for (unsigned int i = 0; i < bestAxiomSets.size(); i++) {
                tmpBest[i] = bestAxiomSets[i];
                bestAxiomSets[i].clear();
        }
        // Выстраиваем вектора tmpBest и axiomSets по возрастанию
        std::vector <int> indeciesSets, indeciesBest;
        sortAxiomSets (axiomSets, indeciesSets);
        sortAxiomSets (tmpBest, indeciesBest);
        // Выбираем какие системы аксиом сохранять
        unsigned int s = 0;
        unsigned int b = 0;
        unsigned int sMax = axiomSets.size();
        unsigned int bMax = tmpBest.size();
        unsigned int upTo = int_max_size_of_vector_axiomExprSet;
        if ((sMax + bMax) < upTo) {
                upTo = sMax + bMax;
                s = sMax;
                b = bMax;
        }
        while ((s + b) < upTo) {
                if (b >= bMax) {
                        s++;
                        continue;
                }
                if (s >= sMax) {
                        b++;
                        continue;
                }
                if (tmpBest[indeciesBest[b]].goal < axiomSets[indeciesSets[s]].goal)
                        b++;
                else
                        s++;
        }

        // Переносим выбранные системы аксиом в вектор лучших
        bestAxiomSets.resize (upTo);
        for (unsigned int i = 0; i < b; i++)
                bestAxiomSets[i] = tmpBest[i];
        for (unsigned int i = 0; i < s; i++, b++)
                bestAxiomSets[b] = axiomSets[i];

        // Очищаем использованные вектора
        for (unsigned int i = 0; i < axiomSets.size(); i++)
                axiomSets[i].clear();
        axiomSets.clear();
        for (unsigned int i = 0; i < tmpBest.size(); i++)
                tmpBest[i].clear();
        tmpBest.clear();
        return 0;
}


/****************************************************************************
*					FuzzyMultiDataExt_AS::run()
*
*	Description: Построение системы аксиом
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			armkor
*	History:
*
****************************************************************************/

void ASStageMultiMarking::run(){
    std::cerr << " Start run";
    // stage2 хранит аксиомы для каждого класса нештатного поведения
    std::vector<int> sizeVector;
    stage2->getAXSize(sizeVector);
    logger->writeComment("Enter AxiomCreator");
    //  Создаем вектор всех аксиом - чтобы его можно было использовать при составленнии систем аксиом
    int numOfAxioms = 0;
    for (unsigned int i = 0; i < sizeVector.size(); i++) {
            for(int j = 0; j < sizeVector[i]; j++) {
                    if(stage2->isAXSelected(i, j)) {
                            ++numOfAxioms;
                    }
            }
    }

    // Вектор аксиом, создаваемый по мере формирования аксиом из элементраных условий
    std::vector <AxiomExpr> bestAxioms;
    bestAxioms.reserve(numOfAxioms);

    for (unsigned int i = 0; i < sizeVector.size(); i++) {
            for(int j = 0; j < sizeVector[i]; j++) {
                    if(stage2->isAXSelected(i, j)) {
                            AxiomExpr ax = stage2->getAX(i, j);
                            bestAxioms.push_back(ax);
                    }
            }
    }

    bestAxiomSets.clear();

    // Создаем набор систем аксиом
    std::vector <AxiomExprSetPlus> axiomSets, newAxiomSets;
    std::vector < std::vector <AxiomExprSetPlus> > nextStepAxiomSets;
    axiomSets.resize(numOfAxioms);

    logger->writeComment("Create one axiom system");
    // Создаем набор систем аксиом, каждая из которых будет содержать только одну аксиому
	//#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < numOfAxioms; i++) {
        // Формируем систему аксиом из одной аксиомы
        axiomSets[i].addAxiom (bestAxioms[i]);
        // Устанавливаем статистику
        axiomSets[i].axiomsIndex.push_back(i);
        // Вычисляем значение целевой функции для такой системы аксиом + учет типа разметки
        matterAxiomSetFunc (axiomSets[i]);

    }
    std::cerr << "Step 1\n";
    // Итерационный алгоритм построения системы аксиом
    bool condition = true;
    std::vector <int> indicesOfBestSets;
    nextStepAxiomSets.clear();
    int iterNum = 0;
    double defMaxGoal;
    defMaxGoal = -1.0; // (axiomSets[axSet].goal + 1)*2.0; // Делаем defMaxGoal достаточно большим (заведомо больше axiomSets[axSet].goal - даже с учетом вычислительной погрешности)
    // Устанавливаем размер вектора по числу аксиом
    unsigned int wholeNum = 0;
    newAxiomSets.resize (bestAxioms.size());
    while (condition) {
            // Подготавливаем переменную для вектора будущих систем аксиом
            wholeNum = 0;
            nextStepAxiomSets.resize (axiomSets.size());
            for (unsigned int axSet = 0; axSet < axiomSets.size(); axSet++) {
                    // Очищаем содержимое вектора с промежуточными результатами
                    for (unsigned int u = 0; u < newAxiomSets.size(); u++)
                            newAxiomSets[u].clear();

                    // Для каждой аксиомы - создаем новую систему на основе данной аксиомы и системы axiomSets[axSet]

                    // Тут ищем номер в bestAxioms последней аксиомы. Номера аксиом в axiomsIndex упорядочены
                    int size=axiomSets[axSet].axiomsIndex.size();
                    unsigned int lastAxiomInSet=axiomSets[axSet].axiomsIndex[size-1];

					//#pragma omp parallel for schedule(dynamic, 1)
                    for (int ax = lastAxiomInSet + 1; ax < (int) bestAxioms.size(); ax++) {
                            // Для всех аксиом, еще не входящих в рассматриваемую систему аксиом создаем новую систему
                                    // создаем новую систему аксиом на основе выбранной - добавляя в нее еще невходивщую аксиому
                                    newAxiomSets[ax] = axiomSets[axSet];
                                    tryAddAxiom(newAxiomSets[ax], bestAxioms[ax], ax);

                    }
                    //Нам не нужно проверять, а есть ли аксиома из-за способа перебора
                    std::cerr << "Step 2\n";
                    // выбираем заданное число лучших систем аксиом и добавляем их в популяцию на следующем шаге алгоритма
                    indicesOfBestSets.clear();
                    chooseIndicesOfBestAxiomSets (newAxiomSets, indicesOfBestSets, axiomSets[axSet].goal);
                    wholeNum += indicesOfBestSets.size();
                    addAxiomSets(nextStepAxiomSets[axSet], newAxiomSets, indicesOfBestSets);
            }
            std::cerr << "Step 3\n";
            // Переносим исходные системы аксиом в специальный вектор результатов и по ходу дела очищаем их содержимое
            addToBestAxiomSets (axiomSets);
            // Переносим системы аксиом для следующего шага в axiomSets
            axiomSets.resize(wholeNum);
            unsigned int cur = 0;
            double curBestVal = -1.0;

            for (unsigned int y = 0; y < nextStepAxiomSets.size(); y++) {
                    for (unsigned int u = 0; u < nextStepAxiomSets[y].size(); u++, cur++) {
                            axiomSets[cur] = nextStepAxiomSets[y][u];
                            nextStepAxiomSets[y][u].clear();
                            if ((curBestVal < -eps) || ((curBestVal - axiomSets[cur].goal) > eps))
                                    curBestVal = axiomSets[cur].goal;
                    }
                    nextStepAxiomSets[y].clear();
            }
            nextStepAxiomSets.clear();
            std::cerr << "Step 3_1\n";
            // - Убрано, так как рост числа систем аксиом уже ограничивается в функции chooseIndicesOfBestAxiomSets
            // Ограничиваем число систем аксиом, чтобы оно не могло разрастаться со скоростью геометрической прогресси
            cutDownAxiomSets (axiomSets);

            // Увеличиваем число итераций, проделанных алгоритмом
            iterNum++;

            // Проверяем критерии останова:
            // - выходим, если не было создано ни одной новой системы аксиом с лучшим значением целевой функции
            if (axiomSets.size() < 1) {
                    logger->writeComment("Stopping because no new axiom sets created");
                    condition = false;
                    break;
            }
            // - выходим, если число итераций больше заданного предела
            if ((iterNum >= maxNumberOfSteps) || ((curBestVal - bestAxiomSetGoal) < eps)) {
                    if(iterNum >= maxNumberOfSteps) {
                            logger->writeComment("Stopping because maximum number of iterations reached");
                    } else {
                            logger->writeComment("Stopping because best axiom set goal value reached");
                    }
                    // Переносим исходные системы аксиом в специальный вектор результатов и по ходу дела очищаем их содержимое
                    addToBestAxiomSets (axiomSets);
                    condition = false;
                    break;
            }
    }

    // Сортировка лучших систем аксиом до заданного числа элементов
  std::cerr << "Step 4\n";
    sortBestAxiomSets ();
    std::sort(bestAxiomSets.begin(), bestAxiomSets.end());
std::cerr << "Step 5\n";
}

/****************************************************************************
*					::run()
*
*	Description: Добавление аксиомы.Создаем разметки эталонных траекторий.
*                    Считаем целевую функцию.
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			armkor
*	History:
*
****************************************************************************/

int ASStageMultiMarking::tryAddAxiom(AxiomExprSetPlus &as, const AxiomExpr &ax, int axIndex) {
    as.addAxiom(ax);
    as.axiomsIndex.push_back(axIndex);
    std::vector<std::vector<std::vector<std::vector<bool> > > > markUpVariants;
    createAllMarkUpVariants(as, markUpVariants);
    matterAxiomSetFunc (as, markUpVariants);
    return -1;
}
