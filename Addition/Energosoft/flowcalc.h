// flowcalc.h
// ������� ��� ������� �������, ������������, ���������

// ���������� ������ � ������� �������
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _PrepareData(
	TENTRY_DATA &ED,	// [in] ����� ������� ������
	TPREP_DATA &PD		// [out] ����� ������, �������������� ��� �������
);

// ������ ���������� ������� � ������� (���� ��������� �������) ������� ����������� ��������,
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _Rashod(
	TENTRY_DATA &ED,	// [in] ����� ������� ������
	TSUBST_DATA &SD,	// [in] �������������� �����
	TPREP_DATA &PD,		// [in] ����� ������, �������������� �������� PrepareData
	TPHYSIC_PROP &PP,	// [out] ���������� �������� �����
	TCALC_RES &RES		// [out] ���������� ������� ������� � ��������� � ��� �������
);

// ������ ������� �� ��������� ���. ��������� �����
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _CalcR(
	TENTRY_DATA &ED,	// [in] ����� ������� ������
	TSUBST_DATA &SD,	// [in] �������������� �����
	TPREP_DATA &PD,		// [in] ����� ������, �������������� �������� PrepareData
	TPHYSIC_PROP &PP,	// [in] ���������� �������� �����
	TCALC_RES &RES		// [out] ���������� ������� ������� � ��������� � ��� �������
);

// ������ ������������ ���������� ����.
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _CalcGasCalor(
	TSUBST_DATA Comp,	// [in] ������������ ������
	int Units,			// [in] ������� ��������� ������������ (��������� �������� u_kkal_kg � u_kJ_kg)
	int Alg,			// [in] �������� ������� (��������� �������� ac_gost � ac_iso)
	double &Res			// [out] ������������
);

// ������ ��������� ����
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _CalcParEnt(
	double Tabs,	// [in] ���������� ����������� ����, �
	double Pabs,	// [in] ���������� �������� ����, ���
	double Fi,		// [in] Fi - ��������� ����, %
	int Units,		// [int] ������� ��������� ��������� (���������� ��������: u_kkal_kg � u_kJ_kg;
	double &Res		// [out] ���������
);

// ������ ��������� ����
// ���������� ��� ������ (��. r_err.h)
extern "C" __declspec(dllexport)
int _CalcVodaEnt(
	double Tabs,	// [in] ���������� ����������� ����, �
	double Pabs,	// [in] ���������� �������� ����, ���
	int Units,		// [int] ������� ��������� ��������� (���������� ��������: u_kkal_kg � u_kJ_kg;
	double &Res		// [out] ���������
);