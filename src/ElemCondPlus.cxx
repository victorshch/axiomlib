//****************************************************************************
//*
//*			������� ������ ElemCondPlus
//*
//****************************************************************************/

#include "ElemCondPlus.h"
#include "ElemConditionsFactory.h"
#include "AxiomLibException.h"

#include "prettyprint.hpp"

#include <boost/lexical_cast.hpp>
		
using namespace AxiomLib;



/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	�����������, ��������� ���� ������ �� ���������
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
ElemCondPlus::ElemCondPlus(void) {
	elemCondition = NULL;
	sign = true;
	dimension = -1;
}

/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	����������� �����������
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
ElemCondPlus::ElemCondPlus(const ElemCondPlus &second)
{
   this->dimension = second.dimension;
   this->sign = second.sign;
   this->name = second.name;
   this->index = second.index;
   if (second.elemCondition != NULL) {
	   this->elemCondition = second.elemCondition->clone();
   } else {
	   this->elemCondition = NULL;
   }
}

ElemCondPlus ElemCondPlus::operator !() const
{
	ElemCondPlus result(*this);
	result.sign = !result.sign;
	return result;
}

/****************************************************************************
*					ElemCondPlus::ElemCondPlus
*
*	Description:	�����������, ������� ��������� ������������ ������� ������
*	Parameters:		-
*	Returns:		-
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
ElemCondPlus::~ElemCondPlus(void) {
	if (elemCondition != NULL)
		delete (elemCondition);
}


/****************************************************************************
*					ElemCondPlus::ecTypeName
*
*	Description:	������� ���������� ��� ������������� �������
*	Parameters:		-
*	Returns:		std::string - ��� ������� �������
*	Throws:			-
*	Author:			dk
*	History:		22.02.10 - wictor - ������������ �� name � ecTypeName
*
****************************************************************************/
std::string ElemCondPlus::ecTypeName (void) const {
	if (elemCondition != NULL) {
		if(this->sign) {
			return elemCondition->name();
		} else {
			return "not " + elemCondition->name();
		}
	}
	throw AxiomLibException("Error in ElemCondPlus::ecTypeName : elementary condition is not defined.");
}

void ElemCondPlus::getDimensions(std::vector<bool> &v) const
{
	std::fill(v.begin(), v.end(), false);

	if((int) v.size() > dimension) {
		v[dimension] = true;
	}
}


/****************************************************************************
*					ElemCondPlus::check
*
*	Description:	������� �������� ������������ �� � ������ ����� ���������, ���� �� ����
*	Parameters:		k - ����� ������� � ����, �� ������� ��������� �������
*					x - ���, �� ������� ��������� �������
*	Returns:		1 - ���� ������� �����������
*					0 - ���� �� �����������
*					-1 - ���� �� ���������
*	Throws:			AxiomLibException - ���� ������������ ������� �� ����������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::check (const unsigned long k, const std::vector<double>& x) const {
	signed int res;
	if (elemCondition == NULL)
		throw AxiomLibException("Error in ElemCondPlus::check : elementary condition is not defined.");
	res = elemCondition->check (k, x);
	if (res < 0) return res;
	if (sign) return res;
	if (res == 0) return 1;
	return 0;
}

/****************************************************************************
*					ElemCondPlus::check
*
*	Description:	������� �������� ������������ �� � ������ ����� ���������, ���� �� ����
*					������ ������� ����� ����� �� ���������, ��� AxiomExpr::check()
*					� �������� ��� ����, ����� ����� ���� ������
*					�������,  ��������� ���������� � �� � ���������
*	Parameters:		k - ����� ������� � ����, �� ������� ��������� �������
*					x - ���, �� ������� ��������� �������
*	Returns:		1 - ���� ������� �����������
*					0 - ���� �� �����������
*					-1 - ���� �� ���������
*	Throws:			AxiomLibException - ���� ������������ ������� �� ���������� 
*					��� ������� �������
*	Author:			wictor
*	History:
*
****************************************************************************/
signed int ElemCondPlus::check(unsigned long k, const std::vector<std::vector<double> > &x) const {
	if(dimension < 0 || dimension >= (int) x.size())
	{
		throw AxiomLibException("ElemCondPlus::check():  dimension out of range: dimension = "
								+ boost::lexical_cast<std::string>(dimension)
								+ ", x.size() = " + boost::lexical_cast<std::string>(x.size())
								);
	}
	const std::vector<double> &v = x[dimension];
	if(k >= v.size())
	{
		throw AxiomLibException("ElemCondPlus::check(): invalid k index : k = " + boost::lexical_cast<std::string>(k)
								+ ", v.size() = " + boost::lexical_cast<std::string>(v.size()) );
	}
	return check(k, v);
}

