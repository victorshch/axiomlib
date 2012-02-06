/**********************************************************************
Данная программа - инструмент для запуска этапов алгоритма FMD.
Она позволяет задавать в коммандной строке какой этап запускать
(и какой файл с результатами использовать - пока не реализовано)

Для компиляции требуется boost.program_options
**********************************************************************/

#include <boost/program_options.hpp>

#include "Environment.h"
#include "AxiomLibException.h"
#include "FuzzyMultiDataLearnAlgorithm.h"
#include "Check.h"
#include <iostream>
#include <time.h>
#include <string>

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

using namespace AxiomLib;

namespace po = boost::program_options;

#define comments true
#define defaultConfigFile	"./axiomlib_fmd.conf"

int main (int argc, char** argv) {
	try {
		bool stage1Flag = false;
		bool stage2Flag = false;
		bool stage3Flag = false;
		
		std::string stage1File;
		std::string stage2File;
		std::string stage3File;
		
		po::options_description od("Allowed options:");
		od.add_options()
			("help", "output help message")
			("stage1", "enable FMD stage 1 computation")
			("stage2", "enable FMD stage 2 computation")
			("stage3", "enable FMD stage 3 computation")
			("stage1file", po::value<std::string>(), "stage 1 .list file. not used yet. ecs read from bestECsList_rank_1")
			("stage2file", po::value<std::string>(), "stage 2 .list file. not used yet. axioms read from bestAxiomsList");
			//("stage3file", po::value<std::string>(), "stage 3 .list file");
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, od), vm);
		po::notify(vm); 
		
		stage1Flag = vm.count("stage1") > 0;
		stage2Flag = vm.count("stage2") > 0;
		stage3Flag = vm.count("stage3") > 0;
		
		if(vm.count("help") > 0) {
			std::cout<<od<<std::endl;
			return 0;
		}
		
		if(!(stage1Flag || stage2Flag || stage3Flag)) {
			std::cerr<<"You need to enable computation of at least one stage\n";
			std::cout<<od<<std::endl;
			return -1;
		}
		
		MPI_Init(&argc, &argv);
		int size, rank = 0;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		std::cout << "\n	process  " << rank << " size " <<  size << "\n";

		srand( (unsigned int) (time(0) + rank*1111));

		if ((comments) && (rank == 0)) {
			std::cout << "\n" << "AxiomLib / Fuzzy data - test of new approach.\n	Reading config-file." << "\n";
			std::cout.flush();
		}

		
		// Testing purposes only
/*		if (rank == 0) {
			std::cout << "\nTo begin press enter.";
			std::cout.flush();
			std::cin.get();
		}*/
		MPI_Barrier (MPI_COMM_WORLD);
	

		Environment envStart;
		envStart.readConfigParams (argc, argv);

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;
		env.saveFile (pathToConfigFile);

		if ((comments) && (rank == 0)) {
			std::cout << "\n	Initializing learning algorithm.\n";
			std::cout.flush();
		}
		
		if(vm.count("stage1file") > 0) {
			//env.setStringParamValue(vm["stage1file"].as<std::string>(), "ECListFileName");
		}
		
		if(vm.count("stage2file") > 0) {
			//env.setStringParamValue(vm["stage2file"].as<std::string>(), "AxiomListFileName");
		}

		FuzzyMultiDataLearnAlgorithm fuzzyMultiDataLearnAlgorithm;
		fuzzyMultiDataLearnAlgorithm.initFromEnv(env);
		fuzzyMultiDataLearnAlgorithm.setComments(comments);

		if ((comments) && (rank == 0)) {
			std::cout << "\n	Starting learning algorithm.\n";
			std::cout.flush();
		}

		//fuzzyMultiDataLearnAlgorithm.run();
		
		std::string res;
		
		// Отдельно - запуск только первого этапа
		if(stage1Flag) {
			res = fuzzyMultiDataLearnAlgorithm.runFirstLevel();
		}

		// Отдельно - запуск только второго этапа
		if(stage2Flag) {
			if (rank == 0)
				res.assign ("./axioms/bestECsList_rank_1");
			else 
				res.assign ("./axioms/bestECsList_rank_2_small");
			res = fuzzyMultiDataLearnAlgorithm.runSecondLevel (res);
		}
		
		// Отдельно - запуск только третьего этапа
		if(stage3Flag) {
			res.assign ("./axioms/bestAxiomsList");
			res = fuzzyMultiDataLearnAlgorithm.runThirdLevel (res);

			if ((comments) && (rank == 0)) {
				std::cout << "\n	Learning done.\n";
				std::cout.flush();
			}
		}

		MPI_Barrier (MPI_COMM_WORLD);

		MPI_Finalize ();

		return 0;

	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << "\n";
		MPI_Finalize ();
		return -1;
	}
}
