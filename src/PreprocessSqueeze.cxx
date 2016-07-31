/****************************************************************************
*
*				Функции класса PreprocessSqueeze
*
****************************************************************************/

#include "PreprocessSqueeze.h"
#include "TemplateRecognizer.h"

using namespace AxiomLib;

#define noValue -1.0
#define minRatio 1.1
#define maxRatio 10.0

// Коструктор класса
PreprocessSqueeze::PreprocessSqueeze (void) {
	squeezeParam = 1.0;
	squeezeParamMin = 0.1;
	squeezeParamMax = 10.0;
	LagrangeDegree = 4;
	factorials.resize(LagrangeDegree + 1);
	factorials[0] = 1;
	for (unsigned int i = 1; i <= LagrangeDegree; i++)
		factorials[i] = factorials[i-1] * (double) i;
	goalStrategy = NULL;
} 


// Деструктор класса
PreprocessSqueeze::~PreprocessSqueeze (void) {
	if (goalStrategy != NULL) delete goalStrategy;
	
}


/****************************************************************************
*						PreprocessSqueeze::initFromEnv
*
*       Description:	Функция устанавливает параметры класса по данным, 
*						полученным из класса окружения
*       Parameters:		env - окружение, содержашее необходимые параметры
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSqueeze::initFromEnv (const Environment& env) {
	// Установка параметров класса из окружения
	// Если один из параметров не установлен, то используется значение по умолчанию
	env.getDoubleParamValue (squeezeParam, "squeezeParam");
	env.getDoubleParamValue (squeezeParamMin, "squeezeParamMin");
	env.getDoubleParamValue (squeezeParamMax, "squeezeParamMax");
	// Проверка корректности установленных параметров
	if ((squeezeParamMin > squeezeParamMax) || (squeezeParamMin < 0.0) || 
		(squeezeParam < squeezeParamMin) || (squeezeParam > squeezeParamMax))
		throw AxiomLibException ("PreprocessSqueeze::initFromEnv : incompatible parameters in environment.");
	// Установка параметра LagrangeDegree
	int tmpInt;
	if (env.getIntParamValue (tmpInt, "LagrangeCoeffDegree") >= 0) {
		if ((tmpInt < 1) || (tmpInt > 20))
			throw AxiomLibException ("PreprocessSqueeze::initFromEnv : LagrangeCoeffDegree of an incompatible value in environment.");
		LagrangeDegree = (unsigned int) tmpInt;
		factorials.resize(LagrangeDegree + 1);
		factorials[0] = 1.0;
		for (unsigned int i = 1; i <= LagrangeDegree; i++)
			factorials[i] = factorials[i-1] * (double) i;
	}
	// Инициализируем стратегию вычисления целевой функции
	std::string goalStrategyName;
	GoalStrategyFactory gsf;
	if (env.getStringParamValue(goalStrategyName, "goalClass") >= 0) {
		goalStrategy = gsf.create(goalStrategyName);
		goalStrategy->setParamsFromEnv(env);
	}
	return 0;
}


/****************************************************************************
*						PreprocessSqueeze::copy
*
*       Description:	Функция создания копии алгоритма предобработки
*       Parameters:		void
*       Returns:		Preprocess* - указатель на созданную копию данного предобработчика
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
Preprocess* PreprocessSqueeze::copy (void) {
	PreprocessSqueeze* toReturn;
	toReturn = new PreprocessSqueeze;
	toReturn->setParams (this->squeezeParam, this->squeezeParamMin, this->squeezeParamMax, this->LagrangeDegree);
	return (Preprocess*) toReturn;
}


/****************************************************************************
*						PreprocessSqueeze::setParams
*
*       Description:	Функция устанавливает специфичные для данного класса параметры
*       Parameters:		sp - соответствует переменной класса squeezeParam
*						spMin - минимально-возможное значение для переменной squeezeParam
*						spMax - максимально-возможное значение для переменной squeezeParam
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSqueeze::setParams (double sp, double spMin, double spMax, unsigned int ld) {
	if ((spMin > spMax) || (spMin < 0.0) || (sp < spMin) || (sp > spMax))
		throw AxiomLibException ("PreprocessSqueeze::setParams : incompatible input parameters.");
	squeezeParam = sp;
	squeezeParamMin = spMin;
	squeezeParamMax = spMax;
	LagrangeDegree = ld;
	if ((ld < 1) || (ld > 20))
		throw AxiomLibException ("PreprocessSqueeze::setParams : LagrangeCoeffDegree of an incompatible value.");
	factorials.resize(LagrangeDegree + 1);
	factorials[0] = 1.0;
	for (unsigned int i = 1; i <= LagrangeDegree; i++)
		factorials[i] = factorials[i-1] * (double) i;
	return 0;
}


/****************************************************************************
*						PreprocessSqueeze::setParams
*
*       Description:	Функция устанавливает специфичные для данного класса параметры
*       Parameters:		sp - соответствует переменной класса squeezeParam
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSqueeze::setParams (double sp) {
	if ((sp < squeezeParamMin) || (sp > squeezeParamMax))
		throw AxiomLibException ("PreprocessSqueeze::setParams : incompatible input parameter.");
	squeezeParam = sp;
	return 0;
}


/****************************************************************************
*						PreprocessSqueeze::learn
*
*       Description:	Функция обучения данного алгоритма предобработки
*       Parameters:		templateRecognizer - шаблон распознавателя, на котором
*						проверять качество обучения данного алгоритма предобработки
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
double PreprocessSqueeze::learn (TemplateRecognizer &templateRecognizer) {
	// 1. Проверка начальных значений
	if (goalStrategy == NULL)
		throw AxiomLibException("PreprocessSqueeze::learn: goalClass is not set in PreprocessSqueeze, but needed for learn function.");

	// 2. Подготовка переменных для поиска
	// 2.1. Начальные значения параметра, по которым вести поиск оптимального значения
	std::vector <double> curSqueezeParams;
	curSqueezeParams.resize(5);
	// Задание начальных значений элементам вектора curSqueezeParams
	curSqueezeParams[2] = squeezeParam;
	curSqueezeParams[0] = squeezeParam / maxRatio;
	curSqueezeParams[4] = squeezeParam * maxRatio;
	if (curSqueezeParams[0] < squeezeParamMin) 
		curSqueezeParams[0] = squeezeParamMin;
	if (curSqueezeParams[4] > squeezeParamMax) 
		curSqueezeParams[4] = squeezeParamMax;
	curSqueezeParams[1] = 0.5*(curSqueezeParams[0] + curSqueezeParams[2]);		
	curSqueezeParams[3] = 0.5*(curSqueezeParams[2] + curSqueezeParams[4]);

	// 2.2. Вектор для значений целевой функции, соответствующей значениям из curSqueezeParams
	std::vector <double> curRes; 
	curRes.resize(5);
	for (unsigned int i=0; i < 5; i++) {
		curRes[i] = noValue;
	}
	
	// 3. Запуск рекурсивного алгоритма поиска 
	return sublearn (templateRecognizer, curRes, curSqueezeParams);
}


/****************************************************************************
*						PreprocessSqueeze::sublearn
*
*       Description:	Вспомогательная функция для алгоритма обучения. 
*						Рекурсивно производит поиск наилучшего значения параметра
*						precision. При этом интервал поиска с каждым циклом 
*						уменьшаяется ровно в 2 раза. Критерий выхода - размер
*						интеравала меньше параметра precMinDelta.
*       Parameters:		templateRecognizer - шаблон, на которо производить вычисление целевой функции
*						curRes - вектор значений целевой функции для значений precision из вектора curSqueezeParams
*						curSqueezeParams - вектор значений precision по которым ведется поиск лучшего значения
*						comparisonTable - матрица взаимного расположения элеметов вектора curRes
*       Returns:		double - 
*       Throws:			AxiomLibExceprion - если функция вызвана с противоречивыми параметрами
*       Author:			dk
*       History:
*
*****************************************************************************/
double PreprocessSqueeze::sublearn (TemplateRecognizer &templateRecognizer, std::vector <double> &curRes, std::vector <double> &curSqueezeParams) {
	// Определение значений целевой функции для значений параметра squeezeParam из вектора curSqueezeParams
	int resFirst, resSecond;
	for (unsigned int i = 0; i < 5; i++) {
		if (curRes[i] == noValue) {
			squeezeParam = curSqueezeParams[i];
			templateRecognizer.run(resFirst, resSecond);
			curRes[i] = goalStrategy->compute (resFirst, resSecond);
		}
	}
	if ((chooseBest (curRes, curSqueezeParams)) || ((curSqueezeParams[3] / curSqueezeParams[2]) < minRatio) ){
		squeezeParam = curSqueezeParams[2];
		return curRes[2];
	} else 
		return sublearn (templateRecognizer, curRes, curSqueezeParams);
}


