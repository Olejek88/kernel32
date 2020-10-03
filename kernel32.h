#define TRNSP extern "C" __declspec(dllexport)

#define BASEDIR		L"\\Compact Flash\\As\\Base"
#define HOMEDIR		"\\Compact Flash\\As\\"
#define CFG1		"\\Compact Flash\\As\\cfg1.txt"
#define CFG2		"\\Compact Flash\\As\\cfg2.txt"
#define CFG3		"\\Compact Flash\\As\\cfg3.txt"
#define CFG4		"\\Compact Flash\\As\\cfg4.txt"
#define CFG5		"\\Compact Flash\\As\\cfg5.txt"
#define CFG6		"\\Compact Flash\\As\\cfg6.txt"
#define CFG7		"\\Compact Flash\\As\\cfg7.txt"
#define CFG8		"\\Compact Flash\\As\\cfg8.txt"
#define CFGA		"\\Compact Flash\\As\\cfgA.txt"
#define LCFGA		L"\\Compact Flash\\As\\cfgA.txt"
#define MsgLog		"\\Compact Flash\\As\\Base\\messagelog.txt"
#define KBuffer		"\\Compact Flash\\As\\Base\\KBuffer.txt"
#define MKBuffer	"\\Compact Flash\\As\\Base\\MKBuffer.txt"
#define CurBase		"\\Compact Flash\\As\\Base\\CurBase.txt"
#define LCurBase	L"\\Compact Flash\\As\\Base\\CurBase.txt"
#define nameF		"\\Compact Flash\\As\\Base\\Base%04d.txt"
#define LnameF		L"\\Compact Flash\\As\\Base\\Base%04d.txt"
#define LnameG		L"\\Compact Flash\\Base%04d.txt"
#define TmpBase		"\\temp\\TmpBase.txt"
#define CfgNet		"\\Compact Flash\\As\\CfgNet.txt"
#define reboot		"\\Compact Flash\\Utility\\reboot.exe"
#define BaseTrbl	L"\\Compact Flash\\As\\Base\\BaseTrbl%04d.txt"
#define AllBase		L"\\Compact Flash\\As\\Base\\Base*.txt"
#define LOG_TEMP	"\\Compact Flash\\temp.txt"
#define HOMEDIRFIL	"\\Compact Flash\\As\\%s"

#define LPORT		3		// порт для коммуникатора "Логика"
#define SERV_PORT	4999	// server port
#define CHANGE_PORT 4994	// Порт обмена данными между контроллерами
#define kStrKB		3000		// кол-во строк в кольцевом буфере
#define mStrKB		3000		// кол-во строк в кольцевом буфере
#define klvPrm		8		// maximum number of parameters
#define befkb	"k+%08x  \tn +%-6s\tv+%016f   t+%08x%08x   s+%u\n"
#define zag1	"    [  File № % 4d TimeStart  ________________ TimeEnd  ________________  ]\n"
#define zag2	"    [  File is created ~ in                 %02d:%02d:%02d %02d/%02d/%02d(~1 sec.)  ]\n"
#define pzStr	30
#define pzEnd	56
//----------------------------------------------------------------------------------
#include "..\public.h"				// for all .dll
#include "..\display\wc.h"			// display
#include "..\sdata_dll\sdt_exp.h"	// sensors
#include "..\report\report.h"		// printer
#include "..\rashod\eval.h"			// flow
#include "..\gsm\gsm.h"			// flow
//----------------------------------------------------------------------------------
#include "Addition\Energosoft\r_types.h"	// Описания типов, используемых в алгоритмах расчёта расхода
#include "Addition\Energosoft\flowcalc.h"	// Функции для расчёта расхода
#include "Addition\Energosoft\r_const.h"	// Константы
//----------------------------------------------------------------------------------
FILE		*Log;		// log file
FILE		*fBuf;		// config file
FILE		*hMsgLog;	// message log file
HINSTANCE	hInst;		// the current instance
HWND		hWnd;		// window descriptor

