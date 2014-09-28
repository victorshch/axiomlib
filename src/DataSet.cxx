/****************************************************************************
*			������� ������ DataSet
*
*	Description:	����� ������ � DataSet - ������ � ������ ������� ������
*	Author:		dk
*	History:	
*
****************************************************************************/

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

#include <set>

#include "Common.h"
#include "Logger.h"

#include "DataSet.h"

using namespace AxiomLib;
using namespace std;

/****************************************************************************
*				DataSet::DataSet
*
*	Description:	������ ����������� � �������������� ���������� ������
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
DataSet::DataSet (void) : DataSetBase() {

}

/****************************************************************************
*				DataSet::ReadDataSet
*
*	Description:	������� ������ ���� ������ �� ������ ������, ������������
*				� ��������� ��������
*	Parameters:	string baseDataSetDir - ���� � ��������, ����������� ����� ������
*	Returns:		0
*	Throws:		AxiomLibExeption - ���� ������� �����������, ��� ���� ������
*				� ������� ������ ������
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readDataSet(const std::string &baseDataSetDir) {
	
	// 0. ��������� ������������ ����, ������ �������� ������� � ����������
	// 1. ������ ��������� ����
	//    ����, ����� �� ���� ��������� � $path/reference/. ��� ��������
	//    ������ �����, � �������� ��������� ���� �� ����� ��������.
	//        ��� ������� ��������, ����� �� ���� ������� � ��� .csv-������,
	//        ������ ��, �� ������� ��������� � ������������� ����.
	// 2. ������ �������� �������
	//    ����, ����� �� ���� ������ xxx.csv, ���� ��� ��� ������ ����� xxx-ref.csv,
	//    ������ ��� ����.
	
	
	// 0
	DataSetBase::readDataSet(baseDataSetDir);
	
  //1. ������ ��������� ����������
	
	this->readClassesTS(baseDataSetDir);
	// �������� �� ������������ ����������:
	// - ��� ������� ������ �������������� ��������� ����� �� ���� �� ���� ��������� ���
	// - ��������� ��� �� ������ �������������� ������
	int countMultiTS = 0;
	for (unsigned int h = 0; h < this->referenceClassesTS.size(); h++) {
		if ((this->referenceClassesTS[h]).size() == 0) {
			countMultiTS++;
		}
	}
	if ((this->referenceClassesTS.size() != this->classNames.size()) || (countMultiTS > 0)) {
		//throw AxiomLibException ("Error in DataSetBase::readDataSet: cannot find all Classes TSs in dataset directory.");
	}
	
  //2. ������ �������� ���������� � ���������� ��������
	
	this->readTestesTS (baseDataSetDir);
	// �������� �� ������������ ����������:
	// - ��� ������� �� �������� ����� ������� �������� � ��� ������� �� ����� � ��������� ���� ��������� ���
	int length1 = 0;
	int length2 = 0;
	for (unsigned int g = 0; g < this->testSets.size(); g++) {
		length1 = 0;
		length2 = 0;
		//std::cout << "\n Num of rows is " << (((this->testSets[g]).data).data).size() << "\n";
		for (unsigned int j = 0; j < (((this->testSets[g]).data).data).size(); j++) {
			length1 = (int) (((this->testSets[g]).data).data[j]).size();
			if (length1 > 0){ 
				//std::cout<< "\n break \n";
				break;
			}
		}
		int validParamsSize = (int)testSets[g].correct.validParams.size();
		length2 = (int) (((this->testSets[g]).correct).data[((this->testSets[g]).correct).validParams.size() - 1]).size();
		//std::cout << "\n len1 is " << length1 << "\n";
		//std::cout << "\n len2 is " << length2 << "\n";
		//std::cout << "\n";
		Logger::getInstance()->writeDebug("validParamsSize = "+boost::lexical_cast<std::string>(validParamsSize));
		if (length1 != length2) {
			std::cout << "\n len1 is " << length1 << "\n";
			std::cout << "\n len2 is " << length2 << "\n";
			throw AxiomLibException ("Error in DataSet::readDataSet: test length doesn't match test ref length.");
		}
	}
}

/****************************************************************************
*				DataSet::ReadDataSet
*
*	Description:	������� ������ ���� ������ �� ������ ������, ������������
*				� ��������� ��������
*	Parameters:	dataSetDir - ���������� ��������, dataSetName - ��� ��������
*
*	Returns:		0
*	Throws:		-
*	Author:		wictor
*	History:
*
****************************************************************************/
signed int DataSet::readDataSet (const std::string &dataSetDir, const std::string &dataSetName)
{
	return readDataSet(dataSetDir + "/" + dataSetName);
}

