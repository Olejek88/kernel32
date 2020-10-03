//----------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
VOID ThrdFnc (LPVOID lpParam);		// рабочий поток
VOID FlsThrdFnc (LPVOID lpParam);	// 
VOID NetThrdFnc (LPVOID lpParam);	//
VOID ChngThrdFnc (LPVOID lpParam);	//
VOID archivData(BOOL print);		// find data to report
VOID noStaff(UINT kodEvents,UINT keSec=0, UINT flag=0);	
UINT FindToLog(MsgExch *MsE,UINT para);
VOID sendAns (VOID);
INT StopStart(HWND hWnd);
INT ServerStart(HWND hWnd);
VOID EvalTime(VOID);
//----------------------------------------------------------------------------------
VOID ULOGW (CHAR* string, ...)
{
TCHAR InBuffer[500]; CHAR buf[500];
Log = fopen(KERNEL_LOG,"a");
GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, NULL, NULL, InBuffer, sizeof(InBuffer));
WideCharToMultiByte(CP_ACP, 0, InBuffer, sizeof(InBuffer), buf, sizeof(buf), NULL,NULL);
fprintf (Log,buf); fprintf (Log," ");
GetTimeFormat(LOCALE_SYSTEM_DEFAULT, TIME_FORCE24HOURFORMAT, NULL, NULL, InBuffer, sizeof(InBuffer));
WideCharToMultiByte(CP_ACP, 0, InBuffer, sizeof(InBuffer), buf, sizeof(buf), NULL,NULL);
fprintf (Log,buf); fprintf (Log,"\t"); 
va_list arg;
va_start(arg, string);
_vsnprintf(buf,sizeof (buf), string, arg);
fprintf (Log, buf); //printf (buf); 
va_end(arg);
fprintf (Log,"\n"); //printf ("\n");
fclose (Log);
}
//----------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow)
{
 hInst = hInstance;		// Store instance handle in our global variable
 WNDCLASS	wc;
 memset(&wc, 0, sizeof(WNDCLASS));
 wc.style			= CS_HREDRAW | CS_VREDRAW;
 wc.lpfnWndProc		= (WNDPROC) WndProc;
 wc.cbClsExtra		= 0;
 wc.hIcon			= 0;
 wc.cbWndExtra		= 0;
 wc.hInstance		= hInstance;
 wc.hCursor			= 0;
 wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
 wc.lpszMenuName	= 0;
 wc.lpszClassName	= L"T";
 RegisterClass(&wc);
 hWnd = CreateWindow(L"T", L"T", WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
 if (!hWnd) return FALSE;
 UpdateWindow(hWnd);
 return TRUE;
}
//----------------------------------------------------------------------------------
BOOL NetDataWR(UCHAR *data, CHAR wr)	// 0/*чтение*/ 1/*запись*/
{
 INT  cb=0; FILE *fBu;	
 CHAR st_[80] = HOMEDIR;
  
 INT i=strlen(st_); 
 INT j=strlen((CHAR*)data);

 memcpy(st_+i,data,j); 
 for (UINT bb=1;bb<10;bb++) if (st_[i+j-bb]<32) st_[i+j-bb]=0; else break;
 st_[i+j]=0;		// +dir+file
 
// ULOGW ("FILE %s",st_);
 if(wr==1)	fBu = fopen (st_,"wb");
 else		fBu = fopen (st_,"rb");
 if(fBu==0) 
	{
	 ULOGW ("can`t open file %s",st_);
	 return FALSE;
	}
 INT qnt;  qnt=data[10]|(data[9]<<8);	// data size
 if(wr==1) cb=fwrite( data+11, sizeof(CHAR),qnt , fBu);
 else	   cb=fread ( data+11, sizeof(CHAR),qnt , fBu);
 fclose(fBu);
 if(HIBYTE(cb)!=HIBYTE(data[9]) && LOBYTE(cb)!=LOBYTE(data[10])) return FALSE;
 return TRUE;
}
//----------------------------------------------------------------------------------
VOID dTime (FILETIME *tms)	// return current timestamp
	{
	 SYSTEMTIME stim;
	 FILETIME ftim;
	 GetLocalTime(&stim);
	 SystemTimeToFileTime(&stim,&ftim);	
	 tms->dwLowDateTime=ftim.dwLowDateTime; 
	 tms->dwHighDateTime=ftim.dwHighDateTime; 
	}
//----------------------------------------------------------------------------------
VOID CopyMem(BYTE &Dest,BYTE &Source,INT Size)
{
 BYTE *p1,*p2;
 p1=&Dest; p2=&Source;	
 for(INT i=0;i<Size;i++) *(p1+i)=*(p2+i);
}
//----------------------------------------------------------------------------------
VOID WriteToLog(CHAR *chMsgLog,BOOL Reset=FALSE)
{
 CHAR ch[80];	FILETIME   ftim;
 hMsgLog = fopen (MsgLog,"r+");
 if(hMsgLog==0)	
	{	
	 hMsgLog=fopen (MsgLog,"w");  fclose(hMsgLog);
	 hMsgLog=fopen (MsgLog,"r+"); if(hMsgLog==0) return;
	}					
 fseek(hMsgLog, 0, SEEK_END);
 if(chMsgLog[0]!=0 && Reset!=TRUE)
	{
	 fseek(hMsgLog, -46, SEEK_END);
	 dTime(&ftim);
	 sprintf(ch,"T +%08x%08x\tMsg +%s\n",ftim.dwHighDateTime,ftim.dwLowDateTime,chMsgLog);
	 dMsLg=fwrite(ch, strlen(ch),1, hMsgLog);
	}
 else
	if(Reset==TRUE) fseek(hMsgLog, -46, SEEK_END);	// ??? 46?
	dTime(&ftim);
	sprintf(ch,"T +%08x%08x\tMsg +20000f02\t//shutdown\n",ftim.dwHighDateTime,ftim.dwLowDateTime);					
	dMsLg=fwrite(ch, strlen(ch), 1, hMsgLog);			
	if(dMsLg!=1 )  ULOGW ("Проблемы записи в messagelog.txt");
	fclose(hMsgLog);
}
//----------------------------------------------------------------------------------
UINT a2i(CHAR *st,INT radix)
{
 UINT a; INT b;
 switch (radix)
	{
	 case 16:
		{a=0;
		 for(INT i=0;i<8;i++)
			{
			 if( st[i]<'0' || st[i]>'f') break;
			 if( st[i]>'9' && st[i]<'a') break;
			 a<<=4;
			 if(st[i] > '9') b=st[i]-'a'+10;
			 else	b=st[i]-'0';
			 a=a|(b&0xf);
			} break;
		}
	 case 10:
		{ a=0; break; }
	}
 return a;
}
//----------------------------------------------------------------------------------
VOID FindTimStrEnd(FILE *hTmp,FILETIME *fStr, FILETIME *fEn)
{
 fseek	(hTmp, 0, SEEK_END);
 UINT	seek = ftell(hTmp)-1;	// 207272
 UINT	chT=sizeof(szT)/2;	// 375
 UINT	kPlu=0;
 DWORD	tmpScn=0;
 while(1)
	{			
	 fseek(hTmp, seek, SEEK_SET);
	 szT[chT]=fgetc(hTmp);			// szT [375]
	 if(szT[chT]=='+')
		{
		 if(kPlu==1) 
			{
			 tmpScn= a2i(szT+chT+1,16);   fEn->dwHighDateTime =tmpScn;
			 tmpScn= a2i(szT+chT+1+8,16); fEn->dwLowDateTime  =tmpScn;
			}
		 kPlu+=1;
		}
	 seek -=1;
	 chT  -=1;
	 if(chT<=5)		break;
	 if(kPlu>=2)	break;
	 if(seek<=0)	break;
	}
// поиск назад
seek=0; chT=sizeof(szT)/2;; kPlu=0;
for(;;)
	{	
	 fseek(hTmp, seek, SEEK_SET);
	 szT[chT]=fgetc(hTmp);
	 if(szT[chT]=='+')
		{
		 if(kPlu==3) 
			{
			 fread( szT, 16, 1, hTmp);
			 tmpScn=a2i(szT,16);		fStr->dwHighDateTime= tmpScn;
			 tmpScn=  a2i(szT+8,16);	fStr->dwLowDateTime= tmpScn;
			}
		 kPlu+=1;
		}
	 seek+=1; chT+=1;
	 if(kPlu>=4)	break;
	 if(feof(hTmp)!=0)	break;	
	}
}
//----------------------------------------------------------------------------------
UINT skokoIh(VOID)
{
 HANDLE hFls=0; BOOL fld;
 WIN32_FIND_DATA FindFls = {0};
 UINT fN=0;
 while(1)
	{
	 WCHAR cW[0xff];
	 wcscpy(cW,AllBase);
	 if(hFls==0)	{hFls =	FindFirstFile(cW, &FindFls); fld=1;}
	 else			fld = FindNextFile(hFls, &FindFls);
	 if(hFls==0 || hFls==(HANDLE)-1) break;
	 if(fld!=(BOOL)1) break;
	 fN+=1;
	} 
 FindClose(hFls);
 return  fN; 
}
//----------------------------------------------------------------------------------
VOID SvToFl(FILE *uFls,UINT nc, UINT kc, UINT flag)
{
 //BYTE *pp; pp=(BYTE *)&KB;
 //ULOGW ("SvToFl (%d > %d)",nc,kc);
 for(UINT j=nc; j<kc; j++)
	{
	 memcpy(&KB,DataM+j*sizeof(CycleBuffer),sizeof(CycleBuffer));
	 //for(UINT i=0; i<sizeof(CycleBuffer);i++) 	*(pp+i)=DataM[j*sizeof(CycleBuffer)+i];
		//else *(pp+i)=Data[j*sizeof(CycleBuffer)+i];
	 if(KB.trg!=0) 
		{
		 //if(j==0) break;
		 //if (j>kc-2) ULOGW ("SvToFl[%d %s %f](%d)",KB.trg,KB.tNmame,(FLOAT)KB.value,KB.status);
		 fprintf(uFls,befkb,KB.trg,KB.tNmame,(FLOAT)KB.value, (DWORD)KB.timestm.dwHighDateTime,(DWORD)KB.timestm.dwLowDateTime, KB.status);
		}
	}
}
//----------------------------------------------------------------------------------
VOID recoveringKB(VOID)
{
 if(DataM==0&&TaskDues==0&&TaskMath==0)	return; 
 FILE *KBuf; KBuf = fopen (KBuffer,"r");
 if(KBuf==0) ULOGW ("Can`t open file\nKBuffer.txt");

 ULOGW("recoveringKB()");
 UINT Polya[11];			//позиции для полей
 UINT kPolya,cb,j=0,prh=0;
 FILETIME cmpTim={0};
 FILETIME prhTim={0};	
 for(j=0;j<kStrKB;j++)
	{
 	 kPolya=0;
	 for(INT p=0;p<255;p++)
		{
 		 cb=fread(szTemp+p,1,1,KBuf);
		 if(szTemp[p]=='+')	{ Polya[kPolya]=p+1; kPolya+=1;  }
		 if(szTemp[p]=='\n') break; 
		 if(cb!=1) break;
		}
	 if(cb!=1) break;
	 cb=Polya[0];	KBtmp.trg = a2i(szTemp+cb,16);
	 cb=Polya[1]+0;	memcpy(KBtmp.tNmame,szTemp+cb,6); for(int i=0;i<sizeof(KBtmp.tNmame);i++) { if(KBtmp.tNmame[i]=='\t'||KBtmp.tNmame[i]=='\n'||KBtmp.tNmame[i]==' ') KBtmp.tNmame[i]=0;}	
	 cb=Polya[2]+0;	KBtmp.value = atof(szTemp+cb);
	 cb=Polya[3];	KBtmp.timestm.dwHighDateTime = a2i(szTemp+cb,16);
	 cb=Polya[3]+8;	KBtmp.timestm.dwLowDateTime  = a2i(szTemp+cb,16);
	 cb=Polya[4];	KBtmp.status=  szTemp[cb]=='1' ? TRUE : FALSE;

	 BYTE *pp; pp=(BYTE *)&KBtmp;
	 for(i=0; i<sizeof(CycleBuffer);i++) 
		DataM[j*sizeof(CycleBuffer)+i]=*(pp+i);		// копия (так надо)

//  сосредоточься на этом коде опять!!! //!!!???
	 if(CompareFileTime(&KBtmp.timestm,&cmpTim)>=0)
		{
		 cmpTim.dwHighDateTime=KBtmp.timestm.dwHighDateTime;
		 cmpTim.dwLowDateTime=KBtmp.timestm.dwLowDateTime;
		 
		 EnterCriticalSection(&fls);		// критический раздел - вхождение
		 Pointer=(j+1)*sizeof(CycleBuffer); 
		 LeaveCriticalSection(&fls);		// критический раздел - выхож
		}
	 else
		{
		 if(prh==0)
			{
			 prhTim.dwHighDateTime = cmpTim.dwHighDateTime;
			 prhTim.dwLowDateTime=cmpTim.dwLowDateTime;
			 prh=j;
			}
		 //prhTim={0}; 
		}
	}
 if(Pointer/sizeof(CycleBuffer)>=kStrKB)
	{
 	 if(prh==0) Pointer=0;
	 else Pointer= (prh)*sizeof(CycleBuffer);
	} 
 fclose(KBuf);
 EnterCriticalSection(&fls); TPointer=TmpTPointer=Pointer; LeaveCriticalSection(&fls);
 ULOGW("recoveringKB complete");
 // MKbuffer.txt >> Data
 //prh=0;
 /*if(Data==0&&TaskDues==0&&TaskMath==0)	return;
 KBuf = fopen (MKBuffer,"r");
 if(KBuf==0) ULOGW ("Can`t open file\nMKBuffer.txt");
 ULOGW("recoveringMKB()");
 for(j=0;j<kStrKB;j++)
	{
 	 kPolya=0;
	 for(INT p=0;p<255;p++)
		{
 		 cb=fread(szTemp+p,1,1,KBuf);
		 if(szTemp[p]=='+')	{ Polya[kPolya]=p+1; kPolya+=1;  }
		 if(szTemp[p]=='\n') break; 
		 if(cb!=1) break;
		}
	 if(cb!=1) break;
	 cb=Polya[0];	MKBtmp.trg = a2i(szTemp+cb,16);
	 cb=Polya[1]+0;	memcpy(MKBtmp.tNmame,szTemp+cb,6); 
	 for(INT i=0;i<sizeof(MKBtmp.tNmame);i++) 
		{ if(MKBtmp.tNmame[i]=='\t'||MKBtmp.tNmame[i]=='\n'||MKBtmp.tNmame[i]==' ') MKBtmp.tNmame[i]=0;}	
	 cb=Polya[2]+0;	MKBtmp.value = atof(szTemp+cb);
	 cb=Polya[3];	MKBtmp.timestm.dwHighDateTime = a2i(szTemp+cb,16);
	 cb=Polya[3]+8;	MKBtmp.timestm.dwLowDateTime  = a2i(szTemp+cb,16);
	 cb=Polya[4];	MKBtmp.status=  szTemp[cb]=='1' ? TRUE : FALSE;

	 BYTE *pp; pp=(BYTE *)&MKBtmp;
	 for(i=0; i<sizeof(CycleBuffer);i++) 
		Data[j*sizeof(CycleBuffer)+i]=*(pp+i);		// копия (так надо)

	 if(CompareFileTime(&MKBtmp.timestm,&cmpTim)>=0)
		{
		 cmpTim.dwHighDateTime=MKBtmp.timestm.dwHighDateTime;
		 cmpTim.dwLowDateTime=MKBtmp.timestm.dwLowDateTime;		 
		 EnterCriticalSection(&fls);		// критический раздел - вхождение
		 MPointer=(j+1)*sizeof(CycleBuffer); 
		 LeaveCriticalSection(&fls);		// критический раздел - выхож
		}
	 else
		{
		 if(prh==0)
			{
			 prhTim.dwHighDateTime = cmpTim.dwHighDateTime;
			 prhTim.dwLowDateTime=cmpTim.dwLowDateTime;
			 prh=j;
			}
		}
	}
 if(MPointer/sizeof(CycleBuffer)>=kStrKB)
	{
 	 if(prh==0) MPointer=0;
	 else MPointer= (prh)*sizeof(CycleBuffer);
	}
 fclose(KBuf);
 EnterCriticalSection(&fls); MTPointer=MTmpTPointer=MPointer; LeaveCriticalSection(&fls);*/
 ULOGW ("recoveringKB TPointer=%d TmpTPointer=%d",TPointer,TmpTPointer);
}
//----------------------------------------------------------------------------------
VOID UpLoadSaveKB (UINT Point1,UINT Point2,CHAR* filename, UINT flag)
	{
	 //ULOGW ("UpLoadSaveKB");
	 FILE *UpLdFls;	 INT tmp=0;
	 EnterCriticalSection(&cs);	 tmp=ExgengeFlags;	 LeaveCriticalSection(&cs);
	 if(tmp!=0)
	 	{
	     UpLdFls = fopen (filename,"r+");
	     if(UpLdFls==0) 
			{
			 UpLdFls = fopen (filename,"w");			 
			 for(UINT j=0;j<kStrKB; j++)
				{
				 fprintf(UpLdFls,befkb,0,"      ",0.0, 0,0,0);
				}
			 fclose(UpLdFls); UpLdFls = fopen (filename,"r+");
			}
		  UINT sm=Point2*70;
		  fseek(UpLdFls,sm,SEEK_SET);
		  if(Point1>Point2)	SvToFl(UpLdFls,Point2,Point1,flag);
		  else
			{ 
			 SvToFl(UpLdFls,Point2,kStrKB,flag);	// SvToFl (4975 > 5000)
			 fseek(UpLdFls,0,SEEK_SET);				// 
			 SvToFl(UpLdFls,0,Point1,flag);			// SvToFl (0 > 3)
			}
		  fclose(UpLdFls);
		}	
	}
//----------------------------------------------------------------------------------
VOID CalculateRashod (UINT units)
{
 ED.r_units=units;
 if(ED.type_su==13) ED.type_su=su_vcorrector;
 //printf ("PP.rnom = %f | dp_units = %d | p_units = %d  | t_units = %d | r_units=%d\n",PP.rnom,ED.dp_units,ED.p_units,ED.t_units,ED.r_units);
 //printf ("PP.rnom = %f | dp_units = %d | p_units = %d  | t_units = %d | r_units=%d\n",PP.rnom,ED.dp_units,ED.p_units,ED.t_units,ED.r_units);
 if((ERC_OK1=_PrepareData(ED, PD))!=0)
	{
	 if (!EventsFlags[0])
		{
		 sprintf(szTemp,"%08x", TYPE_EVENTS|MODULE_FLOWCALC|ERC_OK1);
		 WriteToLog(szTemp); EventsFlags[0]=TRUE;
		}
	 ULOGW ("PrepareData error (%d)",ERC_OK1);
	}
 else 
	{
	 if (EventsFlags[0]) 
		{
		 sprintf(szTemp,"%08x", TYPE_EVENTS|MODULE_FLOWCALC|EVENTS_FLOW_OK);
		 WriteToLog(szTemp); EventsFlags[0]=FALSE;
		}
	}
 if((ERC_OK2=_Rashod(ED, SD, PD, PP, RD))!=0)
	{
	 if (!EventsFlags[1])
		{
		 sprintf(szTemp,"%08x", TYPE_EVENTS|MODULE_FLOWCALC|ERC_OK2);
		 WriteToLog(szTemp); EventsFlags[1]=TRUE;
		}
	 ULOGW ("Rashod error (%d)",ERC_OK2);
	}
 else 
	{
	 if (EventsFlags[1]) 
		{
		 sprintf(szTemp,"%08x", TYPE_EVENTS|MODULE_FLOWCALC|EVENTS_FLOW_OK);
		 WriteToLog(szTemp); EventsFlags[1]=FALSE;
		}
	}
}
//----------------------------------------------------------------------------------
VOID calc_oth_data(INT i)
	{
	 CHAR gprsbf[300]={0};
	 //ULOGW ("calc_oth_data (%d|%d)",tSTART.wYear,tSTART.wMonth);
	 if(tSTART.wYear==0 && tSTART.wMonth==0)
		{
		 GetLocalTime(&tSTART);
		 ULOGW ("archive data started from (%d.%d.%d %d:%d:%d)",tSTART.wDay,tSTART.wMonth,tSTART.wYear,tSTART.wHour,tSTART.wMinute,tSTART.wSecond);
		 WriteToLog("20000f03");
		 wHour = tSTART.wHour; wDay  = tSTART.wDay;
		 wMonth  = tSTART.wMonth; wYear  = tSTART.wYear;
		 return;
		}
	 DWORD tmpM=ttmpSTART.wMinute;
	 GetLocalTime(&ttmpSTART);
	 //-------------------------------------------------------
	 CycleBuffer *tD,*Hrc;					// сначала расчитываем часовые!!!
	 MassRez[0] = (DWORD FAR)PromRez;		// 180 секундные накопления (18 штук за 180 секунд)
	 MassRez[1] = (DWORD FAR)PromRezHrs;	// часовые накопления (20 штук за час)
	 MassRez[2] = (DWORD FAR)PromRezDay;	// за месяц накопления (31 штука за месяц)
	 MassRez[3] = (DWORD FAR)PromRezMonth;	// за год накопления (12 штук за год)
	 MassRez[4] = (DWORD FAR)PromRezYear;	// за все время накопления (не обнуляется)
	 MassRez[5] = (DWORD FAR)PromRezYear;	// за все время накопления (не обнуляется)

	 INT chistka=0;
	 for(INT glbCl=0;glbCl<5;glbCl++)
		{
		 tD = (CycleBuffer *)MassRez[glbCl] + (i*(klvPrm));
		 //ULOGW ("tD(%d,%d,%d)=%f",glbCl,i,klvPrm,tD->value);	// tD(0,0,7) tD(1,0,7)
		 Hrc= (CycleBuffer *)MassRez[glbCl+1] + (i*(klvPrm));
		 for(INT j=0;j<klvPrm;j++)
			{
			 Hrc->trg&=tD->trg;			// current target
			 Hrc->status*=tD->status;	// current status			 			 
			 if (!glbCl)	// раз в 180 секунд
				{
				 SumPromRezMin[i*klvPrm+j]++;						// количество таких промежуточных увеличиваем
				 if (j!=4 && j!=5 && j!=3)
					{
					 Hrc->value+=(tD->value/SumPromRez[i*klvPrm+j]);// за час будет текущее среднее за 180 + текущее часовое				 
					 //ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value/SumPromRez[i*klvPrm+j]),Hrc->value,SumPromRezMin[i*klvPrm+j]);
					}
				  else
					{
					 Hrc->value+=tD->value;							// за час будет сумма
					 //ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value),Hrc->value,SumPromRezMin[i*klvPrm+j]);
					}
				}
			 if (glbCl==1) // раз в час
				{				 
				 SumPromRezHrs[i*klvPrm+j]++;						// часовые добавляются
				 if (j!=4 && j!=5 && j!=3)
					{
					 Hrc->value+=(tD->value/SumPromRezMin[i*klvPrm+j]);	// в день пошла сумма текущего и среднечасового за этот час				 
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value/SumPromRezMin[i*klvPrm+j]),Hrc->value,SumPromRezHrs[i*klvPrm+j]);
					}
				 else 
					{
					 Hrc->value+=(tD->value);					 
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value),Hrc->value,SumPromRezHrs[i*klvPrm+j]);
					}
				}
			 if (glbCl==2) // раз в день
				{
				 SumPromRezDay[i*klvPrm+j]++;						// дневные добавляются
				 if (j!=4 && j!=5 && j!=3)
					{
					 Hrc->value+=(tD->value/SumPromRezHrs[i*klvPrm+j]); // в месяц пошла сумма текущего и среднедневного за этот день					
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value/SumPromRezHrs[i*klvPrm+j]),Hrc->value,SumPromRezDay[i*klvPrm+j]);
					}
				 else
					{
					 Hrc->value+=(tD->value);
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value),Hrc->value,SumPromRezDay[i*klvPrm+j]);
					}					 
				}
			 if (glbCl==3) // раз в месяц
				{
				 SumPromRezMonth[i*klvPrm+j]++;						// месячные добавляются
				 if (j!=4 && j!=5 && j!=3)
					{
					 Hrc->value+=(tD->value/SumPromRezDay[i*klvPrm+j]);// в год пошла сумма текущего и среднемесячного за этот месяц
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value/SumPromRezDay[i*klvPrm+j]),Hrc->value,SumPromRezMonth[i*klvPrm+j]);
					}
				 else
					{
					 Hrc->value+=(tD->value);
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value),Hrc->value,SumPromRezMonth[i*klvPrm+j]);
					}
				}
			 if (glbCl==4) // раз в год
				{
				 SumPromRezYear[i*klvPrm+j]++;						// годовые добавляются
				 if (j!=4 && j!=5 && j!=3)
					{
					 Hrc->value+=(tD->value/SumPromRezMonth[i*klvPrm+j]);// на все время пошла сумма текущего и среднегодового за этот год
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value/SumPromRezMonth[i*klvPrm+j]),Hrc->value,SumPromRezYear[i*klvPrm+j]);
					}
				 else
					{
					 Hrc->value+=(tD->value);
					 ULOGW ("A[%d][%d] %s = %f(%f) [%d]",glbCl,j,Hrc->tNmame,(tD->value),Hrc->value,SumPromRezYear[i*klvPrm+j]);
					}
				}
 			 memcpy(&Hrc->tNmame,&tD->tNmame,6);	// name
			 Hrc+=1;tD+=1;				// next param
			}
		 GetLocalTime(&ttmpSTART);	 GetLocalTime(&tSTART);
		 //ULOGW ("[%d %d] [%d %d] [%d %d]",wHour,ttmpSTART.wHour,wDay,ttmpSTART.wDay,wMonth,ttmpSTART.wMonth);
		 if(wHour==ttmpSTART.wHour && glbCl==0) break;	// раз в 180 секунд проверяем новый ли час
			wHour = tSTART.wHour;							// если новый, то засекаем какой
		 if(wDay ==ttmpSTART.wDay && glbCl==1)  break;	// раз в час проверяем новый ли день
			wDay  = tSTART.wDay;
		 if(wMonth==ttmpSTART.wMonth && glbCl==2) break;	// раз в день проверяем новый ли месяц
			wMonth= tSTART.wMonth;
		 if(wYear==ttmpSTART.wYear && glbCl==3) break;	// раз в месяц проверяем новый ли год
			wYear = tSTART.wYear;
		 if (glbCl==4) break;	// годовые мы уже занесли
		 chistka+=1;										// чистка =1, если час новый, 2 если день,....
		 // Укладываем данные в Кольцевой буфер
		 Hrc= (CycleBuffer *)MassRez[glbCl+1]+(i*klvPrm);	// 
		 (Hrc+klvPrm)->status=1;
		 for(INT b=0;b<klvPrm;b++)
			{	
			 if (indata[i].Enabled==0 && b==1) b=klvPrm-1;
			 //if (indata[i].Enabled==1 && b>=3 && b!=klvPrm-1) b=klvPrm-2;
			 Hrc= (CycleBuffer *)MassRez[glbCl+1]+(i*klvPrm);
			 Hrc+=b;
			 BYTE *p; p=(BYTE *) &KB;
			 KB.trg = (0x80000000|(0x100000<<(glbCl))|i+1)|(b<<24);
			 dTime(&KB.timestm);//GetTickCount();//count;
			 KB.status	= Hrc->status;
			 memcpy(&KB.tNmame,&Hrc->tNmame,6);
			 if (b==7) sprintf (KB.tNmame,"Q%d,kh",i+1); //strcpy (KB.tNmame,"Q,kgh");
			 if (b==6) sprintf (KB.tNmame,"Q%dkr",i+1);  //strcpy (KB.tNmame,"Q_kr");
			 if (b==5) sprintf (KB.tNmame,"V%d,m3",i+1); //strcpy (KB.tNmame,"V,m3");
			 if (b==4) sprintf (KB.tNmame,"M%d,kg",i+1); //strcpy (KB.tNmame,"M,kg");
			 if (b==3) sprintf (KB.tNmame,"K%d,kc",i+1); //strcpy (KB.tNmame,"K,kJ");
 			 //if (indata[i].es.type_su==13 && (b!=5 && b!=4 && b!=3))
			 if ((b!=5 && b!=4 && b!=3))
				{
				 if (glbCl==0) KB.value=(Hrc->value)/SumPromRezMin[i*klvPrm+b];	// раз в час часовое из минутных
				 if (glbCl==1) KB.value=(Hrc->value)/SumPromRezHrs[i*klvPrm+b]; // раз в день дневное из часовых
				 if (glbCl==2) KB.value=(Hrc->value)/SumPromRezDay[i*klvPrm+b]; // раз в месяц месячное из дневных
				 if (glbCl==3) KB.value=(Hrc->value)/SumPromRezMonth[i*klvPrm+b]; // раз в год из месячных
				 if (b==0) sprintf (gprsbf,"%s%s=%f\n",gprsbf,KB.tNmame,KB.value);
				 if (b==1) sprintf (gprsbf,"%s%s=%f\n",gprsbf,KB.tNmame,KB.value);
				 if (b==2) sprintf (gprsbf,"%s%s=%f\n",gprsbf,KB.tNmame,KB.value);
				 if (b==6) sprintf (gprsbf,"%s%s=%f\n",gprsbf,KB.tNmame,KB.value);
				}
			 else 
			 	{				 
				 KB.value=(Hrc->value);
				}		 	 
			 if (wMonth!=ttmpSTART.wMonth && glbCl==3 && j==5) LastMon[i]=KB.value;
		 	 ULOGW (">>A[%d][%d] %s = %f [%d]",glbCl,b,KB.tNmame,KB.value,KB.status);
			 for(INT n=0;n<sizeof(CycleBuffer);n++) DataM[n+Pointer]= *(p+n);
			 Pointer+=(LONG) sizeof(CycleBuffer);
			}
		 if(Pointer>= (LONG) (kStrKB)*sizeof(CycleBuffer)) Pointer=0;
		 //sendsms ("+79227000293",gprsbf);
		}
	if(chistka!=0)
		{
		 for(INT i_=0;i_<chistka;i_++)
			{
			 ULOGW ("chistka (%d)",i_);
			 Hrc=(CycleBuffer *)MassRez[i_+1]+(i*klvPrm);
			 memset(Hrc,0,sizeof(CycleBuffer)*klvPrm);
			 for(INT b=0;b<klvPrm;b++)			 
				{
				 (Hrc+b)->status=1;
				 (Hrc+b)->value=0;
				 if (i_==0) SumPromRezMin[i*klvPrm+b]=0;
				 if (i_==1) SumPromRezHrs[i*klvPrm+b]=0;
				 if (i_==2) SumPromRezDay[i*klvPrm+b]=0;
				 if (i_==3) SumPromRezMonth[i*klvPrm+b]=0;
				}
			}
		}
 return;
}
//----------------------------------------------------------------------------------
CHAR ClkChkSum(CHAR *adr,UINT k)
{
 UINT CheckSum=0; UINT tm;
 for (UINT i=0;i<k;i++)
	{	
	 tm=adr[i]&0xff;
	 CheckSum+=tm;
	 if(CheckSum >= 0x100) CheckSum -= 0xff;
	}
 CheckSum=(CheckSum^0xff)&0xff;
 return (CHAR)(CheckSum&0xff);
}
//----------------------------------------------------------------------------------
VOID recvAns ()
	{
	 package[8]=0;
	 if(NetDataWR(package, 1)==TRUE) Sleep(1);
	 package[8]=4; package[9]=	0; package[10]=0;
	 package[11]= ClkChkSum((CHAR *)package,((package[10]|(package[9]<<8))+11));
	 send(srv_socket, (CHAR *)package, 12, 0);
	}
