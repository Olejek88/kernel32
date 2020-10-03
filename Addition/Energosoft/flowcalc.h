// flowcalc.h
// Функции для расчёта расхода, калорийности, энтальпии

// Подготовка данных к расчёту расхода
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _PrepareData(
	TENTRY_DATA &ED,	// [in] Пакет входных данных
	TPREP_DATA &PD		// [out] Пакет данных, подготовленных для расчёта
);

// Расчёт физических свойств и расхода (либо коррекция расхода) методом переменного перепада,
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _Rashod(
	TENTRY_DATA &ED,	// [in] Пакет входных данных
	TSUBST_DATA &SD,	// [in] Характеристики среды
	TPREP_DATA &PD,		// [in] Пакет данных, подготовленные функцией PrepareData
	TPHYSIC_PROP &PP,	// [out] Физические свойства среды
	TCALC_RES &RES		// [out] Результаты расчёта расхода и связанных с ним величин
);

// Расчёт расхода по известным физ. свойствам среды
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _CalcR(
	TENTRY_DATA &ED,	// [in] Пакет входных данных
	TSUBST_DATA &SD,	// [in] Характеристики среды
	TPREP_DATA &PD,		// [in] Пакет данных, подготовленные функцией PrepareData
	TPHYSIC_PROP &PP,	// [in] Физические свойства среды
	TCALC_RES &RES		// [out] Результаты расчёта расхода и связанных с ним величин
);

// Расчёт калорийности природного газа.
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _CalcGasCalor(
	TSUBST_DATA Comp,	// [in] компонентный состав
	int Units,			// [in] единицы измерения калорийности (допустимы значения u_kkal_kg и u_kJ_kg)
	int Alg,			// [in] алгоритм расчета (допустимы значения ac_gost и ac_iso)
	double &Res			// [out] калорийность
);

// Расчет энтальпии пара
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _CalcParEnt(
	double Tabs,	// [in] абсолютная температура пара, К
	double Pabs,	// [in] абсолютное давление пара, МПа
	double Fi,		// [in] Fi - вложность пара, %
	int Units,		// [int] единицы измерения энтальпии (допустимые значения: u_kkal_kg и u_kJ_kg;
	double &Res		// [out] энтальпия
);

// Расчет энтальпии воды
// Возвращает код ошибки (см. r_err.h)
extern "C" __declspec(dllexport)
int _CalcVodaEnt(
	double Tabs,	// [in] абсолютная температура воды, К
	double Pabs,	// [in] абсолютное давление воды, МПа
	int Units,		// [int] единицы измерения энтальпии (допустимые значения: u_kkal_kg и u_kJ_kg;
	double &Res		// [out] энтальпия
);