/****************************************************************************
*				DataSet::getTSByIndexFromClass
*
*	Description:	������� ���������� ��������� ��� �� ���������� ������ 
*				�� ������������ - ������ ������, ������ ���������� � 
*				������, ������ ���� � ����������. 
*				������� ���������� ��� ��������� ���� �� ���������� ������.
*				������������ ��� �������� ���� � ����� ��������.
*	Parameters:	vec - ����������� ��������� ���
*				indexClass - ����� ������ �������������� �� ���������� 
*						������������� ������� ������, ������ ��������� ���
*				indexMultiTS - ����� ���������� ������ ��������� ��������� ���
*				indexTS - ����� ���������� ���� � ����������
*	Returns:		true - ���� ��� ������� ������
*				false - ���� ��� �� �����-�� �������� �� ������ (�.�. ���� ��� ���)
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) const {
	if ((indexClass < (int) referenceClassesTS.size()) && (indexClass >=0)) {
		if ((indexMultiTS < (int) (referenceClassesTS[indexClass]).size()) && (indexMultiTS >=0)) {
			return (referenceClassesTS[indexClass][indexMultiTS]).getTSByIndex (vec, indexTS);
		}
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromClass
*
*	Description:	������� ������������� ����� ��������� ��� ��
*					������������ - ������ ������, ������ ���������� � 
*					������, ������ ���� � ����������. 
*					������� ���������� ��� ������ ����������� �������������.
*	Parameters:		vec - ����� ��������� ���
*					indexClass - ����� ������ �������������� �� ���������� 
*						������������� ������� ������
*					indexMultiTS - ����� ����������
*					indexTS - ����� ���� � ����������
*	Returns:		true - ���� ��� ������� ����������
*					false - ���� ��� �� �����-�� �������� �� ����������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromClass (std::vector<double> &vec, int indexClass, int indexMultiTS, int indexTS) {
	if ((indexClass < (int) referenceClassesTS.size()) && (indexClass >=0)) {
		if ((indexMultiTS < (int) (referenceClassesTS[indexClass]).size()) && (indexMultiTS >=0)) {
			return (referenceClassesTS[indexClass][indexMultiTS]).setTSByIndex (vec, indexTS);
		}
	}
	return false;
}

/****************************************************************************
*				DataSet::getNumberOfClasses
*
*	Description:	������� ���������� ����� ������� ���������� ���������
*	Parameters:	-
*	Returns:	����� ������� ���������� ���������
*	Throws:		-
*	Author:		wictor
*	History:		-
*
****************************************************************************/
int DataSet::getNumberOfClasses() const {
	return (int) referenceClassesTS.size();
}

