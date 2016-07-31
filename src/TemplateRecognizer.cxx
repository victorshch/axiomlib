/****************************************************************************
*
*				Функции класса TemplateRecognizer
*
****************************************************************************/

#include "TemplateRecognizer.h"

using namespace AxiomLib;

// Коструктор класса
TemplateRecognizer::TemplateRecognizer (void) {
	preprocess = NULL;
	axiomSet = NULL;
	recognizer = NULL;
	refTrajFinder = NULL;
} 


// Деструктор класса - удаляюся все созданные в рамках класса динамические объекты
TemplateRecognizer::~TemplateRecognizer (void) {
	if (preprocess != NULL) delete preprocess;
	if (axiomSet != NULL) delete axiomSet;
	if (recognizer != NULL) delete recognizer;
	if (refTrajFinder != NULL) delete refTrajFinder;	
}


/****************************************************************************
*						TemplateRecognizer::setDataSet
*
*       Description:	Функция установки нового набора данных в текущий класс распознователя
*       Parameters:		newDataSet - новый набор данных
*       Returns:		0
*       Throws:			-
*       Author:			dk
*       History:
*
****************************************************************************/
int TemplateRecognizer::setDataSet (const DataSet &newDataSet) {
	dataSet = newDataSet;
	return 0;
}


/****************************************************************************
*                       TemplateRecognizer::setParamsOfDataSet
*
*       Description:	Функция установки нового значаения переменой класса params
*       Parameters:	newParams - новое значение вектора
*       Returns:		0
*       Throws:		-
*       Author:		dk
*       History:
*
****************************************************************************/
int TemplateRecognizer::setParamsOfDataSet (std::vector<int> &newParams) {
	params = newParams;
	return 0;
}


