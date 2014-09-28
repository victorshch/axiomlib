/****************************************************************************
*				AxiomSetBase.cxx
*
*	Description:	������� ������ AxiomSetBase - ����������
*	Author:		dk
*	History:		
*
****************************************************************************/
#include "AxiomSetBase.h"

using namespace AxiomLib;

// �������� ����� ��� ������ ����� � ��������� ������� ������: 
#define str_not_symbol			"not"
#define str_or_symbol			"or"
// ���������� ����� � ��������� ������� ������
#define str_as_file_ending		".as"

// �������� ����� ��� ������ ����� �� �����������: 
#define str_stat_file_ending	".stat"
#define str_axiomSet_keyWord	"axiomSet"
#define str_I_error_keyWord		"firstError"
#define str_II_error_keyWord	"secondError"

// ������������ ����� ������ (���������� ������� �����)
#define maxAddrLen 1000


// ������ �����������
AxiomSetBase::AxiomSetBase (void) {
	axiomSets.clear();
}

/****************************************************************************
*					AxiomSetBase::compareStringAndPath
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
bool AxiomSetBase::compareStringAndPath (std::string fileName, std::string path, const char* n3) const {
	
	   std::string n1("/");
	   const char* n2 = fileName.c_str();
	   n1.append(n2);
	   n1.append(n3);
	   return checkName (path, (int) (path.size() - n1.size() - 1), (int) (path.size()  - n1.size()), n1);

}


/****************************************************************************
*					AxiomSetBase::checkName
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
bool AxiomSetBase::checkName (std::string name1,signed int from,signed int upTo, std::string name2) const {
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
*					AxiomSetBase::readSets
*
*	Description:	������� ������ ������ ������ - ������ ������ - ���� �������� 
*					�� ���������������� ����� ������� environment
*	Parameters:		-
*	Returns:		0
*	Throws:			AxiomLibException  - ���� �� ���������� �������� � ��������� ������,
*						���������� � ��������������� ����
*						���� �������� ���� - ���� � �����, � �� � ��������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readSets (const char* axiomSetBaseDir) {
	std::string axiomSetPath(axiomSetBaseDir);
	boost::filesystem::path fullPath( axiomSetPath );
	boost::filesystem::path filePath;
	
	std::string axiomSetFileName (str_as_file_ending); // ������ ��� ������� ����� ������� ������
	
	std::string aa; // ��������� ����������, �������� ��� �������� ������������ � ������������ ����� 
	
	std::string str;
	int i = 0;
	// �������� - ����� �� ����
	if ( !boost::filesystem::exists( fullPath  ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readSets: cannot find base directory of axiom set .");
		return 1;
	}
	
	// ��������� - ���������� �� ������ ���� - ��� �������������� � ��������� ��������� ����� ������ ������
	if ( boost::filesystem::is_directory( fullPath ) ) {
		// ��� ���������� - ����� ����� �� ������ � ��������� ������� ������
		this->axiomSets.clear();
		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_iter; ++dir_itr ) {
			if ( boost::filesystem::is_directory( *dir_itr ) ) { // ������ ��������� �������� � ������ ������ - ������� ����� ������� - 
												// ��� �� ������������� �������� - ������ ���������� ���� �������				
        		}
			else {
				filePath = fullPath;
				filePath /= dir_itr->path();
				aa = dir_itr->path().filename().string();
				if ( checkName (aa, (int) aa.size() - 4, (int) aa.size() - 3, axiomSetFileName) ) {
					// �� ���� � dir_itr ������ ��� ����� �������� ������� �������
					AxiomSetStructure ass;
					aa = (*dir_itr).path().string();
					this->readFromAS (aa, ass);
					aa = dir_itr->path().filename().string();
					this->axiomSets[aa.substr(0, aa.size() - 3)] = ass;
				}
				else {
					// �� ���� � ���������� ����� ������ ������� ���� � �� ���������� ��� �������� ������ - ��� ������ ����������
				}
			}
		}
	}
	else {// ��������� ���� �� �������� ��������� - ������� ���������� ��������� ����� �� �����������
		throw AxiomLibException ("Error in AxiomSetBase::readSets: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	������� ������ ���������� � ������� ������ �� ������ .as �����
*				������ ����� ������ � .formats.x
*				� ������ ������� ����������� � ������������ � ������� �����,
*				��� ���������� � ������ ������ �����; ����������� � ����������� 
*				��� ������������ ���� ������; 
*				(����� �� ������ ��������� ����������� �������� ".,';:" )
*				����� � ������� ����������� �������� � ������� ������� � ��������� 
*				������������ �������. ��� ��������� �������� ������������ � 
*				���������� ������������ ��������� ���� map (��. AxiomSetStrucutre.h)
*	Parameters:	filePath - ��� ����� ��� ��������� (���� � ����)
*				ass - ����������� �� ����� �������� ���� AxiomSetStrucutre
*	Returns:		0
*	Throws:		AxiomLibException  - ���� ���� � �������� ����� �� ����������
*								���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*								���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:		dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomSetStructure &ass) {
	std::string str;
	int i = 0;
	// ���������� ���������� � �����
	filePath.append (str_as_file_ending);
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( filePath ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readFromAS: cannot find file '"+filePath+"'");
		return 1;
	}
	
	(ass.axioms).resize(0);
	
	// ��������� ����
	boost::filesystem::ifstream file( filePath );

	boost::char_separator<char> sep(" :;	");
	
	if (i = std::getline(file, str) && i != EOF) {
		// ������ ��� ������� ������ � ���������� � ��������� �����
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if (this->compareStringAndPath(*tok_name, filePath, str_as_file_ending) == false) {
			// ��� ������� ���������� - � �������� ����� � � ��� ���������� - ������ ������
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAX: different filenames in the file and in the name of the file : '" 
			                         + std::string(*tok_name) + "', '" + filePath + "'");
		}
		// ���������� ��� �����
		ass.axiomSetName = *tok_name;
	} else {
		// ���� ���� ����  -  �� ������ - ������ ��� ��� ����� ����������
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - it is empty.");
	}
	
	if (i = std::getline(file, str) && i != EOF) {
		// ������ ��� ����� ������ ������������ � ������ �������
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAX: bad axiom bank name.");
		}
		
		ass.axiomBankDir = *tok_name;;
	} else {
		// ������ �� ������ - ��� ����������� ����������
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - it has no axioms.");
	}
	
	// ����������� ��������� ���������� ����� ������ � ��������� ����������� �������
	int currentAxiomNumber = 0;
	if (i = std::getline(file, str) && i != EOF)  {
		// ���������, ������� ����� ��������� � ���������� � ass
		AxiomStructure as;
		(as.axiomParameters).clear();
		// ��������� ��������� ��� �������� ����� ������� � ����� ��������� �������
		std::string currentParam, currentEC, tempStr;
		std::map<std::string, std::string> readParams;
		
		int toSave = 0; // ���� ������������� ��������� ��������� �������
		int condition = 0;
		while (condition == 0) {
			// ��������� ������ �� ������
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			if ( tok_iter == tokens.end()) {// ������ ������  - �������� ����������� ����� ��������� - 
									//� �������� ������ ���� �� ��������� ������ �����
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			//const char* tempNumber = (*tok_iter).c_str();
			tempStr = *tok_iter;
			// ������� - ����������� ����� �������
			if (atof(tempStr.c_str()) > 0) {
				// ������ �������� �������
				if (toSave > 0) {
					if (!currentEC.empty()) {
						as.axiomParameters[currentEC] = readParams;
					}
					// ��������� ����� ���������� ������� 
					(ass.axioms).push_back(as);
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of .as file - cannot find axiom name.");
				}
				currentAxiomNumber++;
				toSave = 1;
				as.axiomName = *tok_iter;
				(as.axiomParameters).clear();
				currentParam = "";
				currentEC = "";
				if (i = std::getline(file, str) && i != EOF) { 
					// ����� �������� ������� ���-�� - ���� - ���� �� ����� � ������� �����
					continue;
				} else { 
					// ����� �������� ������� ������ ��� - ����� 1 �������
					condition = 1;
					break;
				}
			} else {
				// ������� - ������ ������������ �������, ��� ��������� � ��������
				if (*tok_iter == currentEC) {//������ ���������� ���������� ���������� ������� ������������� �������
				} else {
					if (!currentEC.empty()) {
						as.axiomParameters[currentEC] = readParams;
					}
					currentEC = *tok_iter;
					readParams.clear();
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find name of parameter.");
				}
				currentParam = *tok_iter;
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find -> in .as file while setting parameters.");
				}
				if (*tok_iter != "->") {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format - while setting parameters '->' should be.");
				}
				++tok_iter;
				if ( tok_iter == tokens.end()) {
						throw AxiomLibException ("Error in AxiomSetBase::readFromAS: cannot find a value of parameter.");
				}
				readParams[currentParam] = *tok_iter;
				if (i = std::getline(file, str) && i != EOF) {
					// ����� - ���-�� - ���� - ���� �� ����� � ������� �����
					continue;
				} else { 
					condition = 1;
					break;
				}
			}
		}
		// ���������- ���� ���� �� ���������� ��������� ������
		if (toSave > 0) {
			if (!currentEC.empty()) {
				as.axiomParameters[currentEC] = readParams;
			}
			//��������� ����� ��������� ������� 
			(ass.axioms).push_back(as);
		}
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	������� ������ ���������� � ������� ������ �� ������ .as �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������; 
*					(����� �� ������ ��������� ����������� �������� ".,';:" )
*					����� � ������� ����������� �������� � ������� ������� � ��������� 
*					������������ �������. ��� ��������� �������� ������������ � 
*					���������� ������������ ��������� ���� map (��. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aess - ����������� �� ����� �������� ���� AxiomExprSetStrucutre
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomExprSetStructure &aess) const {
	std::vector <std::string> paramNames;
	paramNames.clear();
	return readFromAS (filePath, aess, paramNames);
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	������� ������ ���������� � ������� ������ �� ������ .as �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������; 
*					(����� �� ������ ��������� ����������� �������� ".,';:" )
*					����� � ������� ����������� �������� � ������� ������� � ��������� 
*					������������ �������. ��� ��������� �������� ������������ � 
*					���������� ������������ ��������� ���� map (��. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aess - ����������� �� ����� �������� ���� AxiomExprSetStrucutre
*					paramNames - �������� ����������� ������ ������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (std::string filePath, AxiomExprSetStructure &aess, const std::vector <std::string> &paramNames) const {
	// ���������� ���������� � ���� � ����� � ��������� ��������� ������� ������
	std::string fullFilePath;
	fullFilePath = filePath;
	fullFilePath.append (str_as_file_ending);
	
	// ��������� - ���������� �� ����
	if ( !boost::filesystem::exists( fullFilePath ) ) {
		throw AxiomLibException("Error in AxiomSetBase::readFromAS: cannot find given axiomSet-description file '" + filePath + "'");
	}

	// ������� ���������� ��������� � ��������� ������� ������
	aess.axioms.clear();
	
	// ��������� ����
	boost::filesystem::ifstream file( fullFilePath );

	boost::char_separator<char> sep(" :;\t[]");
	std::string str;
	int i = 0;
	
	// ������ ��� ������� ������ � ���������� � ��������� �����
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if (this->compareStringAndPath(*tok_name, fullFilePath, str_as_file_ending) == false) {
			// ��� ������� ���������� - � �������� ����� � � ��� ���������� - ������ ������
			// ���������������� �.�. ������-�� � ���� �������� �������������, ���� ��� ���������
			// ������, ���� �����������, ��� ��������� ������ compareStringAndPath
//			file.close();
//			throw AxiomLibException ("Error in AxiomSetBase::readFromAS: different file's names in the file and in the name of the file : '"
//			                         +std::string(*tok_name)+"', '"+fullFilePath+"'");
		}
		// ���������� ��� �����
		aess.axiomSetName = *tok_name;
	} else {
		// ���� ���� ����  -  �� ������ - ������ ��� ��� ����� �� ������
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - it is empty.");
	}
	
	// ������ ��� ����� ������ ������������ � ������ �������
	if (i = std::getline(file, str) && i != EOF) {
		token_separator tokens(str, sep);
		token_separator::iterator tok_name = tokens.begin();
		if ((*tok_name).empty()) {
			file.close();
			throw AxiomLibException ("Error in AxiomSetBase::readFromAS: no banks's name in the file.");
		}
		// ���������� ��� ����� ������
		aess.axiomBankDir = *tok_name;;
	} else {
		// ������ �� ������ - ��� ����� ����� ������
		throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - no axiom bank name is given.");
	}
	
	// ����������� ��������� ���������� ����� ������ � ��������� ����������� �������
	readFromAS (file, aess, paramNames);

	// ��������� - ��� �� ���� ������� �� �����
	if (i = std::getline(file, str) && i != EOF) {
		throw AxiomLibException ("Error in AxiomBase::readFromAS: wrong fowmat - there is something wrong at the end og the file.");
	}
	
	file.close();
	
	return 0;
}


/****************************************************************************
*					AxiomSetBase::readFromAS
*
*	Description:	������� ������ ���������� � ������� ������ �� ������ .as �����
*					������ ����� ������ � .formats.x
*					� ������ ������� ����������� � ������������ � ������� �����,
*					��� ���������� � ������ ������ �����; ����������� � ����������� 
*					��� ������������ ���� ������; 
*					(����� �� ������ ��������� ����������� �������� ".,';:" )
*					����� � ������� ����������� �������� � ������� ������� � ��������� 
*					������������ �������. ��� ��������� �������� ������������ � 
*					���������� ������������ ��������� ���� map (��. AxiomExprSetStrucutre.h)
*	Parameters:		filePath - ��� ����� ��� ��������� (���� � ����)
*					aess - ����������� �� ����� �������� ���� AxiomExprSetStrucutre
*					paramNames - �������� ����������� ������ ������
*					file - �������� ���� �� �������� ����� ������������� ���������� ������� ������
*	Returns:		0
*	Throws:			AxiomLibException  - ���� ���� � �������� ����� �� ����������
*							���� ������ ����� �� ���������� ��������� ������� (��. formats.x)
*							���� ��� ����� �� ����������� � ������ � ������ ������ �����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::readFromAS (boost::filesystem::ifstream &file, AxiomExprSetStructure &aess, const std::vector <std::string> &paramNames) const {
	boost::char_separator<char> sep(" :;\t[]");
	std::string str, tempStr;
	int i = 0;
	
	// ����������� ��������� ���������� ����� ������ � ��������� ����������� �������
	AxiomExprStructure aes;
	AxiomBase axiomBase;
	int condition;

	std::streamoff pos = file.tellg();
	if (i = std::getline(file, str) && i != EOF)  {
		// ���������, ������� ����� ��������� � ���������� � aess
		aes.clear();
		// ������� ������ �� ����� ��������� ������
		condition = 0;
		while (condition == 0) {
			// ��������� ������ �� ������
			token_separator tokens(str, sep);
			token_separator::iterator tok_iter = tokens.begin();
			// ������� - ������ ������ - �������� ����������� ����� ��������� - � �������� ������ ���� �� ��������� ������ �����
			if ( tok_iter == tokens.end()) {
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) {
					continue;
				} else {
					condition = 1;
					break;
				}
			}
			
			// ������� - ����������� ����� �������
			tempStr = *tok_iter;
			if (atof(tempStr.c_str()) > 0) {
				++tok_iter;
				if ( tok_iter == tokens.end()) {
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - cannot find axiom name.");
				}
				if (!aes.axiomName.empty())
					throw AxiomLibException ("Error in AxiomSetBase::readFromAS: wrong format of axiomSet-description file - multiple names per single axiom.");
				aes.axiomName = *tok_iter;
				pos = file.tellg();
				if (i = std::getline(file, str) && i != EOF) { 
					// ����� �������� ������� ���-�� - ���� - ���� �� ����� � ������� �����
					continue;
				} else { 
					// ����� �������� ������� ������ ���
					condition = 1;
					break;
				}
			} 
			// ������� - ������ �������� �������
			file.seekg(pos);
			axiomBase.readFromAX (file, aes, paramNames);
			aess.axioms.push_back (aes);
			aes.clear();
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
	}

	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	������� ���������� ������� ������ � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������� ������
*					AxiomSetName - ��� ������� ������
*					ass - ��������� � ��������� ������� �������, ���� AxiomSetStructure
*					first - ����� ������ I ���� ������ ������� ������ �� �������� �������
*					second - ����� ������ II ���� ������ ������� ������ �� �������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomSetStructure &ass, int first, int second) const {
	// �������� ���� � ������
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(ass.axiomSetName);
	statFile = filePath;
	filePath.append(str_as_file_ending);
	statFile.append(str_stat_file_ending);
	// ��������� �����
	std::ofstream w (filePath.c_str());
	// ��������� ���������� ������ � ������� ������ � ����
	// ���������� ��� ������� ������
	w << ass.axiomSetName << "\n";
	// ���������� ��� ���� ������ ������
	w << ass.axiomBankDir << "\n";
	// ���������� �������
	std::map < std::string, std::map <std::string, std::string> > ecParams;
	std::map < std::string, std::map <std::string, std::string> > :: iterator iterEC;
	std::map <std::string, std::string> params;
	std::map <std::string, std::string> :: iterator iterParams;
	for (unsigned int i = 0; i < ass.axioms.size(); i++) {
		// ����� ���������� � ������ � ����� �������
		w << "\n";
		w << i+1 << ": " << ass.axioms[i].axiomName << "\n";
		// ����� ���������� � ���������� ������������ �������
		for (iterEC = ass.axioms[i].axiomParameters.begin(); iterEC != ass.axioms[i].axiomParameters.end(); ++iterEC) {
			params = iterEC->second;
			for (iterParams = params.begin(); iterParams != params.end(); ++iterParams) {
				w << "	" << iterEC->first << ":		";
				w << iterParams->first << " -> " << iterParams->second << "\n";
			}
		}
	}
	// ��������� �����
	w.close();
	// ���������� ���� �� �����������, ���� ��� ����������
	if ((first < 0) && (second < 0))
		return 0;
	std::ofstream st (statFile.c_str());
	// ��������� ���������� ������ � ���������� �� ������� ������ � ����
	st << str_axiomSet_keyWord << ":\t" << str_I_error_keyWord << ":\t'" << first << "'\n";
	st << str_axiomSet_keyWord << ":\t" << str_II_error_keyWord << ":\t'" << second << "'\n";
	// ��������� �����
	st.close();
	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	������� ���������� ������� ������ � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������� ������
*					AxiomSetName - ��� ������� ������
*					aess - ��������� � ��������� ������� �������, ���� AxiomExprSetStructure
*					first - ����� ������ I ���� ������ ������� ������ �� �������� �������
*					second - ����� ������ II ���� ������ ������� ������ �� �������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomExprSetStructure &aess, int first, int second) const {
	std::vector <std::string> paramNames;
	paramNames.clear();
	return saveToAS (baseDir, aess, paramNames, first, second);
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	������� ���������� ������� ������ � ����
*	Parameters:		baseDir - ����������, ��� ��������� ������������� ���� �������� ������� ������
*					AxiomSetName - ��� ������� ������
*					aess - ��������� � ��������� ������� �������, ���� AxiomExprSetStructure
*					paramNames - �������� ����������� ������ ������
*					first - ����� ������ I ���� ������ ������� ������ �� �������� �������
*					second - ����� ������ II ���� ������ ������� ������ �� �������� �������
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::string baseDir, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames, int first, int second) const {
	std::string statFile;
	std::string filePath = baseDir;
	filePath.append("/");
	filePath.append(aess.axiomSetName);
	statFile = filePath;
	filePath.append(str_as_file_ending);
	statFile.append(str_stat_file_ending);
	// ��������� �����
	std::ofstream w (filePath.c_str());
	// ��������� ���������� ������ � ������� ������ � ����
	// ���������� ��� ������� ������
	w << aess.axiomSetName << "\n";
	// ���������� ��� ���� ������ ������
	w << aess.axiomBankDir << "\n";
	// ����� �������� ������� ������
	saveToAS (w, aess, paramNames);
	// ��������� �����
	w.close();
	// ���������� ���� �� �����������, ���� ��� ����������
	if ((first < 0) && (second < 0))
		return 0;
	std::ofstream st (statFile.c_str());
	// ��������� ���������� ������ � ���������� �� ������� ������ � ����
	st << str_axiomSet_keyWord << ":\t" << str_I_error_keyWord << ":\t'" << first << "'\n";
	st << str_axiomSet_keyWord << ":\t" << str_II_error_keyWord << ":\t'" << second << "'\n";
	// ��������� �����
	st.close();
	return 0;
}


/****************************************************************************
*					AxiomSetBase::saveToAS
*
*	Description:	������� ���������� ������� ������ � ����
*	Parameters:		file - �������� ����, � ������� ������������ �������� ������� ������
*					paramNames - �������� ����������� ������ ������
*					aess - ��������� � ��������� ������� �������, ���� AxiomExprSetStructure
*	Returns:		0 - ���� ��� ���������, � ��������� ������ ����� �� throw
*	Throws:			AxiomLibExeption - ���� ����� �� �������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int AxiomSetBase::saveToAS (std::ofstream &file, AxiomExprSetStructure &aess, std::vector <std::string> &paramNames) const {
	AxiomBase axiomBase;
	// ���������� �������
	std::string curECName;
	std::map <std::string, std::string>::iterator iterParams;
	for (unsigned int i = 0; i < aess.axioms.size(); i++) {
		// ����� ���������� � ������ � ����� �������
		file << "\n" << i+1 << ": " << aess.axioms[i].axiomName << "\n";
		// ����� �������� ����� �������
		axiomBase.saveToAX(file, aess.axioms[i], paramNames);
	}
	return 0;
}
