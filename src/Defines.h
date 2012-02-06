#ifndef __AXIOMS_LIB_DEFINES_HXX
#define __AXIOMS_LIB_DEFINES_HXX

// �������� �� ����������� ���������� � ���������� ����� ������ - ��� qtGui
#define specOutputForGui	true
// ����������� ����������� ��� ���������� ��������� ���������� � qtGui
#define	str_systemMsg_prog	"CurrentProgress: "
#define	str_systemMsg_stop	"CurrentProcess has stopped."

// ����� ���������� ��� �������� �������
//#define abs(a)		(((a) < 0) ? (-a) : (a))
#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) >= (b)) ? (a) : (b))
#define min3(a,b,c)	(min(min(a,b),c))
#define max3(a,b,c)	(max(max(a,b),c))
#define pow2(a)		(a*a)
#define pow3(a)		(a*a*a)
#define pow4(a)		(a*a*a*a)

#endif /* __AXIOMS_LIB_DEFINES_HXX */
