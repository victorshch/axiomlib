#include <set>

#include "Common.h"

#include "DataSetBase.h"

using namespace std;

using namespace AxiomLib;

DataSetBase::DataSetBase()
{
	// ������� ���������� ���������� ������ �� ���������
	std::string nm ("name");
	this->name = nm;
	std::string nl ("null");
	this->null = nl;
}

/****************************************************************************
*				DataSetBase::ReadParams
*
*	Description:	������� ������ ���������� ������ DataSet - ������ �������� 
*				������� ��������� ��������, � ����� �������� ����������, 
*				�� ������� �������������� ��������� ����.
*				������� ���������� �������� ������� �������� �������� � 
*				�������� ���������� - ����� - �� ����������� ������������ � 
*				� ���� ���������� ��� ����������� ��������� ����.
*	Parameters:	path - ������, ���������� ��� ��������� ���������� ������,
*				� ������� �������� ����� � ���������� ���������� � �������
*				��������������
*	Returns:		0
*	Throws:		AxiomLibExcption  - 	���� ���� �� ���� �� ������ �� ����������
*								(����� ��������� ����� ������� �������������
*								� ����� ����������)
*							     - 	���� ���� �� ���� �� ������ ������
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSetBase::readParams(std::string path) {
	// ������� ��������� �������� ���� � ������
	boost::filesystem::path classNamesFile(path);
	boost::filesystem::path paramNamesFile(path);
	
	// ����� �� ��������� ��������� ����� ������� � ����������
	classNamesFile /= "./class_names";
	paramNamesFile /= "./param_names";

	// �������� ������������� ������
	if ( !boost::filesystem::exists( classNamesFile ) ) {
		throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot find classNamesFile.");		
	}
	if ( !boost::filesystem::exists( paramNamesFile ) ) {
		throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot find paramNamesFile.");		
	}
	
	// ��������� ����� ��� ���������
	boost::filesystem::ifstream fileP( paramNamesFile );
	boost::filesystem::ifstream fileC( classNamesFile );
	
	std::string str; // ������ �� ������ ���������� ���������� � �������� ����� � ������ ����������
	//int i; // � ������ ���������� �������� ��� ������ ������ �� �����

	// ������� ��������� ���� � ������ � ��������� ������������
	// wictor: ������� \r
	boost::char_separator<char> sep(";:,.	\r");

	// ��������� �����, ����������� ����� ����������, ������� ������������ ��������� ����
	this->paramNames.resize(0);
	while (!fileP.eof())  {
		std::getline(fileP, str);
		if (!str.empty()) {
			token_separator tokens(str, sep);
			for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				this->paramNames.push_back(*tok_iter);
			}
		}
	 }
	 // �������� - ������� �� ��������� �������� ����������
	 if (this->paramNames.size() == 0) {
	 	throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot read paramNamesFile.");
	 }
 	
	 // ��������� �����, ����������� ����� ������� ��������������
	 this->classNames.resize(0);
	 while (!fileC.eof())  {
		getline(fileC, str);
		if (!str.empty()) {
			token_separator tokens(str, sep);
			for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				this->classNames.push_back(*tok_iter);
			}
		}	
	 }
	 // ������� - ������� �� ��������� �������� �������
	 if (this->classNames.size() == 0) {
	 	throw AxiomLibException ("Error in DataSetBase::ReadParams: cannot read classNamesFile.");
	 }
	 
	 // ��������� ��������� - ��������� ����� � �������
	fileP.close();
	fileC.close();

	return 0;	
}

/****************************************************************************
*				DataSetBase::ReadDataSet
*
*	Description:	������� ������ ���� ������ �� ������ ������, ������������
*				� ��������� ��������
*	Parameters:	const char* path - ���� � ��������, ����������� ����� ������
*	Returns:		0
*	Throws:		AxiomLibExeption - ���� ������� �����������, ��� ���� ������
*				� ������� ������ ������
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSetBase::readDataSet (const std::string &baseDataSetDir)
{
  // ����� ����� ������ �������:
  // 0. ��������� ������������ ����
  // 1. ������ ���� � ���������� ������� � ��������� ������ �������� �������;
  //    ���� ������� ������� ����������� ������������
  // 2. ������ ���� � ���������� ���������� � ��������� ������ �������� ����������
  //    ���� ������� ���������� ����������� ������������

  //0 - 1 - 2 ������ ����������� � ��������� �������: 

	//std::cout << "\n" << "Base file is " << baseDataSetDir << "\n";
	
	this->readParams(baseDataSetDir);
	
	//std::cout << "Configuring params..." << std::endl;
	
	paramNums.clear();
	paramNums.reserve(paramNames.size());
	for (unsigned int i = 0; i < paramNames.size(); i++) {
		if (paramNames[i] != "time" ) {
			paramNums.push_back(i);
		}
	}	
	
  	return 0;
}

/****************************************************************************
*				DataSetBase::placeOfParam
*
*	Description:	�������� ���������� � ��������� ��������� � ������������ 
*				������������� �� ��� �����
*	Parameters:	parameter - ��� ��������� ���������� � ����� �������� � 
*						������������ ������� �������������
*	Returns:		- 1 - ��� ������ - ���� ������ ��������� ���
*				��������������� ����� - ����� ��������� � ������������ �������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
int DataSetBase::placeOfParam(std::string parameter) {
	return isIn (parameter, paramNames);
}

/****************************************************************************
*				DataSetBase::paramName
*
*	Description:	�������� ���������� �� ����� ��������� �� ��� ������ � 
*					������������ �������������
*	Parameters:		parameter - ����� ��������� � ������������ �������
*	Returns:		"" - ������ ������, ���� �������� � ��������� �������� �� �������
*					std::string - ��� ���������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
std::string DataSetBase::paramName(int parameter) const {
	if ( (parameter >= 0) && (parameter < (int) paramNames.size()) ) 
		return paramNames[parameter];
	return (std::string) "";
}

/****************************************************************************
*				DataSetBase::className
*
*	Description:	�������� ���������� �� ����� ����������� ��������� �� ��� 
*					������ � ������������ �������������
*	Parameters:		parameter - ����� �������� ����������� ��������� � ������������ �������
*	Returns:		"" - ������ ������, ���� �������� � ��������� �������� �� �������
*					std::string - ��� ���������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
std::string DataSetBase::className(int parameter) const {
	if ( (parameter >= 0) && (parameter < (int) classNames.size()) ) {
		return classNames[parameter];
	} else if (parameter == -1) {
		return "normal";
	}
	return (std::string) "";
}

std::vector<std::string> DataSetBase::getClassNames() const {
	return classNames;
}

/****************************************************************************
*				DataSetBase::getParamNames
*
*	Description:	�������� ���������� ��� ���� ��������� ���������� ������ 
*					������ � �� ������������ �������������
*	Parameters:		-
*	Returns:		std::vector <std::string> - ��� ����� ����������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
const std::vector <std::string>& DataSetBase::getParamNames (void) const {
//	std::vector <std::string> toReturn;
//	toReturn = paramNames;
//	return toReturn;
	return paramNames;
}

/****************************************************************************
*				DataSetBase::getParamNums
*
*	Description:	������� �� ������� ������������ ������� ������ ������� ���������� 
*				� ������������ ������������� �� ������� ������� ������ ������� ������
*	Parameters:	toReturn - ����������� ������
*				env - ����� � ������� �� ��������� �������
*				envDataSet - ����� � ������� �� ������� ������ ������
*	Returns:		std::vector<int> - ����������� ������ ������� ���������� ��� �������
*	Throws:		AxiomLibException - ���� ��������� � ��������� � ������� ������� �� ������� � ������ ������ ������
*	Author:		dk,gevor
*	History:		-
*
****************************************************************************/
int DataSetBase::getParamNums (std::vector<int>& toReturn, const Environment &env, EnvDataSet &envDataSet) {

	setupParamNums(env, envDataSet);
	
	toReturn = paramNums;
	
	return 0;
}

