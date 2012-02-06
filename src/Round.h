/****************************************************************************
*						round
*
*	Description:	Вспомогательная функция округления числа
*	Parameters:	-
*	Returns:	 	-
*	Throws:		-
*	Author:		dk
*	History:     	-
*
****************************************************************************/
#ifndef __AXIOMS_LIB_ROUND_HXX
#define __AXIOMS_LIB_ROUND_HXX

namespace AxiomLib {

inline int round (double d) {
	if (d > 0){
		if ((d - int(d)) < 0.5)
			return int(d);
		return 1 + int(d);}
	if ((int(d) - d) < 0.5) 
		return int(d);
	return int(d) - 1;
}

}; // end of namespace

#endif
