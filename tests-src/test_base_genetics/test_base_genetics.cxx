#include "Environment.h"
#include "BaseGenetics.h"
#include "AxiomLibException.h"
#include "Check.h"
#include <iostream>
#include <time.h>
#include <string>

using namespace AxiomLib;
using namespace std;

#define comments true
#define	defaultConfigFile	"./axiomlib_bg.conf"

int main (int argc, char** argv) {
  try {
    srand( (unsigned int) time(0));
	if (comments) {
      std::cout << "\n" << "AxiomLib 0.6 / BaseGenetics.\nReading config-file." << "\n";
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
		
    // инициализируем генетический алгоритм
    string gaToUse;
		
    // Проверяем, задан лт ГА
    if (env.getStringParamValue(gaToUse, "GA") < 0) {
      cerr << "No GA set in the environment.\n";
      return -1;
    }
    // Проверяем, задан ли нужный нам ГА
    if (gaToUse == "BaseGenetics") {
      BaseGenetics ga;
      ga.initFromEnv(env);
      // запускаем генетический алгоритм
      ga.run();
      // выводим результаты работы ГА
      ga.writeResultsToDisplay();
      if (comments)
        std::cout << "\n";
      return 0;
    }
    else {
      cerr << "GA '" << gaToUse << "' not implemented.\n";
      return -1;
    }
std::cout << "\n\tTEST 4\n";
  }
  catch ( AxiomLibException testex ) {
    std::cerr << "Fatal toplevel AxiomLibException:\n\t";
    std::cerr << testex.error() << "\n";
  }
}