std::vector<int> DataSetBase::getParamNums() const {
	std::vector<int> result;
	getParamNums(result);
	return result;
}

int DataSetBase::getParamNums(std::vector<int> &toReturn) const {
	toReturn = paramNums;
	return 0;
}

void DataSetBase::setupParamNums(const Environment &env, const EnvDataSet &envDataSet) {
	// ��������� �������� ��� ��������� �� ������ �������
	set<string> paramSet;
	int ret = env.getStringSetParamValue(paramSet, "analizeParameter");

	// ���������, ������� �� ��� � ����� �������
	if (ret >= 0) {
		// �������
		// ������� ��� ���������
		paramNums.resize(paramSet.size());
		int j = 0;
		for (set<string>::const_iterator i = paramSet.begin(); i != paramSet.end(); i++,j++) {
			string paramName = *i;
			int paramNum = placeOfParam(paramName);
			if (paramNum < 0) {
				throw AxiomLibException ("Error in DataSetBase::paramNums: given parameter's name do not exists in dataSet.");
			}
			paramNums[j] = paramNum;
		}
	}
	else { // ������ �� ������� �� ������ ��������� � ����� �������
		int num = envDataSet.getAnalizeNameSize();
		std::string parameter;
		if (num > 0) { // ������ �����-�� �������� ������� � ���������� ������ ������
			paramNums.resize(num);
			for (int i = 0; i < num; i++) {
				parameter = envDataSet.getAnalizeName(i);
				//std::cout << "\n2) parameter name is " << parameter << "\n";
				paramNums[i] = placeOfParam(parameter); // �������� ���������� � ��������� ��������� � ������������ �������������
				if (paramNums[i] < 0) {
					throw AxiomLibException ("Error in DataSetBase::paramNums: given parameter's name do not exists in dataSet*.");
				}
			}
		}
		else {// ����� �� ����� �� ���� �������� - ������ ��������� �������� �� ���� �����
			paramNums.clear();
			paramNums.reserve(paramNames.size());
			for (unsigned int i = 0; i < paramNames.size(); i++) {
				if (paramNames[i] != "time" ) {
					//std::cout << "\n3) parameter num is " << paramNames[i] << "\n";
					paramNums.push_back(i);
				}
			}
		}
	}
	// ���������, ����� ������� ���������� ���� � ������������ �������
	std::sort(paramNums.begin(), paramNums.end());
}

