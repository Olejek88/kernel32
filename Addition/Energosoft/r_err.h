// --------------------------------------------
// ���� ������, ����������� ��� ������� �������
// --------------------------------------------

#ifndef _R_ERR_H
#define _R_ERR_H

const int ERC_OK				= 0;	// ������ �� ����
const int ERC_IDENTIFY			= 1;	// ������ ������������� (��� ����������� - "���������� �� �������")

// ������ � ��������� �����������
const int ERC_REL_DSU			= 10;	// v ������������� ������� �� ������ 0.2 ��� ������ 0.75
const int ERC_MSU				= 11;	// v ������ ��������� ���������� ��������� �������� 0.64
const int ERC_DSU				= 12;	// ������� ��������� ������ 11 �� (��� �����. ��������, ����� ��� � ���� �������)
const int ERC_DTP				= 13;	// ������� ������������ ������ 50 ��� ������ 1000 ��
const int ERC_RKR				= 14;	// ����������� ������ ������ ���������� ������ ���������

// ������ �� ������� ������
const int ERC_CALC_MOD			= 20;	// ��������� ������ ����� �������
const int ERC_MAT_CODE			= 21;	// ��� ��������� ������������ ����� �������
const int ERC_MAT_CODE_SU		= 22;	// ��� ��������� ��������� ���������� ����� �������

// ������ ������ ���������
const int ERC_DP_CONVERT		= 30;	// ������� �������� ������ �������
const int ERC_P_CONVERT			= 31;	// ������� �������� ������ �������
const int ERC_PB_CONVERT		= 32;	// ������� ���������������� �������� ������ �������
const int ERC_T_CONVERT			= 33;	// ������� ����������� ������ �������
const int ERC_R_CONVERT			= 34;	// ������� ��������� ������� ������ �������
const int ERC_COMP_CONVERT		= 35;	// ������� ��������� ������� ������ �������
const int ERC_CALOR_CONVERT		= 36;	// ������� ��������� ������������ ������ �������
const int ERC_CALC_CALOR_ALG	= 37;	// �������� ������� ������������ ������ �������


// ������������ ������
const int ERC_PREPARE_UNKNOWN	= 40;	// ������ � ���������� �����������. ������ ����������.
const int ERC_CALC_UNKNOWN		= 41;	// ������������ ������ ��� ������� �������

// ������, ���������� ��� �����������
const int ERC_DP_PABS			= 50;	// ��������� �������� � ���. �������� ������ 0.25
const int ERC_RE				= 51;	// ���������� �������� ����� ���������� ����� ��� �������, ����������� ��� ������� ���� �������� ��������� (�� 50-411-83, �. 3)
const int ERC_PAR_COND			= 52;	// ��� �������� ��������� T � P ��� ��������������
const int ERC_PAR_VLAG			= 53;	// ��� �������� �������. ������� ������� ���� ������� �������.
//const int ERC_T_VALUE			= 54;	// ��� �������� ����������� ������ �� ���� ����������
//const int ERC_P_VALUE			= 55;	// ��� �������� �������� ������ �� ���� ����������
const int ERC_VODA_ICE			= 56;	// ��� �������� ����������� ���� ���������
const int ERC_VODA_PAR			= 57;	// ��� �������� ����������� � �������� ���� ��������� � ���
const int ERC_COMPOSITION		= 58;	// ������ ����� ����� ������� (����� �� ����� 100%)


// ������ ��� ����������� ��������� ������
#define IS_FATAL(x) \
	(x == ERC_IDENTIFY || \
	x == ERC_REL_DSU || \
	x == ERC_REL_DSU || \
	x == ERC_DSU || \
	x == ERC_RKR || \
	x == ERC_DP_CONVERT || \
	x == ERC_P_CONVERT || \
	x == ERC_PB_CONVERT || \
	x == ERC_T_CONVERT || \
	x == ERC_R_CONVERT || \
	x == ERC_PREPARE_UNKNOWN || \
	x == ERC_VODA_ICE || \
	x == ERC_VODA_PAR || \
	x == ERC_COMPOSITION )

// �������, ������������ ��������� �� ���� ������
char* GetErrMsg(int ErrCode, char * dst);

#endif