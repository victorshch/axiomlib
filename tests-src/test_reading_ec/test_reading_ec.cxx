/****************************************************************************
*			File name: test_reading_ec.cxx
*			File type: C++ program file
*
*			Тестовая площадка для отработки класса ElemConditions
*
*	Copyright (C) dk, 2000 - 2009. All rights reserved
*
*	Version:	0.1
*
*	File history:	29 сентября 2009
*
****************************************************************************/

#include "all_axioms.h"
#include "AxiomExpr.h"
#include "AxiomExprPlus.h"
#include "AxiomExprSet.h"
#include "AxiomSetBase.h"
#include "AxiomBase.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"
#include "AxiomExprSet.h"
#include "AxiomExpr.h"
#include "AxiomExprSetStructure.h"
#include "AxiomExprStructure.h"
#include "ElemCondPlus.h"
#include "ElemCondPlusStat.h"
#include "ElemCondPlusStruture.h"
#include <iostream>
#include <string>
#include <time.h>

using namespace AxiomLib;

#define defaultConfigFile	"./axiomlib_rec.conf"

int main(int argc, char** argv) {
	try {
		std::cout<< "\nTest starts.\n";

		srand( (unsigned)time( NULL ) );

		Environment envStart;
		envStart.readConfigParams (argc, argv);

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;
		
		// 1. ищем базовую директорию с наборами аксиом
		std::string axiomBaseDir;
		if (env.getStringParamValue(axiomBaseDir, "AxiomBaseDir") < 0)
			throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but axiom base dir is not set.");

		// 2. ищем базовую директорию с наборами систем аксиом
		std::string axiomSetDir;
		if (env.getStringParamValue(axiomSetDir, "AxiomSetBaseDir") < 0)
			throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but axiom set base dir is not set.");

		// 3. получаем множество наборов аксиом
		std::string axiomSetName;
		if (env.getStringParamValue(axiomSetName, "AxiomSet") < 0)
			throw AxiomLibException("AxiomSetPop::initFromEnv : popPreset > 0 but no axiom set is given.");
		
		std::vector <std::string> paramNames;
		paramNames.resize(3);
		paramNames[0].assign ("time");
		paramNames[1].assign ("row");
		paramNames[2].assign ("data");

		// Тест на чтение и запись в файл описания системы аксиомы
		// Тест на чтение из файла
		AxiomSetBase axiomSetBase;
		std::string fName;
		fName = axiomSetDir;
		fName.append ("/");
		fName.append (axiomSetName);
		
		AxiomExprSetStructure aess;
		axiomSetBase.readFromAS(fName, aess);

		std::string whereToSave;
		if (env.getStringParamValue(whereToSave, "saveTo") < 0)
			throw AxiomLibException("Top level: main : saveTo is undefined.");

		// Тест на определение структуры аксиомы и на запись в файлы
		AxiomExprSet axiomExprSet;
		axiomExprSet.initAxiomSetFromFile (axiomSetDir, axiomSetName);
		axiomExprSet.saveAxiomSetToFile (axiomSetDir, whereToSave);

		// Тест на случайное определение системы аксиом
		AxiomExprSet axiomExprSet2;
		axiomExprSet2.initAxiomSetByRand (5, 5);
		whereToSave.append ("a");
		axiomExprSet2.saveAxiomSetToFile (axiomSetDir, whereToSave);

		AxiomExprSet axiomExprSet3;
		axiomExprSet3.initAxiomSetByRand (5, 5);
		whereToSave.append ("b");
		axiomExprSet3.saveAxiomSetToFile (axiomSetDir, whereToSave, paramNames);

		AxiomExprSet axiomExprSet4;
		axiomExprSet4.initAxiomSetByRand (5, 5, 0, 2);
		whereToSave.append ("c");
		axiomExprSet4.saveAxiomSetToFile (axiomSetDir, whereToSave, paramNames);

		std::cout<< "\nTest 1 done.\n";


		// Тест на чтение и запись в файл описания аксиомы
		// Получаем множество наборов аксиом
		std::string axiomName;
		if (env.getStringParamValue(axiomName, "Axiom") < 0)
			throw AxiomLibException("AxiomSetPop::initFromEnv : no axiom is given.");
		
		// Тест на чтение из файла
		AxiomBase axiomBase;
		std::string fName2;
		fName2 = axiomBaseDir;
		fName2.append ("/");
		fName2.append (axiomName);
		
		AxiomExprStructure aes, aes1, aes2, aes3;
		axiomBase.readFromAX(fName2, aes);
		axiomBase.readFromAX(fName2, aes1, paramNames);

		fName2.append ("2");
		axiomBase.readFromAX(fName2, aes2);
		axiomBase.readFromAX(fName2, aes3, paramNames);

		std::string whereToSaveAxiom;
		if (env.getStringParamValue(whereToSaveAxiom, "saveTo") < 0)
			throw AxiomLibException("Top level: main : saveTo is undefined.");

		// Тест на определение структуры аксиомы и на запись в файлы
		AxiomExpr axiomExpr;
		axiomExpr.initAxiomFromStructure (aes);
		axiomExpr.saveAxiomToFile (axiomBaseDir, whereToSaveAxiom);

		AxiomExpr axiomExpr1;
		axiomExpr1.initAxiomFromStructure (aes3);
		whereToSaveAxiom.append ("2");
		axiomExpr1.saveAxiomToFile (axiomBaseDir, whereToSaveAxiom, paramNames);

		// Тест на случайное определение системы аксиом
		AxiomExpr axiomExpr2;
		axiomExpr2.initAxiomByRand (5);
		whereToSaveAxiom.append ("a");
		axiomExpr2.saveAxiomToFile (axiomBaseDir, whereToSaveAxiom);

		AxiomExpr axiomExpr3;
		axiomExpr3.initAxiomByRand (6,0,2);
		whereToSaveAxiom.append ("b");
		axiomExpr3.saveAxiomToFile (axiomBaseDir, whereToSaveAxiom, paramNames);

		std::cout<< "\nTest 2 done.\n";


		// Тест на чтение и запись в файл описания элементарного условия
		// Получаем множество наборов условий
		std::string ecName;
		if (env.getStringParamValue(ecName, "ElemCond") < 0)
			throw AxiomLibException("AxiomSetPop::initFromEnv : no ElemCond is given.");
		
		// Тест на чтение из файла
		AxiomBase axiomBaseEC;
		std::string fName3;
		fName3 = axiomBaseDir;
		fName3.append ("/");
		fName3.append (ecName);
		//fName3.append (".ec");
		
		ElemCondPlusStruture ecps;
		axiomBase.readFromEC(fName3, ecps);

		std::string whereToSaveEC;
		if (env.getStringParamValue(whereToSaveEC, "saveTo") < 0)
			throw AxiomLibException("Top level: main : saveTo is undefined.");

		// Тест на определение структуры аксиомы и на запись в файлы
		ElemCondPlus elemCondPlus;
		elemCondPlus.initFromStructure(ecps);
		elemCondPlus.saveECToFile (axiomBaseDir, whereToSaveEC);

		// Тест второго варианта записи 
		whereToSaveEC.append ("plus");
		elemCondPlus.dimension = 1;
		elemCondPlus.saveECToFile (axiomBaseDir, whereToSaveEC, paramNames);

		// тест второго варианта чтения
		std::string fName4;
		fName4 = axiomBaseDir;
		fName4.append ("/");
		fName4.append (ecName);
		fName4.append ("2");
		axiomBase.readFromEC(fName4, ecps, paramNames);
		
		std::cout<< "\nTest 3 done.\n";

		return 0;
	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal exception:\n\t";
		std::cerr << testex.error() << "\n";
	}
}
