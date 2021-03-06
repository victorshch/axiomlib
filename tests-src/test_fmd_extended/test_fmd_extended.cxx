/**********************************************************************
Данная программа - инструмент для запуска этапов алгоритма fuzzy_multi_data_ext.
**********************************************************************/

#include "all_axioms.h"

#include <boost/format.hpp>
#include "Environment.h"
#include "AxiomLibException.h"
#include "FuzzyMultiDataExtAlgorithm.h"
#include "Check.h"

#include <iostream>
#include <time.h>
#include <string>
#include <algorithm>
#include <vector>
#include <string>

//#include <boost/archive/xml_iarchive.hpp>
//#include <boost/archive/xml_oarchive.hpp>

//#include "elem_conditions/Export.h"
//#include "elem_conditions/Export.cxx"

#undef SEEK_SET
#undef SEEK_END
#undef SEEK_CUR

using namespace AxiomLib;

#define comments true
#define defaultConfigFile	"./axiomlib_fmd.conf"

#define QUESTION_MARK_LOCAL_INDEX -1

struct ResultInfo {
	std::string asName;
	double objectiveValue;
	int firstKindErrors;
	int secondKindErrors;
    std::vector<std::string> stringMarkUps;

	ResultInfo() : objectiveValue(-1), firstKindErrors(-1), secondKindErrors(-1) {}

    bool operator< (const ResultInfo& other) const {
		return objectiveValue < other.objectiveValue;
	}
};

int main (int argc, char** argv) {
	try {
		srand (time(NULL));
		Logger::getInstance()->setDebug(true);
		Logger::getInstance()->setComments(true);

        Environment envStart;
		envStart.readConfigParams (argc, argv);

		std::string pathToConfigFile;
		if (envStart.getStringParamValue (pathToConfigFile, "ConfigFile") < 0)
			pathToConfigFile.assign (defaultConfigFile);

		Environment env;
		env.readConfigFile(pathToConfigFile.c_str());
		env += envStart;

		FuzzyMultiDataExtAlgorithm alg;
		alg.initFromEnv(env);

		alg.run();

		std::vector<ResultInfo> result;
		result.reserve(alg.getASSize());

		for(int i = 0; i < alg.getASSize(); ++i) {
			ResultInfo info;
			const AxiomExprSetPlus& as = alg.getAS(i);
			info.asName = as.name();
			info.objectiveValue = as.goal;
			info.firstKindErrors = as.errFirst;
			info.secondKindErrors = as.errSecond;
            for (int classNo = 0; classNo < as.markUps.size(); classNo++) {
                std::string markUp;
                for (int j = 0; j < as.markUps[classNo].size(); j++) {
                    if (as.markUps[classNo][j] == QUESTION_MARK_LOCAL_INDEX) {
                        markUp += "(?)";
                    } else {
                        markUp += "(" + std::to_string(as.markUps[classNo][j]) + ")";
                    }
                }
                info.stringMarkUps.push_back(markUp);
            }
			result.push_back(info);
		}

//		std::sort(result.begin(), result.end());
//		std::reverse(result.begin(), result.end());

		int size = (int) result.size();

		std::cout << "Best axiom sets:" << std::endl;
		for(int i = 0; i < size; ++i) {
			const ResultInfo& currentInfo = result[i];
			std::cout << boost::format("%s\t%s\t(%d, %d)") % currentInfo.asName
						 % currentInfo.objectiveValue
						 % currentInfo.firstKindErrors
						 % currentInfo.secondKindErrors
						<< std::endl;
            std::cout << std::endl;
            for (int classNo = 0; classNo < currentInfo.stringMarkUps.size(); classNo++) {
                std::cout << "Markup for class " + std::to_string(classNo) << std::endl;
                std::cout << currentInfo.stringMarkUps[classNo] << std::endl;
            }
		}

        //alg.saveAxiomSetsToXml();

		return 0;
	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << "\n";
		return -1;
	}
}