/****************************************************************************
*					ElemCondPlus::markUp
*
*	Description:	������� �������� ���� x ������ ��
*	Parameters:		x - ����������� ���
*					markup - ���������� �������� (������ �� 0 � 1 ���� �� �������)
*	Returns:		����� ��������, �� ������� ������� ����������� (�. �. ����� 1 � markup)
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
unsigned long ElemCondPlus::markUp(const std::vector<double>& x, std::vector<int> &markup) const {
	markup.resize(x.size());
	unsigned long result = 0;
	for(unsigned long i = 0; i < x.size(); i++) {
		int checkResult = check(i, x);
		if(checkResult == 1) {
			markup[i] = 1;
			result++;
		} else {
			markup[i] = 0;
		}
	}
	return result;
}

/****************************************************************************
*					ElemCondPlus::clear
*
*	Description:	������� �������� ���������� ������ ���������
*	Parameters:		-
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::clear (void) {
	if (elemCondition != NULL) {
		delete (elemCondition);
		elemCondition = NULL;
	}
	sign = true;
	dimension = -1;
	return 0;
}


/****************************************************************************
*					ElemCondPlus::operator<
*
*	Description:	��������� ������ ������� � �� ����������� ����������� � ������ �������� ���� �� ����.
*	Parameters:		second - ������ ������������ �������
*	Returns:		1 - ���� ������ ������� �������� ������� ������� �������
*					0 - ���� ������� �� �������� ��� ������������
*					-1 - ���� ������ �������� ������� ������� �������
*	Throws:			AxiomLibException - ���� ���� �� ������� �� ������
*	Author:			dk
*	History:
*
****************************************************************************/
double ElemCondPlus::operator< (const ElemCondPlus& second) const {
	double res;
	if ((this->elemCondition == NULL) || (second.elemCondition == NULL))
		throw AxiomLibException("Error in ElemCondPlus::operator< : elementary conditions are not defined.");
	if (this->ecTypeName() == second.ecTypeName()) {
		res = *(this->elemCondition) < *(second.elemCondition);
		if ((res == -1) && (this->sign == true) && (second.sign == true) || 
			(res == 1) && (this->sign == false) && (second.sign == false))
			return -1;
		if ((res == -1) && (this->sign == false) && (second.sign == false) || 
			(res == 1) && (this->sign == true) && (second.sign == true))
			return 1;
	}
	return 0;
}


