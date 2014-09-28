/****************************************************************************
*					������� ������ FuzzyDataSet
*
*	Description:	����� FuzzyDataSet - ����������
*	Author:			dk
*	History:	
*
****************************************************************************/

#include "FuzzyDataSet_old.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"

using namespace AxiomLib;

/****************************************************************************
*				FuzzyDataSet::FuzzyDataSet
*
*	Description:	������ ����������� � �������������� ���������� ������
*	Parameters:	-
*	Returns:		-
*	Throws:		-
*	Author:		dk
*	History:		-
*
****************************************************************************/
FuzzyDataSet_old::FuzzyDataSet_old (void) {
	// ������� ���������� ���������� ������ �� ���������
	std::string nr ("normal");
	this->normal = nr;
}


/****************************************************************************
*					FuzzyDataSet::setNormalStr
*
*	Description:	������� ������������� ����� ����������� ��� ������ �
*					�������� � ��������� ������� ��������� ���������� ����������� 
*					���������
*	Parameters:		env - ����� � ������� �� ����������������� �����
*	Returns:		0
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
signed int FuzzyDataSet_old::setNormalStr (EnvDataSet &envDataSet) {
	int ret;
	std::string normalStr;
	ret = envDataSet.getNormalStr(normalStr);
	if (ret == 0) {
		normal = normalStr;
	}
	return 0;
}


/****************************************************************************
*				FuzzyDataSet::getNormalTS
*
*	Description:	������� ���������� ��������� ���, ��������������� ����������� 
*					���������, �� ���������� ������ �� ������������ - ������ 
*					����������, ������ ���� � ����������.
*	Parameters:		vec - ����������� ��������� ���
*					indexMultiTS - ����� ���������� ������ ��������� ��������� ���
*					indexTS - ����� ���������� ���� � ����������
*	Returns:		true - ���� ��� ������� ������
*					false - ���� ��� �� �����-�� �������� �� ������ (�.�. ���� ��� ���)
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet_old::getNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS) const {
	if ((indexMultiTS < (int) (normalTS.size())) && (indexMultiTS >=0)) {
		return (normalTS[indexMultiTS]).getTSByIndex (vec, indexTS);
	}
	return false;
}


/****************************************************************************
*					FuzzyDataSet::setNormalTS
*
*	Description:	������� ������������� ����� �������� ���� ��������� �������, 
*					��������������� ����������� ���������
*	Parameters:		vec - ����� ��������� ���
*					indexMultiTS - ����� ����������
*					indexTS - ����� ���� � ����������
*	Returns:		true - ���� ��� ������� ����������
*					false - ���� ��� �� �����-�� �������� �� ����������
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet_old::setNormalTS (std::vector<double> &vec, int indexMultiTS, int indexTS) {
	if ((indexMultiTS < (int) (normalTS.size())) && (indexMultiTS >=0)) {
		return (normalTS[indexMultiTS]).setTSByIndex (vec, indexTS);
	}
	return false;
}

 
/****************************************************************************
*					FuzzyDataSet::getNormalClassSize
*
*	Description:	������� ������ ���������� � ����� ��������� ����� 
*					�������������� ����������� ���������
*	Parameters:		numOfMultiTS - ����� �����������
*					numOfTS - ������ ����� ����� � �����������
*	Returns:		true
*	Throws:			-
*	Author:			dk
*	History:		-
*
****************************************************************************/
bool FuzzyDataSet_old::getNormalClassSize (int &numOfMultiTS, std::vector<int> &numOfTS) const {
	numOfMultiTS = normalTS.size();
	numOfTS.resize (numOfMultiTS);
	for (int i = 0; i < numOfMultiTS; i++) {
		numOfTS[i] = (int) normalTS[i].size();
	}
	return true;
}


