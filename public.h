#define MSG_CONSOLE2	WM_USER+123		// ����� �� �������
#define MSG_DISPLAY		WM_USER+124		// ����� �� �������
#define MSG_MSGLOG		WM_USER+125		// ����
#define MSG_MSGPRINT	WM_USER+126		// ����
#define MSG_OPTIONS		WM_USER+127		// ���������
//**************************************************************************//
//			��������� ������, ������������ �������� GetData
//					(����������� ������� �������)
//**************************************************************************//
struct Dues
		{
		UINT	name;					//
		UINT	cTakt;					// ����� ������ ������ (��)
		UINT	vTakt;
		INT		forml;					// ������� ��������� (���������� �����)
		DOUBLE	minVh;					// ������� �������� - ������ ������
		DOUBLE	maxVh;					// ������� �������� - ������� ������
		DOUBLE	minVih;					// �������� �������� - ������ ������
		DOUBLE	maxVih;					// �������� �������� - ������� ������
		DOUBLE	avrSign;				// ��������� ��������
		};

struct CycleDues
		{
		UINT	target;					// ������
		INT		forml;					// ������� ��������� (���������� �����) ???
		CHAR	name[6];				// ��������
		DOUBLE	minVh;					// ������� ��������  - ������ ������
		DOUBLE	maxVh;					// ������� ��������  - ������� ������
		DOUBLE	minVih;					// �������� �������� - ������ ������
		DOUBLE	maxVih;					// �������� �������� - ������� ������
		DOUBLE	rz_n_gr;				// ������� ��������  - ������ ������
		DOUBLE	rz_v_gr;				// ������� ��������  - ������� ������
		DOUBLE	avrSign;				// ��������� �������� (����������)
		UINT	rzrv1[2];
		DOUBLE	rzrv2;
		DOUBLE	rzrv3;
		};
//**************************************************************************//
// ��������� ������, ������������ ��� ���������� ��������������� ��������
//						(����������� ������� �������)
//**************************************************************************//
struct Math
		{
		UINT	name;			//
		UINT	cTakt;			// ����� ������ ������ (��)
		UINT	vTakt;
		
		DOUBLE	d;				// ������� ������������, ��
		DOUBLE	dsu;			// ������� ��������� ����������, ��
		INT		mat;			// �������� ������������ (��. r_const.h)
		INT		mat_su;			// �������� ��������� ���������� (��. r_const.h)
		DOUBLE	roug;			// ���. �������������, ��
		DOUBLE	rad;			// ������ ���������� ������
		INT		type_su;		// ��� ��������� ����������
		DOUBLE	interval;		// ������������� ��������, ���
		DOUBLE	dp;				// ������� ��������
		INT		dp_units;		// ��.���. �������� (��. r_const.h)
		DOUBLE	p;				// ��������
		INT		p_units;		// ��.���. �������� (��. r_const.h)
		BOOL	p_is_abs;		// ������� ����, ��� P - ���������� ��������
		DOUBLE	t;				// �����������
		INT		t_units;		// ��. ���. ����������� (��. r_const.h)

		INT		r_units;		// ��.���. ������� (��. r_const.h)
		DOUBLE	pb;				// ��������������� ��������
		INT		pb_units;		// ��. ���. ���������������� �������� (��. r_const.h)

		DOUBLE	fi;				// ��������� (������ ��� �����), %

//********************************		
		
		INT		module;			// ��������� ������
		INT		gas_alg;		// �������� ������� (������ ��� ���������� ����)
		DOUBLE	rnom;			// ���������, ��/�3 (���������� ��� ��������� �������)
		DOUBLE	sCH4;			// 1.�����
		DOUBLE	sC2H6;			// 2.����
		DOUBLE	sC3H8;			// 3.������
		DOUBLE	sC4H10;			// 4.�-�����
		DOUBLE	sIC4H10;		// 5.�-�����
		DOUBLE	sN2;			// 6.����
		DOUBLE	sCO2;			// 7.�������� ��������
		DOUBLE	sH2S;			// 8.�����������
		DOUBLE	sC5H12;			// 9.�-������
		DOUBLE	sIC5H12;		// 10.�-������
		DOUBLE	sC6H14;			// 11.������
		DOUBLE	sC7H16;			// 12.������
		DOUBLE	sC8H18;			// 13.�����
		DOUBLE	sC3H6;			// 14.��������
		DOUBLE	sO2;			// 15.��������
		DOUBLE	sCO;			// 16.����� ��������
		DOUBLE	sH2;			// 17.�������
		DOUBLE	sCnHm;			// 18.�������������� �������

		BOOL	MolarProc;		// ���� True - ������ ������ � �������� %, ����� - � ��������
};
//**************************************************************************//
struct DplExch
		{		
		CHAR	name[6];				// ��������
		DOUBLE	var;					// ��������
		SHORT	status;					// ������
		FILETIME timestm;				// ����-�����
		
};

#pragma pack(push,8)
struct DplExchO
		{
		
		CHAR	name[4][7];				// ��������
		DOUBLE	var[4];					// ��������
		SHORT	status[4];				// ������
		FILETIME timestm[4];			// ����-�����
};
#pragma pack(pop)

struct MsgExch
		{
		DOUBLE	KodEvents[2];			// ��� �������
		FILETIME timestm[2];			// ����-�����		
};

CHAR AutoPrn;
VOID MsgMcr(UINT Dvc,UINT Type,UINT Events,CHAR* string) {	}



