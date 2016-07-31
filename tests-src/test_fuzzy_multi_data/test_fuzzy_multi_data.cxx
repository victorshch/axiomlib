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

#define comments true
#define defaultConfigFile	"./axiomlib_fmd.conf"

int main (int argc, char** argv) {
	try {
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
//		if (rank == 0) {
//			std::cout << "\nTo begin press enter.";
//			std::cout.flush();
//			std::cin.get();
//		}
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

		FuzzyMultiDataLearnAlgorithm fuzzyMultiDataLearnAlgorithm;
		fuzzyMultiDataLearnAlgorithm.initFromEnv(env);
		fuzzyMultiDataLearnAlgorithm.setComments(comments);

		if ((comments) && (rank == 0)) {
			std::cout << "\n	Starting learning algorithm.\n";
			std::cout.flush();
		}

		fuzzyMultiDataLearnAlgorithm.setComments(true);
		fuzzyMultiDataLearnAlgorithm.run();
		
		std::string res;
		
		// Отдельно - запуск только первого этапа
		//res = fuzzyMultiDataLearnAlgorithm.runFirstLevel();

		// Отдельно - запуск только второго этапа
		//if (rank == 0)
		//	res.assign ("./axioms/bestECsList_rank_1");
		//else 
		//	res.assign ("./axioms/bestECsList_rank_2_small");
		//res.assign ("./axioms/bestECsListPhoneme_rank_1");
		//res = fuzzyMultiDataLearnAlgorithm.runSecondLevel (res);
				
		// Отдельно - запуск только третьего этапа
		//res.assign ("./axioms/bestAxiomsList_fta_15_03_v2New");
		//res = fuzzyMultiDataLearnAlgorithm.runThirdLevel (res);

		if ((comments) && (rank == 0)) {
			std::cout << "\n	Learning done.\n";
			std::cout.flush();
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
	catch (std::exception ex) {
		std::cerr << "Fatal toplevel std::exception:\n\t";
		std::cerr << ex.what() << "\n";
		MPI_Finalize ();
		return -1;		
	}
	catch (...) {
		std::cerr << "Fatal toplevel unknown exception";
		MPI_Finalize ();
		return -1;				
	}
}