/****************************************************************************
*				FuzzyDataSet::readNormalClassTS
*
*	Description:	������ ��������� ���� ��� ������ �������������, �� ������� 
*					���������� ���������� ��������� �������.
*					����, ����� �� ���� ��������� � $path/reference/. 
*					��� ������� ��������, ����� �� ���� ������� � ��� 
*					.csv-������.
*	Parameters:		path - ������ ���������� ����� �������� ���������� ������ ������
*	Returns:		0 - ���� ��� ���� �������� ��������� (����� throw)
*	Throws:			AxiomLibException  - ���� ���������� $path/reference/ - �� ���������� 
*										��� ���� ����� ����������, �� ��� ����
*	Author:			dk
*	History:
*
****************************************************************************/
signed int FuzzyDataSet_old::readNormalClassTS (std::string &path) {
	if (normal.size() < 1)
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: string for normal class is not set.");

	// ������� ����� ��� ������ � ������������ � ������� 
	std::string nPath = path;
	nPath.append ("/reference/");
	nPath.append (normal);
	boost::filesystem::path classPath( nPath ); // ��������� �� ���������� � ������������ ����������� ���������
	boost::filesystem::path filePath( nPath ); // ����� �������������� ����� ������ �� ������������� ���������� � ������ ������ ������������� "normal"
	
	// ������ - �������� ������ ������������ ����� ������ � ���������� ������ ��� ������� ��������������
	std::string fileNameFirstHalf ("ref");
	std::string fileNameLastHalf (".csv");
	std::string aa; // ��������� ����������, �������� ��� �������� ������������ � ������������ ����� 
	
	// �������� - ���������� �� �������� ���� � �����
	if ( !boost::filesystem::exists( classPath ) )
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: path for normal behavior does not exists.");
	
	// ��������� - ���������� �� ������ ���� - ��� �������������� � ��������� ��������� ������ ������
	if ( boost::filesystem::is_directory( classPath ) ) {
		// ���� �� ������ ��������� ����������
		boost::filesystem::directory_iterator end_iter_int;
		for ( boost::filesystem::directory_iterator dir_itr_int( classPath ); dir_itr_int != end_iter_int; ++dir_itr_int ) {
			filePath = *dir_itr_int;
			// ��������� �������� ����� � ������� � char* - ����� �������� �� ������������ (�� ./formats.x)
			aa = dir_itr_int->path().filename().string();
			if ((checkName (aa, -1, 0, fileNameFirstHalf)) && (checkName (aa, (int) aa.size() - 5, (int) aa.size() - 4, fileNameLastHalf))) {
				//�������� ���� ������� ���� � �������� � ���������� ��������� ����� multiTS
				MultiTS multiTStemp;
				aa = (*dir_itr_int).path().string().c_str();
				this->readFromCSV (aa, multiTStemp);
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
				// ��������� ��������� multiTStemp � ���������� ������
				normalTS.push_back(multiTStemp);
			}
		}
	
	} else {// ��������� ���� �� �������� ��������� - ������� ���������� ��������� ����� �� �����������
		throw AxiomLibException ("Error in FuzzyDataSet::readNormalClassTS: wrong path - path is not a dir, it is a file.");
	}
	
	return 0;
}


/****************************************************************************
*					FuzzyDataSet::ReadDataSet
*
*	Description:	������� ������ ���� ������ �� ������ ������, ������������
*					� ��������� ��������
*	Parameters:		const char* path - ���� � ��������, ����������� ����� ������
*	Returns:		0
*	Throws:			AxiomLibExeption - ���� ������� �����������, ��� ���� ������
*					� ������� ������ ������
*	Author:			dk
*	History:
*
****************************************************************************/
signed int FuzzyDataSet_old::readDataSet (std::string dataSetDir, std::string dataSetName) {
	// ������ �������� ������ ������ ����������
	DataSet::readDataSet (dataSetDir, dataSetName);
	// ������ ����������, ����������� � ����������� ���������
	std::string baseDataSetDir = dataSetDir;
	baseDataSetDir.append("/");
	baseDataSetDir.append(dataSetName);
	readNormalClassTS (baseDataSetDir);
	return 0;
}
