#include "Environment.h"
#include "AxiomLibException.h"
#include "FuzzyDataLearnAlgorithm.h"
#include "Check.h"
#include <iostream>
#include <time.h>
#include <string>

using namespace AxiomLib;

#define comments true
#define defaultConfigFile	"./axiomlib_fdt.conf"

int main (int argc, char** argv) {
	try {
		srand( (unsigned int) time(0));

		//std::cout << "\n	To Start please press Enter.";
		//std::cin.get();
		if (comments) {
			std::cout << "\n" << "AxiomLib / Fuzzy data - test of new approach.\n	Reading config-file." << "\n";
			std::cout.flush();
		}
	
		Environment envStart;
		envStart.readConfigParams (argc, argv);

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;

		if (comments) {
			std::cout << "\n	Initializing learning algorithm.\n";
			std::cout.flush();
		}

		FuzzyDataLearnAlgorithm fuzzyDataLearnAlgorithm;
		fuzzyDataLearnAlgorithm.initFromEnv(env);		
		fuzzyDataLearnAlgorithm.setComments(comments);

		if (comments) {
			std::cout << "\n	Starting learning algorithm.\n";
			std::cout.flush();
		}

		fuzzyDataLearnAlgorithm.run();
		
		if (comments) {
			std::cout << "\n	Learning done.\n";
			std::cout.flush();
		}

		return 0;

	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << "\n";
	}
}
