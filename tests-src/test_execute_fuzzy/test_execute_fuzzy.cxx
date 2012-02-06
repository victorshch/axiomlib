#include "Environment.h"
#include "AxiomLibException.h"
#include "FuzzyMultiDataLearnAlgorithm.h"
#include "AxiomExprSet.h"
#include "AxiomExprSetPlus.h"
#include <iostream>
#include <time.h>
#include <string>
#include <set>

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR
#include "mpi.h"

using namespace AxiomLib;

#define comments true
#define defaultConfigFile	"./axiomlib_exf.conf"

int main (int argc, char** argv) {
	try {
		MPI_Init(&argc, &argv);
		int size, rank = 0;
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		std::cout << "\n	process  " << rank << " size " <<  size << "\n";

		srand( (unsigned int) (time(0) + rank*1111));

		if ((comments) && (rank == 0)) {
			std::cout << "\n" << "AxiomLib / Execute AxiomSet on Fuzzy data.\n	Reading config-file." << "\n";
			std::cout << "To continue press enter.";
			std::cin.get();
		}	

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

		// Параметры, по которым сохранять лучщие решения
		std::string axiomSetBaseDir;
		if (env.getStringParamValue(axiomSetBaseDir, "AxiomSetBaseDir") < 0)
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : axiomSetBaseDir directory is undefined.");
		std::set <std::string> axiomSetNames;
		if (env.getStringSetParamValue(axiomSetNames, "AxiomSet") < 0)
			throw AxiomLibException("FuzzyMultiDataLearnAlgorithm::setParamsFromEnv : AxiomSet is undefined.");

		FuzzyMultiDataLearnAlgorithm fuzzyMultiDataLearnAlgorithm;
		fuzzyMultiDataLearnAlgorithm.initFromEnvRecognitionOnly(env);
		fuzzyMultiDataLearnAlgorithm.setComments(comments);

		std::cout << "\nResults are:";
		int errFirst, errSecond;
		double goal;
		int i = 1;
		for (std::set<std::string>::const_iterator c_iter = axiomSetNames.begin(); c_iter != axiomSetNames.end(); ++c_iter) {
			goal = fuzzyMultiDataLearnAlgorithm.matterAxiomSetFunc (axiomSetBaseDir, *c_iter, errFirst, errSecond);
			std::cout << "\n\t" << i <<".\tAxiomSet: " << *c_iter << "\t" << goal << " (" << errFirst << ", " << errSecond << ").";
			i++;
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
