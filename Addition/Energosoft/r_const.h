#ifndef _r_const_h
#define _r_const_h

// 1. Единицы измерения
// ~~~~~~~~~~~~~~~~~~~~

const int u_m3		= 0;	// м3
const int u_1000m3	= 1;	// тыс.м3
const int u_kg		= 2;	// кг
const int u_t		= 3;	// т (тонны)
const int u_Pa		= 4;	// Па
const int u_kPa		= 5;	// кПа
const int u_MPa		= 6;	// МПа
const int u_kgs_m2	= 7;	// кгс/м2
const int u_kgs_sm2 = 8;	// кгс/см2
const int u_mmHg	= 9;	// мм рт.ст.
const int u_C		= 10;	// градус Цельсия
const int u_K		= 11;	// К (гр. Кельвина)
const int u_proc	= 12;	// проценты
const int u_molproc = 13;	// проценты молярные

const int u_vproc = 14;		// проценты объемные	NEW!!!!
const int u_mproc = 15;		// проценты массовые	NEW!!!!

const int u_cal		= 16;	// калории
const int u_J		= 17;	// Дж
const int u_kcal	= 18;	// ккал
const int u_MJ		= 19;	// МДж
const int u_kcal_kg	= 20;	// ккал/кг
const int u_kJ_kg	= 21;	// кДж/кг

const int u_m3_s		= 22;	// м3/с
const int u_1000m3_s	= 23;	// тыс.м3/с
const int u_kg_s		= 24;	// кг/с
const int u_t_s			= 25;	// т/с

const int u_m3_h		= 26;	// м3/ч
const int u_1000m3_h	= 27;	// тыс.м3/ч
const int u_kg_h		= 28;	// кг/ч
const int u_t_h			= 29;	// т/ч


// 2. Расчётные модули
// ~~~~~~~~~~~~~~~~~~~

const int m_gas		= 0;	// природный газ (расчёт по одному из 3-х алгоритмов)
const int m_par		= 1;	// пар
const int m_voda	= 2;	// вода
const int m_azot	= 3;	// Азот
const int m_kislorod	= 4;	// Кислород
const int m_vozd	= 5;	// Воздух
const int m_argon	= 6;	// Аргон

// 3. Алгоритмы расчёта природного газа
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int a_gerg91	= 0;	// GERG-91
const int a_nx19	= 1;	// NX-19 MOD
const int a_smv		= 2;	// ВНИЦ СМВ

// 4. Типы сужающих устройств (Алгоритмы расчета расхода)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

const int su_std_u	= 0;	// Стандартная диафрагма (угловой способ отбора)
const int su_std_f	= 1;	// Стандартная диафрагма (фланцевый способ отбора)
const int su_std_t	= 2;	// Стандартная диафрагма (трехрадиусный способ отбора)
const int su_soplo_isa1932	= 3;	// Сопло ИСА1932
const int su_vent_v1	= 4; // Труба Вентури В1
const int su_vent_v2	= 5; // Труба Вентури В2
const int su_vent_v3	= 6; // Труба Вентури В3

const int su_diaf_con	= 7; // Диафрагма с коническим входом
const int su_soplo_cil	= 8; // Цилиндрическое сопло
const int su_soplo1_4	= 9; // Сопло "четверть круга"
const int su_diaf_segm	= 10; // Сегментная диафрагма
const int su_diaf_iznos	= 11; // Износоустойчивая диафрагма
const int su_std50	= 12; // Стандартная диафрагма с D<50 мм

const int su_vcorrector = 30; // Корректор объемного расхода

// 5. Типы измеряемых величин
// ~~~~~~~~~~~~~~~~~~~~~~~~~~

/*const int mt_t               Температура
        p               Абсолютное давление
        p_rel           Относительное давление
        dp              Перепад
        r               Расход*/

// 6. Стали
// ~~~~~~~~

const int st_8 =		0;	// ст 8
const int st_10 =		1;	// ст 10
const int st_15 =		2;	// ст 15
const int st_15m =		3;	// ст 15М
const int st_16m =		4;	// ст 16М
const int st_20 =		5;	// ст 20
const int st_20m =		6;	// ст 20М
const int st_25 =		7;	// ст 25
const int st_30 =		8;	// ст 30
const int st_35 =		9;	// ст 35
const int st_h6sm =		10; // ст Х6СМ
const int st_h7sm =		11; // ст Х7СМ
const int st_12mh =		12; // ст 12МХ
const int st_12h1mf =	13; //ст 12Х1МФ
const int st_12h17 =	14; // ст 12Х17
const int st_12h18n9t = 15; // ст 12Х18М9Т
const int st_12h18n10t = 16; // ст 12Х18М10Т
const int st_14h17n2 =	17; // ст 14Х17H2
const int st_15hma =	18; // ст 15ХМА
const int st_15h1m1f =	19; // ст 15Х1М1Ф
const int st_15h5m =	20; // ст 15Х5М
const int st_15h12enmf =	21; // ст 15Х12ЕНМФ
const int st_17h18m9 =	22; // ст 17Х18М9
const int st_20h23n13 =	23; // ст 20Х23Н13
const int st_36h18n25s2 = 24; // ст 36h18n25s2

const MAX_MAT_CODE = 24; // Последний код материала (самый большой)

// 7. Аггрегатные состояния среды
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int aggr_liq =	0; // жидкое состояние NEW!!!
const int aggr_gas =	1; // газообразное состояние NEW!!!

// 8. Алгоритмы расчета калорийности природного газа
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int ac_gost =		0; // Расчёт по ГОСТ 22667, таблица 2
const int ac_iso =		1; //  Расчёт по ИСО 6976

#endif