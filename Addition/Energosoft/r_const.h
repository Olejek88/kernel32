#ifndef _r_const_h
#define _r_const_h

// 1. ������� ���������
// ~~~~~~~~~~~~~~~~~~~~

const int u_m3		= 0;	// �3
const int u_1000m3	= 1;	// ���.�3
const int u_kg		= 2;	// ��
const int u_t		= 3;	// � (�����)
const int u_Pa		= 4;	// ��
const int u_kPa		= 5;	// ���
const int u_MPa		= 6;	// ���
const int u_kgs_m2	= 7;	// ���/�2
const int u_kgs_sm2 = 8;	// ���/��2
const int u_mmHg	= 9;	// �� ��.��.
const int u_C		= 10;	// ������ �������
const int u_K		= 11;	// � (��. ��������)
const int u_proc	= 12;	// ��������
const int u_molproc = 13;	// �������� ��������

const int u_vproc = 14;		// �������� ��������	NEW!!!!
const int u_mproc = 15;		// �������� ��������	NEW!!!!

const int u_cal		= 16;	// �������
const int u_J		= 17;	// ��
const int u_kcal	= 18;	// ����
const int u_MJ		= 19;	// ���
const int u_kcal_kg	= 20;	// ����/��
const int u_kJ_kg	= 21;	// ���/��

const int u_m3_s		= 22;	// �3/�
const int u_1000m3_s	= 23;	// ���.�3/�
const int u_kg_s		= 24;	// ��/�
const int u_t_s			= 25;	// �/�

const int u_m3_h		= 26;	// �3/�
const int u_1000m3_h	= 27;	// ���.�3/�
const int u_kg_h		= 28;	// ��/�
const int u_t_h			= 29;	// �/�


// 2. ��������� ������
// ~~~~~~~~~~~~~~~~~~~

const int m_gas		= 0;	// ��������� ��� (������ �� ������ �� 3-� ����������)
const int m_par		= 1;	// ���
const int m_voda	= 2;	// ����
const int m_azot	= 3;	// ����
const int m_kislorod	= 4;	// ��������
const int m_vozd	= 5;	// ������
const int m_argon	= 6;	// �����

// 3. ��������� ������� ���������� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int a_gerg91	= 0;	// GERG-91
const int a_nx19	= 1;	// NX-19 MOD
const int a_smv		= 2;	// ���� ���

// 4. ���� �������� ��������� (��������� ������� �������)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

const int su_std_u	= 0;	// ����������� ��������� (������� ������ ������)
const int su_std_f	= 1;	// ����������� ��������� (��������� ������ ������)
const int su_std_t	= 2;	// ����������� ��������� (������������� ������ ������)
const int su_soplo_isa1932	= 3;	// ����� ���1932
const int su_vent_v1	= 4; // ����� ������� �1
const int su_vent_v2	= 5; // ����� ������� �2
const int su_vent_v3	= 6; // ����� ������� �3

const int su_diaf_con	= 7; // ��������� � ���������� ������
const int su_soplo_cil	= 8; // �������������� �����
const int su_soplo1_4	= 9; // ����� "�������� �����"
const int su_diaf_segm	= 10; // ���������� ���������
const int su_diaf_iznos	= 11; // ���������������� ���������
const int su_std50	= 12; // ����������� ��������� � D<50 ��

const int su_vcorrector = 30; // ��������� ��������� �������

// 5. ���� ���������� �������
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

/*const int mt_t               �����������
        p               ���������� ��������
        p_rel           ������������� ��������
        dp              �������
        r               ������*/

// 6. �����
// ~~~~~~~~

const int st_8 =		0;	// �� 8
const int st_10 =		1;	// �� 10
const int st_15 =		2;	// �� 15
const int st_15m =		3;	// �� 15�
const int st_16m =		4;	// �� 16�
const int st_20 =		5;	// �� 20
const int st_20m =		6;	// �� 20�
const int st_25 =		7;	// �� 25
const int st_30 =		8;	// �� 30
const int st_35 =		9;	// �� 35
const int st_h6sm =		10; // �� �6��
const int st_h7sm =		11; // �� �7��
const int st_12mh =		12; // �� 12��
const int st_12h1mf =	13; //�� 12�1��
const int st_12h17 =	14; // �� 12�17
const int st_12h18n9t = 15; // �� 12�18�9�
const int st_12h18n10t = 16; // �� 12�18�10�
const int st_14h17n2 =	17; // �� 14�17H2
const int st_15hma =	18; // �� 15���
const int st_15h1m1f =	19; // �� 15�1�1�
const int st_15h5m =	20; // �� 15�5�
const int st_15h12enmf =	21; // �� 15�12����
const int st_17h18m9 =	22; // �� 17�18�9
const int st_20h23n13 =	23; // �� 20�23�13
const int st_36h18n25s2 = 24; // �� 36h18n25s2

const MAX_MAT_CODE = 24; // ��������� ��� ��������� (����� �������)

// 7. ����������� ��������� �����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int aggr_liq =	0; // ������ ��������� NEW!!!
const int aggr_gas =	1; // ������������ ��������� NEW!!!

// 8. ��������� ������� ������������ ���������� ����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int ac_gost =		0; // ������ �� ���� 22667, ������� 2
const int ac_iso =		1; //  ������ �� ��� 6976

#endif