/****************************************************************************
*				DataSetBase::paramNamesSize
*
*	Description:	��������� ���������� � ����� ��������� � ������� �������� ����������
*	Parameters:	-
*	Returns:		������������� ����� - ����� ��������� � ������� �������� ����������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
 int DataSetBase::paramNamesSize (void) const {
 	return (int) (this->paramNames).size();
 }

 /****************************************************************************
 *				DataSetBase::setNullStr
 *
 *	Description:	������� ������������� ����� ����������� ��� ��������� ����� ���� ���������� �� ��������� ����
 *	Parameters:	newNullStr - ����� ��� ��� ����������� null
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNullStr (const std::string newNullStr) {
	 //std::cout << "\n newNullStr is " << newNullStr << "\n";
	 this->null = newNullStr;
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::setNullStr
 *
 *	Description:	������� ������������� ����� ����������� ��� ��������� ����� ���� ���������� �� ��������� ����
 *	Parameters:	env - ����� � ������� �� ����������������� �����
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNullStr (EnvDataSet &envDataSet) {
	 int ret;
	 std::string nullStr;
	 ret = envDataSet.getNullStr(nullStr);
	 //std::cout << "\n nullStr is " << nullStr << "\n";
	 if (ret == 0) {
		 null = nullStr;
	 }
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::setNameStr
 *
 *	Description:	������� ������������� ����� ����������� ��� ����� ������� � ��������� ����������
 *	Parameters:	newNameStr - ����� ��� ��� ����������� name
 *	Returns:		0
 *	Throws:		-
 *	Author:		dk
 *	History:		-
 *
 ****************************************************************************/
 int DataSetBase::setNameStr (const std::string newNameStr) {
	 this->name = newNameStr;
	 return 0;
 }
 
 /****************************************************************************
 *					DataSetBase::checkName
 *
 *	Description:	������� ��������� �����.
 *				����� ���� �������� ����������� � ������ ������� � ������ 
 *				������ �������� ��������� � �� ������ ������� ��� �����.
 *				������������ ��� ��������� � �������� ���� ������.
 *	Parameters:	name1 - ������ ��� ���������
 *				from -����� ������� � ������ name1 � �������� �������� ���������
 *				upTo - ����� ������� � ������ name1 �� �������� ����� ���������
 *				name2 - ������ ��� ���������
 *	Returns:		true  - ���� ������ ���� �������� ��� ������ ��� ��� ������
 *				false - � ��������� ������
 *	Throws:		-
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 bool DataSetBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) {
	 const std::basic_string <char>::size_type maxAddrLen = 1000;
	 std::basic_string <char>::size_type indexCh1, indexCh2;
	 static const std::basic_string <char>::size_type npos = maxAddrLen;
	 const char * cName2 = name2.c_str();
	 if ((from >= upTo) || (from > (signed int) name1.size())) {
		 return false;
	 }
	 if (upTo > (int) name1.size()) {
		 upTo = (int) name1.size() - 1;
	 }
	 indexCh1 = name1.rfind ( name2 , upTo );
	 if (from < 0) {
		 from = 0;
		 indexCh2 = npos;
	 }
	 else {
		 indexCh2 = name1.rfind ( name2 , from );
	 }
	 if ((indexCh1 < npos) && (indexCh1 >= 0) && ((indexCh2 >= npos) || (indexCh2 < 0))) {
		 // ��� ����� - ���� �� upTo � ��� �� from
		 return true;
	 }
	 return false;
 }

 /****************************************************************************
 *				DataSetBase::ReadFromCSV
 *
 *	Description:	������� ������ ������ �� csv-�����. �������� ��� ������ ����� � 
 *				������������ �� ��������� ������ � ���������� ������������� ��������� MultiTS
 *	Parameters:	fileName - ��������� ������ � ������ ����� �� �������� ��������� ������� ������
 *				multiTS - ����� ��� ������ ��������� ������
 *	Returns:		0  - ���� ��� ��������� ���������
 *				-1 - ���� � ����� �� ���������� ����� ����������� ��������� ������ �� �� ����������, 
 *				���������� � ��������������� �����.
 *	Throws:		AxiomLibException -  ���� ���������� ���������� ����� �� ���������� � ������� ����������
 *								���� ���� ����� �� ���������, �� ���� � �����-�� ������ ���������� 
 *								������ ��� ������ ��������, ��� ����������� ����������
 *								(������ ����� ��������� ����� ������ � ./ formats.x)
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readFromCSV (std::string fileName, MultiTS &multiTS) {
	 // ��������� ���� ��� ��������
	 boost::filesystem::ifstream file( fileName );
	 
	 //�������� - ���������� �� ����
	 if ( !boost::filesystem::exists( fileName ) ) {
		 // ��� ����� �� ������� - ���������� throw AxiomLibException
		 throw AxiomLibException ("Error in DataSetBase::ReadFromCSV: given file does not  exists.");
	 }

	 multiTS.trajectoryName = fileName;
	 
	 // ������ ����� ��� ���� ����������
	 // ������ ������ ������ �����, ���������� �������� ����������, � ������� ��������� ��������� ����
	 
	 std::string str; // ������ �� ����� ����� ����������� ��������� � ������ ����� � ������ ����������
	 std::vector<int> mask; 	// ������ � ����� multiTS ��������� ����� ���������� � ������������ �������
						 // ����� ���������� ���� ������� - ������� ������ ����� - ������������� ���������� ����� 
						 // ��������� � ����� � ���������� ����� ��������� � ������������ �������
	 //int i; // � ������ ���������� ����� ������������ ��� ������ ��� ������ ������ �� �����
	 int numToMask; // � ������ ���������� ����� ������������ ��������� ������ ��������� � �������� ������ � ������� 
				  // ����� �������������� ������������� ������� ����������
	 
	 // ������� ��������� - � ���������� ������ ������� ����������� � �������������� ����� ������ ����� � ����� ����� �����
	 // �������� csv ��������� ������������� ��� ;, ��� � , � �������� �����������
     boost::char_separator<char> sep(",;:	\t\r\n");
 
	 //��������� � ������������ ������ ������ ����� - ���������� ���������
	 if (!file.eof())  {
		 getline(file, str);
		 int res = (int) str.size();
		 //std::cout << "\n str size is" << str.size() << "\n";
		 if (res) {
			 //�������������� ����� ��� ������ ���������� � �������� ����
			 multiTS.validParams.resize ( this->paramNames.size() );
			 multiTS.data.resize( this->paramNames.size() );
			 mask.resize( this->paramNames.size() );
			 //��������� ������ ����������� ������ �� �����
			 token_separator tokens(str, sep);
			 // ���������� ��������� ������ ������������� ���������� ������
			 for (unsigned int t = 0; t < multiTS.validParams.size(); t++) {
				 multiTS.validParams[t] = false;
			 }
			 int pointer = 0;
			 //��������� ������ mask
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 numToMask = isIn(*tok_iter, paramNames); 
				 if (numToMask != -1) {
					 multiTS.validParams[numToMask] = true;
					 mask[pointer] = numToMask;
				 }
				 else {
					 mask[pointer] = -1;
				 }
				 pointer++;
			 }
		 }
	 }
	 else {
		 // �������������� ���� ����������, �� �� ���� - ��� ���� ������ ������� �� �������
		 //throw AxiomLibException ("Error in DataSetBase::readFromCSV : given .csv file is empty.");
		 file.close();
		 return 0;
	 }
 
	 // ������ ����� ����� ��������� �������� � ������ ����� - ����� ��������� ���� ���� - �� ���� ��������� ���������� mask
	 // ��������� - ���� �� ����� ����� � ����� ����������� ���������, ������������ � ������������ ������� ���������� �� ����������������� �����
	 int numOfRows = 0;
	 for (unsigned int j = 0; j < multiTS.validParams.size(); j++) {
		 if (multiTS.validParams[j] == true) {
			 numOfRows++;
		 }
	 }
	 // ���� ��� ����������, ��� ���� �� ��������� � ������ ������� - � ������� ������ ������ - �� ���� �� ����������� � ���� ������� � ����� -1
	 if (numOfRows == 0) {//������ ������ �� ����� - ������� ������� - � �� �������� �������� - ��� ������ �� ��������� - �� ���� ��� "-1"
		 //std::cout << "\n exit by zero num of rows \n";
		 file.close();
		 return -1;
	 }
	 //std::cout << "\n num of row is " << numOfRows << "\n";
	 // ������ ��� ������ - ���� ����� ������, ���� ���� ������ ������ - � �������� ��� ��������� � multiTS
	 // ����� ���������� �����������: 
	 signed int lenOfTS = 0; // � ������ ���������� ����� ��������� ����� ��������� � ����������� ��������� �����
	 signed int pointer; // ��������������� ����������, ����� ������ �� ������� mask
	 // �������� ���������� �����
	 while (!file.eof())  {
		 getline(file, str);
		 //std::cout << "\n i is " << i <<  "\n";
		 if (!str.empty()) {
			 lenOfTS++; 
			 // ��������� ��������� ������ �� ��������� ��������
			 token_separator tokens(str, sep);
			 pointer = 0;
			 // ���������� ������ ��������� ����������� � ����������� ����� � ����� multiTS
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 while ((pointer  < numOfRows) && (mask[pointer] == -1)) {
					 pointer++;
				 }
				 //�������� �� ����� ��������� � ������ - �� ������ �� �� ��� ����������
				 if (pointer < numOfRows) {
					 //std::cout << "	1 \n";
					 const char * curF = tok_iter->c_str();
					 (multiTS.data[mask[pointer]]).push_back (atof(curF));
				 }
				 else {
					 //std::cout << "	2 \n";
					 file.close();
					 throw AxiomLibException ("Error in DataSetBase::readFromCSV: too many values in one string in cvs file -  more then parameters");
				 }
				 pointer++;
			 }
			 // �������� �� ����� ��������� � ������ - �� ������ �� �� ��� ����������
			 if (pointer < numOfRows) {
				 file.close();
				 throw AxiomLibException ("Error in DataSetBase::readFromCSV: too small amount of values in cvs file -  less then parameters");
			 }
		 }
	 }
	 
	 // �������� ������������� ����� � ����� �� �������
	 file.close();
	 return 0;
 }
 
 
 /****************************************************************************
 *				DataSetBase::ReadFromRefCSV
 *
 *	Description:	������� ������ ������ �� *-ref.csv �����. �������� ��� ������ ����� � 
 *				������������ �� ��������� ������ � ������ ���������� ��������
 *	Parameters:	fileName - ��������� ������ � ������ ����� �� �������� ��������� ������� ������
 *				multiTS - ������ ��� ������������ ����
 *	Returns:		0  - ���� ��� ��������� ���������
 *				-1 - ���� � ����� �� ���������� ����� ����������� ��������� ������ �� �� ����������, 
 *				���������� � ��������������� �����.
 *	Throws:		AxiomLibException -  ���� ���������� ���������� ����� �� ���������� � ������� ����������
 *								���� ���� ����� �� ���������, �� ���� � �����-�� ������ ���������� 
 *								������ ��� ������ ��������, ��� ����������� ����������
 *								(������ ����� ��������� ����� ������ � ./ formats.x)
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readFromRefCSV (std::string fileName, MultiTS &multiTS) {
	 // ��������� ���� ��� ��������
	 boost::filesystem::ifstream file( fileName );
	 
	 //�������� - ���������� �� ����
	 if ( !boost::filesystem::exists( fileName ) ) {
		 // ��� ����� �� ������� - ���������� throw AxiomLibException
		 throw AxiomLibException ("Error in DataSetBase::ReadFromRefCSV: given file do not  exists.");
	 }
	 
	 // ������ ����� ��� ���� ����������
	 // ������ ������ ������ �����, ���������� �������� ����������, � ������� ��������� ��������� ����
	 
	 std::string str; // ������ �� ����� ����� ����������� ��������� � ������ ����� � ������ ����������
	 std::vector<int> mask; 	// ������ � ����� multiTS ��������� ����� ���������� � ������������ �������
						 // ����� ���������� ���� ������� - ������� ������ ����� - ������������� ���������� ����� 
						 // ��������� � ����� � ���������� ����� ��������� � ������������ �������
	 //int i; // � ������ ���������� ����� ������������ ��� ������ ��� ������ ������ �� �����
	 int numToMask; // � ������ ���������� ����� ������������ ��������� ������ ��������� � �������� ������ � ������� 
				  // ����� �������������� ������������� ������� ����������
	 
	 // ������� ��������� - � ���������� ������ ������� ����������� � �������������� ����� ������ ����� � ����� ����� �����
     boost::char_separator<char> sep(";:	\r\n");
 
	 //��������� � ������������ ������ ������ ����� - ���������� ���������
	 if (!file.eof())  {
		 getline(file, str);
		 int res = (int) str.size();
		 if (res) {
			 //�������������� ����� ��� ������ ���������� � �������� ����
			 multiTS.validParams.resize ( this->paramNames.size() + 1);
			 multiTS.data.resize( this->paramNames.size() + 1);
			 mask.resize( this->paramNames.size() + 1);
			 //��������� ������ ����������� ������ �� �����
			 token_separator tokens(str, sep);
			 // ���������� ��������� ������ ������������� ���������� ������
			 for (unsigned int t = 0; t < multiTS.validParams.size(); t++) {
				 multiTS.validParams[t] = false;
			 }
			 //��������� ������ mask
			 int pointer = 0;
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 numToMask = isIn(*tok_iter, paramNames);
				 if (numToMask != -1) {
					 // ��������� �������� - �������� ��������� ( � ��������� time )
					 multiTS.validParams[numToMask] = true;
					 mask[pointer] = numToMask;
				 }
				 else {
					 if ( *tok_iter == this->name ) {
						 // ������� � ��������� ������ �������������� �� ������ �� ��������
						 multiTS.validParams[this->paramNames.size()] = true;
						 mask[pointer] = (int) this->paramNames.size();
					 }
					 else {
						 // ��������� ������� - � ������������ ������� - �� ���������� �� ����������
						 mask[pointer] = -1;
					 }
				 }
				 pointer++;
			 }
		 }
	 }
	 else {
		 // �������������� ���� ����������, �� �� ���� - ��� ���� ������ ������� �� �������
		 //throw AxiomLibException ("Error in DataSetBase::readFromCSV : given .csv file is empty.");
		 file.close();
		 return 0;
	 }
 
	 // ������ ����� ����� ��������� �������� � ������ ����� - ����� ��������� ���� ���� - �� ���� ��������� ���������� mask
	 // ��������� - ���� �� ����� ����� � ����� ����������� ���������, ������������ � ������������ ������� ���������� �� ����������������� �����
	 int numOfRows = 0;
	 for (unsigned int j = 0; j < multiTS.validParams.size(); j++) {
		 if (multiTS.validParams[j] == true) {
			 numOfRows++;
		 }
	 }
	 // ���� ��� ����������, ��� ���� �� ��������� � ������ ������� - � ������� ������ ������ - �� ���� �� ����������� � ���� ������� � ����� -1
	 if (numOfRows == 0) {//������ ������ �� ����� - ������� ������� - � �� �������� �������� - ��� ������ �� ��������� - �� ���� ��� "-1"
		 file.close();
		 return -1;
	 }
 
	 // ������ ��� ������ - ���� ����� ������, ���� ���� ������ ������ - � �������� ��� ��������� � multiTS
	 // ����� ���������� �����������: 
	 signed int lenOfTS = 0; // � ������ ���������� ����� ��������� ����� ��������� � ����������� ��������� �����
	 signed int pointer; // ��������������� ����������, ����� ������ �� ������� mask
	 // �������� ���������� �����
	 int isin = -1;
	 while (!file.eof())  {
		 getline(file, str);
		 if (!str.empty()) {
			 lenOfTS++; 
			 // ��������� ��������� ������ �� ��������� ��������
			 token_separator tokens(str, sep);
			 pointer = 0;
			 // ���������� ������ ��������� ����������� � ����������� ����� � ����� multiTS
			 for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
				 while ((pointer  < numOfRows) && (mask[pointer] == -1)) {
					 pointer++;
				 }
				 //�������� �� ����� ��������� � ������ - �� ������ �� �� ��� ����������
				 if (pointer < numOfRows) {
					 // ������ ���������� ��������
					 if (mask[pointer] == this->paramNames.size()) {
						 // ���������� �������� � ������ ������ ������������� �� ������� �������
						 isin = isIn(*tok_iter, classNames);
						 if ((isin != -1) || (*tok_iter == this->null)) {
							 // ��� ��������� - ���������� ����� ���������� ������ �������������� � ������������ ������� ���� 
							 (multiTS.data[mask[pointer]]).push_back (isin + 1);
						 }
						 else {
							 // ������ ������ �� ������� ����� ������ ������������� - ����� ������ - ������� �� throw
							 std::cout << "\n tok_iter is '" << *tok_iter << "'\n";
							 std::cout << " null is '" << this->null << "'\n";
							 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: wrong name of class");
						 }
					 }
					 else {
						 // ���������� �������� �������� �������� - �������� �������� ( � ������� time )
						 const char * curF = tok_iter->c_str();
						 (multiTS.data[mask[pointer]]).push_back (atof(curF));
					 }
				 }
				 else {
					 file.close();
					 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: too many values in one string in cvs file -  more then parameters");
				 }
				 pointer++;
			 }
			 // �������� �� ����� ��������� � ������ - �� ������ �� �� ��� ����������
			 if (pointer < numOfRows) {
				 file.close();
				 throw AxiomLibException ("Error in DataSetBase::readFromRefCSV: too small amount of values in cvs file -  less then parameters");
			 }
		 }
	 }
	 
	 // �������� ������������� ����� � ����� �� �������
	 file.close();
	 return 0;
 }
   
 /****************************************************************************
 *					DataSetBase::getIntByName
 *
 *	Description:	������� ���������� ����� �� �������� � ������ ����.
 *				����������� ��� ������������ ������ ���������� 
 *				��������� ��� � ���������� ��������.
 *	Parameters:	str - ������ - �� ������� ���������� �����
 *	Returns:		- 1 - ���� ��� ���� � ������ ���
 *				������������ ����� ����� ��� 0 - ����������� �� ������
 *	Throws:		-
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::getIntByName (std::string str) {
	 // ��������� - �� ����� �� ������������� ������
	 if (!str.empty()) {
		 // ���������� ������� ��������� � ������������ ��������� ����������� �����
		 int toReturn = 0;
		 bool flag = 0;
		 char zero = '0';
		 char nine = '9';
		 // ���������� ����� �� ������
		 for (unsigned int i = 0; i < str.size(); i++) {
			 if ((zero <= str[i]) && (str[i] <= nine)) {
				 flag = 1;
				 toReturn = toReturn * 10 + (int) (str[i] - zero);
			 }
		 }
		 if (flag != 0) {
			 // ���� ���� ���� �� ���� ����� � ������ - �� ���������� ������������ �����
			 return toReturn;
		 }
		 else {
			 // ���� �� ���� �� ����� ����� � ������
			 return -1;
		 }
	 }
	 return -1;
 }
 
 bool DataSetBase::checkRefCSVName(const std::string& currentFileName) {
	 // ������ - �������� ������ ������������ ����� ������ � ���������� ������ ��� ������� ��������������

     if(currentFileName.size() <= 4 || currentFileName[currentFileName.size() - 4] != '.') {
         return false;
     }

     std::string extension = currentFileName.substr(currentFileName.size() - 3);
     return extension == "csv";

//	 static const std::string fileNameFirstHalf ("ref");
//	 static const std::string fileNameLastHalf (".csv");
	 
//	 return (checkName (currentFileName, -1, 0, fileNameFirstHalf))
//			 && (checkName (currentFileName, (int) currentFileName.size() - 5, (int) currentFileName.size() - 4, fileNameLastHalf));
 }
 
 /****************************************************************************
 *				DataSetBase::readClassTS
 *
 *	Description:	������ ��������� ���� ��� ��������� ������ 
 *	Parameters:	dirName - ������, ���������� ����� ���������� � �������
 *				classTs - ����������, � ������� ����������� ������
 *	Returns:		0 - ���� ��� ���� �������� ��������� (����� throw)
 *	Throws:		-
 *	Author:		wictor
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readClassTS(const std::string& dirName, ClassTS &classTS) {
	 

	 boost::filesystem::path classPath (dirName);
	 
	 // ���� �� ������ ��������� ���������� �������������� ���������� ������ ��������������
	 boost::filesystem::directory_iterator end_iter_int;
	 int fileCount = 0;
	 for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
		 if(checkRefCSVName((*dir_itr_int).path().string().c_str())) {
			 fileCount++;
		 }
	 }
	 
	 classTS.clear();
	 // ��������� ������ ��� fileCount ��������� �����
	 classTS.reserve(fileCount);
	 
	 for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
		 //std::cout << "\n	EPT 2 \n";
		 //filePath = *dir_itr_int;
		 // ��������� �������� ����� � ������� � char* - ����� �������� �� ������������ (�� ./formats.x)
		 std::string currentFileName = dir_itr_int->path().filename().string();
		 if (checkRefCSVName(currentFileName)) {
			 //�������� ���� ������� ���� � �������� � ���������� ��������� ����� multiTS
			 //std::cout << "\n	EPT 3 \n";
			 MultiTS multiTStemp;
			 currentFileName = (*dir_itr_int).path().string().c_str();
			 this->readFromCSV (currentFileName, multiTStemp);
			 /*// Test Output
			 std::vector<double> vecTemp;
			 std::cout << "\n MultiTS \n";
			 for (int nR = 0; nR < multiTStemp.size(); nR++) {
				 multiTStemp.getTSByIndex(vecTemp, nR);
				 std::cout << "\n";
				 for (int fR = 0; fR < vecTemp.size(); fR++) {
					 std::cout << "  " << vecTemp[fR];
				 }
			 }*/
			 // Storing multiTS
			 classTS.push_back(multiTStemp);
		 }
	 }
	 
	 
	 return 0;
 }
 
 /****************************************************************************
 *				DataSetBase::readReferenceTS
 *
 *	Description:	������ ��������� ���� ��� ������� �� ������� ��������������
 *				����, ����� �� ���� ��������� � $path/reference/. 
 *				��� �������� ������ �����, � �������� ��������� ���� �� 
 *				����� ��������. ��� ������� ��������, ����� �� ���� ������� � ��� 
 *				.csv-������, ������ ��, �� ������� ��������� � ������������� ����.
 *	Parameters:	path - ������ ���������� ����� �������� ���������� ������ ������
 *				refClassesTS - ����������, � ������� ����������� ������
 *	Returns:		0 - ���� ��� ���� �������� ��������� (����� throw)
 *	Throws:		AxiomLibException  - ���� ���������� $path/reference/ - �� ���������� 
 *								��� ���� ����� ����������, �� ��� ����
 *	Author:		dk
 *	History:
 *
 ****************************************************************************/
 signed int DataSetBase::readReferenceTS (const std::string &path, ReferenceClassesTS &refClassesTS) {
	 // ������� ����� ��� ������ � ������������ � ������� 
	 boost::filesystem::path fullPath( path );
	 //fullPath /= "reference";
	 boost::filesystem::path classPath( fullPath ); // ����� �������������� ����� ������ �� ������������� ���������� reference
	 
	 // �������� - ���������� �� �������� ���� � �����
	 if ( !boost::filesystem::exists( fullPath ) ) {
		 throw AxiomLibException ("Error in DataSet::browseFiles: wrong path - path do not exists.");
		 return 1;
	 }
	 
	 // ���� �������� �� ���������� ����� �� ���� - �������� ��������� ������
	 refClassesTS.resize(this->classNames.size());
	 
	 // ��������� - ���������� �� ������ ���� - ��� �������������� � ��������� ��������� ������ ������
	 if ( boost::filesystem::is_directory( fullPath ) ) {
		 int isin = -1; // ���������� ��� �������� ���������� ������ ������� isIn
		 // ��������� ���� - �� ���������� ������ ���������� $path/referenes - 
		 // ����� ������������ ������� �������������� - �������� ��������������� ��������� ����
		 boost::filesystem::directory_iterator end_iter;
		 for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			 if ( boost::filesystem::is_directory( *dir_itr ) ) {
				 // ��������� - ����� �� ����� �������������� � ������ ������� �������������� ���������� � ������. �����
					 isin = isIn(dir_itr->path().filename().string(), classNames);
				 //std::cout<< "\n WARN isin is " << isin << "\n";
				 //std::cout << "\n	EPT 0 \n";
				 if ( isin != -1) {//��� ������ ���� � ������� DataSet - �� ���� ������ �������� � ��������� ����������
					 //std::cout << "\n	EPT 1 \n";
					 classPath = fullPath;
					 classPath /= classNames[isin];
					 
					 readClassTS(classPath.string(), refClassesTS[isin]);
				 }
			} else { // � ���������� $path/reference ������� ��������� ����  - �� ������� �������� - �� ��������� �������� �� ����� - ����������
				 //std::cout << "������� �������� �����: " << dir_itr->leaf() << "\n";
			 }
		 }
	 
	 }
	 else {// ��������� ���� �� �������� ��������� - ������� ���������� ��������� ����� �� �����������
		 throw AxiomLibException ("Error in DataSet::readClassesTS: wrong path - path is not a dir, it is a file.");
	 }
	 
	 return 0;
 }
 
  
