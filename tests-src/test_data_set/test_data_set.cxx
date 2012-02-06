#include "Environment.h"
#include "DataSet.h"
#include "MultiTS.h"
#include <iostream>

using namespace AxiomLib;

int main (int argv, char** argc) {
	try {
		std::cout << "\nTest starts.\n";
		std::cout << "\n" << "\n\ttest: reading CSV files \n";
		DataSet dataSet;
		dataSet.readDataSet("./datasets", "model");
		
		std::cout << "\nTest done.\n";
		return 0;
	}
	catch ( AxiomLibException testex ) {
	      std::cerr << "Fatal exception:\n\t";
	      std::cerr << testex.error() << "\n";
	 }	
}
