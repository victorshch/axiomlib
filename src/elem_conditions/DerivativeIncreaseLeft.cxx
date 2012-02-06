/****************************************************************************
*
*			������� ������ DerivativeIncreaseLeft
*
****************************************************************************/

#include <string>
#include "DerivativeIncreaseLeft.h"
#include "DerivativeDecreaseLeft.h"
#include <stdlib.h>
#include "../AxiomLibException.h"
#include "../Round.h"
#include "../Defines.h"

using namespace AxiomLib;



#define eps		0.0000000001
#define double_default_Min_Level_Limit	0.001

/****************************************************************************
*			DerivativeIncreaseLeft::DerivativeIncreaseLeft
*
*	Description:	��������� �����������, ��������� ����������
*				����������� � ������������ ����������
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
DerivativeIncreaseLeft::DerivativeIncreaseLeft (void) {
	// ���������� �� ��������� �������� ����������
	
	// ����������� ����������� ���������� ������� ������������� �������
	derivIncreaseMinLevel = double_default_Min_Level_Limit;
	
	// ������������� ����������� ��������� ������� ������������� �������
	derivIncreaseMaxLevel = 10;
	
	// ����� ����� ��������� ��� ������������� ������� - �����
	derivIncreaseLeft = 2;
	
	// ����� ����� ��������� ��� ������������� ������� - ������
	derivIncreaseRight = 0;

	// ����������� ������� ��������� ��� ��������� derivIncreaseMinLevel, ������� ����������� ��� ������� ���������� ���������
	derivIncreaseMinLevelLimit = derivIncreaseMinLevel;
	
	// ������������� ������� ��������� ��� ��������� derivIncreaseMaxLevel, ������� ����������� ��� ������� ���������� ���������
	derivIncreaseMaxLevelLimit = derivIncreaseMaxLevel;

	// ��� ��������� ���������� derivIncreaseMinLevel � derivIncreaseMaxLevel, ��� ������� ���������� ���������
	levelDelta = derivIncreaseMaxLevel - derivIncreaseMinLevel;
	
	// ������������ �������� ��������� derivIncreaseLeft, ������� ����������� ��� ������� ���������� ���������
	derivIncreaseLeftLimit = derivIncreaseLeft;
	
	// ������������ �������� ��������� derivIncreaseRight, ������� ����������� ��� ������� ���������� ���������
	derivIncreaseRightLimit = derivIncreaseRight;
}

/****************************************************************************
*			DerivativeIncreaseLeft::name
*
*	Description:	������� ���������� ��� �������
*	Parameters:	-
*	Returns:		std::string - ��� �������
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
std::string DerivativeIncreaseLeft::name (void) const {
	std::string toReturn("derivative_increase_left");
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseLeft::check
*
*	Description:	�������� ���������� ������� ����. ������� ��� ����� � ������� k (������� � ����) ���� x
*	Parameters:	unsigned long k		- ����� �����, ��� ������� ����������� ������������, ������� � 0
*				std::vector<double> x	- ��������������� ��������� ���
*	Returns:		1 ���� ������� �����������
*				0 ���� ������� �� �����������
*				XXX: -1 ���� ������� �� ���������, �������� ��-�� ����, ��� ����� k ������� ������ � ������� ����
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::check (const unsigned long k, const std::vector<double>& x) const{
	signed int toReturn;
	
	// �������� ������
	if (((int) k < derivIncreaseLeft) || ((int)(x.size() - k) <= derivIncreaseRight)) {
		toReturn = -1;
		//throw AxiomLibException("Error in DerivativeIncreaseLeft::check : out of range.");
		return toReturn;
	}
	// �������� ������������� �������
	toReturn = 1;
	for (int i = 0; i < derivIncreaseLeft; i++) {
		if (((x[k-i] - x[k-i-1]) < derivIncreaseMinLevel) || ((x[k-i] - x[k-i-1]) > derivIncreaseMaxLevel)) {
			return 0;
		}
	}
	
	return toReturn;
}

/****************************************************************************
*			DerivativeIncreaseLeft::operator<
*
*	Description:	��������� ���� DerivativeIncreaseLeft �� ��������� (x < y).
*	Parameters:	DerivativeIncreaseLeft& second - ������ ����� ���������
*	Returns:		1 ���� < � second - ��� DerivativeIncreaseLeft
*				-1 ���� > � second - ��� DerivativeIncreaseLeft
*				0 ����� (�� ����, ���� ��������� ���������� �� �����, ��� �� 0.01 ��� ���� second - ��� �� DerivativeIncreaseLeft)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
double DerivativeIncreaseLeft::operator< (const ElemCondition& second) const{
	// �������� �������� y ���� DerivativeIncreaseLeft* �� second ���� el_axiom
	const DerivativeIncreaseLeft* y = dynamic_cast<const DerivativeIncreaseLeft*> (&second);
  
	// ���� y != NULL, �.�. second ������������� ����� ��� DerivativeIncreaseLeft&, �� ����� ���-�� ���������
	// ���� y == NULL, �.�. �� ��������� ��� ������ ������� ������� ����, ����� ���������� 0
	if (y != NULL) {
		if ((derivIncreaseMaxLevel < y->derivIncreaseMaxLevel + 0.01) && (derivIncreaseMinLevel > y->derivIncreaseMinLevel - 0.01))
			return 1;
		else if ((y->derivIncreaseMaxLevel < derivIncreaseMaxLevel + 0.01) && (y->derivIncreaseMinLevel > derivIncreaseMinLevel - 0.01))
				return -1;
			else
				return 0;
	}
	else
		return 0;
}

/****************************************************************************
*			DerivativeIncreaseLeft::transmute
*
*	Description:	�������, �������� �������� ���������� � �� ����� ��� p ���
*	Parameters:	double p - ������� ��������� ���������
*	Returns:		0 ���� ��� ok
*				XXX: -1 ���� p<0 ��� p>1 ��� �� �����-�� �������� �� ���������� ��������
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::transmute (const double p) {
	signed int toReturn = 0;
	// �������� ������ p
	if (p < 0 || p > 1) {
		return -1;
	}
	
	// �������� ������� ���������
	double change = (((double)rand())/ ((double) RAND_MAX)) * p ;
	
	if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
		// ����������� ������� �������
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// ����������� ������� �������
			derivIncreaseMaxLevel = derivIncreaseMaxLevel * (1 + change);
			toReturn = 1;
		}
		else {
			// ��������� ������ �������
			derivIncreaseMinLevel = derivIncreaseMinLevel * (1 - change);
			toReturn = 1;
		}
	}
	else {
		// ��������� ������� �������
		if (((double)rand())/ ((double) RAND_MAX) > 0.5) {
			// ��������� ������� �������
			derivIncreaseMaxLevel = derivIncreaseMaxLevel - (derivIncreaseMaxLevel  - derivIncreaseMinLevel) * change;
			toReturn = 1;
		}
		else {
			// ����������� ������ �������
			derivIncreaseMinLevel = derivIncreaseMinLevel + (derivIncreaseMaxLevel  - derivIncreaseMinLevel) * change;
			toReturn = 1;
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseLeft::setParams
*
*	Description:	������ �������� ���������� ������������� ������� 
*	Parameters:	map<string,string> - ����� �������� ��������� <-> ������� ��������
*	Returns:		0 ���� ��� ok (����� ���� �� ��� ��������� ���� � �����)
*	Throws:		AxiomLibException ���� ����� ������ ��� � ����� ���� ������ ���������
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::setParams (const std::map<std::string,std::string>& newParams) {
	signed int toReturn=0;
	
	// ���������, ������ �� �����?
	if (newParams.size() == 0) {
		toReturn = -1;
		throw AxiomLibException("Error in DerivativeIncreaseLeft::setParams : zero parameter map.");
	}
	int flag = 0;
	// ����� �����: ����� � �����, ��������� ���������
	// ��� ����, �� �����������, ��� �� ��� ��������� ����� �������� � ���� �����, ��� ���
	for (std::map<std::string,std::string>::const_iterator i = newParams.begin(); i != newParams.end(); i++) {
		std::string paramName = i->first;
		std::string paramValue = i->second;
		flag = 0;
		// ����, ������ ��������� ������������� i
		if (paramName == (const std::string)("derivIncreaseMinLevel")) {
			// ok, �����, ������� value
			derivIncreaseMinLevel = atof(paramValue.c_str());
			flag = 1;
		}
		if (paramName == (const std::string)("derivIncreaseMaxLevel")) {
			// ok, �����, ������� value
			derivIncreaseMaxLevel = atof(paramValue.c_str());
			flag = 2;
		}
		if (paramName == (const std::string)("derivIncreaseLeft")) {
			// ok, �����, ������� value
			derivIncreaseLeft = atoi(paramValue.c_str());
			flag = 3;
		}
		if (paramName == (const std::string)("derivIncreaseRight")) {
			// ok, �����, ������� value
			derivIncreaseRight = atoi(paramValue.c_str());
			flag = 4;
		}
		if (flag == 0) {
			// ����������� ��������
			throw AxiomLibException("Error in DerivativeIncreaseLeft::setParams : unknown parameter.");
		}
	}
	
	return toReturn;
}


/****************************************************************************
*			DerivativeIncreaseLeft::getParams
*
*	Description:	���������� �������� ����������
*	Parameters:	map<string,string> - ����������� ����� �������� ��������� <-> ������� ��������
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::getParams (std::map<std::string,std::string> &params) const{
	params.clear();
	char temp[64];
	sprintf( temp, "%f", derivIncreaseMinLevel);
	std::string str1 (temp);
	params["derivIncreaseMinLevel"] = str1;
	sprintf( temp, "%f", derivIncreaseMaxLevel);
	std::string str2 (temp);
	params["derivIncreaseMaxLevel"] = str2;
	sprintf( temp, "%d", derivIncreaseLeft);
	std::string str3 (temp);
	params["derivIncreaseLeft"] = str3;
	sprintf( temp, "%d", derivIncreaseRight);
	std::string str4 (temp);
	params["derivIncreaseRight"] = str4;
	return 0;
}


/****************************************************************************
*			DerivativeIncreaseLeft::setParamValue
*
*	Description:	������� ������������� �������� ������ ��������� ������������� �������, 
*				���� ����� �������� ������������ � ������ ������������ �������
*	Parameters:	param - ��������, �� ������� ����� �������� ��������
*				paramName - ��� ���������, �������� �������� ����� ��������
*	Returns:		0 - ���� ������� �������� �������� ���������
*				-1 - ���� �� ������� �������� �������� �������� (�������� 
*				       �������� � ����� ������ ��� � ������ ������������ �������)
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::setParamValue (const double param, const std::string &paramName) {
	if (paramName == "derivIncreaseMinLevel") {
		derivIncreaseMinLevel = param;
		return 0;
	}
	if (paramName == "derivIncreaseMaxLevel") {
		derivIncreaseMaxLevel = param;
		return 0;
	}
	if (paramName == "derivIncreaseLeft") {
		derivIncreaseLeft = round (param);
		return 0;
	}
	if (paramName == "derivIncreaseRight") {
		derivIncreaseRight = round (param);
		return 0;
	}
	return -1;
} 


/****************************************************************************
*			DerivativeIncreaseLeft::getParamValue
*
*	Description:	������� ���������� �������� ������ ��������� ������������� �������, 
*				���� ����� �������� ������������ � ������ ������������ �������
*	Parameters:	param - ����������� �������� ���������
*				paramName - ����� � ������ ���������, �������� �������� ��������� ��������
*	Returns:		0 - ���� ������� ��������� ��������
*				-1 - ���� �� ������� ������� �������� (�������� ��� ������ ��� � ������ ������������ �������)
*				1 - ���� �������� ������������� ����
*				2 - ���� �������� �� ��������� � �� ������������ ����
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::getParamValue (double &param, const std::string &paramName) const {
	if (paramName == "derivIncreaseMinLevel") {
		param = derivIncreaseMinLevel;
		return 0;
	}
	if (paramName == "derivIncreaseMaxLevel") {
		param = derivIncreaseMaxLevel;
		return 0;
	}
	if (paramName == "derivIncreaseLeft") {
		param = (double) derivIncreaseLeft;
		return 0;
	}
	if (paramName == "derivIncreaseRight") {
		param = (double) derivIncreaseRight;
		return 0;
	}
	return -1;
}


/****************************************************************************
*			DerivativeIncreaseLeft::getECParamNames
*
*	Description:	������� ��������� ������� ������ �������� � ���������� ���������� ������ ������������ �������
*	Parameters:	ecParamNames - ����������� ������ � ���������� ����������
*	Returns:		0
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::getECParamNames (std::vector<std::string> &ecParamNames) const{
	ecParamNames.resize(4);
	ecParamNames[0] = "derivIncreaseMinLevel";
	ecParamNames[1] = "derivIncreaseMaxLevel";
	ecParamNames[2] = "derivIncreaseLeft";
	ecParamNames[3] = "derivIncreaseRight";
	return 0;
}


/****************************************************************************
*			DerivativeIncreaseLeft::checkAndCompatibility
*
*	Description:	��������� ������������� ������������ ������� � ������� �� ���������� ������ "�"
*	Parameters:	const ElemCondition& second - �������, � ������� ��������� �� �������������
*	Returns:		1 ���� ����������
*				0 ���� �� ����������
*				-1 ���� undefined
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::checkAndCompatibility (const ElemCondition& second) const{
	// �������� �������� y ���� DerivativeIncreaseLeft* �� second ���� el_axiom
	const DerivativeIncreaseLeft* y = dynamic_cast<const DerivativeIncreaseLeft*> (&second);
  
	// ���� y != NULL, �.�. second ������������� ����� ��� DerivativeIncreaseLeft&, �� ����� ���-�� ���������
	// ���� y == NULL, �.�. �� ��������� ��� ������ ������� ������� ����, ����� ���������� 0
	if (y != NULL) {
		if ( ((derivIncreaseMaxLevel >= y->derivIncreaseMaxLevel) && (derivIncreaseMinLevel <= y->derivIncreaseMaxLevel)) ||
		     ((derivIncreaseMaxLevel >= y->derivIncreaseMinLevel) && (derivIncreaseMinLevel <= y->derivIncreaseMinLevel)) )
			return 1;
		else
			return 0;
	}
	else {
		const DerivativeDecreaseLeft* x = dynamic_cast<const DerivativeDecreaseLeft*> (&second);
		if (x != NULL)
			return 0;
		return 1;
	}
	return -1;
}


/****************************************************************************
*					DerivativeIncreaseLeft::setLimits
*
*	Description:	������� ������������� ������������ �������� ���������� derivIncreaseLeftLimit � derivIncreaseRightLimit
*	Parameters:		newLeftLimit - ����� �������� ��� ��������� derivIncreaseLeftLimit
*					newRightLimit - ����� �������� ��� ��������� derivIncreaseRightLimit
*	Returns:		0
*	Throws:			AxiomLibException - ���� ���� �� ���� �� ������� ���������� 
*					����� ������������� ��������. 
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::setLimits (const int newLeftLimit, const int newRightLimit) {
	if ((newLeftLimit < 0) || (newRightLimit < 0))
		throw AxiomLibException("Error in DerivativeIncreaseLeft::setLimits : parameters have a wrong values.");
	derivIncreaseLeftLimit = newLeftLimit;
	derivIncreaseRightLimit = newRightLimit;
	// ������ ������ �� ����������� ������� ������ ���� ����� 0
	derivIncreaseRightLimit = 0;
	return 0; 
}


/****************************************************************************
*					DerivativeIncreaseLeft::setLimits
*
*	Description:	������� ������������� �������� ���������� derivIncreaseMinLevelLimit, derivIncreaseMaxLevelLimit � levelDelta
*	Parameters:		row - ������� ����������, �� �������� ����������� �������� ����������
*					reserve - �������� �������������� ������ �� ������� ������� ��������� �������� ��������� ����������
*					numOfLevels - ����� ����� �� ���������� ���������� ������������� ������� 
*						� ������������ �� �������������, �� �������� ������������ levelDelta
*	Returns:		0 - ���� ����� ��������� �����������
*					-1 - ���� �� ������� ���������� ����� ���������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::setLimits (const std::vector<double>& row, const double reserve, const unsigned int numOfLevels) {
	// ������� �������� ������������� �������
	if (row.size() < 2) 
		return -1;
	derivIncreaseMinLevelLimit = max (row[1] - row[0], double_default_Min_Level_Limit);
	derivIncreaseMaxLevelLimit = derivIncreaseMinLevelLimit;
	for (int k = 2; k < (int) row.size(); k++) {
		derivIncreaseMinLevelLimit = min (row[k] - row[k-1], derivIncreaseMinLevelLimit);
		derivIncreaseMinLevelLimit = max (double_default_Min_Level_Limit, derivIncreaseMinLevelLimit);
		derivIncreaseMaxLevelLimit = max (row[k] - row[k-1], derivIncreaseMaxLevelLimit);
	}
	// ����������� ����������� � ������������ ��������
	if (reserve < 0.001)
		return -1;
	if (derivIncreaseMinLevelLimit < 0)
		derivIncreaseMinLevelLimit *= reserve;
	else 
		derivIncreaseMinLevelLimit /= reserve;
	if (derivIncreaseMaxLevelLimit > 0)
		derivIncreaseMaxLevelLimit *= reserve;
	else 
		derivIncreaseMaxLevelLimit /= reserve;
	// ����������� ���� ��������� ����������
	if (numOfLevels < 1)
		throw AxiomLibException("Error in DerivativeIncreaseLeft::setLimits : parameter 'numOfLevels' has a wrong value.");
	levelDelta = (derivIncreaseMaxLevelLimit - derivIncreaseMinLevelLimit) / (double) numOfLevels;
	return 0;
}


/****************************************************************************
*					DerivativeIncreaseLeft::changeParams
*
*	Description:	������� �������� ��������� ������������� ������� �� ����� �����, ��������� � ����������
*	Parameters:		numOfSteps - ����� �����, �� ������� �������� ��������� ������������� �������
*	Returns:		0 - ���� ��������� ��������� ������
*					-1 - ���� ��������� �� ��������� ����� ���� ����������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int DerivativeIncreaseLeft::changeParams (const int numOfSteps) {
	if (numOfSteps == 0) { // ���� ������� �������� = 0 - �� ������������� ��������� �������� ��� ���������� ������
		derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
		derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
		derivIncreaseLeft = 0;
		derivIncreaseRight = 0;
		return 0;
	}
	if (numOfSteps > 0) { // �������� �� ������������ ����� ����� ������ (����������� ����������)
		for (int num = 0; num < numOfSteps; num++) {
			if ((derivIncreaseMaxLevel+levelDelta - derivIncreaseMaxLevelLimit) > eps) {
				if ((derivIncreaseMinLevel+levelDelta - derivIncreaseMaxLevelLimit) > eps) {
					if (derivIncreaseLeft >= derivIncreaseLeftLimit) {
						if (derivIncreaseRight >= derivIncreaseRightLimit)
							return -1;
						else {
							derivIncreaseRight++;
							derivIncreaseLeft = 0;
							derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
							derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
						}
					} else {
						derivIncreaseLeft++;
						derivIncreaseMinLevel = derivIncreaseMinLevelLimit;
						derivIncreaseMaxLevel = derivIncreaseMinLevelLimit;
					}
				} else {
					derivIncreaseMinLevel += levelDelta;
					derivIncreaseMaxLevel = derivIncreaseMinLevel;
				}
			} else
				derivIncreaseMaxLevel += levelDelta;
		}
		return 0;
	} else { // �������� �� ������������ ���������� ����� �����
		for (int num = 0; num > numOfSteps; num--) {
			if ((derivIncreaseMaxLevel-levelDelta - derivIncreaseMinLevel) < -eps) {
				if ((derivIncreaseMinLevel-levelDelta - derivIncreaseMinLevelLimit) < -eps) {
					if (derivIncreaseLeft == 0) {
						if (derivIncreaseRight == 0)
							return -1;
						else {
							derivIncreaseRight--;
							derivIncreaseLeft = derivIncreaseLeftLimit;
							derivIncreaseMinLevel = derivIncreaseMaxLevelLimit;
							derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
						}
					} else {
						derivIncreaseLeft--;
						derivIncreaseMinLevel = derivIncreaseMaxLevelLimit;
						derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
					}
				} else {
					derivIncreaseMinLevel -= levelDelta;
					derivIncreaseMaxLevel = derivIncreaseMaxLevelLimit;
				}
			} else
				derivIncreaseMaxLevel -= levelDelta;
		}
		return 0;
	}

	return -1;
}
