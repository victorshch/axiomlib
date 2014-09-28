/***************************************************************************
*					������� ������ AxiomExpr.h
*
*	Description:	������� � ���� ��������� �� ������������ ������� - ����������
*	Author:			dk
*	History:	
*
****************************************************************************/
#include <algorithm>

#include "AxiomExpr.h"

#include "prettyprint.hpp"

using namespace AxiomLib;

// �����������, ��� ������-�� ������������� ������� ��� ��������� �������� ������� ����� ���������� ���� ���������
#define double_false_sign_prabability	0.2

// ������ ����������� - � �������� ����� ������� �� ���������
AxiomExpr::AxiomExpr () {
	setDefaultName();
	expression.clear();
	nameOfECBank.assign ("no_name_Elem_Cond_Bank");
}


// ������ ����������� � �������� ����� ������� �� ������� ���������
AxiomExpr::AxiomExpr (const std::string sName) {
	this->nameOfAxiomExpr.assign (sName);
	expression.clear();
	nameOfECBank.assign ("no_name_Elem_Cond_Bank");
}

AxiomExpr::AxiomExpr(const ElemCondPlus &ec)
{
	expression.resize(1);
	expression[0].resize(1);
	expression[0][0] = ec;
}


// ���������� ������ - ������� ��� ������������ ��������� �������
AxiomExpr::~AxiomExpr(void) {
	for (unsigned int i = 0; i < expression.size(); i++) {
		for (unsigned int j = 0; j < expression[i].size(); j++)
			expression[i][j].clear();
	}
}


// ������������� ��������� ��� ��� ���������� ������
signed int AxiomExpr::setDefaultName (void) {
	this->nameOfAxiomExpr.assign ("no_name");
	return 0;
}


// �������� ������� (���������� ���������� �� ����������, ������������ ��� ������� ��������� ������)
std::string AxiomExpr::name (void) const {
	return this->nameOfAxiomExpr;
}


// ������� ���������� �������� ����� ����������� ������� � ������ �������
signed int AxiomExpr::getNumOfEC (void) const {
	return size();
}


// ������� ���������� �������� ����� ����������� ������� � ������ �������
signed int AxiomExpr::size(void) const {
	int count = 0;
	for (unsigned int i = 0; i < expression.size(); i++) {
		count += expression[i].size();
	}
	return count;
}


