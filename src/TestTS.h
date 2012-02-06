/****************************************************************************
*						TestTS
*
*	Description:	������ ����� ������ ��� ������� �������� �������� ����������
*				�� ���� � ��� ������������ ���� ��� �������� ���������� ���� � ���
*				�������� ���������� �������� ������� ����.
*	Author:		dk
*	History:     	-
*
****************************************************************************/

#ifndef __AXIOMS_LIB_TestTS_HXX
#define __AXIOMS_LIB_TestTS_HXX

#include "MultiTS.h"
    
namespace AxiomLib {

class TestTS {

public:
		// ������ ������

	// ���� ��� �������� ���������� ����
	MultiTS data;
	
	// ���� ��� �������� ���������� �������� ���������� ����
	MultiTS correct;

	// ���� ��� �������� ���������� �������� ���������� ���� - � ���� ����������� ������� ����� �����
	std::vector<int> corrMarkUp;

		// ������� ������

	// ����������� ������
	TestTS ();

	// ���������� ������
	~TestTS ();
	
	// ������� ������ ��������� ��� �� ������� - ������� ������� - ����� ���� � ������������ ������������������ �����������
	bool getTSByIndex(std::vector<double> &vec, int i) const;

	// ������� ������ ��������� ��� �� ������� - ������� ������� - ����� ���� � ������������ ������������������ �����������
	// ������� �� ���������� ������� - � ������� ���������� ������� - � ������, ���� ����� ������ ����� �����, �� �������� ������ ����� ������
	bool getTSByIndex(std::vector<int> &vec, int i) const;

	// ������� ������������� ����� �������� ���� - ������� ������� - ����� ��� � ����� ���� � ������������ ������������������ �����������
	bool setTSByIndex(std::vector<double> &vec, int i);

	// ������� ������������� ����� �������� ���� - ������� ������� - ����� ��� � ����� ���� � ������������ ������������������ �����������
	bool setTSByIndex(std::vector<int> &vec, int i);
	
	// ������� ���������� ����� ��������� ����� � ������ �������� ��������
	int size(void) const;

	// ������� ������� ������� ���������� �������� � ������������� ������� corrMarkUp
	bool makeIntClone (void);
	
}; //end of class

// ��������� �������� �������� ��������� ����� � ������ ������
typedef std::vector<TestTS> TestSets;

}; //  end of namespace

#endif /* __AXIOMS_LIB_MultiTS_HXX */