// Вспомогательная функция - выбор преобразования для параметра изменения
inline bool PreprocessSqueeze::chooseBest (std::vector <double> &curRes, std::vector <double> &curSqueezeParams) const {
	// Определение лучшего значения
	double bestRes = curRes[2];
	unsigned int bestInd = 2;
	for (unsigned int h = 0; h < 5; h++) {
		if (curRes[h] < bestRes) {
			bestRes = curRes[h];
			bestInd = h;
		}
	}
	switch (bestInd) {
		case 0:
			curSqueezeParams[2] = curSqueezeParams[0];
			curSqueezeParams[4] = curSqueezeParams[1];
			curSqueezeParams[3] = 0.5*(curSqueezeParams[0] + curSqueezeParams[1]);
			curSqueezeParams[0] *= curSqueezeParams[2] / curSqueezeParams[4];
			if (curSqueezeParams[0] < squeezeParamMin)
				curSqueezeParams[0] = squeezeParamMin;
			curSqueezeParams[1] = 0.5*(curSqueezeParams[0] + curSqueezeParams[2]);
			curRes[2] = curRes[0];
			curRes[4] = curRes[1];
			curRes[0] = noValue;
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 1:
			curSqueezeParams[4] = curSqueezeParams[2];
			curSqueezeParams[2] = curSqueezeParams[1];
			curSqueezeParams[3] = 0.5*(curSqueezeParams[2] + curSqueezeParams[4]);
			curSqueezeParams[1] = 0.5*(curSqueezeParams[0] + curSqueezeParams[2]);
			curRes[4] = curRes[2];
			curRes[2] = curRes[1];
			curRes[3] = noValue;
			curRes[1] = noValue;
			break;
		case 2:
			curSqueezeParams[0] = curSqueezeParams[1];
			curSqueezeParams[4] = curSqueezeParams[3];
			curSqueezeParams[1] = 0.5*(curSqueezeParams[1] + curSqueezeParams[2]);
			curSqueezeParams[3] = 0.5*(curSqueezeParams[2] + curSqueezeParams[3]);
			curRes[0] = curRes[1];
			curRes[4] = curRes[3];
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 3:
			curSqueezeParams[0] = curSqueezeParams[2];
			curSqueezeParams[2] = curSqueezeParams[3];
			curSqueezeParams[1] = 0.5*(curSqueezeParams[0] + curSqueezeParams[2]);
			curSqueezeParams[3] = 0.5*(curSqueezeParams[2] + curSqueezeParams[4]);
			curRes[0] = curRes[2];
			curRes[2] = curRes[3];
			curRes[1] = noValue;
			curRes[3] = noValue;
			break;
		case 4:
			curSqueezeParams[2] = curSqueezeParams[4];
			curSqueezeParams[0] = curSqueezeParams[3];
			curSqueezeParams[1] = 0.5*(curSqueezeParams[3] + curSqueezeParams[4]);
			curSqueezeParams[4] *= curSqueezeParams[2] / curSqueezeParams[0];
			if (curSqueezeParams[4] > squeezeParamMax)
				curSqueezeParams[4] = squeezeParamMax;			
			curSqueezeParams[3] = 0.5*(curSqueezeParams[2] + curSqueezeParams[4]);
			curRes[2] = curRes[4];
			curRes[0] = curRes[3];
			curRes[1] = noValue;
			curRes[3] = noValue;
			curRes[4] = noValue;
			break;
		default:
			throw AxiomLibException("PreprocessSqueeze::chooseBest: internal error - unrecgnized value of managment param.");
			break;
	}
	return false;
}


