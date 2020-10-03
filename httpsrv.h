//-----------------------------------------------------------------------------
#define HTTP_PORT	80				// http-port
#define IDENT		"ГРП №1"		// идентификатор
//-----------------------------------------------------------------------------
#include "..\message.h"
#include "logika.h"
//-----------------------------------------------------------------------------
SHORT ver_major=2;
SHORT ver_minor=11;
CHAR  ver_time[50];
CHAR  ver_desc[100];
CHAR  sernum[50];
CHAR  softver[50],wsoftver[50];
SHORT maxmodule=2,kA=0,wkA=0;
SYSTEMTIME time1,time2,time3,st,fn;
FILETIME fst,ffn;
//-----------------------------------------------------------------------------
HANDLE	hCom[10];
CHAR	Com[10];
UCHAR	DId[80];
UINT	speed=4800;			// COM-port speed
//-----------------------------------------------------------------------------
UINT  tTotal=0;				// total quantity of tags
DWORD dwBytesWritten=0,dwBytesRead=0;
UINT  devNum=0,tag_num=0,devBus=0;
UINT  startid=0,quantZp=0,logikAdr=0;
CHAR  lID[25];
BOOL  WriteEnable=TRUE;
UCHAR pck2[1000];
//-----------------------------------------------------------------------------
UCHAR sBuf1[7000],sBuf2[400];
CHAR Out[150];
SPrint Msp;
AnsLog alog;				// structure of answer from logika
RepExch Rep;
SHORT	interval=0;
//-----------------------------------------------------------------------------
SOCKET server_socket=SOCKET_ERROR;			// Сокет для обмена
SOCKET http_cln_socket=SOCKET_ERROR;		// Сокет клиента для http
CHAR wwwroot[MAX_PATH];
CHAR hostname[MAX_PATH];
CRITICAL_SECTION output_criticalsection;
SHORT empt=0;
//-----------------------------------------------------------------------------
VOID ULOGW (CHAR* string, ...);
VOID formParam (VOID);
INT  lScanBus (SHORT blok);
INT  init_tags(VOID);
BOOL  OpenCom (SHORT blok, UINT speed);
SHORT CRCode (BYTE *msg, INT len);
BOOL ReadAPC79 (SHORT adr, SHORT blok);
//VOID ReadMConf (INT dev, INT type, SHORT blok);
BOOL bufWrite (VOID);
BOOL AnalyseSequence (CHAR* dat,UINT len, UINT blok);	
VOID PollDeviceCOM (SHORT blok, SHORT channal, SHORT type);		// polling group device thread
UINT InitDriver();			// init driver
BOOL ReadCPT (SHORT adr, SHORT blok, SHORT pAdr);
VOID ReadMConf (INT dev, SHORT blok, CHAR* name, UINT tagn);
CHAR* FormSequence (UCHAR lNum, UCHAR nFunc, UCHAR nChannal, UINT npAdr, UCHAR nPipe, SPrint Msp);
BOOL hConnection (CHAR tP, INT _s_port, SOCKET &sck, SOCKET &cli);
//----------------------------------------------------------------------------
typedef struct _DeviceL DeviceL;	// controllers info
struct _DeviceL {  
  SHORT type;		// тип модуля 3 - Wincon, 11- Logika (СПТ961), 12- Logika (СПТ762), 2 - АПС79
  SHORT device;		// порядковый номер устройства. (адрес на шине или адрес IP (порядковый))
  CHAR	name[20];	// название прибора 
  SHORT status;		// текущий статус, (0-нет связи, 1-нормально, 2-:).
  SHORT	com;		// коммуникационный интерфейс (номер порта или IP-адрес)
  UINT	speed;		// скорость связи для COM
  UCHAR	pipe;		// трубопровод устройства
  SHORT tag_num;	// количество физических входов/предопределенных параметров на устройстве
};
DeviceL DeviceRU [MAX_LOGIKS_NUM];
//----------------------------------------------------
typedef struct _DataL DataL;		// channels info
struct _DataL {
  SHORT device;		// порядковый номер устройства.
  CHAR  module[20];	// название модуля устройства (для Каскада, например A1-01, DI-02)	
  SHORT kanal;		// порядковый номер канала в  модуле.
  SHORT nmodule;	// порядковый номер модуля в приборе / контроллере
  SHORT status;		// текущий статус, (0-нет связи, 1-нормально, 2-:).
  CHAR  value[500];	// текущее значение.
  SHORT type;		// тип измеряемой величины (0-float, 1-int, 2-text..)
};
DataL DataRE;   DataL DRL[TAGS_NUM_MAX];		// data
//-----------------------------------------------------------------------------
UCHAR sBuf3[] = {0x10,0x01,0x10,0x1F,0x1D,0x10,0x2,0x9,0x30,0x9,0x30,0x30,0x33,0x0C,0x10,0x03,0x0,0x0,0x0};	// 003
UCHAR sBuf99[] = {0x10,0x01,0x10,0x1F,0x1D,0x10,0x2,0x9,0x30,0x9,0x30,0x39,0x39,0x0C,0x10,0x03,0x0,0x0,0x0};// 099
UCHAR sBuf52[] = {0x10,0x01,0x10,0x1F, 0xC,0x10,0x2,0x9,0x30,0x9,0x30,0x35,0x32,0x9,0x30,0x9,0x30,0x30,0x31,0xC,0x10,0x03,0x0,0x0,0x0};	// 052(0)
//-----------------------------------------------------------------------------
VOID StartHttpSrv(LPVOID lpParam)
{
 formParam ();
 printf ("[https] winconn communicator v0.51.11 started\n");
 printf ("[https] working on protocol CRQ 5.56 (january 2006)\n"); 
 printf ("[https] initdriver.....\n"); 
 //devNum=2; devBus=2;
 //if (0)
 if (!InitDriver())
	{
	 printf ("[https] initdriver [error]\n");  ULOGW ("InitDriver error()");	 
	 devNum=3; devBus=3;
	 init_tags ();
	 while (1)
		{
		 BOOL eC = hConnection (1, HTTP_PORT, server_socket, http_cln_socket);
		 if (ExitSignal) 
			{
			 ULOGW("exit signal. finished");
			 return;
			}
		}

	 return;
	}
 else
	{
	 printf ("[https] initdriver [success]\n");
	 printf ("[https] init tags ().....");  ULOGW ("Init tags ()");
	 //if (0)
	 if (!init_tags ()) 
		{
		 printf ("[error]\n");	 ULOGW ("init tags error()");
		 CHAR i; scanf ("> %c",&i);
		 return;
		}
	 else printf ("[success]\n"); ULOGW ("init tags success()");
	 while (1)
		{
		 BOOL eC = hConnection (1, HTTP_PORT, server_socket, http_cln_socket);
		 if (ExitSignal) 
			{
			 ULOGW("exit signal. finished");
			 return;
			}
		}
	 return;
	} 
}
//-------------------------------------------------------------------
UINT InitDriver()
{
 ULOGW ("InitDriver()");
 tTotal = 0;				// total tag quantity
 printf ("open com-port[%d].....",LPORT); 
 if (!OpenCom (LPORT,speed)) { printf ("[failed]\n");  return -1; }
 else printf ("[success]\n");
 printf ("lScanBus [port %d]\n",LPORT);  ULOGW("Scan bus [1]");
 if (lScanBus(LPORT))
	{
	 printf ("total %d devices found\n",devNum); ULOGW("Total %d devices found",devNum);
	 /*if (devNum<2)
		{
		 devNum=0;
		 if (lScanBus(LPORT))
			{
			 printf ("total %d devices found\n",devNum); ULOGW("Total %d devices found",devNum);
			}
		}*/
	 return 1;
	}
 else		
	{
	 printf ("no devices found\n",devNum); ULOGW("No devices found"); 
	 if (lScanBus(LPORT))
		{
		 printf ("total %d devices found\n",devNum); ULOGW("Total %d devices found",devNum);
		 return 1;
		}
	 printf ("attempt2: no devices found\n",devNum); ULOGW("attempt2: no devices found"); 
 	 return 0; 
	}
}
//-----------------------------------------------------------------------------
INT lScanBus (SHORT blok)
{
 UINT devBus=0; CHAR name[20];
 ULOGW("lScanBus [%d]", blok);
 //ReadAPC79 (33,blok);
 if (ReadAPC79 (3,blok) && ReadAPC79 (99,blok) && ReadAPC79 (52,blok))
	{
	 DeviceRU[devNum].com = blok;
	 DeviceRU[devNum].device = 0;	 
	 strcpy (DeviceRU[devNum].name,apc.version);
	 DeviceRU[devNum].speed=speed;
	 DeviceRU[devNum].status = 1;
	 DeviceRU[devNum].tag_num=0;
	 DeviceRU[devNum].type=2;
	 printf ("device found [%s] on address %d\n",DeviceRU[devNum].name, 0);
	 ULOGW("device found [%s] on address %d",DeviceRU[devNum].name, 0);	// write in log
	 devNum++; devBus++;
	 for (INT adr=1;adr<=MAX_LOGIKS_NUM;adr++)
	 for (INT nump=0;nump<=1;nump++)
		{
		 if (ReadCPT (adr, blok, 8))
		 if (alog.checksym)
			{
			 strcpy (DeviceRU[devNum].name,alog.data[0]);
			 DeviceRU[devNum].tag_num = 0;
			 nump=0; Sleep (100);
			 if (ReadCPT (adr, blok, 99))
				 if (alog.checksym)
					{
					 nump=10;
					 memcpy (name,"0000000000",10); strcpy (name,DeviceRU[devNum].name);
					 name[9]=name[7]; name[8]=name[6]; name[7]=name[5]; name[6]=name[4]; name[5]='-'; name[4]=name[3]; name[3]=name[2]; name[2]='-';
					 for (UINT i=6;i<=6+MAX_PIPE_NUMS;i++)
						{
						 BOOL ne=FALSE;
						 strcpy (DeviceRU[devNum].name,name);
						 switch (name[i])
							{
							 case 48: ULOGW("Опана пусто"); break;
							 case 51: ULOGW("Валерик сконфигурировал херово"); break;
							 case 53: DeviceRU[devNum].type =15; DeviceRU[devNum].pipe=i-5;
								      DeviceRU[devNum].tag_num = sizeof(Archive761)/sizeof(Archive761[0]);
									  DeviceRU[devNum].name[6]='5'; DeviceRU[devNum].name[7]=0; ne=TRUE; break; 
							 default: ULOGW("Валерик сконфигурировал херово"); break;
							}
						 if (ne)
							{
							 DeviceRU[devNum].com = blok; DeviceRU[devNum].device = adr;
							 DeviceRU[devNum].speed = speed; DeviceRU[devNum].status = 1;
							 printf ("device found [%s] on address %d. type=%d. tags=%d pipe=%d\n", DeviceRU[devNum].name, adr,DeviceRU[devNum].type,DeviceRU[devNum].tag_num,DeviceRU[devNum].pipe);
							 ULOGW("Device found [%s] on address %d. type=%d. tags=%d pipe=%d", DeviceRU[devNum].name, adr,DeviceRU[devNum].type,DeviceRU[devNum].tag_num,DeviceRU[devNum].pipe); 
							 ReadMConf (devNum, adr, DeviceRU[devNum].name, DeviceRU[devNum].tag_num); kA=kA+4;
							 devNum++; devBus++; ne=FALSE;
							}
						}
					}
				 else ULOGW("wrong checksym");
			 else ULOGW("read failed");
			}
		}
	}
 ULOGW("Total %d logiks found on port Com%d",devBus,blok);
 return devBus;
}
//-------------------------------------------------------------------
VOID WriteToPort (UINT com, UINT device, CHAR* Out)
{
 DWORD dwBytesWritten=-1;
 PurgeComm(hCom[com],PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR);
 for (UINT po=0;po<strlen(Out);po++)	
	{
	 WriteFile(hCom[com], Out+po, 1, &dwBytesWritten, NULL);
	}		 
}
//-----------------------------------------------------------------------------------
VOID PollDeviceCOM (SHORT blok, SHORT channal, SHORT type)
{
 //ULOGW("PollDeviceCOM (1)"); 
 //CHAR *sBuf2,*sBuf1,*Out;
 //sBuf2 = new CHAR [400];	// buffer for rs-232
 CHAR sBuf1[7000];
 CHAR Out[300];
 UINT ccom=blok; SHORT Answer=0;
 SPrint Msp; BOOL arch=FALSE; UINT pip=0;
 //ULOGW("Read COM %d channal=%d,type=%d",ccom,channal,type);
 //for (UINT device=0;device<devNum;device++) ;
 UINT device=1;
 if (channal>0 && channal<10 && type<10)
	{
	 device=(UINT)(channal-1)/4+1;
	 channal=(UINT)(channal-1)%4+1;
	 //ULOGW("device %d channal=%d DeviceRU[device].type=%d DeviceRU[device].tag_num=%d",device,channal,DeviceRU[device].type,DeviceRU[device].tag_num);
	 //DeviceRU[device].type=15; DeviceRU[device].tag_num=25;
	}
 if (channal>10 && type<10)
	{
	 device=channal/20;
	}
 if (DeviceRU[device].type>10)
	{
	 //ULOGW("Read device %d.type=%d.tag=%d",device, DeviceRU[device].type,DeviceRU[device].tag_num);
	 Sleep (1000);
 	 UINT i=0,max_t=1;
	 SYSTEMTIME lpST; FILETIME lpFT; ULARGE_INTEGER ui;
	 GetLocalTime(&lpST); SystemTimeToFileTime(&lpST,&lpFT);
	 ui.QuadPart= lpFT.dwLowDateTime + lpFT.dwHighDateTime * 0x100000000L;
	 Msp.end = lpFT;
	 for (UINT pdr=1; pdr<=(UINT)DeviceRU[device].tag_num; pdr++)
		{		 
		 //ULOGW("type=%d | channal=%d | Archive761[pdr-1].numCH=%d | Archive761[pdr-1].type=%d | interval=%d",DRL[pdr-1].type,channal,Archive761[pdr-1].numCH,Archive761[pdr-1].type,interval); 		 
		 if (!type) if (DRL[pdr-1].type) continue;	// архивные, а запросили мгновенные
		 if (type) if (!DRL[pdr-1].type) continue;	// запросили мгновенные, а запросили архив
		 if (type==12) if (Archive761[pdr-1].type!=12) continue;
		 if (type!=12 && channal && (channal!=Archive761[pdr-1].numCH)) continue;
		 if (type && type!=12) if (Archive761[pdr-1].type != interval) continue;
		 if (type==15) type=0;
		 //Msp.pipe = 1;
		 Msp.quant= 30;
		 Msp.type = interval;
		 Msp.start=fst;
		 Msp.end=ffn;
		 Msp.pipe = DeviceRU[device].pipe; pip=Msp.pipe;
		 Rep.pipe = Msp.pipe;
		 Rep.start = Msp.end;
		 if (Archive761[pdr-1].type)	{ Rep.type=Archive761[pdr-1].type; arch = TRUE;} else arch = FALSE; 
		 if (Archive761[pdr-1].type>10) pip=0;
		 //ULOGW("Rep.type=%d | Msp.pipe=%d | Msp.type=%d",Rep.type,Msp.pipe,Msp.type); 
		 //if (Rep.type!=Msp.type) continue; // не тот тип данных
		 Msp.pipe=pip;
		 //ULOGW("port %d(%d) <%d/%d>",DeviceRU[device].com,arch,pdr,DeviceRU[device].tag_num);
		 if (!arch)
			{
			 if (interval<88) strcpy (Out,FormSequence((UCHAR)DeviceRU[device].device,0x1d,0,Archive761[pdr-1].adr,DeviceRU[device].pipe, Msp));
			 else strcpy (Out,FormSequence((UCHAR)DeviceRU[device].device,0xc,0,Archive761[pdr-1].adr,DeviceRU[device].pipe, Msp));
			 max_t=2;
			}
		 else 
			{
			 Msp.type = Archive761[pdr-1].type; Msp.quant = Archive761[pdr-1].quant;
			 memcpy (&Msp.end,&ffn,8);
			 memcpy (&Msp.start,&fst,8);
			 max_t=10;
			 strcpy (Out,FormSequence((UCHAR)DeviceRU[device].device,0xe,0,Archive761[pdr-1].adr,pip,Msp));
			}			 
//-------------------------------------------------------------------------------------------------------------
		 PurgeComm(hCom[blok],PURGE_RXABORT|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_TXCLEAR);
		 for (UINT po=0;po<strlen(Out)+3;po++)	
			{
			 //ULOGW("o[%d] = 0x%x",po,*(Out+po));
			 WriteFile(hCom[blok], Out+po, 1, &dwBytesWritten, NULL);
			}		 
		 Sleep(1500);
		 if (max_t>2) Sleep (3500);
		 DWORD dwBytesRead=-1;
		 BOOL bSuccess=FALSE;
		 for (UINT rr=0; rr<max_t; rr++)
		 	{
			 COMSTAT comstat; DWORD dwErrors;
			 ClearCommError(hCom[blok], &dwErrors, &comstat);
			 if (comstat.cbInQue)
				{
 				 bSuccess = ReadFile(hCom[blok], sBuf1, 5000, &dwBytesRead, NULL);
				 //ULOGW("dwBytesRead = %d / GetLastError() = %d",dwBytesRead,GetLastError());
				}
		 	 if (bSuccess && dwBytesRead) break;
		 	 Sleep (1000);
		 	}

		 //ULOGW("dwBytesRead = %d / GetLastError() = %d / rr=%d",dwBytesRead,GetLastError(),rr);
		 if (dwBytesRead==-1) continue;
 		 UINT dwbr1 = dwBytesRead; 
		 if (!dwbr1) {alog.quant_param=0; return;}
		 //while (dwBytesRead!=0 && bSuccess)
		 //	{			 
		 //	 bSuccess = ReadFile(hCom[blok], sBuf2, sizeof(sBuf2), &dwBytesRead, NULL);
		 //	 ULOGW("ReadFile2 [bSuccess=%d / dwBytesRead = %d / GetLastError = %d]",bSuccess,dwBytesRead,GetLastError());
		 //	 for (i=0;i<dwBytesRead;i++) sBuf1[dwbr1+i]=sBuf2[i];
		 //	 dwbr1 = dwbr1+dwBytesRead;
		 //	}
		 sBuf1[dwbr1]=0;
		 //ULOGW("Read %d",dwbr1);
		 //for (i=0;i<dwbr1;i++) ULOGW("I[%d] = 0x%x",i,(UCHAR)sBuf1[i]);
		 //ULOGW("AnalyseSequence (%d,%d)",dwbr1,DeviceRU[device].com));
//-------------------------------------------------------------------------------------------------------------
		 if (AnalyseSequence ((CHAR*)sBuf1,dwbr1,DeviceRU[device].com))
		 if (alog.checksym)
		 for (UINT rr=0;rr<tag_num;rr++)
			{
			 GetLocalTime(&time1);
			 //ULOGW ("[%d] %d %d=%d %d=%d (%d)",rr,DRL[rr].type,DRL[rr].kanal,pdr,DRL[rr].device,device,alog.checksym);
			 if ((UINT)DRL[rr].kanal==pdr && (UINT)DRL[rr].device==device)
				{				 
				 DRL[rr].status = 1; //Dspl[rr].status = 1;
				 DRL[rr].value[0] = 0;
				 ULOGW("type = %d / alog=%s / quant=%d",DRL[rr].type,alog.data[0],alog.quant_param);
				 if (DRL[rr].type==0)
						{
						 strcpy (DRL[rr].value,alog.data[0]);
						 DRL[rr].status=1;
						 alog.quant_param=1;
						 //strcpy (Dspl[rr].var,alog.data[0]);
						 //sprintf (Dspl[rr].var,"%f",(DOUBLE) ((DOUBLE)Random()/(DOUBLE)107374182));
						 //for (UINT pp=0; pp<strlen(Dspl[rr].var);pp++) if (Dspl[rr].var[pp]=='.') Dspl[rr].var[pp]=',';
						 //DataRU[fpos].value = (DOUBLE) ((DOUBLE)Random()/(DOUBLE)107374182);
						}
				 if (DRL[rr].type==1)
					{
					 strcpy (DRL[rr].value,"");
					 if (alog.quant_param)
					 for (UINT j=0; j<alog.quant_param; j++)
						{
						 //ULOGW ("%s|%s",alog.time[j],alog.data[j]);						 
						 //strcpy (Rep.name[Answer],alog.type[j]);
						 //strcpy (Rep.value[Answer][j],alog.data[j]);
						 //strcpy (Rep.time[Answer][j],alog.time[j]);						 
						}
					 GetLocalTime(&time2);
					}					 
				 if (strstr(alog.data[0],"Дата/время"))
					{
					 DRL[rr].status = 4;
					 strcpy (DRL[rr].value,"0");
					}
				 if (strstr(alog.data[0],"Пара")) 
					{
					 DRL[rr].status = 4;
					 strcpy (DRL[rr].value,"0");
					}
				 if (strstr(alog.data[0],"Нет") || strstr(alog.data[0],"испол"))
					{
					 DRL[rr].status = 5;
					 ULOGW ("net dannih");
					 alog.quant_param=250;
					 strcpy (DRL[rr].value,"0");
					}
				 Answer++; rr=tag_num+1;
				}
			 else DRL[rr].status = 1;
			}
		 Sleep (100);
		}

	 SYSTEMTIME sts,curr;
	 FILETIME ftt; SHORT ks;
	 GetLocalTime (&curr);
	 GetLocalTime (&sts);
	 // проверка времени 	 
	 // если 1 апреля и время меньше 10 минут (30.03.2007 15:00)
	 if (curr.wDay==25 && curr.wMonth==4 && curr.wHour==2 && curr.wMinute<20)
		{		 
		 // на зимнее время переходят обычно в 3:00 последнего воскресенья октября -1 час
		 ULOGW ("winter time prog");	
		 sts.wSecond=0; sts.wMonth=10; sts.wHour=3; sts.wMinute=0; sts.wDay=31;
		 SystemTimeToFileTime(&sts,&ftt);
		 FileTimeToSystemTime(&ftt,&sts);
		 sts.wDay=31-sts.wDayOfWeek;
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c%02d-%02d-%02d%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,1+48,HT,1+48,FF,HT,sts.wDay,sts.wMonth,sts.wYear-2000,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н01
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c%02d-%02d-%02d%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,2+48,HT,1+48,FF,HT,3,0,0,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н02			 
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c-1%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,3+48,HT,1+48,FF,HT,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н03
		}
	 if (curr.wDay==1 && curr.wMonth==11 && curr.wHour==2 && curr.wMinute<20)
		{		 
		 // на летнее время в 2:00 последнего воскресенья марта +1 час
		 sts.wSecond=0; sts.wMonth=3; sts.wHour=2; sts.wMinute=0; sts.wDay=31;
		 SystemTimeToFileTime(&sts,&ftt);
		 FileTimeToSystemTime(&ftt,&sts);
		 sts.wDay=sts.wDay-sts.wDayOfWeek;			 
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c%02d-%02d-%02d%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,1+48,HT,1+48,FF,HT,sts.wDay,sts.wMonth,sts.wYear-2000,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н01
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c%02d-%02d-%02d%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,2+48,HT,1+48,FF,HT,2,0,0,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н02
		 sprintf (Out,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c22%c%c%c%c%c%c-1%c%c%c",DLE,SOH,DeviceRU[device].device,0x80,DLE,ISI,0x14,0x53,0x34,DLE,STX,HT,0+48,HT,HT,3+48,HT,1+48,FF,HT,1+48,FF,DLE,ETX);
		 ks = CRCode ((UCHAR*)Out+2, strlen(Out)-2);
		 sprintf (Out,"%s%c%c",Out,(UCHAR)(ks>>8),(UCHAR)(ks%256));		 
		 WriteToPort (DeviceRU[device].com, device, Out);	// 022н03
		}
	}
 //ULOGW("PollDeviceCOM (1) complete"); 
}
//---------------------------------------------------------------------------------------------------
BOOL ReadAPC79 (SHORT adr, SHORT blok)
{
ULOGW ("ReadAdrAPC79 (%d) %d",blok,adr);
UINT dwbr1;
UINT len3=strlen((CHAR*)sBuf3);
UINT len52=strlen((CHAR*)sBuf52);
UINT len99=strlen((CHAR*)sBuf99);
//UINT len33=strlen((CHAR*)sBuf99);
SHORT res = CRCode (sBuf52+2,len52-2); sBuf52[len52]=(UCHAR)(res>>8); sBuf52[len52+1]=(UCHAR)(res%256);
res = CRCode (sBuf3+2,len3-2); sBuf3[len3]=(UCHAR)(res>>8); sBuf3[len3+1]=(UCHAR)(res%256);
res = CRCode (sBuf99+2,len99-2); sBuf99[len99]=(UCHAR)(res>>8); sBuf99[len99+1]=(UCHAR)(res%256);
//res = CRCode (sBuf33+2,len33-2); sBuf33[len33]=(UCHAR)(res>>8); sBuf33[len33+1]=(UCHAR)(res%256);
for (UINT nump=0;nump<10;nump++)
	{
	 if (adr==3)  
		{
		 WriteFile(hCom[blok], sBuf3, sizeof(sBuf3)-1, &dwBytesWritten, NULL); 
		 //for (UINT i=0;i<strlen((CHAR*)sBuf3);i++) ULOGW("[%d] = 0x%x",i,(UCHAR)sBuf3[i]);
		}
	 if (adr==99) 
		{
		 WriteFile(hCom[blok], sBuf99, sizeof(sBuf99)-1, &dwBytesWritten, NULL); 
		 //for (UINT i=0;i<strlen((CHAR*)sBuf99);i++) ULOGW("[%d] = 0x%x",i,(UCHAR)sBuf99[i]);
		}
	 if (adr==52) 
		{
		 WriteFile(hCom[blok], sBuf52, sizeof(sBuf52)-1, &dwBytesWritten, NULL); 
		 //for (UINT i=0;i<strlen((CHAR*)sBuf52);i++) ULOGW("[%d] = 0x%x",i,(UCHAR)sBuf52[i]);
		}
	 Sleep (nump*100+500);
	 BOOL bSuccess = ReadFile(hCom[blok], sBuf1, sizeof(sBuf1), &dwBytesRead, NULL);
	 dwbr1 = dwBytesRead;
	 while (dwBytesRead!=0 && bSuccess)
		{			 
		 bSuccess = ReadFile(hCom[blok], sBuf2, sizeof(sBuf2), &dwBytesRead, NULL);
		 for (UINT i=0;i<dwBytesRead;i++) sBuf1[dwbr1+i]=sBuf2[i];
		 dwbr1 = dwbr1+dwBytesRead;
		}
	 sBuf1[dwbr1] = 0;
	 ULOGW ("ReadFile %d",dwbr1);
	 //for (UINT i=0;i<dwbr1;i++) ULOGW(">>[%d] = 0x%x",i,(UCHAR)sBuf1[i]);
	 if (dwbr1>5) nump=10;
	}
if (dwbr1!=0)
	{
	 //ULOGW ("AnalyseSequence");
	 if (AnalyseSequence ((CHAR*)sBuf1,dwbr1,blok))
		{
		 //ULOGW("[%d](%x->%x) %x %s|[%d](%d) (%d-%d) [%s %s]",alog.checksym,alog.from,alog.to,alog.func,alog.head,alog.pipe,alog.npAdr,alog.from_param,alog.quant_param,alog.data[0],alog.type[0]);
		 if (alog.checksym)
			{
			 if (adr==3) strcpy(apc.spec,alog.data[0]);		// Спецификация внешнего оборудования. peslraahhv.
			 if (adr==99) strcpy(apc.version,alog.data[0]);	// Версия адаптера.
			 if (adr==52) apc.period = atoi (alog.data[0]);	// Периодичность опроса
			 return TRUE;
			}
		}
	}
return FALSE;
}
//---------------------------------------------------------------------------------------------------
BOOL ReadCPT (SHORT adr, SHORT blok, SHORT pAdr)
{
 CHAR sBuf2[400];	// buffer for rs-232
 CHAR sBuf1[7000];
 CHAR Out[150];
 SPrint Msp={0};
 
 for (UINT i=0;i<30;i++) sBuf1[i]=0;
 strcpy (Out,FormSequence((UCHAR)adr,0x1d,0,pAdr,0, Msp)); 
 //for (i=0;i<strlen(Out);i++) ULOGW("o[%d] = 0x%x",i,(UCHAR)Out[i]);
 WriteFile(hCom[blok], Out, strlen(Out), &dwBytesWritten, NULL); 
 Sleep(2200);
 BOOL bSuccess = ReadFile(hCom[blok], sBuf1, 1000, &dwBytesRead, NULL);
 UINT dwbr1 = dwBytesRead;
 while (dwBytesRead!=0 && bSuccess)
	{			 
	 bSuccess = ReadFile(hCom[blok], sBuf2, 200, &dwBytesRead, NULL);
	 for (i=0;i<dwBytesRead;i++) sBuf1[dwbr1+i]=sBuf2[i];
	 dwbr1 = dwbr1+dwBytesRead;
	}
 sBuf1[dwbr1] = 0;
 //for (i=0;i<dwbr1;i++) ULOGW("i[%d] = 0x%x",i,(UCHAR)sBuf1[i]);
 if (AnalyseSequence ((CHAR*)sBuf1,dwbr1,blok)) return TRUE;
 else return FALSE;
}
//----------------------------------------------------------------------------------------
BOOL AnalyseSequence (CHAR* dat,UINT len, UINT blok)
{
 CHAR buffer[500]; UINT quantZp=0;
 UINT i=0,start=0, startm=0;
 //ULOGW ("AnalyseSequence (%d)",len);
 dat[len]=0; dat[len+1]=0x9;
 alog.checksym = FALSE;
 while (i<len)
	{
	 //ULOGW ("<F1 dat[%d]=%d",i,dat[i]);
	 if (dat[i]==DLE && dat[i+1]==SOH) 
		{		 
		 if (dat[i+2]!=DLE)
			{
			 alog.from = (UCHAR)dat[i+3];
			 alog.to = (UCHAR)dat[i+2];			 
			 startm=i+2; i=i+4;
			}
		 else 
			{
			 i=i+2; startm=i;
			}
		 //ULOGW ("dat[%d]=%x = dat[%d]=%x len=%d",i,dat[i],i+1,dat[i+1],len);
		 while (i<len)
			{
			 //ULOGW ("<F3 dat[%d]=%d",i,dat[i]);
			 if (dat[i]==DLE && dat[i+1]==STX) break;
			 if (dat[i]==DLE && dat[i+1]==ISI)
				{ 
				 alog.func=(UCHAR)dat[i+2];
				 start=i+3; // DataHead
				 //ULOGW("[ISI] = 0x%x 0x%x 0x%x",dat[i],dat[i+1],dat[i+2]);
				 //ULOGW("alog.func = 0x%x",dat[i+2]);
				}			 
			 i++;
			}		  
		}
	 //ULOGW ("<F11 dat[%d]=%d",i,dat[i]);
	 if (dat[i]==DLE && dat[i+1]==STX)
		{
		 //ULOGW ("%d %d",dat+start,i-start);
		 strncpy (alog.head,dat+start,i-start);
		 i=i+2;
		 //ULOGW ("F3 dat[%d]=%d",i,dat[i]);
		 if (dat[i]==HT) 
			{
			 alog.pipe = dat[i+1]-48; i=i+2;
			 //ULOGW ("alog.pipe=%d",alog.pipe);
			 if (dat[i]==HT)
				{
				 //ULOGW ("F5 dat[%d]=%d",i,dat[i]);
				 i++; start=i;
				 while (dat[i]!=FF && dat[i]!=HT && i<len) i++; // !!!!!!05.12.2006 
				 //ULOGW ("[%d] %d -> %d",i,dat+start,i-start);
				 strncpy (buffer,dat+start,i-start);				 
				 alog.npAdr = atoi (buffer);
				 //ULOGW ("alog.to [0x%x] alog.from [0x%x] pipe=%d npAdr = %d func = 0x%x",alog.from,alog.to,alog.pipe,alog.npAdr,alog.func);
				 if (!alog.npAdr) return FALSE;
				 alog.from_param = 0;
				 alog.quant_param = 0;
				 if (alog.func==0x14)
					{
					 i++; start=i;
					 while (dat[i]!=FF && dat[i]!=HT && i<len) i++; // !!!!!!05.12.2006
					 strncpy (buffer,dat+start,i-start);
					 alog.from_param = atoi (buffer);
					 i++; start=i;
					 while (dat[i]!=FF && dat[i]!=HT && i<len) i++; // !!!!!!05.12.2006
					 strncpy (buffer,dat+start,i-start);
					 alog.quant_param = atoi (buffer);
					}
				 if (alog.func==0x7f) return TRUE;
				 if (alog.func==0x16) 
					{
 					 i++; start=i;
					 while (dat[i]!=FF && i<len) i++; // !!!!!!05.12.2006
					 i++;
					 while (dat[i]!=FF && i<len) i++; // !!!!!!05.12.2006
					 //UL_INFO ((LOGID,"start from = %d",i));
					}				 
	 			 UINT pos=0;
				 CHAR datt[8000];
				 CHAR value[ARCHIVE_NUM_MAX][30];
				 if (len>i+10)
					{
					 strncpy (datt,dat+i+2,len-i-7);
					 for (UINT o=0;o<=len-i-7;o++)
						{
						 if (*(dat+i+2+o)==0xc && *(dat+i+3+o)==0x10 && *(dat+i+4+o)==0x3) { datt[o]=0x9; datt[o+1]=0; o=len-i-6;}
						 else datt[o]=(UCHAR)*(dat+i+2+o);
						 #ifdef LOGS
						 ULOGW ("datt[%d]=%x",o,datt[o]);
						 #endif
						}
					 datt[o]=0;
					 CHAR* token = strtok(datt,"\t");
					 while(token!=NULL)
						{
						 if (strlen(token)<29) sprintf (value[pos],token);
						 else sprintf (value[pos]," ");
						 //ULOGW ("value[%d]=%s",pos,value[pos]);
						 token = strtok(NULL,"\t"); pos++;
						}
					}
				 else { value[0][0]=0; value[1][0]=0; value[2][0]=0; }
				 if (alog.func==0x16)
					{
					 //ULOGW ("log[blok].func==0x16");
					 // HT Значение HT Единицы HT Метка времени
					 // HT Значение HT HT Метка времени
					 BOOL some_ed=FALSE;
					 BOOL no_ed=FALSE;
					 //UL_INFO ((LOGID,"%s %s %s %s %s",value[0],value[1],value[2],value[3],value[4]));
					 // value[0]=-в1-03-03
					 // value[1]= 
					 // value[2]=06-09-06/10:26:20
					 // log[blok].func==0x16
					 // [] ( ) -в1-03-03
					 // [] () 06-09-06/10:26:20
					 if (strlen(value[4])>14 && strlen(value[2])>14)
						{ 
						 some_ed=TRUE;
						 //UL_INFO ((LOGID,"%d=%d",strlen(value[4]),strlen(value[6])));
						}
					 if (strlen(value[1])>14 && alog.npAdr>100) no_ed=TRUE;
					 for (UINT j=0,r=0;j<pos;j++)
						{
						 if (pos<2 || (!strlen(value[j]) && !strlen(value[j+1]))) continue;
						 strcpy (alog.data[r],value[j]); // -в1-03-03
						 j++;							 
						 if (!no_ed)		
							{
							 if (some_ed) strcpy (alog.type[r],value[1]); 
							 if (!some_ed) { strcpy (alog.type[r],value[j]);	j++;} // -
							 if (j==1 && some_ed) j++;
							}
						 else strcpy (alog.type[r],"-");	 
						 strcpy (alog.time[r],value[j]);
						 UINT ln = strlen(alog.time[r]);
						 alog.time[r][ln-1]=0;						 
						 //ULOGW ("[%s] (%s) %s",alog.time[r],alog.type[r],alog.data[r]);
						 r++; quantZp++;
						 if (ln<15) j--;
						}
					 alog.quant_param = quantZp;
					 //ULOGW ("quantZp==%d",quantZp);
					}
				if (alog.func==0x14 || alog.func==0x3)
					{
					 strncpy (alog.data[0],value[0],17); alog.data[0][17]=0;
					 strncpy (alog.type[0],value[1],17); alog.type[0][17]=0;
					 strcpy (alog.time[0],"");
					 alog.quant_param = 1;
					 //if (alog.npAdr>100) sprintf (alog.data[0],"%f",(DOUBLE)((DOUBLE)rand()/(DOUBLE)10737));
					}
				}
			 else { ULOGW ("HT1 %x %x %x",dat[i-1],dat[i],dat[i+1]); return FALSE; }
			}
		 else { ULOGW ("HT2 %x %x %x",dat[i-1],dat[i],dat[i+1]); return FALSE; }
		}
	 if (dat[i]==DLE && dat[i+1]==ETX)
		{		 
		 SHORT ks=CRCode ((UCHAR*)dat+startm,len-startm-2);
		 //ULOGW ("KS [%d->%d] %x",startm,len-startm-2,ks);
		 if (((UCHAR)(ks/256)==(UCHAR)dat[i+2])&&((UCHAR)(ks%256)==(UCHAR)dat[i+3])) alog.checksym = TRUE;
		 else alog.checksym = TRUE;
		 //else alog.checksym = FALSE;

		 //UL_DEBUG ((LOGID,"([%x %x] [%x %x])",(UCHAR)(ks>>8),(UCHAR)ks%256,(UCHAR)dat[i+2],(UCHAR)dat[i+3]));
		 //break;
		 //UL_DEBUG ((LOGID,"alog.checksym=%d",alog.checksym));
		 return TRUE;
		}
	 i++;
	}
 return TRUE;
}
//---------------------------------------------------------------------------------------------------
// (10 01) (10 1F) 1D (10 02) 09 30 09 33 0C (10 03) 74 D3 	
// (10 01) 01 80 (10 1F) 1D 53 30 (10 02) 09 30 09 39 39 0C (10 03) 61 81 	
CHAR* FormSequence (UCHAR lNum, UCHAR nFunc, UCHAR nChannal, UINT npAdr, UCHAR nPipe, SPrint Msp)
{
 CHAR buf[300]; CHAR *pbuf=buf;  CHAR buffer[13][50];
 sprintf (buf,"i mozhesh ne otvechat");
 if (nFunc==0x1d) 
	{
	 if (npAdr!=358) sprintf (buf,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%s%c%c%c",DLE,SOH,lNum,0x80,DLE,ISI,nFunc,0x53,0x30,DLE,STX,HT,nPipe+48,HT,_itoa(npAdr,buffer[0],10),FF,DLE,ETX);
	 else sprintf (buf,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c358%c%c%c",DLE,SOH,lNum,0x80,DLE,ISI,nFunc,0x53,0x30,DLE,STX,HT,nPipe+48,HT,FF,DLE,ETX);
	 //ULOGW ("[A%d](%d)",nPipe,npAdr);
	}
 if (nFunc==0xc) 
	{
	 sprintf (buf,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%s%c%c%c%c%c%c%c",DLE,SOH,lNum,0x80,DLE,ISI,nFunc,0x53,0x30,DLE,STX,HT,nPipe+48,HT,_itoa(npAdr,buffer[0],10),HT,0x32,HT,0x31,FF,DLE,ETX);
	 //ULOGW ("[A%d](%d)",nPipe,npAdr);
	}
 if (nFunc==0xe)  
	{	 
	 SYSTEMTIME stb,ste;
	 //tmy=ty.wHour+ty.wDay*100+ty.wMonth*100*100+ty.wYear*100*100*100;
	 //[/crq?req=archive&type=v&n1=1&n2=15&t1=20061205125950w&t2=20061205125950w&interval=main]
	 //[A2](1)(960)[25.03.2007 01:59 - 25.03.2007 03:59]
		// [25.03.2007 02:59 - 25.03.2007 04:59]
	 //[A2](1)(960)[28.10.2007 02:59 - 28.10.2007 04:59]
		// [25.03.2007 02:59 - 25.03.2007 04:59]
	 //if (tmy>tmy_l && tmy<tmy_w) ;

	 FileTimeToSystemTime(&Msp.start,&stb);
	 FileTimeToSystemTime(&Msp.end,&ste);
	 ULOGW ("[A%d](%d)(%d)[%d.%d.%d %d:%d - %d.%d.%d %d:%d]",Msp.pipe,Msp.type,Msp.quant,stb.wDay,stb.wMonth,stb.wYear,stb.wHour,stb.wMinute,ste.wDay,ste.wMonth,ste.wYear,ste.wHour,ste.wMinute);
	 sprintf (buf,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%s%c%c%s%c%s%c%s%c%s%c%s%c%s%c%c%s%c%s%c%s%c%s%c%s%c%s%c%c%c",DLE,SOH,lNum,0x80,DLE,ISI,nFunc,0x53,0x30,DLE,STX,HT,nPipe+48,HT,_itoa(npAdr,buffer[0],10),FF,
			  HT,_itoa(ste.wDay,buffer[1],10),HT,_itoa(ste.wMonth,buffer[2],10),HT,_itoa(ste.wYear,buffer[3],10),HT,_itoa(ste.wHour,buffer[4],10),HT,_itoa(ste.wMinute,buffer[5],10),HT,_itoa(ste.wSecond,buffer[6],10),FF,
			  HT,_itoa(stb.wDay,buffer[7],10),HT,_itoa(stb.wMonth,buffer[8],10),HT,_itoa(stb.wYear,buffer[9],10),HT,_itoa(stb.wHour,buffer[10],10),HT,_itoa(stb.wMinute,buffer[11],10),HT,_itoa(stb.wSecond,buffer[12],10),FF,
			  DLE,ETX);
	 //UL_INFO ((LOGID,"[%s]",buf));
	}
 SHORT ks = CRCode ((UCHAR*)buf+2, strlen(buf)-2);
 sprintf (buf,"%s%c%c",buf,(UCHAR)(ks>>8),(UCHAR)(ks%256));
 return pbuf;
}
//---------------------------------------------------------------------------------------------------
// Функция вычисляет и возвращает циклический код для последовательности из len байтов, указанной *msg.
// Используется порождающий полином: (X в степени 16)+(X в степени 12)+(X в степени 5)+1.
// Полиному соответствует битовая маска 0x1021.
SHORT CRCode (BYTE *msg, INT len)
{
SHORT crc,j;
crc = 0;
while (len-- > 0)
{
crc = crc ^ (INT) *msg++ << 8;
for (j=0;j<8;j++)
{
if(crc&0x8000) crc = (crc << 1) ^ 0x1021;
else crc <<= 1;
}
}
return crc;
}
//---------------------------------------------------------------------------------------------------
BOOL OpenCom (SHORT blok, UINT speed)
{
DCB dcb = {0};								// com port settings struct 
DWORD dwStoredFlags;
COMSTAT comstat;
dwStoredFlags = EV_RXCHAR | EV_TXEMPTY | EV_RXFLAG;	
COMMTIMEOUTS timeouts;
timeouts.ReadIntervalTimeout = 5;			//20
timeouts.ReadTotalTimeoutMultiplier = 1;
timeouts.ReadTotalTimeoutConstant = 90;	//190
timeouts.WriteTotalTimeoutMultiplier = 0; 
timeouts.WriteTotalTimeoutConstant = 20;	//25
switch (blok)
	{
	 case 0: hCom[blok] = CreateFile(_T("COM1:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"COM1"); break;
	 case 1: hCom[blok] = CreateFile(_T("COM2:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"COM2"); break;
	 case 2: hCom[blok] = CreateFile(_T("COM3:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"COM3"); break;
	 case 3: hCom[blok] = CreateFile(_T("MSP1:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP1"); break;
	 case 4: hCom[blok] = CreateFile(_T("MSP2:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP2"); break;
	 case 5: hCom[blok] = CreateFile(_T("MSP3:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP3"); break;
	 case 6: hCom[blok] = CreateFile(_T("MSP4:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP4"); break;
	 case 7: hCom[blok] = CreateFile(_T("MSP5:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP5"); break;
	 case 8: hCom[blok] = CreateFile(_T("MSP6:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP6"); break;
	 case 9: hCom[blok] = CreateFile(_T("MSP7:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP7"); break;
	 case 10:hCom[blok] = CreateFile(_T("MSP8:"),GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); strcpy (Com,"MSP8");
	}
ULOGW("Attempt open COM-port %s [%d] ( hCom[%d])",Com,speed,blok);	// write in log
if (hCom[blok] == INVALID_HANDLE_VALUE)
	{
	 ULOGW ("Open com-port: error! %d",GetLastError ());
	 return -1;
	}
if (!GetCommState(hCom[blok], &dcb))							// get current DCB
	{
	 ULOGW ("GetCommState: error!");	 
	 return -1;	
	}
GetCommState(hCom[blok],&dcb);
dcb.BaudRate = speed;
dcb.Parity = NOPARITY;
dcb.StopBits = ONESTOPBIT;
dcb.ByteSize = 8;
dcb.fDsrSensitivity = FALSE;
dcb.fOutxCtsFlow = FALSE;
dcb.fOutxDsrFlow = FALSE;
dcb.fOutX = FALSE;
dcb.fInX = FALSE;
SetCommState(hCom[blok], &dcb);
SetCommTimeouts(hCom[blok], &timeouts);
ULOGW ("Set com-port. Speed %d. Parity: NOPARITY",speed);
DWORD dwErrors;
if (!ClearCommError(hCom[blok], &dwErrors, &comstat)) 
	{
	 ULOGW ("Failed in call to ClearCommError");
	}
return TRUE;
}
//---------------------------------------------------------------------------------------------------
// ReadMConf (blok, blok, DeviceRU[devNum].name, DeviceRU[devNum].tag_num);
VOID ReadMConf (INT dev, SHORT blok, CHAR* name, UINT tagn)
{
UINT max=1;
//if (DeviceRU[dev].type==3) max=1; else max=MAX_PIPE_NUMS;
for (UINT slot=1; slot<=max; slot++)
for (UINT r=1; r<=tagn; r++)
{
 DRL[tag_num].nmodule = slot;
 DRL[tag_num].kanal = r;
 DRL[tag_num].device = dev;
 sprintf (DRL[tag_num].module, name);
 DRL[tag_num].status = 0;
 strcpy (DRL[tag_num].value,"0.0");
 DRL[tag_num].type = 0;
// printf ("[logika] Tag [%d] [Device %d.Module %d %s.Channel %d]\n", tag_num, dev, slot, DRL[tag_num].module, r);
 ULOGW ("Tag [%d] [Device %d.Module %d %s.Channel %d]", tag_num, dev, slot,DRL[tag_num].module, r);
 tag_num++;
}
}
//------------------------------------------------------------------------------------
INT init_tags(VOID)
{
 ULOGW("init_tags(%d)",tag_num);
 FILETIME ft;		//  64-bit value representing the number of 100-ns intervals since January 1,1601
 SYSTEMTIME st;
 BOOL arch=FALSE;
 GetSystemTime(&st);	// retrieves the current system date and time
 SystemTimeToFileTime(&st,&ft);
 if (!tag_num) return 0;
 for (UINT i=0; i<devNum; i++)
	{
	 UINT max = (UINT)DeviceRU[i].tag_num;
	 for (UINT r=1; r<=max; r++)
		{
		 //sprintf(Dspl[r-1].name,"unknown tag %d",i);
		 if (!Archive761[r-1].type) arch=FALSE;
		 else arch=TRUE;
		 if (!arch) DRL[tTotal].type = 0;		// instant
		 else	 DRL[tTotal].type = 1;		// archive
		 //Dspl[r-1].timestm = ft;
		 ULOGW("[%d] AddRealTag (%s) = (t=%d)",i, Archive761[r-1].name, DRL[r-1].type);
		 tTotal++;
		}
	 } 
  return 1;
}
//--------------------------------------------------------------
VOID formParam (VOID)
{
 ver_major=2;
 ver_minor=14;
 //sprintf (ver_time,"%02d-%02d-%04d %02d:%02d:%02d.%03d",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
 sprintf (ver_time,"14-10-2006 10:00:01");
 strcpy (ver_desc,"winconn communicator");
 strcpy (sernum,"08301R200004H0500001");
 strcpy (softver,"0.4.17");
 strcpy (wsoftver,"0.83.21");
 maxmodule=1;
 kA=0; wkA=16;
}
//-------------------------------------------------------------------------------------------------
#define COMM_BUFFER_SIZE 1024
#define SMALL_BUFFER_SIZE 10
//--------------------------------------------------------------
struct HTTPRequestHeader
	{
	 CHAR method[SMALL_BUFFER_SIZE];
	 CHAR url[MAX_PATH];
	 CHAR filepathname[MAX_PATH];
	 CHAR httpversion[SMALL_BUFFER_SIZE];
     IN_ADDR client_ip;
	};
struct ClientInfo
	{
	 SOCKET *client_socket;
	 IN_ADDR client_ip;
	};
struct MimeAssociation
	{
	 CHAR *file_ext;
	 CHAR *mime;
	};
//--------------------------------------------------------------
SOCKET StartWebServer();
INT WaitForClientConnections(SOCKET server_socket);
VOID HandleHTTPRequest(VOID *data );
BOOL ParseHTTPHeader(CHAR *receivebuffer, HTTPRequestHeader &requestheader);
VOID OutputHTTPError(SOCKET client_socket, INT statuscode);
INT SocketRead(SOCKET client_socket,CHAR *receivebuffer,INT buffersize);
VOID DetermineHost(CHAR *hostname );
//---------------------------------------------------------------
BOOL hConnection (CHAR tP, INT _s_port, SOCKET &sck, SOCKET &cli)
{
 // get name of webserver machine. needed for redirects
 // gethostname does not return a fully qualified host name
 //printf ("DetermineHost(hostname)\n");
 //DetermineHost(hostname);
 // create a critical section for mutual-exclusion synchronization on cout
 //InitializeCriticalSection (&output_criticalsection);
 // init the webserver
 printf ("start web server\n");
 sck = StartWebServer();
 if (sck)
	{
     WaitForClientConnections(sck);
	 closesocket(sck);
	}
 else ULOGW ("Error in StartWebServer()");
 // delete and release resources for critical section
 //DeleteCriticalSection (&output_criticalsection);
 WSACleanup();
 return 0; 
}
//--------------------------------------------------------------
// WaitForClientConnections()
//		Loops forever waiting for client connections. On connection
//		starts a thread to handling the http transaction
//--------------------------------------------------------------
SOCKET client_socket=INVALID_SOCKET;

INT WaitForClientConnections(SOCKET server_socket)
{
 SOCKADDR_IN client_address;
 INT client_address_len;
 HANDLE hThrd=NULL;
// SOCKET client_socket=INVALID_SOCKET;
 //ClientInfo ci;
 client_address_len = sizeof(SOCKADDR_IN);
 printf ("listen incoming connection ()\n"); ULOGW("listen incoming connection ()");
 if (listen(server_socket,SOMAXCONN) == SOCKET_ERROR )
	{
	 printf ("error in listen()\n"); ULOGW("Error in listen()");
	 closesocket(server_socket);
	 return(0);
	}
 // loop forever accepting client connections. user ctrl-c to exit!
 while (1)
	{
	 if (ExitSignal)
		{
		 ULOGW("exit signal. finished");
		 return(0);
		}
	 printf ("accept socket\n"); //ULOGW("accept socket");
 	 client_socket = accept(server_socket,(struct sockaddr *)&client_address,&client_address_len);
	 //ULOGW("client_socket create [%p]",client_socket);
	 if (client_socket == INVALID_SOCKET)
		{
		 printf ("error in accept\n"); ULOGW("Error in accept()");
		 closesocket(server_socket);
		 return(0);
		}
	 //Sleep (10);
	 //if (client_socket) closesocket(client_socket);

     // copy client ip and socket so the HandleHTTPRequest thread
     // and process the request.
	 //ci = new ClientInfo;
	 //ci = (struct ClientInfo *) malloc(sizeof(struct ClientInfo));
	 //ci.client_socket=&client_socket;
	 //ci.client_ip.S_un.S_un_w=client_address.sin_addr.S_un.S_un_w;
	 //memcpy(&(ci.client_ip),&client_address.sin_addr.s_addr,4);
	 // for each request start a new thread!
	 //ddDWORD	dwThrdID;
	 if (hThrd) CloseHandle(hThrd);
	 hThrd = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) HandleHTTPRequest, NULL, 0, &dwThrdID);
	 
	 //Sleep(1000);
	 //HandleHTTPRequest(&ci);
	 //ULOGW("close client_socket close [%p]",client_socket);
	 //closesocket(client_socket);
	}
}
//--------------------------------------------------------------
// Executed in its own thread to handling http transaction
//--------------------------------------------------------------
VOID HandleHTTPRequest (VOID *data)
{
// SOCKET client_socket;
 HTTPRequestHeader requestheader;
 INT size;
 CHAR receivebuffer[COMM_BUFFER_SIZE];
 CHAR sendbuffer[COMM_BUFFER_SIZE];
// client_socket = *(SOCKET *) data;
// Sleep (10);
// if (client_socket) 
//	{
	 //closesocket(client_socket);
	 //ULOGW("closesocket (%d) [%d]",ret,WSAGetLastError());
//	}
  //ExitThread(0);
 
 //requestheader.client_ip = ((ClientInfo *)data)->client_ip;
 /*Sleep(1000);
 if (client_socket) 
	{
	 ret=closesocket(client_socket);
	 ULOGW("closesocket (%d) [%d]",ret,WSAGetLastError());
	}
 ExitThread(0);*/
 //client_socket = ci.client_socket;
 //requestheader.client_ip = ci.client_ip;
 size = SocketRead(client_socket,receivebuffer,COMM_BUFFER_SIZE);
 if (size == SOCKET_ERROR || size == 0)
	{
	 printf ("Error calling recv (%d) [%d]\n",size,WSAGetLastError());
	 ULOGW("Error calling recv (%d) [%d]",size,WSAGetLastError());
	 closesocket(client_socket);
	 return;
	}
 //ULOGW("recv %s (%d)",receivebuffer,size);
 receivebuffer[size] = NULL;
 if (!ParseHTTPHeader(receivebuffer,requestheader))
	{	 
	 OutputHTTPError(client_socket, 400);   // 400 - bad request
	 return;
	}
 if (strstr(requestheader.method,"GET"))
	{
	 // send the http header and the file contents to the browser
	 strcpy(sendbuffer,"HTTP/1.0 200 OK\r\n");

	 // form answer
	 // printf ("recieve header [%s : %s] ver: %s\n",requestheader.method,requestheader.url,requestheader.httpversion);
	 // /crq?req=version
	 if (requestheader.url[1]=='c' && requestheader.url[2]=='r' && requestheader.url[3]=='q')
		{
		 CHAR *pdest;
		 pdest = strstr(requestheader.url,"req=");
		 if(pdest!=NULL )
			{
			 CHAR buffer[10000];
			 BOOL rightreq=FALSE;
			 //printf ("%s\n",pdest);
			 #define MAX_PARAMETRS 8		// количество физических параметров на одной трубе
			 UINT max=(devNum-1)*4;
			 //rightreq=TRUE;
			 #include "logika_com.cpp"
			 ULOGW ("[%s] (%d)(%d)",requestheader.url,strlen (sendbuffer),strlen (buffer));
			 ULOGW("send %s",sendbuffer);
			 if (!rightreq)
				{
				 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
			 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
				 send(client_socket,sendbuffer,strlen(sendbuffer),0);	
 				 sprintf(buffer,"ошибка неверный запрос (параметр req)\r\n");
				 send(client_socket,buffer,strlen(buffer),0);
				}
			 //GlobalMemoryStatus(&ms);
			 //ULOGW ("mload[%d%%] mem[%d/%d] page[%d/%d] virt[%d/%d]",ms.dwMemoryLoad,ms.dwAvailPhys,ms.dwTotalPhys,ms.dwAvailPageFile,ms.dwTotalPageFile,ms.dwAvailVirtual,ms.dwTotalVirtual);
			 //-----------------------------------------------------------------------------------
			}
		 else OutputHTTPError(client_socket, 405);
		}
	 else OutputHTTPError(client_socket, 400);
	 //EnterCriticalSection (&output_criticalsection);
	 //LeaveCriticalSection (&output_criticalsection);
	 printf ("end get\n");
	}
 else
	{
	 OutputHTTPError(client_socket, 501);   // 501 not implemented
	}
 //ULOGW("client_socket close begin [%p]",client_socket);
 if (client_socket) closesocket(client_socket);
 //ULOGW("client_socket close end [%p]",client_socket);
}
//--------------------------------------------------------------
//	Reads data from the client socket until it gets a valid http
//	header or the client disconnects.
//--------------------------------------------------------------
INT SocketRead(SOCKET client_socket, CHAR *receivebuffer, INT buffersize)
{
 INT size=0,totalsize=0;
 do
	{
	 size = recv(client_socket,receivebuffer+totalsize,buffersize-totalsize,0);
	 if (size!=0 && size != SOCKET_ERROR)
		{
	 	 totalsize += size;
		 //ULOGW("recv (%s)",receivebuffer);
		 // are we done reading the http header?
		 if (strstr(receivebuffer,"\r\n\r\n")) break;
		}
	 else totalsize = size;			// remember error state for return	
	} 
 while (size!=0 && size != SOCKET_ERROR );
 return(totalsize);
}
//--------------------------------------------------------------
//		Sends an http header and html body to the client with
//		error information.
//--------------------------------------------------------------
VOID OutputHTTPError(SOCKET client_socket,INT statuscode)
{
 CHAR headerbuffer[COMM_BUFFER_SIZE];
 CHAR htmlbuffer[COMM_BUFFER_SIZE];
 sprintf(htmlbuffer,"<html><body><h2>Error: %d</h2></body></html>",statuscode);
 sprintf(headerbuffer,"HTTP/1.0 %d\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n",statuscode,strlen(htmlbuffer));
 send(client_socket,headerbuffer,strlen(headerbuffer),0);
 send(client_socket,htmlbuffer,strlen(htmlbuffer),0);
 printf ("OutputHTTPError [%d]\n",statuscode); ULOGW ("OutputHTTPError [%d]",statuscode);
// closesocket(client_socket);
}
//--------------------------------------------------------------
// Fills a HTTPRequestHeader with method, url, http version
// and file system path information.
//--------------------------------------------------------------
BOOL ParseHTTPHeader(CHAR *receivebuffer, HTTPRequestHeader &requestheader)
{
 CHAR *pos;
 // http request header format method uri httpversion
 EnterCriticalSection (&output_criticalsection);
 LeaveCriticalSection (&output_criticalsection);
 // end debuggine	
 pos = strtok(receivebuffer," ");
 if (pos == NULL) return(FALSE);
 strncpy(requestheader.method,pos,SMALL_BUFFER_SIZE);
 pos = strtok(NULL," ");
 if (pos == NULL) return(FALSE);
 strncpy(requestheader.url,pos,MAX_PATH);
 pos = strtok(NULL,"\r");
 if (pos == NULL) return(FALSE);
 strncpy(requestheader.httpversion,pos,SMALL_BUFFER_SIZE);
 printf ("recieve header [%s : %s] ver: %s\n",requestheader.method,requestheader.url,requestheader.httpversion);
 //ULOGW ("[%s : %s] ver: %s",requestheader.method,requestheader.url,requestheader.httpversion);
 //ULOGW ("[%s]",requestheader.url);
 // based on the url lets figure out the filename + path
 strncpy(requestheader.filepathname,wwwroot,MAX_PATH);
 strncat(requestheader.filepathname,requestheader.url,MAX_PATH);
 return(TRUE);
}
//--------------------------------------------------------------
//	Creates server sock and binds to ip address and port
//--------------------------------------------------------------
SOCKET StartWebServer()
{
 SOCKET s;
 INT rc = WSAStartup(MAKEWORD(2, 2), &WSAData);
 printf ("WSAStartup.....");
 if(rc != 0)
	{
	 printf ("[error]\n");
	 ULOGW ("WSAStartup failed. Error: %x",WSAGetLastError ());
	 Sleep (10000);
	 return(0);
	}
 else printf ("[success]\n");
 printf ("create socket....."); ULOGW ("create socket");
 s = socket(AF_INET,SOCK_STREAM,0);
 if (s==INVALID_SOCKET)
	{
	 printf ("[error]\n"); ULOGW("Error creating sock()");
	 Sleep (10000);
	 return(0);
	}
 else printf ("[success]\n");
 SOCKADDR_IN si;
 si.sin_family = AF_INET;
 si.sin_port = htons(80);		// port
 si.sin_addr.s_addr = htonl(INADDR_ANY); 
 printf ("bind socket\n"); ULOGW ("bind socket");
 if (bind(s,(struct sockaddr *) &si,sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
 	 ULOGW("Error in bind()");
	 closesocket(s);
	 Sleep (10000);
	 return(0);
	}
 return(s);
}
//--------------------------------------------------------------
// If webserver needs to redirect user from directory to 
// default html file the server builds a full url and hence
// needs it's full domain name for http address.
//--------------------------------------------------------------
VOID DetermineHost(CHAR *hostname )
{
 IN_ADDR in;
 hostent *h;
 gethostname(hostname,MAX_PATH);
 h = gethostbyname(hostname);
 memcpy(&in,h->h_addr,4);
 h = gethostbyaddr((CHAR *)&in,4,PF_INET);
 strcpy(hostname,h->h_name);
}