// /****************************************************************************
// *				DataSet::IsIn
// *
// *	Description:	������� ��������� - ��������� �� ������ ������ � ������ ������� �����
// *	Parameters:	whatToFind - ������, ������� ����, whereToFind - ������ �����. ����� ������� ����
// *	Returns:		- 1 - ���� ������ ��� � ������ ���
// *				������������� ����� - ����� ������ � ������
// *	Throws:		-
// *	Author:		dk
// *	History:		-
// *
// ****************************************************************************/
// signed int DataSet::isIn (std::string whatToFind, std::vector<std::string> &whereToFind) {
//	 for (signed int i = 0; i < (int) whereToFind.size(); i++) {
//		 if (whatToFind == whereToFind[i]) {
//			 return i;
//		 }
//	 }
//	 return -1;
// }
 
 
// /****************************************************************************
// *				DataSet::IsIn
// *
// *	Description:	������� ���������� - ��������� �� ������ ����� � ������ ������� �����
// *	Parameters:	whatToFind - �����, ������� ����, whereToFind - ������ �����. ����� ������� ����
// *	Returns:		- 1 - ���� ����� ��� � ������ ���
// *				������������� ����� - ����� ����� � ������
// *	Throws:		-
// *	Author:		dk
// *	History:		-
// *
// ****************************************************************************/
// signed int DataSet::isIn (int whatToFind, std::vector<int> &whereToFind) {
//	 for (signed int i = 0; i < (int) whereToFind.size(); i++) {
//		 if (whatToFind == whereToFind[i]) {
//			 return i;
//		 }
//	 }
//	 return -1;
// }
