/****************************************************************************
*						AxiomStructure
*
*	Description:	-
*	Author:		dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_AxiomStructure_HXX
#define __AXIOMS_LIB_AxiomStructure_HXX

namespace AxiomLib {

class AxiomStructure {

public:
	
	std::string axiomName;
	
	std::map<std::string, std::map<std::string, std::string> > axiomParameters;
	
	// функция записи в toReturn всех названий элементарных условий в axiomParameters
	int getVectorOfEC (std::vector<std::string>& toReturn) {
		toReturn.resize( axiomParameters.size() );
		std::map<std::string, std::map<std::string, std::string> >::iterator mIter;
		unsigned int i = 0;
		for (mIter = axiomParameters.begin(); mIter != axiomParameters.end(); mIter++) {
			if ( i >= toReturn.size() )
				throw AxiomLibException("Error in AxiomStructure::getVectorOfEC: wrong parameters of map-structure.");
			toReturn[i] = mIter->first;
			i++;
		}
		return 0;
	}
	
}; //end of class

}; //  end of namespace

#endif /* __AXIOMS_LIB_AxiomStructure_HXX */
