#define MSG_CONSOLE2	WM_USER+123		// вывод на консоль
#define MSG_DISPLAY		WM_USER+124		// Вывод на дисплей
#define MSG_MSGLOG		WM_USER+125		// Логи
#define MSG_MSGPRINT	WM_USER+126		// Логи
#define MSG_OPTIONS		WM_USER+127		// Настройки
//**************************************************************************//
//			Структура данных, используемая функцией GetData
//					(формируется верхним уровнем)
//**************************************************************************//
struct Dues
		{
		UINT	name;					//
		UINT	cTakt;					// Время опроса канала (мс)
		UINT	vTakt;
		INT		forml;					// формула пересчёта (порядковый номер)
		DOUBLE	minVh;					// входная величина - нижний предел
		DOUBLE	maxVh;					// входная величина - верхний предел
		DOUBLE	minVih;					// выходная величина - нижний предел
		DOUBLE	maxVih;					// выходная величина - верхний предел
		DOUBLE	avrSign;				// аварийное значение
		};

struct CycleDues
		{
		UINT	target;					// объект
		INT		forml;					// формула пересчёта (порядковый номер) ???
		CHAR	name[6];				// название
		DOUBLE	minVh;					// входная величина  - нижний предел
		DOUBLE	maxVh;					// входная величина  - верхний предел
		DOUBLE	minVih;					// выходная величина - нижний предел
		DOUBLE	maxVih;					// выходная величина - верхний предел
		DOUBLE	rz_n_gr;				// рабочий диапазон  - нижний предел
		DOUBLE	rz_v_gr;				// рабочий диапазон  - верхний предел
		DOUBLE	avrSign;				// аварийное значение (договорное)
		UINT	rzrv1[2];
		DOUBLE	rzrv2;
		DOUBLE	rzrv3;
		};
//**************************************************************************//
// Структура данных, используемая для заполнения энергосовтовких структур
//						(формируется верхним уровнем)
//**************************************************************************//
struct Math
		{
		UINT	name;			//
		UINT	cTakt;			// Время опроса канала (мс)
		UINT	vTakt;
		
		DOUBLE	d;				// Диаметр трубопровода, мм
		DOUBLE	dsu;			// Диаметр сужающего устройства, мм
		INT		mat;			// Материал трубопровода (см. r_const.h)
		INT		mat_su;			// Материал сужающего устройства (см. r_const.h)
		DOUBLE	roug;			// Абс. шероховатость, мм
		DOUBLE	rad;			// Радиус скругления кромки
		INT		type_su;		// Тип сужающего устройства
		DOUBLE	interval;		// Межповерочный интервал, лет
		DOUBLE	dp;				// Перепад давления
		INT		dp_units;		// Ед.изм. перепада (см. r_const.h)
		DOUBLE	p;				// Давление
		INT		p_units;		// Ед.изм. давления (см. r_const.h)
		BOOL	p_is_abs;		// Признак того, что P - абсолютное давление
		DOUBLE	t;				// Температура
		INT		t_units;		// Ед. изм. температуры (см. r_const.h)

		INT		r_units;		// Ед.изм. расхода (см. r_const.h)
		DOUBLE	pb;				// Барометрическое давление
		INT		pb_units;		// Ед. изм. барометрического давления (см. r_const.h)

		DOUBLE	fi;				// Влажность (только для газов), %

//********************************		
		
		INT		module;			// Расчётный модуль
		INT		gas_alg;		// Алгоритм расчёта (только для природного газа)
		DOUBLE	rnom;			// Плотность, кг/м3 (необходима для неполного состава)
		DOUBLE	sCH4;			// 1.метан
		DOUBLE	sC2H6;			// 2.этан
		DOUBLE	sC3H8;			// 3.пропан
		DOUBLE	sC4H10;			// 4.н-бутан
		DOUBLE	sIC4H10;		// 5.и-бутан
		DOUBLE	sN2;			// 6.азот
		DOUBLE	sCO2;			// 7.двуокись углерода
		DOUBLE	sH2S;			// 8.сероводород
		DOUBLE	sC5H12;			// 9.н-пентан
		DOUBLE	sIC5H12;		// 10.и-пентан
		DOUBLE	sC6H14;			// 11.гексан
		DOUBLE	sC7H16;			// 12.гептан
		DOUBLE	sC8H18;			// 13.октан
		DOUBLE	sC3H6;			// 14.пропилен
		DOUBLE	sO2;			// 15.кислород
		DOUBLE	sCO;			// 16.окись углерода
		DOUBLE	sH2;			// 17.водород
		DOUBLE	sCnHm;			// 18.углеводородный остаток

		BOOL	MolarProc;		// Если True - значит состав в молярных %, иначе - в объёмных
};
//**************************************************************************//
struct DplExch
		{		
		CHAR	name[6];				// название
		DOUBLE	var;					// значение
		SHORT	status;					// статус
		FILETIME timestm;				// тайм-штамп
		
};

#pragma pack(push,8)
struct DplExchO
		{
		
		CHAR	name[4][7];				// название
		DOUBLE	var[4];					// значения
		SHORT	status[4];				// статус
		FILETIME timestm[4];			// тайм-штамп
};
#pragma pack(pop)

struct MsgExch
		{
		DOUBLE	KodEvents[2];			// код события
		FILETIME timestm[2];			// тайм-штамп		
};

CHAR AutoPrn;
VOID MsgMcr(UINT Dvc,UINT Type,UINT Events,CHAR* string) {	}



