#include "Environment.h"
#include "AxiomSetPop.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include "RecognizerMetric.h"
#include "RecognizerExtend.h"
#include "RecognizerFactory.h"
#include "Defines.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace AxiomLib;

#define comments true
#define defaultConfigFile	"./axiomlib_ext.conf"

int main (int argc, char** argv) {
	try {
		srand((unsigned int) time(0));
		//std::cout << "Please, press enter.";
		//std::cin.get();
		if (comments) { 
			std::cout << "\n" << "AxiomLib 0.6 / Test_Execute.\nReading config-file." << "\n"; 
			std::cout.flush();
		}

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "1\n"; std::cout.flush(); }

		 // Считываем файл конфигурации из заданного источника
		Environment envStart;
		envStart.readConfigParams (argc, argv);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "10\n"; std::cout.flush(); }

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;
		env.saveFile (pathToConfigFile);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "20\n"; std::cout.flush(); }

		// Заполняем набор систем аксиом
		AxiomSetPop axiomSetPop;
		// инициализируем набор систем аксиом
		axiomSetPop.initFromEnv(env);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "30\n"; std::cout.flush(); }

		// Инициализируем стратегию вычисления целевой фуекции
		string goalStrategyName;
		GoalStrategyFactory gsf;
		if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
			throw AxiomLibException("Top level: main : goal strategy is undefined.");
		GoalStrategy* goalStrategy;
		goalStrategy = gsf.create(goalStrategyName);
		goalStrategy->setParamsFromEnv(env);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "40\n"; std::cout.flush(); }

		// Класс распознавателя
		//RecognizerExtend *recognizer;
		//recognizer = new RecognizerExtend;
		//RecognizerMetric recognizer;
		RecognizerFactory recognizerFactory;
		Recognizer* recognizer;
		std::string recognizerStr;

		if (env.getStringParamValue(recognizerStr, "Recognizer") < 0)
			throw AxiomLibException("Top level: main : Recognizer is undefined.");

		// Создаем распознаватель
		recognizer = recognizerFactory.create (recognizerStr);

		// Инифиализируем распознаватель
		recognizer->setParamsFromEnv(env);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "50\n"; std::cout.flush(); }

		// 1. Прогоняем на существующей популяции алгоритм разметки и распознавания, получаем число ошибок первого и второго рода, заполняем статистику.
		recognizer->run(axiomSetPop);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "70\n"; std::cout.flush(); }

		// 2. Прогоняем вычисление H(first, second)
		goalStrategy->compute(axiomSetPop);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "80\n"; std::cout.flush(); }

		// Уничтожаем созданный распознаватель
		delete recognizer;

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "90\n"; std::cout.flush(); }

		// Сохранение систем аксиом в файлы
		std::string saveFile;
		if (env.getStringParamValue (saveFile, "PathToSaveResults") >= 0)
			axiomSetPop.saveAxiomSetPopToFiles (env);

		if (specOutputForGui) { std::cout << str_systemMsg_prog << "100\n"; std::cout.flush(); }

		// Вывод полученных результатов на экран
		int tmpFirst, tmpSecond;
		double goal;
		std::cout << "\n 	Goals are \n";
		for (int asNum = 0; asNum < axiomSetPop.size(); asNum++) {
			axiomSetPop.getStats (asNum, tmpFirst, tmpSecond, goal);
			std::cout << "	first " << tmpFirst;
			std::cout << "	second " << tmpSecond;
			std::cout << "	goal " << goal << "\n";
		}
		std::cout << "\n";
		std::cout.flush();

		if (specOutputForGui) { std::cout << str_systemMsg_stop << "\n"; std::cout.flush(); }

		return 0;
	}
	catch ( AxiomLibException testex ) {
	      std::cerr << "Fatal exception:\n\t";
	      std::cerr << testex.error() << "\n";
	 }
}
