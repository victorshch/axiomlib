/****************************************************************************
*				AxiomBase.cxx
*
*	Description:	������� ������ AxiomBase - ����������
*	Author:		dk
*	History:		
*
****************************************************************************/
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "AxiomBase.h"

using namespace AxiomLib;

// �������� ����� ��� ������ ����� � ��������� ������� ������: 
#define str_not_symbol			"not"
#define str_or_symbol			"or"
#define str_ax_file_ending		".ax"
#define str_ec_file_ending		".ec"
#define str_list_file_ending	".list"


// ������ �����������
AxiomBase::AxiomBase (void) {
}


// ������� ���������� �����  ������ � ��������� ���������
int AxiomBase::size (void) const {
	return (int) axioms.size();
}


/****************************************************************************
*					AxiomBase::compareStringAndPath
*
*	Description:	������� ��������� ����� ����� � ����� � �����.
*				��������� ���������� �� ������: 
*				- � ����� ����� ������������ ���������� - ����������� � ����������
*				- � ������ ����� ������������ "/"
*				- ���������� ��� ����� ������ � ����� ���� � �����
*	Parameters:	fileName - ��� �����
*				path - ���� � �����(�� ���� ��� � �����)
*				n3 - ���������� �����, � ������� �������������� ����� ���� � ���� � �����
*	Returns:		bool
*	Throws:		-
*	Author:		dk
*	History:
*
****************************************************************************/
bool AxiomBase::compareStringAndPath (std::string fileName, std::string path, const char* n3) const {
	
	   std::string n1("/");
	   const char* n2 = fileName.c_str();
	   n1.append(n2);
	   n1.append(n3);
	   return checkName (path, (int) (path.size() - n1.size() - 1), (int) (path.size()  - n1.size()), n1);

}