HANDLE	hThrd[10];		// newlycreated thread handle
HANDLE  hReadEvents[10];// thread events
HANDLE  hReadEventsDpl;	// display thread event
HANDLE  hReadEventsFls;	// database thread event
HANDLE  hReadEventsNet;	// net thread event
HANDLE  hReadEventsMdm;	// net thread event
HANDLE  hReadEventsHtp;	// http thread event
HANDLE  hReadEventsChg;	// controllers change thread
DWORD	dwThrdID;		// newlycreated thread ID value
BOOL	ExitSignal=FALSE;	// http exit signal
BOOL	workwithbuf=FALSE;
BOOL	reqend=FALSE;
//----------------------------------------------------------------------------------
CRITICAL_SECTION fls;	// pointer critical section
CRITICAL_SECTION cs;	// ?? critical section
LONG Pointer;				// pointer
LONG TPointer,TmpTPointer;	// temp pointer
LONG MPointer;				// pointer
LONG MTPointer,MTmpTPointer;// temp pointer
//----------------------------------------------------------------------------------
SOCKET srv_socket=SOCKET_ERROR;			// server socket
SOCKET opc_cln_socket=SOCKET_ERROR;		// Сокет клиента для opc
SOCKET chg_socket=SOCKET_ERROR;			// Сокет для обмена
SOCKET cln_socket=SOCKET_ERROR;			// Сокет клиента для обмена
WSADATA WSAData;
PHOSTENT phe;
struct sockaddr_in srv_address;
SOCKADDR_IN	accept_sin;					// Receives the address of the connecting entity
INT		accept_sin_len;                 // Length of accept_sin
//----------------------------------------------------------------------------------
CHAR strC[50];
SHORT countint[10][20]={0};

UINT Tmessage=0; WPARAM	TwParam=0;	LPARAM	TlParam=0;
UINT _Tmessage;  WPARAM	_TwParam;	LPARAM	_TlParam;
SPrint MPrnTmp={0};		// print structure to sdata
SPrint MPrn={0};		// print structure to print
DplExch		*tmpDspl;	// pointer to display structure
DplExch		*htpDspl;	// pointer to display structure
DplExchO	DsplO;		// pointer to exchange display structure (4 values)
DplExchO	DsplH;		// pointer to exchange display structure (32 values)
CHAR gprsbuf[20000]={0};

SYSTEMTIME	TDisp;		// dispetcher SYSTEMTIME structure 
SYSTEMTIME	tmpTDisp={0};	// prev SYSTEMTIME structure 
SYSTEMTIME	tSTART={0};		//
SYSTEMTIME	ttmpSTART={0};	// 
SYSTEMTIME	prevHTime={0};

ULONG tmy,tmy_l,tmy_w;


WORD wHour,wDay,wMonth,wYear;		// 
DWORD FAR MassRez[15];
DWORD FAR MRez;
DOUBLE LastMon[2]={0,0};// data from last month
CHAR DataO [kStrKB*2][75];
CHAR pck[9000];

DOUBLE logik[15];
BOOL RecHour=TRUE;

INT ERC_OK1,ERC_OK2;	// return code of flowmeter function
BYTE FAR	*DataM;		// pointer to  
UINT ExgengeFlags=0;	// flags
MsgExch		MsE;		// message exchange structure
RepExch		PrnVl;		// report exchange structure
PrmExch		Prm;		// parametrs exchange structure
DtaExch		Dta;		// data exchange structure
//----------------------------------------------------------------------------------
ULONG	count = 0;			// counter of calculation
UINT	GetSlaveData=0;		// 
UINT	GetSlaveVal=0;		// current param for display from secondary contr.
UINT	ChInData=0;			// main channals quantity
UINT	TaskDues_length;	// cfg2.txt lenght
UINT	TaskMath_length;	// cfg3.txt lenght
BYTE FAR *RDspl;			// pointer to display structure
BYTE FAR *TaskMath;			// pointer to
BYTE FAR *TaskDues;			// pointer to data structure

DOUBLE	Rez[10*klvPrm];		// instant result
DOUBLE	PrValues[20]={0};

BYTE FAR *PromRez;			// result
BYTE FAR *PromRezHrs;		// результат за час
BYTE FAR *PromRezDay;		// результат за месяц
BYTE FAR *PromRezMonth;		// результат за месяц
BYTE FAR *PromRezYear;		// результат за все время