/****************************************************************************
*						PreprocessSqueeze::run
*
*       Description:	Основная функция класса - запуск алгоритма предобработки
*       Parameters:		dataSet - преобразуемый набор данных
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int PreprocessSqueeze::run (DataSet &dataSet) {

	// подготовка внутренних переменных
	bool bres; // Локальная переменная, используемая в цикле
	std::vector <double> row; // Сюда будут вытягиваться ряды из dataSet'a
	std::vector <double> newRow; // Сюда будут вытягиваться ряды из dataSet'a
	std::vector <int> corrMarkUp; // Здесь будет лежать корректная раметка текущего ряда
	std::vector <int> newCorrMarkUp; // Здесь будет лежать корректная раметка текущего ряда
	
	// Запрос числа тестовых рядов в dataSet'e
	int numOfTests = 0;
	std::vector<int> numOfTS;
	bool c = dataSet.getTestSize (numOfTests, numOfTS);
			
	// указание распараллеливать цикл вычислений
	// omp_set_dynamic(1);
	// #pragma omp parallel for schedule(dynamic, 1)
	// for ...
	//cout << "\n	process  " << omp_get_num_threads() << endl;

	// Цикл по всем тестам из dataSet'a
	for (int t = 0; t < numOfTests; t++) {
		// Вытаскиваем корректную разметку данного теста и обновляем ее
		bres = dataSet.getTSByIndexFromTests (corrMarkUp, t, -1);
		if (bres) {
			apply (newCorrMarkUp, corrMarkUp);
			bres = dataSet.setTSByIndexFromTests (newCorrMarkUp, t, -1);
			if (!bres)
				throw AxiomLibException("Error in PreprocessSqueeze::run: for unknown reason a row of correct value coult not be changed in dataSet.");
		}
		else {
			throw AxiomLibException("Error in PreprocessSqueeze::run: correct-params are missing in Tests in DataSets.");
		}
		// Цикл по всем размерностям текущего выбранного теста из dataSet'a и их обновление
		for (int prm = 0; prm < (int) numOfTS[t]; prm++) {
			// Вытягиваем одномерный ряд как часть составного ряда
			bres = dataSet.getTSByIndexFromTests (row, t, prm);
			if (bres) {
				apply (newRow, row);
				bres = dataSet.setTSByIndexFromTests (newRow, t, prm);
				if (!bres)
					throw AxiomLibException("Error in PreprocessSqueeze::run: for unknown reason a row coult not be changed in dataSet.");
			}
			else {
				throw AxiomLibException("Error in PreprocessSqueeze::run: some params are missing in Tests in DataSets.");
			}
		}
	}

	// подготовка внутренних переменных функции
	int numOfClasses;
	std::vector<int> numOfMultiTS;
	std::vector<std::vector<int> > numOfTSSets;

	bres = dataSet.getClassSize (numOfClasses, numOfMultiTS, numOfTSSets);
	std::vector <double> vec; // Вектор 
	std::vector <double> newVec; // Вектор 

	// Заполнение массива класса неиспрваностей
	for (int i = 0; i < numOfClasses; i++) {
		for (int j = 0; j < numOfMultiTS[i]; j++) {
			for (int p = 0; p < numOfTSSets[i][j]; p++) {
				bres = dataSet.getTSByIndexFromClass (vec, i, j, p);
				if (bres) {
					apply (newVec, vec);
					bres = dataSet.setTSByIndexFromClass (newVec, i, j, p);
					if (!bres)
						throw AxiomLibException("Error in PreprocessSqueeze::run: new alue for class-row could not be set.");
				}
			}
		}
	}

	return 0;
}


/****************************************************************************
*						PreprocessSqueeze::apply
*
*       Description:	Применение данного алгоритрма предобработки для одного 
*						отдельно взятого ряда действительных чисел
*       Parameters:		newVec - преобразованное значение вектора
*						vec - старое значение вектора
*       Returns:		true - если преобразование проведено успешно
*						false - если преобразование произошло неуспешно
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
bool PreprocessSqueeze::apply (std::vector <double> &newVec, std::vector <double> &vec) const {
	// Проверка начальных условий
	unsigned int size = vec.size();
	if (size < 1) {
		newVec.clear();
		return false;
	}

	// Вычисление длины преобразованного вектора
	unsigned int newSize = (unsigned int) round (((double) size )* squeezeParam);
	if (newSize < 1)
		newSize = 1;

	// Три варианта результата сравнения длин нового и старого вектора
	if (newSize == size) { // Значит никакого преобразования не происходит
		newVec.assign (vec.begin(), vec.end());
		return true;
	}
	
	if (newSize > size) { // Значит происходит процедура интерполяции
		// Проверка на возможность использования реализованного алгоритма интерполяции
		if (newSize <= LagrangeDegree) {
			newVec.assign (vec.begin(), vec.end());
			return false;
		}
		newVec.resize (newSize);
		
		// Вычисление значений элементов вектора
		unsigned int j; // Индекс исходного ряда, соответствующий значению текущей точке i
		double ld; // Переменная для подсчтета интерполяционного множителя Лагранжа
		double relativeShift; // Переменная для значения разницы между положением точки, в которой считаем значение и значением в центральной точке j
		unsigned int left; // Половина LagrangeDegree - для определения индексов используемых точек
		unsigned int right; // Половина LagrangeDegree - для определения индексов используемых точек
		left = (unsigned int) round ( (double) LagrangeDegree / 2.0);
		right = LagrangeDegree - left;
		for (unsigned int i = 0; i < newSize; i++) {
			j = (unsigned int) round ((double) i / squeezeParam);
			if (j < left)
				j = left;
			if ((size - j) <= right)
				j = size - right - 1;
			// Вычисление значения в точке i по средставам интерполяции
			newVec[i] = 0.0;
			relativeShift = (double) i / squeezeParam - j + left;
			for (int t = 0; t <= (int) LagrangeDegree; t++) {
				// Подсчет множителя Лагранжа ld
				ld = 1.0;
				// Числитель
				for (int n = 0; n <= (int) LagrangeDegree; n++) {
					if (n==t) continue;
					ld *= relativeShift - n;
				}
				// Знаменатель
				ld /= factorials[t] * factorials[LagrangeDegree - t];
				if ( ((LagrangeDegree - t)%2) != 0) 
					ld *= -1.0;
				// Накопление итогового значения в точке
				newVec[i] += vec[j-left+t] * ld;
			}
		}
		return true;
	}

	if (newSize < size) { // Значит происходит процедура свертки более длинного ряда в более короткий
		newVec.resize (newSize);
		// Вычисление значений элементов нового вектора
		unsigned int left; // Половина сворачиевомого участка - для определения индексов используемых точек
		unsigned int right; // Половина сворачиевомого участка - для определения индексов используемых точек
		double j; // Отображение новой точки на старый ряд
		double delta; // Размер сворачиевомого участка
		double deltaHalf; // Половина от размера участка
		double curDelta; // Текущее значение длины сворачивоемого участка - актуально для граничных точек
		delta = 1.0 / squeezeParam;
		deltaHalf = delta / 2.0;
		double posLeft; // Позиция по оси времени, левая граница сворачивоемого участка
		double posRight; // Позиция по оси времени, правая граница сворачивоемого участка
		double pointLeft; // Позиция по оси времени, левая граница окрестности точки
		double pointRight; // Позиция по оси времени, правая граница окрестности точки
		for (unsigned int i = 0; i < newSize; i++) {
			j = (double) i / squeezeParam;
			// Определяем left и right
			posLeft = j - deltaHalf;
			if (posLeft < 0.0)
				posLeft = 0.0;
			posRight = j + deltaHalf;
			if (posRight > (size - 1))
				posRight = size - 1;
			curDelta = posRight - posLeft;
			left = (unsigned int) round (posLeft);
			right = (unsigned int) round (posRight);
			// Вычисляем значение нового ряда в точке i
			newVec[i] = 0.0;
			for (unsigned int h = left; h <= right; h++) {
				pointLeft = h - 0.5;
				pointRight = h + 0.5;
				if (pointLeft < posLeft)
					pointLeft = posLeft;
				if (pointRight > posRight)
					pointRight = posRight;
				newVec[i] += vec[h] * (pointRight - pointLeft);
			}
			newVec[i] /= curDelta;
		}
		return true;
	}

	return false;
}


/****************************************************************************
*						PreprocessSqueeze::apply
*
*       Description:	Применение данного алгоритрма предобработки для одного 
*						отдельно взятого ряда целых чисел
*       Parameters:		newVec - преобразованное значение вектора
*						vec - старое значение вектора
*       Returns:		true - если преобразование проведено успешно
*						false - если преобразование произошло неуспешно
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
bool PreprocessSqueeze::apply (std::vector <int> &newVec, std::vector <int> &vec) const {
	// Проверка начальных условий
	unsigned int size = vec.size();
	if (size < 1) {
		newVec.clear();
		return false;
	}

	// Вычисление длины преобразованного вектора
	unsigned int newSize = (unsigned int) round (((double) size )* squeezeParam);
	if (newSize < 1)
		newSize = 1;

	// Три варианта результата сравнения длин нового и старого вектора
	if (newSize == size) { // Значит никакого преобразования не происходит
		newVec.assign (vec.begin(), vec.end());
		return true;
	}
	
	if (newSize > size) { // Значит происходит процедура интерполяции
		// Проверка на возможность использования реализованного алгоритма интерполяции
		if (newSize <= LagrangeDegree) { // Оставляем это условие, так как оно требуется для преобразования вектора целых чисели и если менять вектора, то всегда одновременно
			newVec.assign (vec.begin(), vec.end());
			return false;
		}
		newVec.resize (newSize);
		// Вычисление значений элементов вектора
		int j; // Индекс исходного ряда, соответствующий значению текущей точке i
		for (unsigned int i = 0; i < newSize; i++) {
			j = round( (double) i / squeezeParam );
			if (j >= (int) size)
				j = size - 1;
			newVec[i] = vec[j];
		}
		return true;
	}

	if (newSize < size) { // Значит происходит процедура свертки более длинного ряда в более короткий
		newVec.resize (newSize);
		// Вычисление значений элементов нового вектора
		int left; // Половина сворачиевомого участка - для определения индексов используемых точек
		int right; // Половина сворачиевомого участка - для определения индексов используемых точек
		int curVal; // Текущее значение, которое возможно будет перенесено со старого ряда в новый
		for (unsigned int i = 0; i < newSize; i++) {
			// Определяем left и right
			left = round ( ((double) i - 0.5) / squeezeParam );
			if (left < 0)
				left = 0;
			right = round ( ((double) i + 0.5) / squeezeParam );
			if (right >= (int) size)
				right = size - 1;
			curVal = 0;
			for (int h = left; h <= right; h++) {
				if (vec[h] > 0) {
					curVal = vec[h];
					break;
				}
			}
			newVec[i] = curVal;
		}
		return true;
	}

	return false;
}