//----------------------------------------------------------------------------------
VOID sendAns (CHAR* filename)
	{
	 package[8]=4;
	 FILE *fBuf; fBuf = fopen (filename,"rb");
	 if(fBuf==0) ULOGW ("cannot open file");
	 fseek(fBuf, 0, SEEK_END);
	 package[9]= HIBYTE(ftell(fBuf)); package[10]=LOBYTE(ftell(fBuf));
	 fseek(fBuf, 0, SEEK_SET);
	 fclose(fBuf);
	 //package[9]=	sizeof(Devc[DEVICE_NUM_MAX]);		//0;	// => всё хорошо
	 if(NetDataWR(package, 0)==TRUE) 
		{ 
		 package[(package[10]|(package[9]<<8))+11]=ClkChkSum((CHAR *)package,((package[10]|(package[9]<<8))+11));
		 send(srv_socket, (CHAR *)package, (package[10]|(package[9]<<8))+12, 0);		
		}
	}
//-------------------------------------------------------------------------------------------------
// 0 - slave, 1-master (opc), 2-master (change) 
BOOL eConnection (CHAR tP, INT _s_port, SOCKET &sck, SOCKET &cli)
{
 INT rc;
 // Инициализация и проверка версии Windows Sockets
 rc = WSAStartup(MAKEWORD(2, 2), &WSAData);
 if(rc != 0)
	{
	 ULOGW ("WSAStartup failed. Error: %x",WSAGetLastError ());
	 return FALSE;
	}
 // Создаем сокет 
 sck = socket(AF_INET, SOCK_STREAM, 0);
 if(sck == INVALID_SOCKET)
	{
	 ULOGW ("Allocating socket failed. Error: %x",WSAGetLastError ());
	 return FALSE;
	}
// _s_port= SERV_PORT;
 // Устанавливаем адрес IP и номер порта
 srv_address.sin_family = AF_INET;
 srv_address.sin_addr.s_addr = INADDR_ANY;
 srv_address.sin_port = htons(/*SERV_PORT*/_s_port);
  
 //Связываем адрес IP с сокетом
 if(bind(sck, (LPSOCKADDR)&srv_address,sizeof(srv_address)) == SOCKET_ERROR)
	{
	 // При ошибке закрываем сокет
	 closesocket(sck);
	 //ULOGW ("Bind Error");
	 return FALSE;
	}
 // Устанавливаем адрес IP и номер порта
 srv_address.sin_family = AF_INET;
 //ULOGW ("gethostbyname(%s)",strC);
 if (!tP)
	{	 
	 phe = gethostbyname(strC);
	 //if(phe == NULL) 	 ULOGW ("gethostbyname %s not found | reason is %x",strC, WSAGetLastError ());
	 if(phe != NULL) // Копируем адрес узла
		{
		 memcpy((CHAR FAR *)&(srv_address.sin_addr), phe->h_addr, phe->h_length);
		 //ULOGW ("hostbyname \"%s\" is found ", strC);
		}
	 else srv_address.sin_addr.s_addr = inet_addr(strC);
	 // ULOGW ("Establish a connection to the server socket");
	 if(connect(sck, (PSOCKADDR)&srv_address, sizeof(srv_address)) < 0)
		{
		 closesocket(sck);
		 //ULOGW("Establish a connection to %s ... connect Error | reason is %d",strC,WSAGetLastError ());
		 return FALSE;
		}
	 else 
		{
		 ULOGW("Establish a connection to %s ... connect success",strC);
		 sprintf(szTemp,"%08x",TYPE_EVENTS|MODULE_KERNEL|EVENTS_SERVER_CONNECT );	// передаём код сообщения, и только его!
		 //WriteToLog(szTemp);
		}
	}
 else
	{
	 ULOGW ("Establish a socket to listen for incoming connections");
	 if (listen (sck, 4) == SOCKET_ERROR) 
		{
		 ULOGW ("Listening to the client failed. Error: %d", WSAGetLastError ());
		 closesocket (sck);
		 return FALSE;
		}
	 accept_sin_len = sizeof (accept_sin);
	 cli = accept (sck,(struct sockaddr *) &accept_sin,(int *) &accept_sin_len);
	 printf ("Stop listening for connections from clients\n");
	 ULOGW ("Stop listening for connections from clients");
	 closesocket (sck);
	 if (cli == INVALID_SOCKET)
		{
		 ULOGW ("Accepting connection with client failed. Error: %d", WSAGetLastError ());
		 return FALSE;
		}
	 ULOGW ("Config socket");
	 BOOL l = TRUE;
	 if (!ioctlsocket (cli,FIONBIO,(unsigned long* ) &l)) ULOGW ("Sending data");
	 //memcpy(package,"Instrumentalniy ceh",20);
	 //package[20]= RDspl_length;
	 //for (UINT ww=0;ww<RDspl_length; ww++)
	//	{
	//	 tmpDsp=(DplExch *)RDspl+ww;
	//	 if(tmpDsp->name[0]==0) strcpy(tmpDsp->name,"EMPTY");
	//	 memset(Dspl.name,0,7);
	//	 memcpy(Dspl.name,tmpDsp->name,7);
	//	 memcpy(package+21+ww*7,&Dspl.name,7);
	//	 ULOGW ("[opc](%d/%d) %s = %f (%d)",ww,RDspl_length,Dspl.name,Dspl.var,Dspl.status); 
	//	 printf ("[opc](%d/%d) %s = %f (%d)\n",ww,RDspl_length,Dspl.name,Dspl.var,Dspl.status);
	//	}
	 //send(cli,(CHAR *)package, 1200, 0);		// Ответ
	}
 return TRUE;
}
//-------------------------------------------------------------------------------------------------
INT StopStart(HWND hWnd)
{
 // Отменяем приход любых извещений в главную функцию окна при возникновении любых событий, связанных
 // с системой Windows Sockets //////WSAAsyncSelect(srv_socket, hWnd, 0, 0); 
 // Если сокет был создан, закрываем его //	if(srv_socket != INVALID_SOCKET)
 INT tmp;
 if(srv_socket != INVALID_SOCKET)
	if((tmp=closesocket(srv_socket))!=0)  
		{} //ULOGW ("Error: in function Closesocket()");
 if((tmp=WSACleanup())!=0)  ULOGW ("Error: in function WSACleanup()");
 hReadEvents[0]=(HANDLE) 0x30;
 return 0;
}
//-------------------------------------------------------------------------------------------------
INT ServerStart(HWND hWnd)
{ 
 FILE	*cfgNet; cfgNet = fopen (CfgNet,"rt");
 WCHAR	fPort[16]={0}; WCHAR	fSNam[160]={0}; 
 if(cfgNet==0)	
	{ wcscpy(fPort,L"4999");	wcscpy(fSNam,L"Southpark"); }
 else	
	{
	 fseek(cfgNet,pzst,SEEK_SET);
	 INT uk=0,p=0; CHAR sm=0;
	 memset(strC,0,sizeof(strC));
	 fPort[0]=0;
	 while(1)
		{
		 sm=fgetc(cfgNet) ;
		 if( sm!=' ' && sm!='\t' && sm!='\n' && sm!=EOF )
			{
			 strC[p]=sm; p+=1;
			}
		 else
			{
			 if(p!=0)
				{
				 if(fPort[0]==0)
					{
					 MultiByteToWideChar(CP_ACP, 0, strC, strlen(strC)+1,fPort,sizeof(fPort)/sizeof(fPort[0])); 
					 p=0; 
					}
				 else 
					{
					 MultiByteToWideChar(CP_ACP, 0, strC, strlen(strC)+1,fSNam,sizeof(fSNam)/sizeof(fSNam[0]));
					 uk+=1;	p=0;  break; 
					}
				}
			}
		 uk+=1;
		 if(sm==EOF)	
			{ 
			 uk=0; pzst=0; break; 
			}
		}
	if(sm==EOF)	{ uk=0 ; pzst=0;}
	pzst+=uk;
	}
 fclose(cfgNet);
 BOOL ret = eConnection (0, SERV_PORT, srv_socket, opc_cln_socket);
 if (ret) 
	{ 
	 hReadEvents[0]=(HANDLE) 0x20;
	 return TRUE;
	}
 else return FALSE;
}
//-------------------------------------------------------------------------------------------------
VOID EvalTime (VOID)
{
 SYSTEMTIME sts; FILETIME fts;
 GetLocalTime(&sts);
 sts.wSecond=0; sts.wMonth=3; sts.wHour=2; sts.wMinute=0; sts.wDay=31;
 SystemTimeToFileTime (&sts,&fts);
 FileTimeToSystemTime (&fts,&sts);
 sts.wDay=31-sts.wDayOfWeek;
 tmy_l=sts.wHour+sts.wDay*100+sts.wMonth*100*100+sts.wYear*100*100*100;

 sts.wSecond=0; sts.wMonth=10; sts.wHour=3; sts.wMinute=0; sts.wDay=31;
 SystemTimeToFileTime (&sts,&fts);
 FileTimeToSystemTime (&fts,&sts);
 sts.wDay=31-sts.wDayOfWeek;
 tmy_w=sts.wHour+sts.wDay*100+sts.wMonth*100*100+sts.wYear*100*100*100;
 ULOGW ("Time [w:%d|s:%d]",tmy_w,tmy_l);
}
//-------------------------------------------------------------------------------------------------
