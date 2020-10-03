//----------------------------------------------------------------------------
#define KERNEL_LOG	"\\Compact Flash\\kernel.log"
#define	SNS	1			//"Переменная определена - с датчиками"
#define	TIC_TIMER	1000	// временной срез (мс) для системного таймера
//----------------------------------------------------------------------------
#include <windows.h>
#include <commctrl.h>
#include <winsock.h>
#include <wincon.h>
#include <string.h>
#include "..\error.h"
#include "kernel32.h"
//----------------------------------------------------------------------------
DWORD prtime=0,prtime2=0,prtime3=0;
#include "function.h"
#include "httpsrv.h"
//----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, INT nCmdShow)
{
 MSG msg;
 Log = fopen(KERNEL_LOG,"w"); fclose (Log);
 if(hPrevInstance!=0)				// Была запущ. копия приложения?
	{
	 ULOGW("Error: application already running");
	 return FALSE;
	}
 // Perform application initialization:
 if (!InitInstance (hInstance, nCmdShow)) 
	{
	 ULOGW("Error: failed in InitInstance");
	 return FALSE;
	}
 // Main message loop:
 while (GetMessage(&msg, NULL, 0, 0)) 
	{
 	 TranslateMessage(&msg);
	 DispatchMessage(&msg);
	}
 return msg.wParam;
}
//-----------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 //INT wmId, wmEvent;
 HDC hdc;
 PAINTSTRUCT ps;	
 switch (message) 
	{
	 //----------------------------------------------------------------------
	 case WM_CREATE:
		ULOGW("create directory ");
		CreateDirectory(BASEDIR,NULL);
		RDspl_length=0;
		//-------------------------------------------------------------------------------		
		DataM = (BYTE FAR*) calloc (kStrKB*2, sizeof(CycleBuffer));	// for archive data
		ULOGW("Recovering cycle buffer...");
		recoveringKB();
		//-------------------------------------------------------------------------------
		ULOGW("create message log ");
		WriteToLog("");				// создаем лог, если его не было
		WriteToLog("20000f01\t// -Enabling the system-");	// пишем момент включения
		ULOGW("read configuration ");
		//-------------------------------------------------------------------------------
		fBuf = fopen (CFG2,"rb"); if(fBuf==0) ULOGW("cannot open file cfg2.txt");
		fseek(fBuf, 0, SEEK_END);
		TaskDues_length = ftell(fBuf);							
		fseek(fBuf, 0, SEEK_SET);
		fclose(fBuf);
		TaskDues =	(BYTE FAR*) calloc (1, TaskDues_length);
		memcpy(package,"cfg2.txt",9);	package[9]= HIBYTE(TaskDues_length);
										package[10]=LOBYTE(TaskDues_length);
		if(NetDataWR(package, 0)==TRUE)
			{
			 ULOGW("data read from cfg2.txt >> TaskDues");
			 memcpy(TaskDues,package+11,TaskDues_length);
			}
		//-------------------------------------------------------------------------------
		fBuf = fopen (CFG3,"rb"); if(fBuf==0) ULOGW("cannot open file cfg3.txt");
		fseek(fBuf, 0, SEEK_END);
		TaskMath_length = ftell(fBuf)-1;
		fseek(fBuf, 0, SEEK_SET);
		fclose(fBuf);
		TaskMath =	(BYTE FAR*) calloc (1, TaskMath_length);
		memcpy(package,"cfg3.txt",9);	package[9]= HIBYTE(TaskMath_length+1);
										package[10]=LOBYTE(TaskMath_length+1);
		if(NetDataWR(package, 0)==TRUE) 
			{
			 ULOGW("data read from cfg3.txt >> ChInData");
			 memcpy(TaskMath,package+12,TaskMath_length);
			 indata=(MainStruct*) (TaskMath);
			 memcpy(&ChInData,package+11,1); 
			}		
		PromRez		= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof(CycleBuffer));
		PromRezHrs	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof(CycleBuffer));	
		PromRezDay	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof(CycleBuffer));	
		PromRezMonth= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof(CycleBuffer));
		PromRezYear = (BYTE FAR*) calloc (ChInData*klvPrm, sizeof(CycleBuffer));

		SumRez			= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRez		= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRezMin	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRezHrs	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRezDay	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRezMonth	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		SumPromRezYear	= (BYTE FAR*) calloc (ChInData*klvPrm, sizeof((DWORD)0));
		//----------------------------------------------------------------------		
		fBuf = fopen (CFG4,"rb"); if(fBuf==0) ULOGW("cannot open file cfg4.txt"); 
		fseek(fBuf, 0, SEEK_END);
		memcpy(package,"cfg4.txt",9);	package[9]	 = HIBYTE(ftell(fBuf));
										package[10]	 = LOBYTE(ftell(fBuf));
		fseek(fBuf, 0, SEEK_SET);
		fclose(fBuf);		
		if(NetDataWR(package, 0)==TRUE) 
			{
			 ULOGW("data read from cfg4.txt >> SD");
			 memcpy(&SD,package+11,sizeof(TSUBST_DATA));
			}
		//-------------------------------------------------------------------------------
		fBuf = fopen (CFG6,"rb");
		if(fBuf==0) ULOGW("cannot open file cfg6.txt");
		fseek(fBuf, 0, SEEK_END);
		UINT F_length;
		F_length = ftell(fBuf);		// узнал размер файла
		fseek(fBuf, 0, SEEK_SET);
		fclose(fBuf);		
		memcpy(package,"cfg6.txt",9);	package[9]= HIBYTE(F_length);
										package[10]=LOBYTE(F_length);
		if(NetDataWR(package, 0)==TRUE) 
			{
			 ULOGW("data read from cfg6.txt >> AutoPrn");
			 AutoPrn=package[11];
			}
		//-------------------------------------------------------------------------------
		fBuf = fopen (CFG7,"rb");
		if(fBuf==0) ULOGW("cannot open cfg7.txt");
		fseek(fBuf, 0, SEEK_END);
		F_length = ftell(fBuf);	// узнал размер файла
		fclose(fBuf);		
		memcpy(package,"cfg7.txt",9);	package[9]= HIBYTE(F_length);
										package[10]=LOBYTE(F_length);
		if(NetDataWR(package,0)==TRUE) 
			{
			 ULOGW("data read from cfg7.txt >> Modem");				 
			 memcpy(&Modem,package+11,sizeof(MODEM));
			 //if(Modem.tip==7)
			 if(0)
				{
				 //TimModem = Modem.sendGsm.TactSn;
				 //if(mdScanBus (hWnd, Modem.com ,Modem.speed)==-1)
				 /*CHAR name[100];
				 gethostname(name,sizeof(name));
				 ULOGW ("%s",name);
				 WSADATA wsaData; 
				 WSAStartup(MAKEWORD (1,1),&wsaData); 
				 char HostName[1024];
				 DWORD m_HostIP = 0;
				 LPHOSTENT lphost;
				 if(!gethostname(HostName, 1024))
					{
					 if(lphost=gethostbyname(HostName))
					 m_HostIP = ((LPIN_ADDR)lphost->h_addr)->s_addr;
					}
				 CHAR szIPAddr[17];
				 strcpy(szIPAddr,inet_ntoa(*((in_addr*)lphost->h_addr_list[0])));
				 ULOGW ("%d %s",m_HostIP,szIPAddr);*/
				 if(!mdScanBus (hWnd,4,115200))
					{
					 WorkModem=0; ULOGW("modem not found");
					}
				 else 
					{
					 WorkModem=1;		// ждём
					 //sendsms ("+79227000293","[grp2] wincon start");
					 hThrd[5] = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) recievedata, &hReadEventsMdm, 0, &dwThrdID);
					 if (hThrd[5] ==NULL) ULOGW ("Modem(5) Thread Error");
					 //BOOL res = OpenGPRSConnection ("", "tpchel-eco.ru", "ptfiber@tpchel-co.ru", "ptfiber");
					 //SetFileName (1); SetFileName (2); SetFileName (3);
					}
				 //BOOL res = OpenGPRSConnection ("", "tpchel-eco.ru", "ptfiber@tpchel-co.ru", "ptfiber");
				}			
			}
		//-------------------------------------------------------------------------------
		if (Wincon_mode==0) ULOGW("Controller is configure as stand alone");
		if (Wincon_mode==1) ULOGW("Controller is configure as master");
		if (Wincon_mode==2) ULOGW("Controller is configure as slave");
		//----------------------------------------------------------------------
		Devc DVC[DEVICE_NUM_MAX];				// reserve array for device
		memset(DVC,0,sizeof(Devc[DEVICE_NUM_MAX]));	
		memcpy(package,"cfg1.txt",9);	package[9] =HIBYTE(sizeof(Devc[DEVICE_NUM_MAX]));
										package[10]=LOBYTE(sizeof(Devc[DEVICE_NUM_MAX]));
		if(NetDataWR(package, 0)==TRUE)
			{
			 ULOGW("data read from cfg1.txt >> DVC");
			 memcpy(DVC,package+11,sizeof(Devc[DEVICE_NUM_MAX]));
			}
		#ifdef SNS
			 UINT A; A=0;
			 for(tmp=0;tmp<DEVICE_NUM_MAX;tmp++)
				{
				 ULOGW ("[%d] Type=%d Com=%d Speed=%d",tmp,DVC[tmp].Type,DVC[tmp].Com, DVC[tmp].Speed);
				 if((DVC[tmp].Type==0 && DVC[tmp].Com==0 && DVC[tmp].Speed==0) || (DVC[tmp].Type==5 || DVC[tmp].Type==6));
				 else A+=1;
				}
			 ULOGW ("total %d device configured",A); ULOGW ("attempt initialise sensors dll");
			 BOOL StatInit; StatInit = InitSData(hWnd, A, DVC);
			 if(StatInit==FALSE) ULOGW("invalid configuration");
			 else ULOGW("sensor configuration accepted");
		#endif
		
		Sleep (1000);
		ULOGW("create threads ");
		//--------------------------------------------------------------------------------
		hThrd[1] = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) NetTdhrdFnc, &hReadEventsNet, 0, &dwThrdID);
		if (hThrd[1] ==NULL) ULOGW ("Net(1) Thread Error");
		//----------------------------------------------------------------------
		INT TypeU,i;
		for(i=0;i<DEVICE_NUM_MAX;i++)
			{
			 if(DVC[i].Type==6)		// printer
				pScanBus(hWnd,DVC[i].Com ,DVC[i].Speed);
			 if(DVC[i].Type==5)		// console
				TypeU=dScanBus(hWnd,DVC[i].Com ,DVC[i].Speed );
			}		
		if(TypeU>0)		// console init
			{
			 ULOGW ("create display thread");
			 hThrd[2] = CreateThread (NULL,	0,(LPTHREAD_START_ROUTINE) dRead, &hReadEventsDpl, 0, &dwThrdID);
			 if (hThrd[2]==NULL) ULOGW ("thread Error");
			 else SetThreadPriority(hThrd[2], THREAD_PRIORITY_ABOVE_NORMAL); 
			}
		RDspl_length=TaskDues_length/sizeof(CycleDues)+ChInData;
		RDspl_length+=ChInData*4;		// su_type
		ULOGW ("%d = %d + %d",RDspl_length, TaskDues_length/sizeof(CycleDues), ChInData);
		RDspl = (BYTE FAR*) calloc (RDspl_length, sizeof(DplExch));	// memory for instant value
		//----------------------------------------------------------------------
		ULOGW ("create sensor thread");
		hReadEvents[0]=(HANDLE)10;				// exit immediatly after scan		
		hThrd[0] = CreateThread (NULL, 0,(LPTHREAD_START_ROUTINE) ThrdFnc,	&hReadEvents[0], 0,	&dwThrdID);			// returned ID value (ignored)
		if (hThrd[0]==NULL) ULOGW ("Thread Error");
		SetThreadPriority( hThrd[0],THREAD_PRIORITY_HIGHEST); 
		//----------------------------------------------------------------------		
		hThrd[3] = CreateThread (NULL, 0,(LPTHREAD_START_ROUTINE) FlsThrdFnc, &hReadEventsFls, 0,&dwThrdID);
		if (hThrd[3] ==NULL) ULOGW ("Files Thread Error");
		else SetThreadPriority(hThrd[3], THREAD_PRIORITY_NORMAL);
		//----------------------------------------------------------------------				
		hThrd[4] = CreateThread (NULL, 0,(LPTHREAD_START_ROUTINE) StartHttpSrv, &hReadEventsHtp, 0,&dwThrdID); 
		if (hThrd[4] ==NULL) ULOGW ("Httpsrv Thread Error");
		else SetThreadPriority(hThrd[4], THREAD_PRIORITY_NORMAL);
		//--------------------------------------------------------------------------------
		//if (Wincon_mode) { hThrd[6] = CreateThread (NULL, 0,(LPTHREAD_START_ROUTINE) ChngThrdFnc, &hReadEventsChg, 0, &dwThrdID);
		//if (hThrd[6] ==NULL) ULOGW ("Inc(6) Thread Error"); }
		//--------------------------------------------------------------------------------
		EvalTime();
		StopStart(hWnd); ServerStart(hWnd);  Sleep(3000);		
		SetTimer(hWnd, 1, TIC_TIMER, NULL);
		TCHAR os[50];
		GetOSversion (os);
		EnableWDT(240000);
		ULOGW ("OS version %ls",os);
		break;
	 //----------------------------------------------------------------------
	 case MSG_DISPLAY:
		#ifdef SNS
		    //ULOGW ("request [%d][%d]",wParam,lParam);
			tmpDspl= (DplExch *)RDspl+lParam;
			memset(&DsplO,0,sizeof(DplExchO));
			tmp=(INT)lParam; INT iu;
			for(iu=0;iu<4;iu++)
				{
				 if(tmpDspl->name[0]==0) strcpy(tmpDspl->name,"empty");
				 memcpy(DsplO.name[iu],tmpDspl->name,7);
				 DsplO.var[iu]=tmpDspl->var;
				 //DsplO.reserve[iu_]	=tmpDspl->reserve;
				 DsplO.timestm[iu].dwLowDateTime= tmpDspl->timestm.dwLowDateTime;
				 DsplO.timestm[iu].dwHighDateTime=tmpDspl->timestm.dwHighDateTime;
				 DsplO.status[iu]=tmpDspl->status;
				 //ULOGW ("[%d] %s=%f",iu+lParam,DsplO.name[iu],DsplO.var[iu]);
				 tmpDspl+=1;
				}
			if (!wParam) GetValues (DsplO ,RDspl_length, 0);
			GetSlaveVal = wParam;
		#endif
		break;
	 //----------------------------------------------------------------------
	 case WM_TIMER:
		 GetLocalTime(&TDisp);		 
		 //ULOGW ("timer %d (%d,%d)",count,TDisp.wSecond,tmpTDisp.wSecond);
		 MEMORYSTATUS	ms;
		 GlobalMemoryStatus(&ms);
		 if (ms.dwAvailPhys<6000000 || ms.dwMemoryLoad>75)
			{
			 ULOGW ("CreateProcess(reboot)");
			 CreateProcess(TEXT(reboot),NULL,NULL,NULL,NULL,CREATE_NEW_CONSOLE,NULL,NULL,NULL,NULL); 
			 Sleep (2000);
			}
		 if (TDisp.wDay==1 && TDisp.wHour==0 && TDisp.wMinute==1)
			{
			 WCHAR cW[0xff],cE[0xff]; UINT ctn=0,qva=skokoIh();
			 for (UINT i=0;i<500;i++)
				{	
				 swprintf(cW,LnameF,i);
				 swprintf(cE,LnameG,i);
				 //if (CopyFile(cW,cE,FALSE)) 
				 if (1)
					{
					 DeleteFile (cW); ctn++;
					}
				 if (ctn>=qva) break;
				}
			}
		 if(TDisp.wSecond==0) ULOGW ("mload[%ld%%] mem[%ld|%ld]",ms.dwMemoryLoad,ms.dwAvailPhys,ms.dwAvailVirtual);
		 if(count==0 && TDisp.wSecond==0 && TDisp.wMinute%3==0) count=1;
		 if(TDisp.wSecond!=tmpTDisp.wSecond && count!=0)
			{
			 DisableWDT();
			 EnableWDT(120000);
			 DWORD pptim=GetTickCount();
			 //ULOGW ("disp time %d",(pptim-prtime)); prtime=pptim;
			 count+=1; if(count==0) count=1;	// !			 
			 EnterCriticalSection(&fls);
				#include "data_processing.cpp"
			 LeaveCriticalSection(&fls);
			 tmpTDisp.wSecond=TDisp.wSecond;
			}
		break;
	  //----------------------------------------------------------------------
	  case	MSG_MSGPRINT:
			ULOGW("[print] %x %x", wParam, lParam);
			EnterCriticalSection(&fls);	
			Tmessage=MSG_MSGPRINT; TwParam=wParam;	TlParam=lParam;
			if(Tmessage==MSG_MSGPRINT) 
				{
				 memcpy(&MPrnTmp,(const void *)TwParam,sizeof(MPrn));
				 ULOGW("[%d] %d",MPrnTmp.pipe,MPrnTmp.quant);
				}
			LeaveCriticalSection(&fls);
			break;
	 //----------------------------------------------------------------------
	 case	MSG_OPTIONS:
			//ULOGW("[options] %x %x", wParam, lParam);
			strcpy (Prm.name,PM[wParam].name);
			printf ("(ED) dp_units = %d | p_units = %d  | t_units = %d | r_units=%d\n",ED.dp_units,ED.p_units,ED.t_units,ED.r_units);
			if ((UINT)wParam==0) sprintf (Prm.var,"%6.2f",(DOUBLE)ED.d);
			if ((UINT)wParam==1 || (UINT)wParam==3) 
				{
				 UINT units=0;
				 if ((UINT)wParam==1) units=ED.p_units;
				 if ((UINT)wParam==3) units=ED.r_units;
				 switch (units)
					{
					 case 0:	sprintf (Prm.var,"м3"); break;
					 case 5:	sprintf (Prm.var,"кПа"); break;
					 case 6:	sprintf (Prm.var,"МПа"); break;
					 case 22:	sprintf (Prm.var,"м3/с"); break;
					 case 26:	sprintf (Prm.var,"м3/ч"); break;
					}
				}
			if ((UINT)wParam==2) sprintf (Prm.var,"Абсолютный");
			if ((UINT)wParam==4) sprintf (Prm.var,"%f",(DOUBLE)ED.pb);
			if ((UINT)wParam==5) sprintf (Prm.var,"%f",(DOUBLE)ED.fi);
			if ((UINT)wParam==6) 
				{
				 switch (SD.gas_alg)
					{
					 case 0: sprintf (Prm.var,"GERG-91");
					 case 1: sprintf (Prm.var,"NX-19 MOD");
					 case 2: sprintf (Prm.var,"ВНИЦ СМВ");
					}
				}
			if ((UINT)wParam==7) sprintf (Prm.var,"%f",(DOUBLE)SD.rnom);
			if ((UINT)wParam==8) sprintf (Prm.var,"%f",(DOUBLE)SD.sCH4);
			if ((UINT)wParam==9) sprintf (Prm.var,"%f",(DOUBLE)SD.sC2H6);
			if ((UINT)wParam==10) sprintf (Prm.var,"%f",(DOUBLE)SD.sC3H8);
			if ((UINT)wParam==11) sprintf (Prm.var,"%f",(DOUBLE)SD.sC4H10);
			if ((UINT)wParam==12) sprintf (Prm.var,"%f",(DOUBLE)SD.sIC4H10);
			if ((UINT)wParam==13) sprintf (Prm.var,"%f",(DOUBLE)SD.sN2);
			if ((UINT)wParam==14) sprintf (Prm.var,"%f",(DOUBLE)SD.sCO2);
			if ((UINT)wParam==15) sprintf (Prm.var,"%f",(DOUBLE)SD.sH2S);
			if ((UINT)wParam==16) sprintf (Prm.var,"%f",(DOUBLE)SD.sC5H12);
			if ((UINT)wParam==17) sprintf (Prm.var,"%f",(DOUBLE)SD.sIC5H12);
			if ((UINT)wParam==18) sprintf (Prm.var,"%f",(DOUBLE)SD.sC6H14);
			if ((UINT)wParam==19) sprintf (Prm.var,"%f",(DOUBLE)SD.sC7H16);
			if ((UINT)wParam==20) sprintf (Prm.var,"%f",(DOUBLE)SD.sC8H18);
			if ((UINT)wParam==21) sprintf (Prm.var,"%f",(DOUBLE)SD.sC3H6);
			if ((UINT)wParam==22) sprintf (Prm.var,"%f",(DOUBLE)SD.sO2);
			if ((UINT)wParam==23) sprintf (Prm.var,"%f",(DOUBLE)SD.sCO);
			if ((UINT)wParam==24) sprintf (Prm.var,"%f",(DOUBLE)SD.sH2);
			if ((UINT)wParam==25) sprintf (Prm.var,"%f",(DOUBLE)SD.sCnHm);
			//GetOptions (Prm, 25);
			break;
	 //----------------------------------------------------------------------
	  case	MSG_DATA:
			//ULOGW("[data] %x %x", wParam, lParam);
			/*CycleBuffer *tD;
			MassRez[1] = (DWORD FAR)PromRezHrs;
			MassRez[1] = (DWORD FAR)PromRezHrs;
			MassRez[2] = (DWORD FAR)PromRezDay;
			MassRez[3] = (DWORD FAR)PromRezMonth;
			tD = (CycleBuffer *)MassRez[1] + (klvPrm-3);
			sprintf (Dta.val[0],"час:   %f",tD->value);
			tD = (CycleBuffer *)MassRez[2] + (klvPrm-3);
			sprintf (Dta.val[1],"день:  %f",tD->value);
			tD = (CycleBuffer *)MassRez[3] + (klvPrm-3);
			sprintf (Dta.val[2],"месяц: %f",tD->value);
			sprintf (Dta.val[3],"пп.месяц: %f",LastMon[0]);
			//ULOGW("GetDatas (%s)(%s)(%s)(%s)",Dta.val[0],Dta.val[1],Dta.val[2],Dta.val[3]);
			GetDatas (Dta, 3);*/
			break;
	 //-------------------------------------------------------------
	 case MSG_MSGLOG:
			FILETIME ftim;
			EnterCriticalSection(&fls);	
			Tmessage=	MSG_MSGLOG;
			TwParam=	wParam;
			TlParam=	lParam;
			LeaveCriticalSection(&fls);	
			if(wParam==0)
				{
				 dTime(&ftim);
				 sprintf(szTemp,"%08x",lParam);
				 WriteToLog(szTemp);	
				}
			else {
			memset( &MsE, 0, sizeof(MsgExch) );
			UINT q=FindToLog(&MsE, (UINT)lParam);	
			GetMessages (MsE,q);
			}
			break;
	 //----------------------------------------------------------------------
	 case	WM_PAINT:
			RECT rt;
			hdc = BeginPaint(hWnd, &ps);
			GetClientRect(hWnd, &rt);
			EndPaint(hWnd, &ps);
			break;
	 //----------------------------------------------------------------------
	 case WM_DESTROY:
			CloseGPRSConnection ();
			#ifdef SNS
				DeinitSdata();
				dDeInit();
			#endif
			KillTimer(hWnd, 1);			
			//DisableWDT();
			break;
	 //----------------------------------------------------------------------
	}
 return 0;
}
//-----------------------------------------------------------------------------------------------------
VOID ThrdFnc (LPVOID lpParam)			// sensors thread
{
 #ifdef SNS
	ULOGW ("sensor thread in progress");
	ScanBus((LPVOID)0);
	WorkEnableF (0, MPrnTmp);
 #else
	ULOGW ("work without sensors");
 #endif
 ULOGW ("scanbus complete");
 EnterCriticalSection(&cs); ExgengeFlags  =0x011; LeaveCriticalSection(&cs);
 HANDLE hEvent; //, old_hEvent=0; //INT tmp=0;
 while(1)
	{
     hEvent = * (HANDLE *) lpParam;		// this thread's read event
	 if(hEvent==(HANDLE) 0x10) break;	
	 #ifdef SNS
		Read((LPVOID)0);
	 #endif
	 Sleep(500);						// read sensors and pause
	}
 //&dwThreadID=(LPDWORD )0x10l;
 ULOGW ("sensor thread closed");
 ExitThread(0);
}
//----------------------------------------------------------------------------------
VOID FlsThrdFnc (LPVOID lpParam)
{
 BOOL trigger;
 HANDLE hEvent;
 FILE *Fls;
 Sleep (30000);
 TmpTPointer=0; MTmpTPointer=0;
 EnterCriticalSection(&fls); TPointer=Pointer; MTPointer=MPointer; LeaveCriticalSection(&fls);
 if(TPointer > (sizeof(CycleBuffer)*kStrKB)/2)	trigger=TRUE;
 else											trigger=FALSE;
 while (1)
	{
	 //ULOGW ("FlsThrdFnc TPointer=%d TmpTPointer=%d",TPointer,TmpTPointer);
	 CreateDirectory(BASEDIR,NULL);
	 hEvent = *(HANDLE *) lpParam; 
	 EnterCriticalSection(&fls); TPointer=Pointer; MTPointer=MPointer; LeaveCriticalSection(&fls);
	 _Tmessage=Tmessage; _TwParam=TwParam; _TlParam=TlParam;
	 //if(_Tmessage==MSG_MSGLOG) ULOGW ("MSG_MSGLOG");
	 if(_Tmessage==MSG_MSGPRINT)
		 { memset(&MPrn,0,sizeof(MPrn)); memcpy(&MPrn,&MPrnTmp,sizeof(MPrn)); ULOGW ("MPrn.pipe=%d MPrn.quant=%d",MPrn.pipe, MPrn.quant);}
	 Tmessage=0; TwParam=0;	TlParam=0;	 
	 Sleep(1000);
	 if(abs(TPointer-TmpTPointer)>=(8*sizeof(CycleBuffer)))	
		{
		 // UpLoadSaveKB (3,4975)
		 //ULOGW ("UpLoadSaveKB (%d,%d)",TPointer/sizeof(CycleBuffer),TmpTPointer/sizeof(CycleBuffer));
		 UpLoadSaveKB(TPointer/sizeof(CycleBuffer),TmpTPointer/sizeof(CycleBuffer),KBuffer,1);
		 TmpTPointer=TPointer;  
		}
	// if(abs(MTPointer-MTmpTPointer)>=(10*sizeof(CycleBuffer)))	
	//	{	
	//	 UpLoadSaveKB(MTPointer/sizeof(CycleBuffer),MTmpTPointer/sizeof(CycleBuffer),MKBuffer,2);
	//	 MTmpTPointer=MTPointer;  
	////	}
	 WCHAR cW[500];
	 if(TPointer > (sizeof(CycleBuffer)*kStrKB)/2 && trigger==FALSE)
		{// сохраняем 1-ю половину	
		 workwithbuf=TRUE;
		 ULOGW ("save first half (%d>%d) > %s",0,kStrKB/2,CurBase);
		 Fls = fopen (CurBase,"w");
		 fseek(Fls, 0, SEEK_SET);
		 fprintf(Fls,zag1,skokoIh());
		 fprintf(Fls,zag2,0,0,0,0,0,0);
		 SvToFl(Fls,0,kStrKB/2,1);
		 fclose(Fls);
		 trigger=TRUE;
		 ULOGW ("end save (%d)",workwithbuf);
		 workwithbuf=FALSE;
		}
	 //--------
	 if(TPointer < (sizeof(CycleBuffer)*kStrKB)/2 && trigger==TRUE)
		{// сохраняем 2-ю половину
		 workwithbuf=TRUE;
		 ULOGW ("save second half (%d>%d) > %s",kStrKB/2,kStrKB,CurBase);
		 Fls = fopen (CurBase,"a+b");
		 fseek(Fls, 0, SEEK_END);
		 SvToFl(Fls,kStrKB/2,kStrKB,1);	// SvToFl (2500 > 5000)
		 fclose(Fls);
		 
		 trigger=FALSE;
		 Fls = fopen (CurBase,"r+");
		 FILETIME fStr_; FILETIME fEn_;
		 FindTimStrEnd(Fls,&fStr_,&fEn_);
		 fseek(Fls, strlen(zag1), SEEK_SET);
		 SYSTEMTIME sftim; GetLocalTime(&sftim);
		 while(sftim.wYear>=100) sftim.wYear-=100;
		 fprintf(Fls,zag2,sftim.wHour,sftim.wMinute,sftim.wSecond,sftim.wDay,sftim.wMonth,sftim.wYear);
		 fseek(Fls, pzStr, SEEK_SET); fprintf(Fls,"%08x%08x",fStr_.dwHighDateTime,fStr_.dwLowDateTime );
		 fseek(Fls, pzEnd, SEEK_SET); fprintf(Fls,"%08x%08x",fEn_.dwHighDateTime,fEn_.dwLowDateTime );
		 fclose(Fls);
		 
		 swprintf(cW,LnameF,skokoIh());
		 //ULOGW ("CopyFile");
		 if(!CopyFile(LCurBase,cW,TRUE))
			{
			 swprintf(cW+100,BaseTrbl,skokoIh()/*filNo*/);
			 ULOGW ("CopyFile2");
			 CopyFile(cW,cW+100,FALSE);
			 CopyFile(LCurBase,cW,FALSE);
			}
		 ULOGW ("end save (%d)",workwithbuf);
		 workwithbuf=FALSE;
		}
	 //----------------------------
	 switch (_Tmessage)
		{
		  case MSG_MSGLOG:
			{			  
			  if(_TwParam==0)
				{
				 //ULOGW ("%08x",_TlParam);
				 sprintf(szTemp,"%08x",_TlParam); WriteToLog(szTemp);
				}
			  else
				{
				 memset(&MsE, 0, sizeof(MsgExch));
				 UINT q;
				 for(INT i=0;i<100;i++)
				 {
				  q=FindToLog(&MsE, (UINT)_TlParam);
				  if(q>0) break;
				  Sleep(10);
				 }
				 //GetMessages (MsE,q);		
				}
			break;	
			}
		 //----------------------------
		 case MSG_MSGPRINT:
			 {
			 ReportForming (TRUE);
			 if(_TlParam!=0)	
				{
				 ReportForming (FALSE);
				 break;
				}
			 if(MPrn.type==LOGIKA)	
				{			 
				 WorkEnableF (MPrn.quant, MPrn);
				 ReportForming(FALSE);
				 break;
				}
			 if(MPrn.type==NS_MESSAGE)	// нештатные ситуации
				{
				 noStaff((TYPE_EVENTS|MPrn.pipe));
				 ReportForming (FALSE);
				 break;
				}
			 if(MPrn.type==SOURGE_ERROR)
				{
				 noStaff((TYPE_EVENTS|MODULE_KERNEL|EVENTS_POWER_ON),(TYPE_EVENTS|MODULE_KERNEL|EVENTS_POWER_OFF),1);
				 ReportForming (FALSE);
				 break; 
				}
			 if(MPrn.type==ARCHIVE_DATA) // архивные значения
				{	 
				 archivData(FALSE);
				 break;
				}
			 ReportForming (FALSE);
			 break;	
			}
		}
	_Tmessage=0; _TwParam=0; _TlParam=0;
	Sleep(5000);
 }
 ExitThread(0);
}
//-------------------------------------------------------------------------
VOID archivData(BOOL print)
{
 DWORD ctime = GetTickCount(); DWORD vtime=0;
 DWORD target=0x80000000;
 DOUBLE srcval[klvPrm];
 srcval[0]=srcval[1]=srcval[2]=srcval[3]=0; srcval[4]=srcval[5]=srcval[6]=0;
 INT l=(MPrn.pipe-1);
 //ULOGW ("klvPrm=%d MPrn.pipe=%x MPrn.quant=%d",klvPrm,MPrn.pipe, MPrn.quant);
 srcval[klvPrm-1]=0x86000000 + MPrn.pipe + (MPrn.quant<<(5*4));
 //ULOGW ("Attempt to found 0x%x",srcval[klvPrm-1]);
 //---------------------------------------
 FILE *hTmp; FILETIME fs,fe;
 UINT schet=59,cb, _seek;
 CycleBuffer freshData;
 UINT locIndx=0, GlbIndx=0;
 memset (&PrnVl,0,sizeof(PrnVl));
 //---------------------------------------
 SYSTEMTIME st,stt,lt;
 FileTimeToSystemTime (&MPrn.start,&stt);
 FileTimeToSystemTime (&MPrn.end,&st);
 GetLocalTime(&lt);
 //ULOGW ("[%d:%d:%d %d/%d/%d - %d:%d:%d %d/%d/%d]",stt.wHour,stt.wMinute,stt.wSecond,stt.wDay,stt.wMonth,stt.wYear,st.wHour,st.wMinute,st.wSecond,st.wDay,st.wMonth,st.wYear);
 // так, сохраним что есть в буфере сначала!
 target=0x80000000;
 EnterCriticalSection(&fls); TPointer=Pointer; LeaveCriticalSection(&fls);
 //ULOGW ("Pointer 0->%d", (TPointer)/(sizeof(CycleBuffer)));
 /*hTmp = fopen (TmpBase,"w");
 fseek( hTmp,0, SEEK_SET);
 fprintf(hTmp,zag1,0);	
 fprintf(hTmp,zag2,0,0,0,0,0,0);
 SvToFl(hTmp,1, (TPointer)/(sizeof(CycleBuffer))+0,1);
 fclose(hTmp);

 hTmp = fopen (TmpBase,"r+");
 FILETIME fStr; FILETIME fEn;
 FindTimStrEnd(hTmp,&fStr,&fEn);
 fseek(hTmp, strlen(zag1), SEEK_SET);
 SYSTEMTIME sftim;
 GetLocalTime(&sftim);
 while(sftim.wYear>=100) sftim.wYear-=100;
 fprintf(hTmp,zag2,sftim.wHour,sftim.wMinute,sftim.wSecond,sftim.wDay,sftim.wMonth,sftim.wYear);
 fseek(hTmp, pzStr, SEEK_SET);
 fprintf(hTmp,"%08x%08x",fStr.dwHighDateTime,fStr.dwLowDateTime );
 fseek(hTmp, pzEnd, SEEK_SET);
 fprintf(hTmp,"%08x%08x",fEn.dwHighDateTime,fEn.dwLowDateTime );
 fclose(hTmp);*/
 //ULOGW ("prepare (%d)",GetTickCount()-ctime);
 UINT buffile=0;
 //if (MPrn.quant)
 buffile=skokoIh(); //else buffile=1;
 UINT chF=0;
 if (buffile>15 && lt.wDay==stt.wDay) chF=buffile-10;
 else chF=0;
 if (buffile>200) chF=150;
// ------------------------------------------------------------
 //for(UINT chF=buffile; chF>0; chF--)  
 for(; chF<=buffile; chF++)
	{
	 Sleep(1); vtime=GetTickCount();
	 if(GlbIndx >= schet) break;
	 if(chF==buffile)		// т.е. первое вхожление !!!
			sprintf(szT,KBuffer);	 
	 else	//sprintf(szT,nam	eF, chF-1 );
			sprintf(szT,nameF, chF);
	 hTmp = fopen (szT,"rb");
	 if(hTmp==0) 
		{ 
		 ULOGW ("Не могу открыть файл c расчётами по расходу");
		 break;
		}
	 //else ULOGW ("Open file %s", szT);
	 // ищем в базах, но ещё нужно в кольцевом смотреть...
	 if(chF!=buffile)
		{
		 fseek( hTmp , 0 , SEEK_END );
		 _seek= ftell(hTmp)-1-1;
		 fseek( hTmp , pzStr , SEEK_SET );
		 cb=fread( szT, 16, 1, hTmp);
		 fs.dwHighDateTime=a2i(szT,16);
		 fs.dwLowDateTime= a2i(szT+8,16);
		 fseek( hTmp , pzEnd , SEEK_SET );
		 cb=fread( szT, 16, 1, hTmp);
		 fe.dwHighDateTime=a2i(szT,16);
		 fe.dwLowDateTime= a2i(szT+8,16);	
		 _seek=70*(kStrKB);	// !!!
		 //ULOGW ("[%d] MPrn= %x %x / [%x %x] - [%x %x]",chF,MPrn.start,MPrn.end,fs.dwHighDateTime,fs.dwLowDateTime,fe.dwHighDateTime,fe.dwLowDateTime);
		}
	 else
		{
		 fseek(hTmp, 0, SEEK_END);
		 _seek= ftell(hTmp);
		 fseek(hTmp, 0,SEEK_SET);
 		 _seek=70*(kStrKB);	// !!!
		 //ULOGW ("Kbuffer len=%d (%d) & %d",_seek,_seek/70,kStrKB+5);
		}
	 if ((_seek/70)>(kStrKB+5)) continue;
	 //if( CompareFileTime( &fe , &MPrn.start )<0 ) break; //!!!!!!!!!!!!!!!!
	 DWORD wtime=0,gtime=0;
	 //------------------------------------------------------------
	 if	(chF==buffile || ((CompareFileTime(&fs , &MPrn.start )>0 || CompareFileTime( &fs , &MPrn.end )<0) &&	(CompareFileTime( &fe , &MPrn.start )>0 || CompareFileTime( &fe , &MPrn.end )<0)))
		{// если да, то будем собирать, собирать... + сброс счётчика
		 UINT p_=0,ct=0;	// !!!
		 gtime=GetTickCount();
		 if (chF!=buffile) { fgets(DataO[0],100,hTmp); fgets(DataO[0],100,hTmp);}
		 while (!feof(hTmp))
			{	
			 if (_seek/70-p_>0) 
				{
				 //fgets(DataO[_seek/70-p_-2],74,hTmp); 
				 fgets(DataO[ct],74,hTmp);	// !!!
				 if (strlen (DataO[ct])>74) DataO[ct][0]=0;
				 //if (_seek/70-p_<5) ULOGW ("D[%d][%d] %s",_seek/70-p_,ct,DataO[ct]);
				 p_++; ct++;
				}
			 else fgets(szT,74,hTmp);
			}
		 //ULOGW ("p_=%d",p_);
		 UINT items = p_; p_=1; // p_=2;
		 while (1)
			{
			 strcpy (szT,DataO[p_]);
			 freshData.trg = a2i(szT+2,16);				 
			 freshData.timestm.dwLowDateTime = a2i(szT+54,16);
			 freshData.timestm.dwHighDateTime = a2i(szT+46,16);
			 //ULOGW ("[%d][0x%x] %s = %f [%x %x](%d)",p_,freshData.trg,freshData.tNmame,freshData.value,freshData.timestm.dwHighDateTime,freshData.timestm.dwLowDateTime,freshData.status);
			 //if (chF==buffile) ULOGW ("[%x | %x | %x] %d %d",(UINT)srcval[klvPrm-1]&0xf0f0000f,freshData.trg&0xf0f0000f,freshData.trg&0xf0000000,CompareFileTime(&freshData.timestm, &MPrn.start),CompareFileTime(&freshData.timestm, &MPrn.end));
	 		 if(((((UINT)srcval[klvPrm-1]&0xf0f0000f) == (freshData.trg&0xf0f0000f))||locIndx!=0)&&((freshData.trg&0xf0000000)==target) &&
				 (CompareFileTime(&freshData.timestm, &MPrn.start)>0 && CompareFileTime(&freshData.timestm, &MPrn.end)<0))
				{
 				 memcpy(freshData.tNmame,szT+16,6);
				 for(INT i_=0;i_<sizeof(freshData.tNmame);i_++) 
				 if(freshData.tNmame[i_]=='\t' || freshData.tNmame[i_]=='\n' || freshData.tNmame[i_]==' ') 
						freshData.tNmame[i_]=0;
				 freshData.value = atof(szT+29);
				 freshData.status=  szT[67]=='1' ? TRUE : FALSE;
				 UINT prm=(freshData.trg&0xf000000) >> 24;
				 PrnVl.name[prm][0]=0;
				 memcpy(PrnVl.name[prm], &freshData.tNmame,6);	// имя
				 PrnVl.value[prm][GlbIndx]  = freshData.value;	// значение
				 PrnVl.status[prm][GlbIndx] = freshData.status;	// статус
				 PrnVl.time[GlbIndx] = freshData.timestm;
				 //ULOGW ("[%d] (%d,%d) %s=%f [%x %x]",p_,GlbIndx,prm,PrnVl.name[prm],PrnVl.value[prm][GlbIndx],PrnVl.time[GlbIndx].dwLowDateTime,PrnVl.time[GlbIndx].dwHighDateTime);
				 if((freshData.trg&0x8f000000)==0x87000000) GlbIndx+=1;
				 if( GlbIndx >= schet) break;
				}
			 p_++;
			 if (p_>=items) break;
			}
	 }
	 //ULOGW ("[%d] (%d)",chF,GetTickCount()-vtime);
	 fclose(hTmp);
	}
 PrnVl.type=MPrn.quant;
 PrnVl.start=MPrn.start;
 PrnVl.pipe =MPrn.pipe;
 //if (!print) ValuesToReport(PrnVl,1,1);
 printf ("ArchiveData (%d)\n",GetTickCount()-ctime);
 GetSlaveData=1; 
 ReportForming(FALSE);
}
//-------------------------------------------------------------------------
VOID noStaff(UINT kodEvents,UINT keSec,UINT flag)
{
 DWORD ctime = GetTickCount();
 LONG seekStf=0;	BOOL strtSeek=FALSE;
 UINT count=0; INT os,_q; 
 ULOGW ("noStaff (0x%x,0x%x)",kodEvents,keSec);
 while (1)
	{	
	 MsE.KodEvents[0]=MsE.KodEvents[1]=0;
	 for(INT is=0;is<100;is++)	
		{
		 _q=FindToLog(&MsE,seekStf);
		// ULOGW ("FindToLog(%d)",_q);
		 if(_q>0) break;
		 //Sleep(150);
		} 
	 os=_q;
	 //if(MPrn.quant<=0) break;
	 if ((UINT)MsE.KodEvents[0]!=(TYPE_EVENTS+MODULE_KERNEL+EVENTS_POWER_ON) && (UINT)MsE.KodEvents[0]!=(TYPE_EVENTS+MODULE_KERNEL+EVENTS_POWER_OFF))
	 if (((UINT)MsE.KodEvents[0]!=0x20000f01) && ((UINT)MsE.KodEvents[0]!=0x20000f02) && !flag)	
		{
		 LONG cmpT=CompareFileTime (&MsE.timestm[0],&MPrn.end);
		 ULOGW ("CompareFileTime(%d)",cmpT);		 
		 //if(cmpT<=0	&& ((UINT)MsE.KodEvents[0]&0xf000000f) == (UINT)(kodEvents))
		 if(cmpT<=0 && (((UINT)MsE.KodEvents[0]&0xf0000000)==0x20000000))
			{
			 MEO.event[count]=(UINT)MsE.KodEvents[0];
			 MEO.fevnt[count].dwHighDateTime=MsE.timestm[0].dwHighDateTime;
			 MEO.fevnt[count].dwLowDateTime=MsE.timestm[0].dwLowDateTime;
			 count+=1;
			 MPrn.quant-=1;
			}
		}
	 for(INT k=0;k<=1;k++)
		{
		 LONG cmpT=CompareFileTime (&MsE.timestm[0],&MPrn.end);
		 //ULOGW ("CompareFileTime(%d)",cmpT);		 
		 if(strtSeek==FALSE && cmpT<=0	&& ((UINT)MsE.KodEvents[k]) == (UINT)(kodEvents)|| (UINT)MsE.KodEvents[k] == (UINT)(keSec))
			{
			 //ULOGW ("strtSeek=TRUE");
			 strtSeek=TRUE;
			 MEO.start.dwHighDateTime = MsE.timestm[k].dwHighDateTime;
			 MEO.start.dwLowDateTime  = MsE.timestm[k].dwLowDateTime;
			}
		 if(strtSeek==TRUE && MPrn.quant!=0)
			{
			 if( ((UINT)MsE.KodEvents[k] == (UINT)(kodEvents)) ||  (UINT)(MsE.KodEvents[k]) == (UINT)(keSec))
				{
				 //ULOGW ("MEO[%d]=%x",count,MsE.KodEvents[k]);
				 MEO.event[count]=(UINT)MsE.KodEvents[k];
				 MEO.fevnt[count].dwHighDateTime =MsE.timestm[k].dwHighDateTime;
				 MEO.fevnt[count].dwLowDateTime	=MsE.timestm[k].dwLowDateTime;
				 count+=1;
				 MPrn.quant-=1;
				}
			}
		}
	seekStf+=2;
	if(os<=0) break;
	if(MPrn.quant<=0) break;
	}
 MEO.pipe=MPrn.pipe;
 MEO.start=MPrn.start;
 EventsToReport(MEO);
 ULOGW ("noStaff (%d)",GetTickCount()-ctime);
 ReportForming(FALSE);		
}
//-------------------------------------------------------------------------
UINT FindToLog(MsgExch *MsE,UINT para)
	{
	 FILE *hMsgLogF; hMsgLogF = fopen (MsgLog,"rt");	
	 if(hMsgLogF==0) 
		{  
		 ULOGW ("Не могу открыть файл\nmessagelog.txt"); 
		 return 0;
		}	
	 UINT otsch=0,pztc=0,tmpScn=0;
	 INT dblseek; UINT upztc[8]={0};
	 LONG locseek=sizeof(szT)/2;
	 DWORD	scn[4+8]={0};
	 if(para==0)
		{
		 fseek(hMsgLogF, 0, SEEK_END);
		 ftl_seek = ftell(hMsgLogF)-1;
		 stroka=para;
		}
	 memset(szT,0,sizeof(szT));
	 dblseek=ftl_seek;
	 do	{
		 fseek(hMsgLogF, dblseek, SEEK_SET);
		 //dMsLg=fread( szT+locseek, 1, 1, hMsgLogF);
		 szT[locseek]=fgetc(hMsgLogF);
		 //if(feof(hMsgLogF)!=0){dblseek=0;break;}
		 if(szT[locseek]=='+')
			{ 
			 if(stroka<=para)
				{
				 if((pztc&1)==0 )
					 MsE->KodEvents[pztc/2]=a2i(szT+locseek+1,16); 
				 else
					{
					 MsE->timestm[pztc/2].dwHighDateTime=a2i(szT+locseek+1,16);
					 MsE->timestm[pztc/2].dwLowDateTime= a2i(szT+locseek+1+8,16);	
					}
				}
			else
				{
				 if((pztc&1)==1 )
					{
					 fread( szT+locseek+1, 8, 1, hMsgLogF);
					 MsE->KodEvents[pztc/2]=a2i(szT+locseek+1,/*tmpScn,*/16);
					 dblseek+=7;locseek+=7;	
					}
				 else
					{ 
					 fread( szT+locseek+1, 8, 1, hMsgLogF);
					 fread( szT+locseek+1+8, 8, 1, hMsgLogF);
					 MsE->timestm[pztc/2].dwHighDateTime=a2i(szT+locseek+1,16);
					 MsE->timestm[pztc/2].dwLowDateTime= a2i(szT+locseek+1+8,16);
					 dblseek+=15;locseek+=15;	
					}
				}
			pztc+=1;	
			}
		if(stroka<=para)	{ dblseek-=1;locseek-=1;}
		else				{ dblseek+=1;locseek+=1;}
	}
 while(pztc<4 && locseek>0 && dblseek>0);
 fclose(hMsgLogF);
 if(dblseek<=0) dblseek=0;

 stroka=para;
 ftl_seek=dblseek;
 return dblseek;
}
//-------------------------------------------------------------------------------------------------
VOID NetTdhrdFnc (LPVOID lpParam)
{
 HANDLE hEvent;
 CHAR CheckSum;
 INT i=0,index=0; 
 Sleep(30000);
 while(1)
	{
     hEvent = * (HANDLE *) lpParam;
	 if(index==0) strcpy((CHAR *) package,"cfg .txt");
	 package[index]=0;
	 NetState= recv(srv_socket,(CHAR *)package+index,16000, 0);
	 UINT err = WSAGetLastError ();
	 index+=NetState;	 
	 if(package[0]!='c'||package[1]!='f'||package[2]!='g' || package[4]!='.'||package[5]!='t'||package[6]!='x'||package[7]!='t') index=0;
	 else
		{
		index = (package[10]|(package[9]<<8))+12;
		if(index == (package[10]|(package[9]<<8))+12)		
			{
			 CheckSum = ClkChkSum((CHAR *)package,((package[10]|(package[9]<<8))+11));
			 if((CHAR)package[index-1]==(CHAR)CheckSum)
				{
				 index=0;			 	 
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg1.txt",8)==0&&package[8]==1)	 recvAns ();
				 if(memcmp(package,"cfg1.txt",8)==0&&package[8]==0)	 sendAns (CFG1);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg2.txt",8)==0&&package[8]==1) recvAns ();
				 if(memcmp(package,"cfg2.txt",8)==0&&package[8]==0) sendAns (CFG2);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg3.txt",8)==0&&package[8]==1) 
					{
					 recvAns ();
					 //memcpy(TaskMath,package+11,TaskMath_length);
					 //indata=(MainStruct*) (TaskMath);
					 fBuf = fopen (CFG3,"rb"); if(fBuf==0) ULOGW("cannot open file cfg3.txt");
					 fseek(fBuf, 0, SEEK_END);
					 TaskMath_length = ftell(fBuf)-1;
					 fseek(fBuf, 0, SEEK_SET);
					 fclose(fBuf);
					 TaskMath =	(BYTE FAR*) calloc (1, TaskMath_length);
					 memcpy(package,"cfg3.txt",9);	package[9]= HIBYTE(TaskMath_length+1);
													package[10]=LOBYTE(TaskMath_length+1);
					 if(NetDataWR(package, 0)==TRUE) 
						{
						 ULOGW("data read from cfg3.txt >> ChInData");
						 memcpy(TaskMath,package+12,TaskMath_length);
						 indata=(MainStruct*) (TaskMath);
						 memcpy(&ChInData,package+11,1); 
						}		
					}
				 if(memcmp(package,"cfg3.txt",8)==0&&package[8]==0) sendAns (CFG3);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg4.txt",8)==0&&package[8]==1)
					{	
					 recvAns ();
					 memcpy(&SD,package+11,(package[10]|(package[9]<<8)));
					}
				 if(memcmp(package,"cfg4.txt",8)==0&&package[8]==0) sendAns (CFG4);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg5.txt",8)==0&&package[8]==1) recvAns ();
		 		 if(memcmp(package,"cfg5.txt",8)==0&&package[8]==0) sendAns (CFG5);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg6.txt",8)==0&&package[8]==1) recvAns ();
				 if(memcmp(package,"cfg6.txt",8)==0&&package[8]==0) sendAns (CFG6);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfg7.txt",8)==0&&package[8]==1)
					{				
					 DeleteFile(LCFGA); recvAns ();
					}
				 if(memcmp(package,"cfg7.txt",8)==0&&package[8]==0) sendAns (CFG7);
				 //------------------------------------------------------------------
				 if(memcmp(package,"cfgA.txt",8)==0&&package[8]==1) // Запись / пакет № A (архив)
					{	
					 INT cb=0; FILE *fBuf;	
					 if(package[11]==0 && package[12]==0) fBuf = fopen (CFGA,"wb");
					 else fBuf = fopen (CFGA,"ab");
					 if(fBuf==0) ULOGW ("Не могу открыть файл cfgA.txt");
					 INT qnt; qnt=package[10]| (package[9]<<8);
					 cb=fwrite( package+11+2, sizeof(CHAR),qnt-2 , fBuf); //это запись
					 fclose(fBuf);
					 if(HIBYTE(cb)!=HIBYTE(package[9]) && LOBYTE(cb)!=LOBYTE(package[10]))	{;}
					 else	ULOGW ("Данные зарписаны в cfgA.txt");
					 package[8]=4; package[9]=0; package[10]=0;
					 package[11]= ClkChkSum((CHAR *)package,((package[10]|(package[9]<<8))+11));
					 send(srv_socket,(CHAR *)package, 12, 0);
					}
				 if(memcmp(package,"cfgA.txt",8)==0&&package[8]==0) sendAns (CFGA);
				 //------------------------------------------------------------------
		 		 if(memcmp(package,"cfgX.txt",8)==0&&package[8]==1)		// Запись / пакет № X
					{		
					 if(package[11]==0xaa)
						{
						 ULOGW ("[0xaa] писец"); DestroyWindow(hWnd); break;
						}
					 if(package[11]==0xbb)	// данные!
						{
						}
					 if(package[11]==0xcc)	// 
						{
						}			
				 	 if(package[11]==0x60)	// Мгновенные данные походу. авотхуй. это просто файлики.
						{	
						}
					 if(package[11]==0x55)
						{
		 				 ULOGW ("[0x55] качаем файлики");
						 package[8]=4; package[9]=0; package[10]=1;	 package[11]=0x55;
						 CHAR st[100]={0};
						 strcat(st,(CHAR *)package+12);
						 fclose(idFile);						 
						 if (strstr (st,"cfgA.txt")) strcpy (st,"\\Compact Flash\\As\\cfgA.txt");
						 ULOGW ("[%s]",st);
						 idFile = fopen ( st/*package+12*/,"r+b");
						 if(idFile==0)
							{
							 ULOGW ("Не могу открыть файл cfgX.txt");
							 memcpy(package,"cfgX.txt",8);
							 package[8]=4; package[9]=0; package[10]=1; package[11]=0x57;				
							 package[1+11]= ClkChkSum((CHAR *)package, (1+11));
							 send(srv_socket, (CHAR *)package,1+12, 0);		dFil=NULL;
							}
						 else
							{
							 fseek(idFile, 0, SEEK_END);
							 dFil = ftell(idFile);						
							 fseek(idFile, 0, SEEK_SET);			
							 package[1+11]= ClkChkSum((CHAR *)package,((package[10]|(package[9]<<8))+11));
							 send(srv_socket, (CHAR *)package, 1+12, 0);
							}
						}	
					 if(package[11]==0x58)	
						{
						 UINT k;
							 if((INT)dFil>0)
							{
							 if(dFil>10000)	k= 10000;
							 else			k= dFil;
							 k+=1;
							 memset(package,0xff,k+15);
							 memcpy(package,"cfgX.txt",8);
							 package[8]=4; // => означает ответ!!!
							 package[9]= HIBYTE(k);	// => в ответ идёт команда
							 package[10]=LOBYTE(k);
							 package[11]=0x56;
							 fread( package+12, sizeof(CHAR), k-1	 , idFile);				
	 						 package[k+11]= ClkChkSum((CHAR *)package , (k+11) );
							 send(srv_socket,(CHAR *)package,k+12 , 0);		// Ответ
							 dFil-=10000;				   
							}
						 else
							{
							 fclose(idFile);
							 memcpy(package,"cfgX.txt",8);
							 package[8]=4; // => означает ответ!!!
							 package[9]=0;	// => в ответ идёт команда
							 package[10]=1;
							 package[11]=0x57;				
							 package[1+11]= ClkChkSum((CHAR *)package , (1+11) );
							 send(srv_socket, (CHAR *)package,1+12 , 0);		// Ответ	
							}
						}
					}
				 //------------------------------------------------------------------
				}
				else { ULOGW ("[net] wrong checksum"); index=0; }
			}
		}
	  if(NetState==SOCKET_ERROR||NetState==0) 
		{ 
		 if (NetState==SOCKET_ERROR) 
			{
			 switch (err)
				{
				 case 10048: ULOGW ("WSAEADDRINUSE: address in use"); break;
				 case 10061: ULOGW ("WSAECONNREFUSED: connection not accepted"); break;
				}
			}
		 if (!NetState) ULOGW ("NetState=0");
		 if(NetState==0)
			{
			 sprintf(szTemp,"%08x",TYPE_EVENTS|MODULE_KERNEL|EVENTS_SERVER_DISCONNECT);
			 WriteToLog( szTemp );
			} 	 
		 index=0; StopStart(hWnd); ServerStart(hWnd); 
		 Sleep(5000);	
		}
	 Sleep(10);
	}
 ExitThread(0);
}
//-------------------------------------------------------------------------------------