/****************************************************************************
*					ElemCondPlus::operator=
*
*	Description:	�������� ������������ ��� ������������� �������
*	Parameters:		second - ������������� ������������ ������
*	Returns:		ElemCondPlus - ����������� ������������ �������
*	Throws:			-
*	Author:			dk
*	History:		wictor, 26.11.2009 : ���������� � �������������� ElemCondition.clone()
*
****************************************************************************/
ElemCondPlus& ElemCondPlus::operator= (const ElemCondPlus& second) {
	if(this != &second) {
		this->dimension = second.dimension;
		this->sign = second.sign;
		this->name = second.name;
		this->index = second.index;
		
//		// ������� ���������, �� �������� ������ ���������� ElemCondition
//		if(this->elemCondition && second.elemCondition && this->elemCondition->assign(second.elemCondition)) {
//			return *this;
//		}
		
		// ���� �� ����������, �� ��������� ����������� �������
		if(this->elemCondition != NULL) {
			delete this->elemCondition;
		}
		if (second.elemCondition != NULL) {
			this->elemCondition = second.elemCondition->clone();
		} else {
			this->elemCondition = NULL;
		}
	}
	return *this;
}


 /****************************************************************************
*					ElemCondPlus::initFromStructure
*
*	Description:	������� �������� ������������ ������� � ��� ��������� ��
*					������������������ ��������� � ��������� �������
*	Parameters:		ecps - ��������� � ��������� ���������� ������������� �������
*	Returns:		0
*	Throws:			AxiomLibException - ���� ��� ������������� ������� ������� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromStructure (ElemCondPlusStruture &ecps) {
	this->clear();
	ElemConditionsFactory ecf;
	// ��������� ����� ������������� �������
	this->sign = ecps.sign;
	this->dimension = ecps.dimension;
	this->elemCondition = ecf.createAxiomByName(ecps.ecName.c_str());
	// �������� ��� ������� � ��������� ������ ������� � ���� �������
	if (this->elemCondition == NULL) {
		throw AxiomLibException("Error in ElemCondPlus::initFromStructure : cannot creat elementary condition with given name.");
	}
	// ��������� ���������� ������������� �������
	this->elemCondition->setParams (ecps.ecParams);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::createStructure
*
*	Description:	������� ��������� ��������� ������� ������������� �������
*					� ����������� ��������� ElemCondPlusStruture
*	Parameters:		ecps - ����������� ���� ��������� ������������� �������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::createStructure (ElemCondPlusStruture &ecps) const {
	// ������� ����������� ��������� �������
	ecps.clear();
	// ��������� ����� ���������
	ecps.ecName = this->ecTypeName();
	ecps.dimension = this->dimension;
	ecps.sign = this->sign;
	this->elemCondition->getParams(ecps.ecParams);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::saveECToFile
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ����� ��������� ������������ ����
*					ecName - ���, ��� ������� ��������� ����
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::saveECToFile (const std::string baseDir, const std::string ecName) const {
	ElemCondPlusStruture ecps;
	createStructure (ecps);
	AxiomBase axiomBase;
	axiomBase.saveToEC (baseDir, ecName, ecps);
	return 0;
}


/****************************************************************************
*					ElemCondPlus::saveECToFile
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ����� ��������� ������������ ����
*					ecName - ���, ��� ������� ��������� ����
*					paramNames - ������ ������ �������� ���������� �������� ������
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::saveECToFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) const {
	if ((this->dimension < 0) || (this->dimension >= (int) paramNames.size()))
		throw AxiomLibException("Error in ElemCondPlus::saveECToFile : num of dimensions in elem condition is out of range: "
								+ boost::lexical_cast<std::string>(this->dimension));
	ElemCondPlusStruture ecps;
	createStructure (ecps);
	AxiomBase axiomBase;
	axiomBase.saveToEC (baseDir, ecName, ecps, paramNames[this->dimension]);
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	������� �������� �� �����
*	Parameters:		fullName - ������ ��� �����
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string &fullName) {
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (fullName, ecps);
	// ������������� ������� ������ �� ��������� ���������
	initFromStructure(ecps);
	
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ����� ��������� ������������ ����
*					ecName - ���, ��� ������� ��������� ����
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string baseDir, const std::string ecName) {
	// �������� ������� ���� � �����
	std::string filePath;
	filePath = baseDir;
	filePath.append("/");
	filePath.append(ecName);
	//filePath.append(str_ec_file_ending);
	// ���������� ��������� �� �����
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps);
	// ������������� ������� ������ �� ��������� ���������
	initFromStructure(ecps);
	this->name = ecName;
	return 0;
}


/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ����� ��������� ������������ ����
*					ecName - ���, ��� ������� ��������� ����
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string baseDir, const std::string ecName, const std::vector <std::string> &paramNames) {
	// �������� ������� ���� � �����
	std::string filePath;
	filePath = baseDir;
	filePath.append("/");
	filePath.append(ecName);
	//filePath.append(str_ec_file_ending);
	// ���������� ��������� �� �����
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps, paramNames);
	// ������������� ������� ������ �� ��������� ���������
	initFromStructure(ecps);
	this->name = ecName;
	return 0;
}

/****************************************************************************
*					ElemCondPlus::initFromFile
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		filePath - ������ ���� � ����� .ec (��� .ec)
*					ecName - ���, ��� ������� ��������� ����
*	Returns:		0
*	Throws:			-
*	Author:			wictor
*	History:
*
****************************************************************************/
signed int ElemCondPlus::initFromFile (const std::string &filePath, const std::vector <std::string> &paramNames) {
	// ���������� ��������� �� �����
	ElemCondPlusStruture ecps;
	AxiomBase axiomBase;
	axiomBase.readFromEC (filePath, ecps, paramNames);
	// ������������� ������� ������ �� ��������� ���������
	initFromStructure(ecps);
	return 0;
}

std::ostream& operator<<(std::ostream& ostr, const ElemCondPlus& ecPlus) {
	ostr << ecPlus.dimension << " : ";
	ostr << (ecPlus.sign ? "+" : "-");
	ostr << ecPlus.elemCondition->name();
	std::map<std::string, std::string> ecParams;
	ecPlus.elemCondition->getParams(ecParams);
	
	ostr << "{";
	for (std::map<string, string>::const_iterator i = ecParams.begin(); i != ecParams.end(); i++) {
		ostr << (*i).first << "-->" << (*i).second << std::endl;
	}	

	ostr << "}";

	return ostr;
}
