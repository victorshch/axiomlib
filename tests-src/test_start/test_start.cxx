#include "Environment.h"
#include "AxiomLibException.h"
#include "StartStrategy.h"
#include <iostream>
#include <time.h>
#include <string>

using namespace AxiomLib;

#define comments true

int main (int argv, char** argc) {
	try {
		srand(time(0));
		if (comments)
			std::cout << "\n" << "AxiomLib Start_test .\nReading config-file." << "\n";
		// создаем стратегию
		StartStrategy ss;
		// инициализирем ее
		ss.init();
		// запускаем
		ss.start();
	}
	catch ( AxiomLibException testex ) {
		std::cerr << "Fatal toplevel AxiomLibException:\n\t";
		std::cerr << testex.error() << "\n";
	}
}
