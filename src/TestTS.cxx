/****************************************************************************
*			������� ������ TestTS.h
*
*	Description:	������ ����� ������ ��� ������� �������� �������� ����������
*				�� ���� � ��� ������������ ���� ��� �������� ���������� ���� � ���
*				�������� ���������� �������� ������� ����. - ���������� ������.
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "TestTS.h"

using namespace AxiomLib;

// ����������� ������
TestTS::TestTS () {
	this->corrMarkUp.clear();
}

// ���������� ������ (������)
TestTS::~TestTS () {

}

// ������� ���������� ����� ��������� ����� � ������ �������� ��������
int TestTS::size(void) const {
	return data.size();
}

/****************************************************************************
*					TestTS::getTSByIndex
*
*	Description:	������� ������ ��������� ��� �� �������, ��� 
*				������� ������� - ����� ���� � ������������ ������������������ ����������
*	Parameters:	vec - ����������� �������� ������� ����� 
*				i - ����� �������������� ���� � ������������ ������������������ ����������
*	Returns:		true - ���� ��������� ��� ���������� � �� ������� ������� � ���������� vec
*				false - ���� ������������� ��� �� ���������� ��� �� ������� ��� ����������� � vec
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool TestTS::getTSByIndex(std::vector<double> &vec, int i) const {
	// ���� ������ �������� -1 - �� ���������� ���������� ��������
	if (i == -1) {
		return correct.getTSByIndex (vec, i);
	}
	// ���� ����� ������������� �������� - �� ���������� ��� �� ���������� �� ��� ������� �� ���������� �������������
	if (i >= 0) {
		return data.getTSByIndex (vec, i);
	}
	return false;
}


/****************************************************************************
*					TestTS::getTSByIndex
*
*	Description:	������� ������ ��������� ��� �� �������, ��� 
*				������� ������� - ����� ���� � ������������ ������������������ ����������.
*				������� �� ���������� ������� - � ������� ���������� ������� - � ������, 
*				���� ����� ������ ����� �����, �� �������� ������ ����� ������.
*	Parameters:	vec - ����������� �������� ������� ����� 
*				i - ����� �������������� ���� � ������������ ������������������ ����������
*	Returns:		true - ���� ��������� ��� ���������� � �� ������� ������� � ���������� vec
*				false - ���� ������������� ��� �� ���������� ��� �� ������� ��� ����������� � vec
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool TestTS::getTSByIndex(std::vector<int> &vec, int i) const {
	// ���� ������ �������� -1 - �� ���������� ���������� ��������
	if (i == -1) {
		if (corrMarkUp.size() > 0) {
			vec = corrMarkUp;
			return true;
		} 
		return false;
	}
	// ���� ����� ������������� �������� - �� ���������� ��� �� ���������� �� ��� ������� �� ���������� �������������
	if (i >= 0) {
		// ���������� ���������� ��� ���������� ������� ������������ ����� � ����������� ��������� � ������ ����� �����
		std::vector<double> vecTemp;
		bool boolTemp;
		// ������ ��������������� ���� ���. �����
		boolTemp = data.getTSByIndex (vecTemp, i);
		if (boolTemp) {
			// ������� ���� � �������������
			vec.resize(vecTemp.size());
			for (unsigned int i = 0; i < vecTemp.size(); i++)
				vec[i] = (int) vecTemp[i];
			return true;
		} 
		return false;
	}
	return false;
}


/****************************************************************************
*					TestTS::setTSByIndex
*
*	Description:	������� ������������ ����� �������� ���� �� ���������� ���������
*	Parameters:		vec - ����� ���
*					i - ����� ���� � ������������ ������������������ ����������
*	Returns:		true - ���� ������� ����������� ����� ��������
*					false - ���� ����� �������� �� �����������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
bool TestTS::setTSByIndex(std::vector<double> &vec, int i) {
	// ���� ������ �������� -1 - �� ������������� ����� ���������� ��������
	if (i == -1) {
		if (correct.setTSByIndex (vec, i))
			return makeIntClone();
		return false;
	}
	// ���� ����� ������������� �������� - �� ������������ ��� �� ���������� �� ��� ������� �� ���������� �������������
	if (i >= 0) {
		return data.setTSByIndex (vec, i);
	}
	return false;
}


/****************************************************************************
*					TestTS::setTSByIndex
*
*	Description:	������� ������������ ����� �������� ���� �� ���������� ���������
*					������� �� ���������� ������� - � ������� ���������� ������� - � ������, 
*					���� ����� ������ ����� �����, �� ��������������� ������ ����� ������.
*	Parameters:		vec - ����� �������� ������� ����� 
*					i - ����� ���� � ������������ ������������������ ����������
*	Returns:		true - ���� ����� �������� ����������� �������
*					false - ���� ����� �������� �� �����������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
bool TestTS::setTSByIndex(std::vector<int> &vec, int i) {
	// ������� ������������� ��� ������� ���� � ���� ���� �������������� �����
	std::vector<double> vecTemp;
	vecTemp.resize(vec.size());
	for (unsigned int j = 0; j < vec.size(); j++)
		vecTemp[j] = (double) vec[j];
	// ���� ������ �������� -1 - �� ������������� ���������� ��������
	if (i == -1) {
		corrMarkUp.assign (vec.begin(), vec.end());
		return correct.setTSByIndex (vecTemp, i);
	}
	// ���� ����� ������������� �������� - �� ������������� ��� �� ���������� �� ��� ������� �� ���������� �������������
	if (i >= 0) {		
		return data.setTSByIndex (vecTemp, i);
	}
	return false;
}


/****************************************************************************
*					TestTS::makeIntClone
*
*	Description:	������� ������� ������� ���������� �������� � ������������� ������� corrMarkUp
*	Parameters:	-
*	Returns:		true - ���� ������� ����������� ���������� �������� � ������������� ������ corrMarkUp
*				false - ���� �� ������� ����� ��� ���������� ��������
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/ 
bool TestTS::makeIntClone (void) {
	// ���������� ���������� ��� ��������� ���������� �������� � ���� ���� ���. �����
	std::vector<double> vecTemp;
	bool boolTemp;
	// ������ ���� ���������� ��������
	boolTemp = correct.getTSByIndex (vecTemp, -1);
	if (boolTemp) {
		// ���������� �������������� ���� ���������� ��������
		corrMarkUp.resize(vecTemp.size());
		for (unsigned int i = 0; i < vecTemp.size(); i++)
			corrMarkUp[i] = (int) vecTemp[i];
		return true;
	}
	// ���� �� ������� ����� ���������� �������� - ���������� false
	return false;
}
