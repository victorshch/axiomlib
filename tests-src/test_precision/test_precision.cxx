#include "Environment.h"
#include "AxiomSetPop.h"
#include "GoalStrategyFactory.h"
#include "GoalStrategy.h"
#include "RecognizerMetric.h"
#include "RecognizerExtend.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace AxiomLib;

#define comments true
#define	defaultConfigFile	"./axiomlib_prec.conf"

int main (int argc, char** argv) {
	try {
		srand((unsigned int) time(0));
		if (comments) {
			std::cout << "\n" << "AxiomLib 0.6 / Test_precision.\nЧитаем конфигурационный файл." << "\n";
			std::cout.flush();
		}
		 // Считываем файл конфигурации из заданного источника
		Environment envStart;
		envStart.readConfigParams (argc, argv);

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;
		
		if (comments) {
			std::cout << "\nИнициализируем перменные.\n";
			std::cout.flush();
		}
		double start, finish, delta;
		int popSize;
		if (env.getDoubleParamValue(start, "precisionMin") < 0)
			throw AxiomLibException("Top level: main : precisionMin is undefined.");
		if (env.getDoubleParamValue(delta, "precisionDelta") < 0)
			throw AxiomLibException("Top level: main : precisionDelta is undefined.");
		if (env.getDoubleParamValue(finish, "precisionMax") < 0)
			throw AxiomLibException("Top level: main : precisionMax is undefined.");
		if (env.getIntParamValue(popSize, "popSize") < 0)
			throw AxiomLibException("Top level: main : popSize is undefined.");
		if (popSize < 1)
			throw AxiomLibException("Top level: main : popSize is out of range.");
		if (start > finish)
			throw AxiomLibException("Top level: main : precisionMax is less than precisionMin.");

		std::vector <AxiomSetPop> axiomSetPopVector;
		std::vector < std::vector <double> > precMatrix;

		double cur;
		cur = start;
		axiomSetPopVector.resize(0);
		precMatrix.resize(0);

		while (cur <= finish) {
			precMatrix.resize(precMatrix.size() + 1);
			precMatrix[precMatrix.size() - 1].resize(popSize);
			for (int i = 0; i < popSize; i++) precMatrix[precMatrix.size() - 1][i] = cur;
			//axiomSetPopVector[axiomSetPopVector.size() - 1].initFromEnv(env);
			cur += delta;
		}
		axiomSetPopVector.resize(precMatrix.size());
		for (unsigned int h = 0; h < axiomSetPopVector.size(); h++) {
			axiomSetPopVector[h].initFromEnv(env);
		}

		if (comments) {
			std::cout << "\nИнициализируем стратегию вычисления целевой функции.\n";
			std::cout.flush();
		}
		// Инициализируем стратегию вычисления целевой фуекции
		string goalStrategyName;
		GoalStrategyFactory gsf;
		if (env.getStringParamValue(goalStrategyName, "goalClass") < 0)
			throw AxiomLibException("Top level: main : goal strategy is undefined.");
		GoalStrategy* goalStrategy;
		goalStrategy = gsf.create(goalStrategyName);
		goalStrategy->setParamsFromEnv(env);
		
		if (comments) {
			std::cout << "\nИнифиализируем распознаватель.\n";
			std::cout.flush();
		}
		// Класс распознавателя
		RecognizerExtend recognizer;
		//RecognizerMetric recognizer;
		// Инифиализируем распознаватель
		recognizer.setParamsFromEnv(env);
		
		if (comments) {
			std::cout << "\nЗапуск тестирования.\n";
			std::cout.flush();
		}

		int tmpFirstBest, tmpSecondBest;
		double goalBest, precBest;
		tmpFirstBest = -1;
		tmpSecondBest = -1;
		goalBest = -1.0;
		precBest =  - 1.0;
		for (int i = 0; i < (int) axiomSetPopVector.size(); i++) {
				
			// 1. Прогоняем на существующей популяции алгоритм разметки и распознавания, получаем число ошибок первого и второго рода, заполняем статистику.
			recognizer.run(axiomSetPopVector[i], precMatrix[i]);
			
			// 2. Прогоняем вычисление H(first, second)
			goalStrategy->compute(axiomSetPopVector[i]);
			
			int tmpFirst, tmpSecond;
			double goal;
			std::cout << "\n 	prec = " << start + i*delta << "	Goals are : ";
			for (int asNum = 0; asNum < axiomSetPopVector[i].size(); asNum++) {
				axiomSetPopVector[i].getStats (asNum, tmpFirst, tmpSecond, goal);
				std::cout << "	first " << tmpFirst;
				std::cout << "	second " << tmpSecond;
				std::cout << "	goal " << goal << "\n";
				std::cout.flush();
				if ((i == 0) && (asNum == 0)) {
					tmpFirstBest = tmpFirst;
					tmpSecondBest = tmpSecond;
					goalBest = goal;
					precBest = precMatrix[i][asNum];
				} else {
					if (goal < goalBest) {
						tmpFirstBest = tmpFirst;
						tmpSecondBest = tmpSecond;
						goalBest = goal;
						precBest = precMatrix[i][asNum];
					}					
				}
			} 
		}
		
		if (comments) std::cout << "\nРезультаты:";
		std::cout << "\n	goal is " << goalBest << " ( " << tmpFirstBest << ", " << tmpSecondBest << " )";
		std::cout << "\n	prec is " << precBest << " \n";
		std::cout.flush();
		return 0;
	}
	catch ( AxiomLibException testex ) {
	      std::cerr << "Fatal exception:\n\t";
	      std::cerr << testex.error() << "\n";
	 }
}