/****************************************************************************
*				TemplateRecognizer::initFromEnv
*
*	Description:	Функция устанавливает параметры объекта по данным из 
*					окружения, которое подается как параметр функции
*	Parameters:	env - объект класса окружения, откуда берутся параметры данного класса
*	Returns:	0
*	Throws:		AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:		dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::initFromEnv (const Environment& env) {
	
	return 0;
}


/****************************************************************************
*					TemplateRecognizer::run
*
*	Description:	Основная функция в классе - реализует алгоритм работы
*					распознавателя.
*	Parameters:		resFirst - заполняемое число ошибок I рода
*					resSecond - заполняемое число ошибок II рода
*					asIndex - индекс, под которым сохранять систему аксиом
*	Returns:		0 - если функция отработала корректно
*					-1 - если распознаватель не определен (отсутсвуют какие-либо из его элементов)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::run (int &resFirst, int &resSecond, const int asIndex) {
	// Проверка готовности распознавателя
	if ((preprocess == NULL) || (axiomSet == NULL) || (recognizer == NULL))
		return -1;
	
	// Создание копии набора данных для ее изменения и использования
	prepDataSet = dataSet;

	// Предобработка набора данных
	preprocess->run (prepDataSet);
	
	// Запуск распознавания с подсчетом числа ошибок первого и второго рода
	recognizer->run (*axiomSet, prepDataSet, params, resFirst, resSecond, asIndex);
	
	// Выход из функции
	return 0;
}


/****************************************************************************
*					TemplateRecognizer::runWithOutput
*
*	Description:	Запускает алгоритм работы распознавателя с выводом результатов в файл.
*	Parameters:		resFirst - заполняемое число ошибок I рода
*					resSecond - заполняемое число ошибок II рода
*					asIndex - индекс, под которым сохранять систему аксиом
*						(необязательный параметр, значение по умолчанию -1 - при этом
*						 индекс, под которым сохрянять выбирается последовательно из уже существующих)
*	Returns:		0 - если функция отработала корректно
*					-1 - если распознаватель не определен (отсутсвуют какие-либо из его элементов)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::runWithOutput (int &resFirst, int &resSecond, const int asIndex) {
	int toRet;
	recognizer->setOutputToFile(true);
	toRet = run (resFirst, resSecond, asIndex);
	recognizer->setOutputToFile(false);
	// Выход из функции
	return toRet;
}


/****************************************************************************
*					TemplateRecognizer::runPreprocessor
*
*	Description:	Запуск алгоритма предобработки исходного набора данных
*	Parameters:		void
*	Returns:		0 - если функция отработала корректно
*					-1 - если распознаватель не определен (отсутсвуют какие-либо из его элементов)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::runPreprocessor (void) {
	// Проверка готовности распознавателя
	if (preprocess == NULL)
		return -1;
	
	// Создание копии набора данных для ее изменения алгоритмом предобработки
	prepDataSet = dataSet;

	// Предобработка набора данных
	preprocess->run (prepDataSet);

	// Выход из функции
	return 0;
}


/****************************************************************************
*					TemplateRecognizer::runRecognizer
*
*	Description:	Вызывает только алгоритм распознавания по уже предобработанному набору данных
*	Parameters:		resFirst - заполняемое число ошибок I рода
*					resSecond - заполняемое число ошибок II рода
*	Returns:		0 - если функция отработала корректно
*					-1 - если распознаватель не определен (отсутсвуют какие-либо из его элементов)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::runRecognizer (int &resFirst, int &resSecond) {
	// Проверка готовности распознавателя
	if ((axiomSet == NULL) || (recognizer == NULL))
		return -1;
	
	// Запуск распознавания с подсчетом числа ошибок первого и второго рода
	recognizer->run (*axiomSet, prepDataSet, params, resFirst, resSecond);
	
	// Выход из функции
	return 0;
}


/****************************************************************************
*					TemplateRecognizer::runRecognizer
*
*	Description:	Вызывает только алгоритм распознавания по уже предобработанному набору данных
*					При этом идет подсчет статистики по аксиомам используемой системы.
*	Parameters:		resFirst - заполняемое число ошибок I рода
*					resSecond - заполняемое число ошибок II рода
*	Returns:		0 - если функция отработала корректно
*					-1 - если распознаватель не определен (отсутсвуют какие-либо из его элементов)
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::runRecognizer (int &resFirst, int &resSecond, std::vector<double> &stat) {
	// Проверка готовности распознавателя
	if ((axiomSet == NULL) || (recognizer == NULL))
		return -1;
	
	// Запуск распознавания с подсчетом числа ошибок первого и второго рода
	recognizer->run (*axiomSet, prepDataSet, params, resFirst, resSecond, stat);
	
	// Выход из функции
	return 0;
}


/****************************************************************************
*					TemplateRecognizer::learn
*
*	Description:	Функция обучения шаблона
*	Parameters:		void
*	Returns:		0
*	Throws:			AxiomLibException - если в одном из алгоритмов возникла ошибка
*	Author:			dk
*	History:
*
****************************************************************************/
int TemplateRecognizer::learn (void) {
	// Проверка задан ли шаблон полностью
	if ((preprocess == NULL) || (axiomSet == NULL) || (recognizer == NULL))
		return -1;

	return 0;
}


/****************************************************************************
*				TemplateRecognizer::operator=
*
*	Description:	Оператор присваивания шаблонов
*	Parameters:		second - присваиваемый шаблон
*	Returns:		*this
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
TemplateRecognizer & TemplateRecognizer::operator= (const TemplateRecognizer &second) {
	// Копирование алгоритма предобработки
	if (preprocess != NULL) {
		delete preprocess;
		preprocess = NULL;
	}
	if (second.preprocess != NULL)
		preprocess = second.preprocess->copy();
	
	// Копирование системы аксиом
	if (axiomSet == NULL)
		axiomSet = new AxiomSet;
	if (second.axiomSet == NULL) {
		delete axiomSet;
		axiomSet = NULL;
	} else {
		*axiomSet = *(second.axiomSet);
	}

	// Копирование распознавателя
	if (recognizer != NULL) {
		delete recognizer;
		recognizer = NULL;
	}
	if (second.recognizer != NULL)
		recognizer = second.recognizer->copy();

	// Копирование алгоритма выделения эталонных траекторий
	if (refTrajFinder != NULL) {
		delete refTrajFinder;
		refTrajFinder = NULL;
	}
	if (second.refTrajFinder != NULL)
		refTrajFinder = second.refTrajFinder->copy();

	// Копирование DataSet'а и его параметров
	dataSet = second.dataSet;
	params.clear();
	params = second.params;

	return *this;
}