int DataSet::getNumberOfTests() const
{
	return (int) testSets.size();
}

 
/****************************************************************************
*				DataSet::getClassSize
*
*	Description:	������� ������ ���������� � ����� ��������� ����� 
*				�� ���������� ������������� ������� ������
*	Parameters:	numOfClasses - ����������� ����� ������� ��������������
*				numOfMultiTS - ������ ����� ����������� � ������ �� �������
*				numOfTS - ������ �������� ����� ����� � �������
*	Returns:		true - ���� ���������� ���� ������� ��������
*				false - ���� ���������� �������� �� �������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getClassSize (int &numOfClasses, std::vector<int> &numOfMultiTS, std::vector<std::vector<int> > &numOfTS) const {
	if (referenceClassesTS.size() > 0) {
		numOfClasses = (int) referenceClassesTS.size();
		numOfMultiTS.resize(numOfClasses);
		numOfTS.resize(numOfClasses);
		for (int i = 0; i < numOfClasses; i++) {
			numOfMultiTS[i] = (int) (referenceClassesTS[i]).size();
			(numOfTS[i]).resize(numOfMultiTS[i]);
			for (int j = 0; j < numOfMultiTS[i]; j++) {
				numOfTS[i][j] = (referenceClassesTS[i][j]).size();
			}
		}
		return true;
	}
	return false;
}


/****************************************************************************
*				DataSet::getTestSize
*
*	Description:	������� ���������� ���������� � ����� ��������
*				������� � ������ ���������� ������
*	Parameters:	numOfTests - ����������� ����� �������� �������
*				numOfTS - ����������� ������ ����� ��������� ����� � 
*						�������� ������
*	Returns:		true - ���� �������� ���������� �������
*				false - ���� ������ ���������� �� �������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTestSize (int &numOfTests, std::vector<int> &numOfTS) const {
	if (testSets.size() > 0) {
		numOfTests = (int) testSets.size();
		numOfTS.resize(numOfTests);
		for (int i = 0; i < numOfTests; i++) {
			numOfTS[i] = (testSets[i]).size();
		}
		return true;
	}
	return false;
}


/****************************************************************************
*				DataSet::getTSByIndexFromTests
*
*	Description:	������� ���������� ��������� ��� �� ������ �����, 
*				�� ������ ���� � ���������� � �����
*	Parameters:	vec - ����������� ��������� ���
*				indexTest - ���������� ����� ��������� ������ � ������ ������
*				indexTS - ���������� ����� ���������� ���� � �������� ������
*						(�����: ���� ������ ��� �������� ������ '-1' - �� �����
*						������� ��� ����������� ��������)
*	Returns:		true - ���� �������� ���������� �������
*				false - ���� �������� ���������� �� �������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS) const {
	
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).getTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromTests
*
*	Description:	������� ������������� ����� ��������� ��� �� ������ �����, 
*					�� ������ ���� � ���������� � �����
*	Parameters:		vec - ����� ��������� ���
*					indexTest - ���������� ����� ��������� ������ � ������ ������
*					indexTS - ���������� ����� ���������� ���� � �������� ������
*						(�����: ���� ������ ��� �������� ������ '-1' - �� �����
*						������� ��� ����������� ��������)
*	Returns:		true - ���� �������� ����� �������� �������
*					false - ���� �������� �� �������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromTests (std::vector<double> &vec, int indexTest, int indexTS) {
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).setTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::getTSByIndexFromTests
*
*	Description:	������� ���������� ��������� ��� �� ������ �����, 
*				�� ������ ���� � ���������� � �����
*	Parameters:	vec - ����������� ��������� ���
*				indexTest - ���������� ����� ��������� ������ � ������ ������
*				indexTS - ���������� ����� ���������� ���� � �������� ������
*						(�����: ���� ������ ��� �������� ������ '-1' - �� �����
*						������� ��� ����������� ��������)
*	Returns:		true - ���� �������� ���������� �������
*				false - ���� �������� ���������� �� �������
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
bool DataSet::getTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS) const {
	
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).getTSByIndex(vec, indexTS);
	}
	return false;
}


/****************************************************************************
*				DataSet::setTSByIndexFromTests
*
*	Description:	������� ������������� ����� ��� �� ������ �����, 
*					�� ������ ���� � ���������� � �����
*	Parameters:		vec - ����� ��������� ���
*					indexTest - ���������� ����� ��������� ������ � ������ ������
*					indexTS - ���������� ����� ���������� ���� � �������� ������
*						(�����: ���� ������ ��� �������� ������ '-1' - �� �����
*						������� ��� ����������� ��������)
*	Returns:		true - ���� �������� ����� �������� ���� �������
*					false - ���� �������� �� �������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool DataSet::setTSByIndexFromTests (std::vector<int> &vec, int indexTest, int indexTS) {
	if ((indexTest < (int) testSets.size()) && (indexTest >=0)) {
		return (testSets[indexTest]).setTSByIndex(vec, indexTS);
	}
	return false;
}

/****************************************************************************
*				DataSet::readClassTS
*
*	Description:	������ ��������� ���� ��� ������� �� ������� ��������������
*				����, ����� �� ���� ��������� � $path/reference/. 
*				��� �������� ������ �����, � �������� ��������� ���� �� 
*				����� ��������. ��� ������� ��������, ����� �� ���� ������� � ��� 
*				.csv-������, ������ ��, �� ������� ��������� � ������������� ����.
*	Parameters:	path - ������ ���������� ����� �������� ���������� ������ ������
*	Returns:		0 - ���� ��� ���� �������� ��������� (����� throw)
*	Throws:		AxiomLibException  - ���� ���������� $path/reference/ - �� ���������� 
*								��� ���� ����� ����������, �� ��� ����
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readClassesTS (const std::string &path) {	
	std::string nPath = path;
	nPath.append ("/");
	nPath.append ("reference");
	return readReferenceTS (nPath, referenceClassesTS);
}

/****************************************************************************
*				DataSet::readTestesTS
*
*	Description:	������� ������ �������� �������.
*				����, ����� �� ���� ������ .csv ��������� �� ��������� 
*				�������� $path/testing, ��������� �� � ������ 
*				������ TestTS - ������ ������� ����� ��������� ����
*				�� ���� � ���� ����� ����������� ������ �� ���� ������ 
*				� ����������: runX.cvs � runX-ref.csv. ��� ���� � ������ 
*				����� ��������� �������� ��������� ����, � �� ������ -
*				���������� �������� ����������� �����.(�� ���� �������� 
*				��� ����������� ��������� ��������� � � ������ ������ ���
*				���������)
*	Parameters:	path - ������ ���������� ��� ��������� �������� ������ ������
*	Returns:		0
*	Throws:		AxiomLibException  - ���� �� ���������� �������� $path/testing (�
*								������� �� ��������� �������� �������� �������)
*								���� ���� ����� ���� ����������, �� ��� ����
*	Author:		dk
*	History:
*
****************************************************************************/
signed int DataSet::readTestesTS (const std::string &path) {
	// ������� ���� � ���������� �� ��������� ���������� �������� ����
	boost::filesystem::path fullPath( path );
	fullPath /= "./testing";
	boost::filesystem::path filePath( fullPath ); // ����� �������������� ����� ������ �� ������������� ����������
	// ������� ������� ��� ���� ������ ������� ������ ������������� ����� ������ � ��������� ���������� ������
	std::string fileNameFirstHalf ("run");
	std::string fileNameLastHalf1 ("-ref.csv");
	std::string fileNameLastHalf2 (".csv");
	std::string aa; // � ������ ���������� ����� ��������� ����� �������������� � ��������� ������ ������
	std::vector<int> readSeq; // ������� ��������� ������ � ������� ������� ���������� � ���, ����� � ������ �������� �������� � ������ �� ������
						  // ���� ������ ��������� ��� ������������ - � ������� ���� runX.csv ��� �������, � runX-ref.csv - �� �� �������� ������� 
						  // � ��������� ����� ����� - �� ��� ���� �� ��������� ��������� ����� ��������� TestTS - � ������������ ��� ��������� 
						  // ��� runX.csv
	int isin = -1; // ���������� ��� �������� ����������� ������ ������� isIn
	
	// ��������� ���������� �� �������� ���� - �� ���� $path/testing
	if ( !boost::filesystem::exists( fullPath ) ) {
		throw AxiomLibException ("Error in DataSet::readTestesTS: wrong path - path do not exists.");
		return 1;
	}
	
	// ���� ����� ��� ��� �������� - �� ��� ����� ������ �������� ������ ��������� ��� ������
	this->testSets.resize(0);
	
	// ��������� ���������� �� ���� $path/testing
	if ( boost::filesystem::is_directory( fullPath ) ) {
		int intByName = -1; // ���������� ��� �������� ����� ������������� �� ����� �������� ��������������� �����
		// ��������� ���� �� ������ �������� $path/testing
        	boost::filesystem::directory_iterator end_itr;
		for ( boost::filesystem::directory_iterator dir_itr( fullPath ); dir_itr != end_itr; ++dir_itr ) {
			filePath = fullPath;
			filePath /= dir_itr->path();
			// ��������� ��� �������� ��������������� ����� � ������
			aa = dir_itr->path().filename().string();
			// ���������� ����� �� ����� �����
			intByName = this->getIntByName(aa);
			// ��������� - �� ����������� ����� - ������������� �� ��� ����� ������� ���� �������� �����
			if (intByName >= 0) {
				// ��������� ������ ���� ������ ���� - � ���������� ������ ��� � ��������� ��������� ���������� �� ��������� ����
				if ( (checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 9, (int) aa.size() - 8, fileNameLastHalf1)) ) {
					// ������ ��� ref - ����
					// ������ ���� � ���������� ������
					MultiTS correctTS;
					aa = (*dir_itr).path().string();
					this->readFromRefCSV (aa, correctTS);
					Logger::getInstance()->writeDebug("Read ref csv length: " +
													  boost::lexical_cast<std::string>(correctTS.length()));
					// ��������� ��������� �� ���� ��� ������� ����� (��. �������� ������ �������)
					isin = isIn (intByName, readSeq);
					if (isin >= 0) {
						Logger::getInstance()->writeDebug("isin >= 0");
						// ���� ������� - ������ ��������� ����� ������
						(testSets[isin]).correct = correctTS;
						// ����� ������� ��� ����������� ���������� �������� � ������ ����� �����
						(testSets[isin]).makeIntClone();
					}
					else {
						Logger::getInstance()->writeDebug("isin < 0");
						// ���� �� �������  - ������� ����� ��������� ������ TestTS
						TestTS testTS;
						testTS.correct = correctTS;
						// ����� ������� ��� ����������� ���������� �������� � ������ ����� �����
						testTS.makeIntClone();
						this->testSets.push_back(testTS);
						readSeq.push_back(intByName);
					}
				}
				else {
					// ��������� ������ ���� �������������� ����
					if ( (checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 5, (int) aa.size() - 4, fileNameLastHalf2)) ) {
						// ������ ��� ������ cvs - ���� � ��������� ��������� ����
						// ���������� ������ ������� �����
						MultiTS multiTStemp;
						aa = (*dir_itr).path().string();
						this->readFromCSV (aa, multiTStemp);
						Logger::getInstance()->writeDebug("Read test csv length: " +
														  boost::lexical_cast<std::string>(multiTStemp.length()));
						// ���������� ������� �� ���� ��� ������� �����
						isin = isIn (intByName, readSeq);
						if (isin >= 0) {
							// ���� ������� - ������ ��������� ����� ������ � ��� ���������� ���������� ������ TestTS
							(testSets[isin]).data = multiTStemp;
						}
						else {
							// ���� �� ������� - ������� ����� ��������� ������ TestTS
							TestTS testTS;
							testTS.data = multiTStemp;
							this->testSets.push_back(testTS);
							readSeq.push_back(intByName);
						}
					}
					else {
						//���� �� ������������� �� ������ ������� - ��� ������ �� ������������ 
						Logger::getInstance()->writeDebug("Warning: ignoring data set file "
														  +aa);
					}
				}
			}
		}
	}
	else {// �� ���������� ���� $path/testing - ���������� �� �������, � ���� - 
		  //������������� ���������� ������ �������� ������ �� ������������� ��������� (������ ./formats.x)
		throw AxiomLibException ("Error in DataSet::readClassRows: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}

