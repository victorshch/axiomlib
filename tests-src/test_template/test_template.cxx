#include "Environment.h"
#include "TemplateManager.h"
#include "AxiomLibException.h"
#include "Check.h"
#include "Defines.h"
#include <iostream>
#include <time.h>
#include <string>
#include <stdexcept>

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

#include "Logger.h"

using namespace AxiomLib;

#define comments true
#define	defaultConfigFile	"./axiomlib_tpl.conf"

int main (int argc, char** argv) {
	try {
		Logger::getInstance()->setComments(true);
		Logger::getInstance()->setDebug(true);
		
		//Logger::getInstance()->writeDebug("Starting Template...");
		
		MPI_Init(&argc, &argv);
		int size, rank = 0;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		//std::cout << "\n	process  " << rank << " size " <<  size << endl;

		srand( (unsigned int) (time(0) + rank*1111));

		if ((comments) && (rank == 0)) {
			std::cout << "\n" << "AxiomLib / Template approach.\n	Reading config-file." << "\n";
			std::cout.flush();
		}

		// Блок используемый исключительно для отладки
//		if (rank == 0) {
//			std::cout << "\n\tTo start press enter, please.";
//			std::cin.get();
//		}
//		MPI_Barrier (MPI_COMM_WORLD);
		// Конец блока для отладки

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "1\n"; std::cout.flush(); }

		Environment envStart;
		envStart.readConfigParams (argc, argv);

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "10\n"; std::cout.flush(); }

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;
		env.saveFile (pathToConfigFile);

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "20\n"; std::cout.flush(); }

		if (comments) { std::cout << "\n	Initializing template manager.\n"; std::cout.flush(); }

		TemplateManager templateManager;
		templateManager.initFromEnv(env);

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "30\n"; std::cout.flush(); }

		if (comments) {	std::cout << "\n	Starting template learning algorithm.\n"; std::cout.flush(); }

		// Запуск обучения
		templateManager.run();
		// Запуск только проверки качества работы распознавателей на заданном наборе данных
		//templateManager.runExecuteOnly();
		
		if (rank == 0) { 
			std::cout << "\n	Learning done. \n";	
			std::cout.flush(); 
		}

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "90\n"; std::cout.flush(); }

		MPI_Barrier (MPI_COMM_WORLD);

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_prog << "100\n"; std::cout.flush(); }
		
		MPI_Finalize ();

		if ((specOutputForGui) && (rank == 0)) { std::cout << str_systemMsg_stop << "\n"; std::cout.flush(); }

		return 0;
	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << std::endl;
		MPI_Finalize ();
	}
	catch ( std::exception e) {
		std::cerr << "Fatal exception:\n\t";
		std::cerr << e.what() << std::endl;
	}
}