/****************************************************************************
*					AxiomBase::checkName
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
#define maxAddrLen 1000
bool AxiomBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) const {
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
*					AxiomBase::readFromAX
*
*	Description:	������� ��������� .ax ������
*				(��������������� ������� � readAxioms)
*				������� ��������� ���������� �� ����� �������� �������
*				� ���������� ����������� ���������� � ���������� 
*				������������ ��������� - ����������� �� ����� ������� � 
*				������ ������������ �������. ������������ ������ �������
*	Parameters:	filePath - ���� � ������������� ����� �������� �������
*				vec - ����������� ������ ������������ ������� ������������ 
*					������ �������
*	Returns:	0 - ���� ���� ��� ������ � ��������� ��������
*				-1 - ���� ���� �� ��� ������
*	Throws:		AxiomLibException  - ���� ���� � �������� ����� �� ������ 
*								���� �� ����� ���� ��������� �������
*								���� ������ ����� �� ������������� 
*								�������� �������� (��. formats.x)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (std::string filePath, std::vector<std::string> &vec) {
	std::string str;
	int i = 0;
	int flagAND = 0;
	
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromAX: cannot find given .ax file.");
		return 1;
	}
	
	vec.resize(0);
	
	// ��������� ����
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" .,:;	");
	
	if (i = std::getline(file, str) && i != EOF) {
		// ������ ��� ������� � ���������� � ��������� �����
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (this->compareStringAndPath(*tok_name, filePath, ".ax") == false) {
			// ��� ������� ���������� - � �������� ����� � � ��� ���������� - ������ ������
//			file.close();
//			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		
		// ����������� ��������� ���������� ����� ������������ ������� � ����� 
		while (i = std::getline(file, str) && i != EOF)  {
			if (!str.empty()) {
				token_separator tokens(str, sep);
				for (token_separator::iterator tok_iter = tokens.begin();  tok_iter != tokens.end();  ++tok_iter) {
					if (flagAND == 0) { // ���� ��������� ����������� �������
						vec.push_back(*tok_iter);
						flagAND = 1;
					}
					else { // ���� AND - � ���� ��� - �� ������ � ������� ����� �������
						if (*tok_iter != "AND") {
							// ���� �������� �������� ��� ����� ������� �������� ������� - ������� ���������� ���
							file.close();
							vec.clear();
							return -1;
							//throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format in axiom file .");
						}
						flagAND = 0;
					}
				}
			}
 		}
	}
	
	file.close();
	
	if ((flagAND == 0) && (vec.size() > 0)) { // ���� ������� ��������� ������������ ������� - �� �� ����� ����� AND - ��� ������ �������
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format in axiom file .");
	}
	
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromX
*
*	Description:	������� ��������� .x �����
*				������� ��������� ��� ����� ������ � ���������� � ������ ������� �����
*				(��������������� ������� � readAxioms)
*	Parameters:	filePath - ���� � ������������� .x �����
*				strName - ����������� �������� - ������ ����� ������ - ����������� �� �����
*	Returns:		0
*	Throws:		AxiomLibException  - ���� �������������� ����� ��� - �� ���� ���� �� ������
*								���� ���� ����
*								���� ��� ����� ������, ���������� � ����� ����������� 
*								�� ����� ����� �����
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromX (std::string filePath, std::string &strName) {
	std::string str;
	int i = 0;
	
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromX: cannot find given .x file.");
	}
	
	// ��������� ����
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" .,:;	");
	while (i = std::getline(file, str) && i != EOF) {
		// ������ ������ ���� ���� �� ������ ������ �� ������ ������
		if ( str != "\n" ) {
			break;
		}
	}
	if ((str.empty()) || (str == " ")) {
		throw AxiomLibException ("Error in AxiomBase::readFromX: name of axiom_base should be in .x file  .");
	}
	if (i != EOF) {
		// ������ ��� ���� ������ � ���������� � ��������� �����
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if (this->compareStringAndPath(*tok_name, filePath, ".x") == false) {
			// ��� ���� ������ ���������� - � �������� ����� � � ��� ���������� - ������ ������
//			file.close();
//			throw AxiomLibException ("Error in AxiomBase::readFromX: different file's names in the file and in the name of the file .");
		}
		strName = *tok_name;
	}
	else {
		file.close();
		throw AxiomLibException ("Error in AxiomBase::readFromX: given .x file is empty - there should be an axiom_base_name.");	
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readAxioms
*
*	Description:	������� ������ ���� ������.
*				������� �������� �� �������� ��������� ���������� 
*				� ����� (���� � ��������) �������� ���� ������. ����� ������� 
*				� ����� ����� ������ �������� - � ���������� ��� ������ 
*				�� ��������������� ��������. ����� ������ ������ - ����������
*				�������, ��������� � ������.
*	Parameters:	axiomBaseDir - ������ � ����� � �������� ���� ������
*	Returns:		0
*	Throws:		AxiomLiException  - 	���� �� ������� ��������, ���������� � 
*								���������������� ����� ��� ���� ������
*								���� �������� ���� � ����� �������� - ���� � �����
*								���� � �������� �� ���������� ����� .x � ���������
*								����� ����� ������ (���� ����� ������ ����� ������)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readAxioms (const char* axiomBaseDir) {
	std::string axiomDir (axiomBaseDir);
	boost::filesystem::path fullPath( axiomDir );
	boost::filesystem::path filePath;
	
	std::string templateAxiomFile (".ax"); // ������1 ���������� ����� � �������� �������
	std::string templateBankFile (".x"); // ������2 ���������� ����� � ������ ����� ������
	
	std::string aa; // ��������� ����������, �������� ��� �������� ������������ � ������������ ����� 
	
	std::string str;
	int i = 0;
	
	// �������� - ���������� �� ������� � ������ ������
	if ( !boost::filesystem::exists( fullPath  ) ) {
		throw AxiomLibException("Error in AxiomBase::readAxioms: cannot find base axiom directory "
								"'" + axiomDir + "'");
		return 1;
	}
	
	// ��������� - ���������� �� ������ ���� - ��� �������������� � ��������� ��������� ����� ������
	if ( boost::filesystem::is_directory( fullPath ) ) {
		int numOfXFiles = 0;
		this->axioms.clear();
		// ����������� �������� ���������� - ������������ ��� ���������� ��� ������� �����
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( boost::filesystem::is_directory( *dir_itr ) ) { // ������ ��������� �������� � ������ ������ - ������� ����� ������� - 
												// ��� �� ������������� �������� - ������ ���������� ���� �������				
        		}
			else {
				filePath = fullPath;
				filePath /= dir_itr->path();
				aa = dir_itr->path().filename().string();
				if ( checkName (aa, (int) (aa.size() - 4), (int) (aa.size() - 3), templateAxiomFile) ) {
					// �� ���� � dir_itr ������ ��� ����� �������� �������
					std::vector<std::string> vec;
					aa = (*dir_itr).path().string().c_str();
					this->readFromAX (aa, vec);
					if (vec.size() > 0) {
						aa = dir_itr->path().filename().string();
						this->axioms[aa.substr(0, aa.size() - 3)] = vec;
					}
				}
				else {
					if ( checkName (aa, (int) aa.size() - 3, (int) aa.size() - 2, templateBankFile) ) {
						// �� ���� � dir_itr_int ������ ��� ����� � ��������� ����� ������
						if (numOfXFiles == 0) {
							std::string strName;
							aa = (*dir_itr).path().string().c_str();
							this->readFromX (aa, strName);
							this->name = strName;
							numOfXFiles++;
						}
						else {
							throw AxiomLibException("Error in AxiomBase::readAxioms: to many .x files.");
						}
					}
					else {
						// �� ���� � ���������� ����� ������ ������� ���� � �� ���������� ��� �������� ������ - ��� ������ ����������
					}
				}
			}
		}
		if (numOfXFiles == 0) {// �� ���� ������� �������� ���� ������ - ������� ����� � ������� - ��� ������� �����
			throw AxiomLibException ("Error in AxiomBase::readAxioms: cannot find a file with a name of the AxiomBase.");
		}
	}
	else {// ��������� ���� �� �������� ��������� - ������� ���������� ��������� ����� �� �����������
		throw AxiomLibException ("Error in AxiomBase::readAxioms: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					AxiomBase::getVectorOfEC
*
*	Description:	������ ������ ������ ������������ ������� �������������� 
*				����������� �������, ���� ������ ������ ���� �� ������� ������� 
*				� �������� ������
*	Parameters:	toReturn - ������������ ������
*			axiomName - ��� �������, ���������� � ������� ����
*	Returns:		0
*	Throws:		AxiomLiException  - 	���� �� ������� ��������, ���������� � 
*								���������������� ����� ��� ���� ������
*								���� �������� ���� � ����� �������� - ���� � �����
*								���� � �������� �� ���������� ����� .x � ���������
*								����� ����� ������ (���� ����� ������ ����� ������)
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomBase::getVectorOfEC (std::vector<std::string>& toReturn, std::string axiomName) {
	toReturn.resize(0);
	if ((this->axioms).count(axiomName)) {
		toReturn = this->axioms[axiomName];
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	������� ������ ���������� �� ������ �� ������ .ax �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������������ �������; 
*					(����� �� ������ ��������� ����������� �������� " ,';:[]" )
*					����� � ������� ����������� �������� � ������ ������� � �� ���������.
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. AxiomExprStrucutre.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aes - ����������� �� ����� �������� ���� AxiomExprStrucutre
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (const std::string & filePath, AxiomExprStructure &aes) const {
	std::vector <std::string> tmpParamNames;
	tmpParamNames.clear();
	return readFromAX(filePath, aes, tmpParamNames);
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	������� ������ ���������� �� ������ �� ������ .ax �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������������ �������; 
*					(����� �� ������ ��������� ����������� �������� " ,';:[]" )
*					����� � ������� ����������� �������� � ������ ������� � �� ���������.
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. AxiomExprStrucutre.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aes - ����������� �� ����� �������� ���� AxiomExprStrucutre
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (const std::string &filePath, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	// ���������� ���������� � ����� �����
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_ax_file_ending);
	
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromAX: cannot find given file with axiom-description.");
		return 1;
	}

	// ������� ���������� ��������� � ��������� �������
	aes.clear();

	// ��������� ����
	boost::filesystem::ifstream file( fullFilePath );

	boost::char_separator<char> sep(" :;\t[],");
	std::string str;
	int i = 0;

	// ������ ��� ������� � ���������� � ��������� �����
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (compareStringAndPath(*tok_name, fullFilePath, str_ax_file_ending) == false) {
			// ��� ������� ���������� - � �������� ����� � � ��� ���������� - ������ ������
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		// ���������� ��� �����
		aes.axiomName = *tok_name;
	} else {
		// ���� ���� ����  -  �� ������ - ������ ��� ���� ����
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - it is empty.");
	}
	
	// ������ ��� ����� ������������ �������, ������������ � �������
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomBase::readFromAX: different file's names in the file and in the name of the file .");
		}
		// ���������� ��� ����� ������
		aes.elemCondBankName = *tok_name;;
	} else {
		// ������ �������� - ��� ����� ������������� ����� ������������ �������
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - it has no axioms.");
	}
	
	// ������ �������� �������
	readFromAX (file, aes, paramNames);

	// ��������� - ��� �� ���� ������� �� �����
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong fowmat - there is something wrong at the end og the file.");
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromAX
*
*	Description:	������� ������ ���������� �� ������ �� ������ .ax �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������������ �������; 
*					(����� �� ������ ��������� ����������� �������� " ,';:[]" )
*					����� � ������� ����������� �������� � ������ ������� � �� ���������.
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. AxiomExprStrucutre.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aes - ����������� �� ����� �������� ���� AxiomExprStrucutre
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*					file - �������� ���� �� �������� ����� ������������� ���������� �������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromAX (boost::filesystem::ifstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {	
	boost::char_separator<char> sep(" :;\t[],");
	std::string currentParam;
	std::string currentEC, curStr, currentDimen;
	std::vector <ElemCondPlusStruture> andECs;
	ElemCondPlusStruture ecPlusStruture;
	int toSave, condition;
	std::string str, tempStr;
	int i = 0;
	// ����������� ��������� ���������� ����� ������������ ������� � �� ���������
	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF) {
		// ���������, ������� ����� ��������� � ���������� � aes
		ecPlusStruture.clear();
		// ��������� ��������� ��� �������� ����� ������� � ����� ��������� �������
		andECs.clear();
		// ������� ������ �� ����� ��������� ������
		condition = 0;
		toSave = 0;
		while (condition == 0) {
			// ��������� ������ �� ������
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// ������� - ���������� ������ ������
			if ( tok_iter == tokens.end() ) {
				// ������ ������  - �������� ����������� ����� ��������� - 
				// � �������� ������ ���� �� ��������� ������ �����
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			// ������� - ����������� ����� �������
			//const char* tempNumber = (*tok_iter).c_str();
			tempStr = *tok_iter;
			if (atof(tempStr.c_str()) > 0) {
				// ������ �������� �������
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAX: wrong format of axiom-description - cannot find axiom name.");
				}
				if (!aes.axiomName.empty()) {
					// ��������� ��������� ������� - � ������� � ������ ��������� - ���������� ��������� � ������� �� �������
					file.seekg(pos);
					condition = 1;
					break;
				}
				aes.axiomName = *tok_iter;
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) { // ����� �������� ������� ���-�� - ���� - ���� �� ����� � ������� �����
					continue;
				} else { // ����� �������� ������� ������ ���
					condition = 1;
					break;
				}
			}
			// ������� - ��������� ������ ���������� � �������� ��������� �������
			if ( (*tok_iter) == str_or_symbol) {
				// ��������� �������� ������� �� � ��������� � ���������� ����������
				if (!andECs.empty()) {
					aes.elemConds.push_back (andECs);
					andECs.clear();
				}
				toSave = 0; // ���������, ��� ������� ����� ������������ ������� ��������� � �������
				++tok_iter;
				if ( tok_iter != tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - after there is some else in the string, beginning with 'or'.");
				}
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					// ����� - ���-�� - ���� - ���� �� ����� � ������� �����
					continue;
				} else {
					throw AxiomLibException ("Error in AxiomBase::readFromAX: wrong format of axiom-description - there is nothing after the string, beginning with 'or'.");
				}
			} 
			// ������� - ������ ������������ �������, ��� ��������� � ��������
			file.seekg(pos);
			readFromEC(file, ecPlusStruture, paramNames);
			// ��������� ��������� �������
			andECs.push_back (ecPlusStruture);
			ecPlusStruture.clear();
			toSave = 1;
			// ��������� ���� �� ��� ������
			pos = file.tellg();
			if (i = std::getline(file, str) && i != EOF) {
				// ����� - ���-�� - ���� - ���� �� ����� � ������� �����
				continue;
			} else {
				condition = 1;
				break;
			}
		}
		// ���������- ���� ���� �� ���������� ��������� ������������ �������
		if (toSave > 0) {
			aes.elemConds.push_back (andECs);
		}
	}
	
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	������� ���������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ���� �������� �������
*					AxiomName - ��� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (const std::string & baseDir, AxiomExprStructure &aes) const {
	std::vector <std::string> tmpParamNames;
	tmpParamNames.clear();
	return saveToAX (baseDir, aes, tmpParamNames);
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	������� ���������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ���� �������� �������
*					AxiomName - ��� �������
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (const std::string & baseDir, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(aes.axiomName);
	filePath.append(str_ax_file_ending);
	// ��������� �����
	std::ofstream w (filePath.c_str());
	// ��������� ���������� ������ �� ������� � ����
	// ���������� ��� �������
	w << aes.axiomName << "\n";
	// ���������� ��� ���� ������ ������
	w << aes.elemCondBankName << "\n";
	// ���������� ������������ �������
	saveToAX (w, aes, paramNames);
	// ��������� �����
	w.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToAX
*
*	Description:	������� ���������� ������� � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ���� �������� �������
*					AxiomName - ��� �������
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*					file - �������� ����, � ������� ������������ �������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToAX (std::ofstream &file, AxiomExprStructure &aes, const std::vector<std::string> &paramNames) const {
	std::string emptyStr;
	emptyStr.clear();
	// ���������� ������������ �������
	for (unsigned int iterExprI = 0; iterExprI < aes.elemConds.size(); iterExprI++) {
		if (iterExprI > 0) {
			// ������ ������ ������� �������� ���� ���������� � ����� ��������� ������������ �������
			file << "\t" << str_or_symbol << "\n";
		}
		for (unsigned int iterExprII = 0; iterExprII < aes.elemConds[iterExprI].size(); iterExprII++) {
			if ((aes.elemConds[iterExprI][iterExprII].dimension >=0) && (aes.elemConds[iterExprI][iterExprII].dimension < (int) paramNames.size()))
				saveToEC(file, aes.elemConds[iterExprI][iterExprII], paramNames[aes.elemConds[iterExprI][iterExprII].dimension]);
			else
				saveToEC(file, aes.elemConds[iterExprI][iterExprII], emptyStr);			
		}
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	������� ������ ���������� ������������� ������� �� ������ .ec �����
*					������ ����� ������ � .formats.x
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. ElemCondPlusStruture.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					ecps - ����������� �� ����� �������� ���� ElemCondPlusStruture
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (const std::string & filePath, ElemCondPlusStruture &ecps) const {
	std::vector<std::string> paramNames;
	return readFromEC(filePath, ecps, paramNames);
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	������� ������ ���������� ������������� ������� �� ������ .ec �����
*					������ ����� ������ � .formats.x
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. ElemCondPlusStruture.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					ecps - ����������� �� ����� �������� ���� ElemCondDataDimensionStruture
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (const std::string &filePath, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const {
	// ���������� � ����� ����������
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_ec_file_ending);

	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromEC: cannot find given .ec file.");
		return 1;
	}

	// ������� ���������� ��������� � ��������� �������
	ecps.clear();
	
	// ��������� ����
	boost::filesystem::ifstream file( fullFilePath );

	try {
	// ������ ���������� �����
	readFromEC (file, ecps, paramNames);
	} catch(AxiomLibException e) {
		// ����� ������� � ���������� �� ������ ����������� ��� �����
		throw AxiomLibException(e.error() + std::string(" Filename : '") + fullFilePath + "'");
	}

	// ��������� - ��� �� ���� ������� �� �����
	std::string str;
	int i = 0;
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong fowmat - there is something wrong at the end of the file.");
	}

	// ��������� ������������ ����
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromEC
*
*	Description:	������� ������ ���������� ������������� ������� �� ������ .ec �����
*					������ ����� ������ � .formats.x
*					��� ��������� �������� ������������ � ���������� ������������ 
*					���������, ��. ElemCondPlusStruture.h.
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					ecps - ����������� �� ����� �������� ���� ElemCondDataDimensionStruture
*					paramNames - ������ ���������� ������ ������, �.�. �������� ������������ ������ ������
*					file - �������� ���� �� �������� ����� ������������� ���������� �������� ������������� �������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::readFromEC (boost::filesystem::ifstream &file, ElemCondPlusStruture &ecps, const std::vector<std::string> &paramNames) const {
	boost::char_separator<char> sep(" :;\t[],");
	std::string currentParam, currentDimen, curStr;
	currentDimen.clear();
	int condition;
	bool curSign;
	std::string str;
	int i = 0;
	// ����������� ��������� ���������� ����� ������������ ������� � �� ���������
	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF)  {
		// ��������� ��������� ��� �������� ����� ������� � ����� ��������� �������
		currentParam.clear();
		// ������� ������ �� ����� ��������� ������
		condition = 0;
		while (condition == 0) {
			// ��������� ������ �� ������
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// ������� - ���������� ������ ������
			if ( tok_iter == tokens.end() ) {
				// ������ ������  - �������� ����������� ����� ���������, � �������� ������ ���� �� ��������� ������ �����
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				}
				else {
					condition = 1;
					break;
				}
			}
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			// ������� - ��������� ������ ���������� � �������� ��������� �������
			if ((*tok_iter) == str_or_symbol) {
				file.seekg(pos);
				condition = 1;
				break;
			}
			// ������� - ������ ������������ �������, ��� ��������� � ��������
			curSign = true;
			if (*tok_iter == str_not_symbol) {
				
				//std::cout<<"Current token is negation"<<std::endl;
				
				//������ ����� �������� �� ���� ��������� - ���������� ���
				curSign = false;
				do {
					++tok_iter;
				} while(*tok_iter == str_not_symbol);
				
				//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
				
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find name of elementary condition.");
				}
			}
			if (*tok_iter == ecps.ecName) {
				//������ ���������� ���������� ���������� ������� ������������� �������
			} else {
				if (!ecps.ecName.empty()) {
					file.seekg(pos);
					condition = 1;
					break;
				}
				ecps.ecName = *tok_iter;
				ecps.sign = curSign;
			}
			++tok_iter;
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find name of parameter.");
			}
			// ������� �������� ����������� ��������� ������ ��� �������� ��������� ������������� �������
			curStr = *tok_iter;
			++tok_iter;
			
			//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
			
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find -> in .ec file while setting parameters.");
			}
			if (*tok_iter == "->") {
				// ������ ��������� ����� ������ - ���� ���������� ������������� �������
				currentParam = curStr;
			} else {
				// ������ ����� ������� ����������� ������ ������
				if (curStr == currentDimen) {
					//������ ���������� ���������� ���������� ������� ������������� �������
				} else {
					if (!currentDimen.empty()) {
						// ������ �������� ��������� ������ �������, ������� ����� ���� ��������, �� ���������� ��� ������ ����������� ������
						file.seekg(pos);
						condition = 1;
						break;
						// ������ ������ �� ����� - �������� ���������
						//throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong format of elementary condition - different dimension.");
					}
					currentDimen = curStr;
					for (unsigned int u = 0; u < paramNames.size(); u++) {
						if (currentDimen == paramNames[u]) {
							ecps.dimension = u;
							break;
						}
					}
				}
				// ��������� �������� ��������� ������������� �������
				currentParam = *tok_iter;
				// ��������� ����� '->'
				++tok_iter;
				
				//std::cout<<"Current *tok_iter : "<<*tok_iter<<std::endl;
				
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find -> in .ec file while setting parameters.");
				}
				// ������� ������� '->'
				if (*tok_iter != "->") {
					throw AxiomLibException ("Error in AxiomBase::readFromEC: wrong format -- expected '->', got '"+std::string(*tok_iter)+"'");
				}				
			}
			// ��������� �������� �������� ���������
			++tok_iter;
			if ( tok_iter == tokens.end()) {
				throw AxiomLibException ("Error in AxiomBase::readFromEC: cannot find a value of parameter.");
			}
			// ������� �������� �������� ���������
			ecps.ecParams[currentParam] = *tok_iter;
			// ���� ���� ���-�� ��� - ���� �� ����� � �������� �����
			pos = file.tellg();
			if (i = std::getline(file, str) && i != EOF) {
				continue;
			} else { 
				condition = 1;
				break;
			}
		}
	}
	
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		filePath - ������ ���� � ������������� ����� � ����������� 
*					ecps - ��������� ���� ElemCondPlusStruture � ����������� 
*						   ���������� ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ���� �� ������� ��������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string &baseDir, const std::string &fileName, ElemCondPlusStruture &ecps) const {
	std::string paramName;
	paramName.clear();
	return saveToEC (baseDir, fileName, ecps, paramName);
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		filePath - ������ ���� � ������������� ����� � ����������� 
*					ecps - ��������� ���� ElemCondPlusStruture � ����������� 
*						   ���������� ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ���� �� ������� ��������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string & baseDir, const std::string & fileName, ElemCondPlusStruture &ecps, const std::vector <std::string> &paramNames) const {
	if ((ecps.dimension >= 0) && (ecps.dimension < (int) paramNames.size()))
		return saveToEC (baseDir, fileName, ecps, paramNames[ecps.dimension]);
	std::string paramName;
	paramName.clear();
	return saveToEC (baseDir, fileName, ecps, paramName);
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		filePath - ������ ���� � ������������� ����� � ����������� 
*					ecps - ��������� ���� ElemCondPlusStruture � ����������� 
*						   ���������� ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ���� �� ������� ��������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (const std::string & baseDir, const std::string & fileName, ElemCondPlusStruture &ecps, const std::string &paramName) const {
	// ������� ���� � ���������� ���������� � �����
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_ec_file_ending);
	// ��������� �����
	std::ofstream w (filePath.c_str());
	// ��������� ���������� ������ �� ������� � ����
	saveToEC (w, ecps, paramName);
	// ��������� �����
	w.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToEC
*
*	Description:	������� ���������� ���������� ������������� ������� � ����
*	Parameters:		filePath - ������ ���� � ������������� ����� � ����������� 
*					ecps - ��������� ���� ElemCondPlusStruture � ����������� 
*						   ���������� ������������� �������
*					file - �������� ����, � ������� ������������ �������� ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ���� �� ������� ��������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToEC (std::ofstream &file, ElemCondPlusStruture &ecps, const std::string &paramName) const {
	// ��������� ���������� ������ �� ������� � ����
	std::map <std::string, std::string>::iterator iterParams;
	// ����� ���������� � ���������� ������������� �������
	for (iterParams = ecps.ecParams.begin(); iterParams != ecps.ecParams.end(); ++iterParams) {
		// ����������� ����������� ��������� ������������� �������
		if (!(ecps.sign))
			file << "\t" << str_not_symbol << ": \t";
		else
			file << "\t\t\t";
		if (paramName.size() > 0)
			file << "[" << ecps.ecName << ", " << paramName << "]:\t\t";
		else
			file << ecps.ecName << ":\t\t";
		file << iterParams->first << " -> " << iterParams->second << "\n";
	}
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToList
*
*	Description:	������� ������ � ���� ������ � ���������� ������
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ����
*					fileName - ��� �����
*					strList - ��������� � ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToList (const std::string & baseDir, const std::string & fileName, const std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const {
	// ������� ���� � ���������� ���������� � �����
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_list_file_ending);
	// ��������� �����
	std::ofstream file (filePath.c_str());
	// ��������� ���������� ������ �� ������� � ����
	for (unsigned int i = 0; i < strList.size(); i++) {
		for (unsigned int j = 0; j < strList[i].size(); j++) {
			for (unsigned int k = 0; k < strList[i][j].size(); k++) {
				for (unsigned int l = 0; l < strList[i][j][k].size(); l++) {
					file << i+1 << ".\t" << j+1 << ".\t" << k+1 << ".\t" << l+1 << ".\t";
					file << strList[i][j][k][l] << "\n";
				}
				file << "\n";
			}
			file << "\n";
		}
		file << "\n";
	}
	// ��������� �����
	file.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::saveToList
*
*	Description:	������� ������ � ���� ������ � ���������� ������
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ����
*					fileName - ��� �����
*					strList - ��������� � ������������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			-
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomBase::saveToList (const std::string & baseDir, const std::string & fileName, const std::vector <std::vector <std::string> > &strList) const {
	// ������� ���� � ���������� ���������� � �����
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(fileName);
	filePath.append(str_list_file_ending);
	// ��������� �����
	std::ofstream file (filePath.c_str());
	// ��������� ���������� ������ �� ������� � ����
	for (unsigned int i = 0; i < strList.size(); i++) {
		for (unsigned int j = 0; j < strList[i].size(); j++) {
			file << i+1 << ".\t" << j+1 << ".\t";
			file << strList[i][j] << "\n";
		}
		file << "\n";
	}
	// ��������� �����
	file.close();
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromList
*
*	Description:	������ �� ����� �� ������� �������� ������
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ����
*					fileName - ��� �����
*					strList - ���������, � ������� ����������� ������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			-
*	Author:			dk
*	History:
*

****************************************************************************/
signed int AxiomBase::readFromList (const std::string & filePath, std::vector <std::vector <std::string> > &strList) const {
	// ���������� � ����� ����������
	std::string fullFilePath;
	fullFilePath.assign (filePath);
	checkedAddExtension(fullFilePath, str_list_file_ending);
	//fullFilePath.append (str_list_file_ending);
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromList: cannot find file at '" + fullFilePath + "'");
		return 1;
	}
	// ������� ���������� ��������� � ��������� �������
	strList.clear();
	// ��������� ����
	boost::filesystem::ifstream file( fullFilePath );
	// ������ ���������� �����

	std::string curStr, str;
	int i = 0;
	int curLev1, curLev2, lev1, lev2, ret;
	char s[1001]; // ������������ ����������� ����� ����� ����� � ��������� ���������
	// ����������� ��������� ���������� ����� ������������ ������� � �� ���������
	// �������������� ��������� ��� ������ � ��� ����
	strList.resize(1);
	lev1 = 0;
	strList[0].clear();
	lev2 = 0;
	// ������ �� ����� �� ����� ������
	while (i = std::getline(file, str) && i != EOF)  {
		// ��������� ������ �� ������������
		ret = sscanf (str.c_str(), "%d.\t%d.\t%1000s", &curLev1, &curLev2, s);
		// ��������� ������� ��������� ������� ��������
		if (ret < 1) {
			continue;
		}
		if ((ret > 0) && (ret != 3)) {
			std::cout << "\n\tWarning: string '" << str << "' was not correctly recognized.\n";
			continue;
		}
		// ��������� ������������������ ��������� ����� � �����
		if (curLev1 == (lev1 + 2)) {
			strList.resize(lev1 + 2);
			lev1++;
			strList[lev1].clear();
			lev2 = 0;
		}
		if (curLev1 != (lev1+1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev2 != (lev2+1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		lev2++;
		// ��������� �������� ������
		strList[lev1].push_back ((std::string) s);
	}
	// ������� ���������� ���������� ������������ ���������, ��� ��� ��� ����� ���� �����
	if (strList[lev1].empty()) {
		strList.pop_back();
	}
	
	// ��������� ������������ ����
	file.close();	
	return 0;
}


/****************************************************************************
*					AxiomBase::readFromList
*
*	Description:	������ �� ����� �� ������� �������� ������
*	Parameters:		baseDir - ����������, ��� ��������� ������������ ����
*					fileName - ��� �����
*					strList - ���������, � ������� ����������� ������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			-
*	Author:			dk
*	History:
*

****************************************************************************/
signed int AxiomBase::readFromList (const std::string & filePath, std::vector <std::vector <std::vector <std::vector <std::string> > > > &strList) const {
	// ���������� � ����� ����������
	std::string fullFilePath;
	fullFilePath.assign (filePath);
	//fullFilePath.append (str_list_file_ending);
	checkedAddExtension(fullFilePath, str_list_file_ending);
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomBase::readFromList: cannot find file at '"+fullFilePath+"'");
		return 1;
	}
	// ������� ���������� ��������� � ��������� �������
	strList.clear();
	// ��������� ����
	boost::filesystem::ifstream file( fullFilePath );
	// ������ ���������� �����

	std::string curStr, str;
	int i = 0;
	int curLev1, curLev2, curLev3, curLev4, lev1, lev2, lev3, lev4, ret;
	char s[1001]; // ������������ ����������� ����� ����� ����� � ��������� ���������
	// ����������� ��������� ���������� ����� ������������ ������� � �� ���������
	// �������������� ��������� ��� ������ � ��� ����
	strList.resize(1);
	lev1 = 0;
	strList[0].resize(1);
	lev2 = 0;
	strList[0][0].resize(1);
	lev3 = 0;
	strList[0][0][0].clear();
	lev4 = 0;
	// ������ �� ����� �� ����� ������
	while (i = std::getline(file, str) && i != EOF)  {
		// ��������� ������ �� ������������
		ret = sscanf (str.c_str(), "%d.\t%d.\t%d.\t%d.\t%1000s", &curLev1, &curLev2, &curLev3, &curLev4, s);
		// ��������� ������� ��������� ������� ��������
		if (ret < 1) {
			continue;
		}
		if (ret != 5) {
			std::cout << "\n\tWarning: string '" << str << "' was not correctly recognized.\n";
			continue;
		}
		// ��������� ������������������ ��������� ����� � �����
		if (curLev1 == (lev1 + 2)) {
			strList.resize(lev1 + 2);
			lev1++;
			strList[lev1].resize(1);
			lev2 = 0;
			strList[lev1][0].resize(1);
			lev3 = 0;
			strList[lev1][0][0].clear();
			lev4 = 0;
		}
		if (curLev1 != (lev1 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev2 == (lev2 + 2)) {
			strList[lev1].resize(lev2 + 2);
			lev2++;
			strList[lev1][lev2].resize(1);
			lev3 = 0;
			strList[lev1][lev2][0].clear();
			lev4 = 0;
		}
		if (curLev2 != (lev2 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev3 == (lev3 + 2)) {
			strList[lev1][lev2].resize(lev3 + 2);
			lev3++;
			strList[lev1][lev2][lev3].clear();
			lev4 = 0;
		}
		if (curLev3 != (lev3 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		if (curLev4 != (lev4 + 1)) {
			std::cout << "\n\tWarning: file '" << fullFilePath << "' skiped, because it has wrong structure.\n";
			//strList.clear();
			break;
		}
		lev4++;
		// ��������� �������� ������
		strList[lev1][lev2][lev3].push_back ((std::string) s);
	}
	// ������� ���������� ���������� ������������ ���������, ��� ��� ��� ����� ���� �����
	if (strList[lev1][lev2][lev3].empty()) {
		strList[lev1][lev2].pop_back();
	}
	if (strList[lev1][lev2].empty()) {
		strList[lev1].pop_back();
	}
	if (strList[lev1].empty()) {
		strList.pop_back();
	}
	
	// ��������� ������������ ����
	file.close();	
	return 0;
}

bool AxiomBase::checkExtension(const std::string &fileName, const std::string &extension) {
	return boost::algorithm::iends_with(fileName, extension);
}

void AxiomBase::checkedAddExtension(std::string &fileName, const std::string &extension) {
	if(!checkExtension(fileName, extension)) {
		fileName.append(extension);
	}
}