BYTE FAR *SumRez;			// instant result quantity
BYTE FAR *SumPromRez;		// result
BYTE FAR *SumPromRezMin;	// result
BYTE FAR *SumPromRezHrs;	// результат за час
BYTE FAR *SumPromRezDay;	// результат за месяц
BYTE FAR *SumPromRezMonth;	// результат за месяц
BYTE FAR *SumPromRezYear;	// результат за все время

UINT	RDspl_length;		// ???? lenght 
CycleDues Ole1;				// data request to me
CycleDues Dis;
DataR	DR;					// data array from me
DataR	DisR;					// data array from me
inValues Rsh;				// flow structure 
INT		tmp;				// temporary registers
UINT	dMsLg;				// message log lenght
CHAR	szTemp[512];		// buffer for events
UINT	pzst=0;				// current server
FILE	*idFile; UINT dFil;	// 
INT		NetState=0;
CHAR	szT[1750]={0};
UINT	ftl_seek=0,stroka=0;
MesExch MEO={0};
BOOL	EventsFlags[10];	// 
//----------------------------------------------------------------------------------
struct MainStruct
	{
	 UINT Enabled;			// enable channal
	 UINT TaktData[2];		// такт опроса датчиков
	 UINT TaktCalc[2];		// такт расчёта расхода
     DOUBLE target;			// tag identificator
     CHAR Name[8];			// name
     TENTRY_DATA es;		// ???
	 DOUBLE trnsfm[8];		// ???
    };
MainStruct *indata ;

struct CycleBuffer
	{
	 UINT		trg;		// target
	 CHAR		tNmame[6];	// name
	 DOUBLE		value;		// конечный результат
	 FILETIME	timestm;	// тайм-штамп
	 SHORT		status;		// статус
	};
CycleBuffer	KB,KBtmp;		// cycle buffer structure
CycleBuffer	MKB,MKBtmp;		// cycle buffer structure

UCHAR	package[16000];		// network buffer
//----------------------------------------------------------------------------------
TCALC_RES	RD;	// Результаты расчёта расхода и связанных с ним величин
TENTRY_DATA	ED;	// Пакет входных данных, общий для всех сред и всех сужающих устройств
TPREP_DATA	PD;	// Пакет промежуточных данных, расчитываемых на основе входных данных
TSUBST_DATA SD;	// Данные по среде
TPHYSIC_PROP PP;	// Физические свойства среды
//----------------------------------------------------------------------------------
struct Parametrs
{
	CHAR name [40];
};
Parametrs PM[] =
	{{"Диаметр трубопровода, мм"},
	{"Единицы измерения давления"},
	{"Тип датчика давления"},
	{"Единицы измерения расхода"},
	{"Барометрическое давление,кПа"},
	{"Влажность,%"},
	{"Алгоритм расчёта"},
	{"Плотность, кг/м3"},
	{"метан (CH4)"}, {"этан (C2H6)"}, {"пропан (C3H8)"}, {"н-бутан (C4H10)"}, 
	{"и-бутан (IC4H10)"}, {"азот (N2)"}, {"двуокись углерода"}, 
	{"сероводород (H2S)"}, {"н-пентан (C5H12)"}, {"и-пентан (IC5H12)"}, 
	{"гексан (C6H14)"}, {"гептан (C7H16)"}, {"октан (C8H18)"}, {"пропилен (C3H6)"}, 
	{"кислород (O2)"}, {"окись углерода (CO)"}, {"водород (H2)"}, {"углеводородный остаток"}};
//----------------------------------------------------------------------------------
struct GSMST
	{
	 CHAR TelN[16];
	 DOUBLE tipMsg;
     DOUBLE TactSn;
    }sendGsm;

struct MODEM
	{
     CHAR tip;
     CHAR com;
	 UINT speed;
	 GSMST sendGsm;
	}Modem;

UINT Wincon_mode=0;
DOUBLE TimModem;
UINT WorkModem;						// есть ли модем?
//---------'-------------------------------------------------------------------------
VOID NetTdhrdFnc (LPVOID lpParam);