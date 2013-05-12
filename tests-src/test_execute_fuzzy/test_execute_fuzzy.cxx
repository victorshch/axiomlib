#include "Environment.h"
#include "AxiomLibException.h"
#include "FuzzyMultiDataExtAlgorithm.h"
#include "AxiomExprSet.h"
#include "AxiomExprSetPlus.h"
#include <iostream>
#include <time.h>
#include <string>
#include <set>

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR

using namespace AxiomLib;

#define comments true
#define defaultConfigFile	"./axiomlib_exf.conf"

int loadAxoimExprSet (const std::string baseDir, const std::string fileName,
					  const std::vector <std::string>& dataSetParams, AxiomExprSetPlus& aesp) {
	// Инициализация системы аксиом из указанного файла
	AxiomSetBase axiomSetBase;
	AxiomExprSetStructure aess;
	std::string axiomSetPath = baseDir;
	axiomSetPath.append("/");
	axiomSetPath.append(fileName);
	// Считываем информацию о системе аксиом в aess
	axiomSetBase.readFromAS (axiomSetPath, aess, dataSetParams);
	aesp.initAxiomSetFromStructure (aess);

	return 0;
}

std::set<std::string> getAxiomSetNames(const std::string& axiomSetNameTemplate, const std::string& axiomSetDir) {
	std::set<std::string> all_matching_files;

	boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
	for( boost::filesystem::directory_iterator i(axiomSetDir); i != end_itr; ++i )
	{
		// Skip if not a file
		if( !boost::filesystem::is_regular_file( i->status() ) ) continue;

		std::string currentAxiomName = i->path().stem();

		if(currentAxiomName.compare(0, axiomSetNameTemplate.size(), axiomSetNameTemplate) != 0) {
			continue;
		}

		// File matches, store it
		all_matching_files.insert( currentAxiomName );
	}

	return all_matching_files;
}

int main (int argc, char** argv) {
	try {
		if ((comments)) {
			std::cout << "\n" << "AxiomLib / Execute AxiomSet on Fuzzy data.\n" << std::endl;
		}

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
			throw AxiomLibException("Execute_fuzzy : axiomSetBaseDir directory is undefined.");
		std::set <std::string> axiomSetNames;
		if (env.getStringSetParamValue(axiomSetNames, "AxiomSet") < 0) {
			std::string axiomSetNameTemplate;
			if (env.getStringParamValue(axiomSetNameTemplate, "AxiomSetNameTemplate")) {
				throw AxiomLibException("Execute_fuzzy : no AxiomSet specified");
			}
			axiomSetNames = getAxiomSetNames(axiomSetNameTemplate, axiomSetBaseDir);
		}

		FuzzyMultiDataExtAlgorithm fuzzyMultiDataExtAlgorithm;
		fuzzyMultiDataExtAlgorithm.initFromEnvRecognitionOnly(env);
		fuzzyMultiDataExtAlgorithm.setComments(comments);

		const FuzzyDataSet& dataSet = fuzzyMultiDataExtAlgorithm.getDataSet();

//		std::vector<std::string> dataSetParams = fuzzyMultiDataLearnAlgorithm.getDataSet().getParamNames();

		std::vector<std::string> asNames;
		for (std::set<std::string>::const_iterator c_iter = axiomSetNames.begin();
			 c_iter != axiomSetNames.end(); ++c_iter) {
			asNames.push_back(*c_iter);
		}

		fuzzyMultiDataExtAlgorithm.loadAxiomSetsFromXml(asNames);

		std::cout << "\nResults are:";

		for(int i = 0; i < (int)fuzzyMultiDataExtAlgorithm.getASSize(); ++i) {
			AxiomExprSetPlus& aess = fuzzyMultiDataExtAlgorithm.getAS(i);
			fuzzyMultiDataExtAlgorithm.recalculateMatterAxiomSetFunc(aess);
			std::cout << "\n\t" << i <<".\tAxiomSet: " << aess.name() << "\t"
					  << aess.goal << " (" << aess.errFirst << ", " << aess.errSecond << ").";

			std::cout << std::endl;
			std::cout << "Detailed error statistics:" << std::endl;

			int classCount = dataSet.getClassCount();
			for(int classNo = -1; classNo < classCount; classNo++) {
				std::cout << "Class '" + dataSet.className(classNo) + "'" << std::endl;
				int multiTSCount = dataSet.getMutiTSCount(FuzzyDataSet::Testing, classNo);
				std::cout << "Trajectory statistics:" << std::endl;
				for(int multiTS = 0; multiTS < multiTSCount; multiTS++) {
					//std::string name = dataSet.getMultiTSName(FuzzyDataSet::Testing, classNo, multiTS);
					std::cout << "\tMultiTS no. " << multiTS ;//<<", name " << name;
					//std::cout << std::endl;
					std::pair<int, int> errors = aess.getErrorsForTraj(classNo, multiTS);
					std::cout << "\tErrors: (" << errors.first << ", " << errors.second << ")\n";
				}
			}
			std::cout << std::endl;
		}



		return 0;

	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << "\n";
		return -1;
	}
}