/****************************************************************************
*					AxiomExpr::clear
*
*	Description:	������� ������� ���������� ������� - ������� ��� ��������
*					������������ ������� � �������� ��������� �������.
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::clear (void) {
	for (unsigned int i = 0; i < expression.size(); i++) {
		for (unsigned int j = 0; j < expression[i].size(); j++)
			expression[i][j].clear();
		expression[i].clear();
	}
	expression.clear();
	return 0;
}


/****************************************************************************
*					AxiomExpr::check 
*
*	Description:	���������, ����������� �� ������ ������� �������� ����� k ���� ts
*	Parameters:		k - ����� � ���� ��� ������� ����������� ������������ �������
*					ts - ����������� ���
*	Returns:		1  - ���� �����������
*					0  ���� �� �����������
*					-1  ���� ������ ������� ������� �� ���������
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::check (const unsigned long k, const std::vector<double>& ts) const {
	int res = 1;
	for (unsigned int i = 0; i < expression.size(); i++) {
		res = 1;
		for (unsigned int j = 0; j < expression[i].size(); j++){
			res *= expression[i][j].check(k,ts);
			if (res <= 0) {
				break;
			}
		}
		if (res > 0) {
			break;
		}
	}  
	return res;
}


/****************************************************************************
*					AxiomExpr::check 
*
*	Description:	���������, ����������� �� ������ ������� �������� ����� k ���� ts
*	Parameters:		k - ����� � ���� ��� ������� ����������� ������������ �������
*					ts - ����������� ����������� ���
*	Returns:		1  - ���� �����������
*					0  ���� �� �����������
*					-1  ���� ������ ������� ������� �� ���������
*	Throws:			AxiomLibException - ���� ������� ��������� �� ������������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::check (const unsigned long k, const std::vector < std::vector<double> >& ts) const {
	int res = 1;
	for (unsigned int i = 0; i < expression.size(); i++) {
		res = 1;
		for (unsigned int j = 0; j < expression[i].size(); j++){
			if  ((expression[i][j].dimension < 0) || (expression[i][j].dimension >= (int) ts.size()))
				throw AxiomLibException("Error in AxiomExpr::check : wrong input parameters.");
			res *= expression[i][j].check(k, ts[expression[i][j].dimension]);
			if (res <= 0) {
				break;
			}
		}
		if (res > 0) {
			return res;
		}
	}  
	return res;
}


/****************************************************************************
*					AxiomExpr::operator<
*
*	Description:	��������� ������ ������� � �� ����������� ����������� � ������ ��������.
*					�� �������, ��������� - ������
*	Parameters:		second - ������ ������������ �������
*	Returns:		�������������� ����� �� -1 �� 1:
*					��� ����� � 1 - ��� � ������� ������������ ����� �������,
*					��� ������ ������� ������ �������� �� ������ (�.�. ������ ������ ������)
*					���� -1 - �� ������ ������ ������
*					���� 0 - �� ������� ���� ���������, ���� �� �������� ����� �����.
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
double AxiomExpr::operator< (const AxiomExpr& second) {
	double endres, res, curres, tmpres;
	// �������� ������� �������
	if (this->size() == 0)  {
		return -1;
	}
	if (second.size() == 0) {
		return 1;
	}

	endres = 0;
	for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
		curres = -2;
		for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
			//if (second.expression[j1].size() != this->expression[i1].size())
			//	continue;
			res = 0;
			curres = -2;
			for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
				curres = -2;
				for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
					if (this->expression[i1][i2].elemCondition->name() != second.expression[j1][j2].elemCondition->name())
						continue;
					tmpres = (*(this->expression[i1][i2].elemCondition)) < *(second.expression[j1][j2].elemCondition);
					if (tmpres > curres) {
						curres = tmpres;
					}
				}
				if (curres == -2)
					break;
				res += curres;
			}
			if (curres == -2)
				continue;
			endres += res;
			break;
		}
		if (curres == -2)
			return 0;
	}

	endres = endres / (double) this->size();
	return endres;
}


/****************************************************************************
*				AxiomExpr::operator=
*
*	Description:	�������� ������������
*	Parameters:	������������� �������
*	Returns:		*this
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
//AxiomExpr & AxiomExpr::operator= (const AxiomExpr& second) {
//	// �������� ����������� ������ �������
//	this->clear();
//	// ����������� �������� ������� � ������������� ����� ������������ �������
//	nameOfAxiomExpr = second.name();
//	this->nameOfECBank = second.nameOfECBank;
//	// ���������� ���������� ������������ �������
//	this->expression.resize (second.expression.size());
//	for (unsigned int i = 0; i < second.expression.size(); i++) {
//		this->expression[i].resize (second.expression[i].size());
//		for (unsigned int j = 0; j < second.expression[i].size(); j++)
//			this->expression[i][j] = second.expression[i][j];
//	}
//	return *this;
//}


/****************************************************************************
*				AxiomExpr::checkWeakEquivalence
*
*	Description:	�������� (������) ����������� ������������ ���� ������:
*					��������� ������������ ������� ���������.
*	Parameters:		const AxiomExpr& second - �������, � ������� ���� ��������� ���������
*	Returns:		0 ���� ����������� ��������������� ���
*					1 ���� ����������� ��������������� ����
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::checkWeakEquivalence(const AxiomExpr& second) const {
	bool flag; 
	// �������� ��������� �������
	if ((this->expression.size() == 0) && (second.expression.size() == 0))
		return 1;
	if ((this->expression.size() == 0) || (second.expression.size() == 0))
		return 0;
	// ��������� ��������� � ����� ������
	// ��� ������ ���������� �� � ������ ������� ���� ������������ �� ������ �������
	for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
		flag = false;
		for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
			flag = false;
			for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
				flag = false;
				for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
					if (this->expression[i1][i2].elemCondition->name() == second.expression[j1][j2].elemCondition->name()) {
						flag = true;
						break;
					}
				}
				if (!flag)
					break;
			}
			if (flag)
				break;
		}
		if (!flag)
			return 0;
	}
	// ����� ������������ � �������� �������:
	// ��� ������ ���������� �� �� ������ ������� ���� ������������ � ������ �������
	for (unsigned int j1 = 0; j1 < second.expression.size(); j1++) {
		flag = false;
		for (unsigned int i1 = 0; i1 < this->expression.size(); i1++) {
			flag = false;
			for (unsigned int j2 = 0; j2 < second.expression[j1].size(); j2++) {
				flag = false;
				for (unsigned int i2 = 0; i2 < this->expression[i1].size(); i2++) {
					if (this->expression[i1][i2].elemCondition->name() == second.expression[j1][j2].elemCondition->name()) {
						flag = true;
						break;
					}
				}
				if (!flag)
					break;
			}
			if (flag)
				break;
		}
		if (!flag)
			return 0;
	}
	return 1;
}


/****************************************************************************
*					AxiomExpr::operator&
*
*	Description:	������ ������ �� ��������� �
*	Parameters:		������ �������, ������������ � �������� ����������� ������
*	Returns:		AxiomExpr - ����� �������, ��� ����������� *this � second
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
AxiomExpr AxiomExpr::operator& (const AxiomExpr& second) const {
	AxiomExpr result;
	result.andExpr (*this, second);
	return result;
}


/****************************************************************************
*					AxiomExpr::operator|
*
*	Description:	������ ������ �� ��������� ���
*	Parameters:		������ �������, ������������ � �������� ����������� ������
*	Returns:		AxiomExpr - ����� �������, ��� ����������� *this � second
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
AxiomExpr AxiomExpr::operator| (const AxiomExpr& second) const {
	AxiomExpr result;
	result.orExpr (*this, second);
	return result;
}

AxiomExpr &AxiomExpr::operator &=(const AxiomExpr &second)
{
	AxiomExpr first(*this);
	andExpr(first, second);
	return *this;
}

AxiomExpr &AxiomExpr::operator |=(const AxiomExpr &second)
{
	AxiomExpr first(*this);
	orExpr(first, second);
	return *this;
}


/****************************************************************************
*					AxiomExpr::andExpr
*
*	Description:	������ ������ �� ��������� � � ������ �������
*	Parameters:		first - ������ �������, ������������ � �������� ����������� ������
*					second - ������ �������, ������������ � �������� ����������� ������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::andExpr (const AxiomExpr& first, const AxiomExpr& second) {
	int numDisjunts, numDisjunts1, numDisjunts2;
	this->clear();
	numDisjunts1 = first.expression.size();
	numDisjunts2 = second.expression.size();
	numDisjunts = numDisjunts1*numDisjunts2;
	if (numDisjunts < 1) {
		return orExpr(first, second);
	}
	this->expression.resize(numDisjunts);
	unsigned int curLen;
	unsigned int k = 0;
	for (int i = 0; i < numDisjunts1; i++) {
		curLen = first.expression[i].size();
		for (int j = 0; j < numDisjunts2; j++, k++) {
			this->expression[k].resize (curLen + second.expression[j].size());
			for (unsigned int l = 0; l < curLen; l++)
				this->expression[k][l] = first.expression[i][l];
			for (unsigned int l = 0; l < second.expression[j].size(); l++)
				this->expression[k][curLen + l] = second.expression[j][l];
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::orExpr
*
*	Description:	������ ������ �� ��������� ��� � ������ �������
*	Parameters:		first - ������ �������, ������������ � �������� ����������� ������
*					second - ������ �������, ������������ � �������� ����������� ������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::orExpr (const AxiomExpr& first, const AxiomExpr& second) {
	int numDisjunts, numDisjunts1, numDisjunts2;
	this->clear();
	numDisjunts1 = first.expression.size();
	numDisjunts2 = second.expression.size();
	numDisjunts = numDisjunts1 + numDisjunts2;
	if (numDisjunts < 1)
		return 0;
	this->expression.resize(numDisjunts);
	for (int i = 0; i < numDisjunts1; i++) {
		this->expression[i].resize (first.expression[i].size());
		for (unsigned int l = 0; l < first.expression[i].size(); l++)
			this->expression[i][l] = first.expression[i][l];
	}
	for (int j = 0; j < numDisjunts2; j++) {
		this->expression[numDisjunts1 + j].resize (second.expression[j].size());
		for (unsigned int l = 0; l < second.expression[j].size(); l++)
			this->expression[numDisjunts1 + j][l] = second.expression[j][l];
	}
	return 0;
}

/****************************************************************************
*					AxiomExpr::getDimensions
*
*	Description:	���������� ������������ ��������, ����������� ��� ������
*					�������
*	Parameters:		v - ���� ������������ true �� ����� ������������, �������
*					������������. ���� ������ ������� ������������, �� �� ��
*					������������� (�. �. ����� �������� ���������� ������ �������)
*	Returns:		-
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
void AxiomExpr::getDimensions(std::vector<bool> &v) const {
	std::fill(v.begin(), v.end(), false);
	
	for(unsigned int i = 0; i < expression.size(); i++) {
		for(unsigned int j = 0; j < expression[i].size(); j++) {
			const ElemCondPlus& ecPlus = expression[i][j];			
			
			if((int) v.size() > ecPlus.dimension) {
				v[ecPlus.dimension] = true;
			}
		}
	}
}

/****************************************************************************
*					AxiomExpr::initAxiomFromStructure
*
*	Description:	������� �������� ������� �� ������������������ ��������� � ��������� �������
*	Parameters:		aes - ��������� � ��������� �������
*	Returns:		0
*	Throws:			AxiomLibException - ���� ��� ������ �� ������������ ������� ������� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromStructure (AxiomExprStructure &aes) {
	this->clear();
	ElemConditionsFactory ecf;
	if (aes.axiomName.size() > 0)
		this->nameOfAxiomExpr = aes.axiomName;
	if (aes.elemCondBankName.size() > 0)
		this->nameOfECBank = aes.elemCondBankName;
	this->expression.resize (aes.elemConds.size());
	for (unsigned int i = 0; i < aes.elemConds.size(); i++) {
		this->expression[i].resize (aes.elemConds[i].size());
		for (unsigned int j = 0; j < aes.elemConds[i].size(); j++) {
			this->expression[i][j].initFromStructure (aes.elemConds[i][j]);
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomByRand
*
*	Description:	������� ������ ������ ������� ��� ��������� ��� ������������� ���������, ���������� ��������� �������.
*					���������� ������������ ��������� � ����� ������� ������ - ������ �������� ������������ ������� -
*					� ��������� �������� ���������� ������ ����������� ������������ �������,
*					����� ������������ ������� ��� ���� �������� ��� ��������
*	Parameters:		numOfEC - ����� ������������ �������, ������� ����� ������� � ������ �������
*	Returns:		-1 - ����  ������� �� �������
*					 0 - ���� ��� ������ ���������
*	Throws:			AxiomLibExeption - ���� ���������� ��������� numOfEC ����������� ������������ �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomByRand (const int numOfEC) {
  	return initAxiomByRand(numOfEC, -1, -1);
}


/****************************************************************************
*					AxiomExpr::initAxiomByRand
*
*	Description:	������� ������ ������ ������� ��� ��������� ��� ������������� ���������, ���������� ��������� �������.
*					��� ������� ������� �������� ������������ ����������� ������ ������, �� ������� ��� ���������, �� ��������� ��������.
*					���������� ������������ ��������� � ����� ������� ������ - ������ �������� ������������ ������� -
*					� ��������� �������� ���������� ������ ����������� ������������ �������,
*					����� ������������ ������� ��� ���� �������� ��� ��������
*	Parameters:		numOfEC - ����� ������������ �������, ������� ����� ������� � ������ �������
*					dimensionMin - ����������� �������� ��� ����������� �������� ������
*					dimensionMax - ������������ �������� ��� ����������� �������� ������
*	Returns:		-1 - ����  ������� �� �������
*					 0 - ���� ��� ������ ���������
*	Throws:			AxiomLibExeption - ���� ���������� ��������� numOfEC ����������� ������������ �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomByRand (const int numOfEC, const int dimensionMin, const int dimensionMax) {
	// �������� ������� ���������� - ������������� �� �������� ��� dimension
	bool defineDimension = true;
	if ((dimensionMax < dimensionMin) || (dimensionMin < 0))
		defineDimension = false;
	// ������� ���������� ������ �������
	this->clear();
	ElemConditionsFactory ecf;
	std::vector <std::vector <int> > numECs;
	// ����������� - ����� ������������ ������� � � ����� ������� �������� � ������ �������
	ecf.createAxiomByRand(numECs, numOfEC);
	this->expression.resize (numECs.size());
	for (unsigned int i = 0; i < numECs.size(); i++) {
		this->expression[i].resize (numECs[i].size());
		for (unsigned int j = 0; j < numECs[i].size(); j++) {
			// ������� ��������� ��� ������������� ���������
			this->expression[i][j].elemCondition = ecf.createAxiomByNum (numECs[i][j] + 1);
			// ��������� ����������� ����� � �������� �� ��������� �������� true
			this->expression[i][j].sign = true;
			if ( ( (double) rand() / (double) RAND_MAX ) < double_false_sign_prabability )
				this->expression[i][j].sign = false;
			// ������������� ��������� �������� ����������� �������� ������, �� ������� ��������� ������������ �������
			if (defineDimension)
				this->expression[i][j].dimension = dimensionMin + (int) (((double) rand() / ((double) RAND_MAX + 1.0) ) * (double) (dimensionMax - dimensionMin + 1));
		}
	}
  	return 0;
}


/****************************************************************************
*					AxiomExpr::createAxiomExprStructure
*
*	Description:	������� ��������� ��������� ��������� ������� � AxiomExprStructures
*	Parameters:		aes - ����������� ���� ��������� �������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::createAxiomExprStructure (AxiomExprStructure &aes) const {
	// ������� ����������� ��������� �������
	aes.clear();
	// ��������� ����� ���������
	aes.axiomName = name();
	aes.elemCondBankName = nameOfECBank;
	aes.elemConds.resize (expression.size());
	for (unsigned int i = 0; i < expression.size(); i++) {
		aes.elemConds[i].resize(expression[i].size());
		for (unsigned int j = 0; j < expression[i].size(); j++) {
			expression[i][j].createStructure (aes.elemConds[i][j]);
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomFromFile
*
*	Description:	������� ������� ������� �� �����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������
*					AxiomSetName - ��� ����� � ��������� �������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromFile (const std::string baseDir, const std::string axiomName) {
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	std::string fileName;
	// �������� ������� ���� � �����
	fileName = baseDir;
	fileName.append ("/");
	fileName.append (axiomName);
	// ���������� �������� � ���������
	axiomBase.readFromAX(fileName, aes);
	// ������������� �� ���������
	initAxiomFromStructure(aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::initAxiomFromFile
*
*	Description:	������� ������� ������� �� �����
*					(� ��������� ����������� ������ ������, �� ������� ����������� ������ ������������ ������� �� ������ �������)
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������
*					AxiomSetName - ��� ����� � ��������� �������
*					paramNames - ������ ���������� ������ ������ - ��� �������� �����������,
*						�� ������� ��������� ������ ������������ ������� �� �������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::initAxiomFromFile (const std::string baseDir, const std::string axiomName, const std::vector <std::string> &paramNames) {
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	std::string fileName;
	// �������� ������� ���� � �����
	fileName = baseDir;
	fileName.append ("/");
	fileName.append (axiomName);
	// ���������� �������� � ���������
	axiomBase.readFromAX(fileName, aes, paramNames);
	// ������������� �� ���������
	initAxiomFromStructure(aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::saveAxiomToFile
*
*	Description:	������� ���������� ������ � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������
*					AxiomSetName - ���, ��� ������� ��������� ������ ������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::saveAxiomToFile (const std::string baseDir, const std::string axiomName) const {
	AxiomExprStructure aes;
	createAxiomExprStructure (aes);
	aes.axiomName = axiomName;
	AxiomBase axiomBase;
	axiomBase.saveToAX (baseDir, aes);
	return 0;
}


/****************************************************************************
*					AxiomExpr::saveAxiomToFile
*
*	Description:	������� ���������� ������ � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������
*					AxiomSetName - ���, ��� ������� ��������� ������ ������
*					paramNames - ������ ���������� ������ ������ - ��� �������� �����������,
*						�� ������� ��������� ������ ������������ ������� �� �������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomExpr::saveAxiomToFile (const std::string baseDir, const std::string axiomName, std::vector <std::string> &paramNames) const {
	AxiomExprStructure aes;
	createAxiomExprStructure (aes);
	aes.axiomName = axiomName;
	AxiomBase axiomBase;
	axiomBase.saveToAX (baseDir, aes, paramNames);
	return 0;
}

ostream &operator<<(ostream &ostr, const AxiomExpr &axiomExpr)
{
	for (auto iter = axiomExpr.expression.begin(); iter != axiomExpr.expression.end(); iter++){
		for (auto item_iter = (*iter).begin(); item_iter != (*iter).end(); item_iter++){
			ostr << (*item_iter) << std::endl;
		}
		ostr << std::endl;
	}
	return ostr;
}
