//--------------------------------------------------------------------------
BYTE *p;					// just pointer
CycleDues	*pp;			// temporary sensors info for dispetcher
DOUBLE val[7]={0,0,0,0,0,0,0};	// current values
CycleBuffer	*prr,*prrt;			// pointer to buffer structure
INT i=0; INT inP=(TaskDues_length/sizeof(CycleDues)); // input parametrs
pp= (CycleDues *) TaskDues;	// link to recieved data for server
if(TaskDues!=0)				// if we have something to do
{
for(i=0;i<(INT)ChInData;i++)	// calculating values
for(INT j=0;j<(INT)inP;j++)		// sensors configurate
	{
	 //ULOGW ("time [%d] %d %d",j,pp[j].rzrv1[1],pp[j].rzrv1[0]);
	 if(pp[j].rzrv1[1]==0 && pp[j].rzrv1[0]!=0)						// time is zero
		{
		 CopyMem((BYTE &)Ole1.target,(BYTE &)pp[j].target,sizeof(CycleDues));	// target for me
		 #ifdef SNS
			Ole1.forml=(INT)Ole1.rzrv2;				// formuls for me
			//ULOGW ("[%d] %s",j,Ole1.name);
			DR = GetData(Ole1);						// get data from me
		 #endif
		 //------------
		 UINT ty=88;
		 if((UINT)indata[i].es.d==pp[j].target) ty=5;
		 if((UINT)indata[i].es.dsu==pp[j].target) ty=4;
		 if((UINT)indata[i].es.roug==pp[j].target)	ty=3;
		 if((UINT)indata[i].es.t==pp[j].target) ty=2;
		 if((UINT)indata[i].es.p==pp[j].target) ty=1;
		 if((UINT)indata[i].es.dp==pp[j].target) ty=0;
		 
		 p=(BYTE *) &KB;
		 KB.trg = (DR.device<<16)|(DR.nmodule<<8)|DR.kanal;	// form target
		 if(KB.trg==0) KB.trg=-1;			// huynu return
		 KB.status = DR.status;			// status directom
		 KB.value = DR.value;			// data directom
		 dTime(&KB.timestm);				// current time
		 memcpy(&KB.tNmame,&Ole1.name,6);	// copy name direct from me		

		 if (ty<88)
			{
			 Rez[i*klvPrm+ty]=Rez[i*klvPrm+ty]+(DOUBLE)(KB.value);
			 SumRez[i*klvPrm+ty]++;
			 //ULOGW ("[%d][%d] %f(%d)",i,j,Rez[i*klvPrm+ty],(DWORD)SumRez[i*klvPrm+ty]);
			}
		 //------------
		 if(RDspl_length!=0)				
			{
			 tmpDspl= (DplExch *)RDspl+j;
			 memcpy(tmpDspl->name,&Ole1.name,6);// name > display
			 tmpDspl->var=(DR.value);			// value > display
			 tmpDspl->status=KB.status;			// status > display
			 tmpDspl->timestm.dwLowDateTime=KB.timestm.dwLowDateTime;
			 tmpDspl->timestm.dwHighDateTime=KB.timestm.dwHighDateTime;				 
			 //ULOGW ("tmpDspl(%d) %s=%f(%d)",j,tmpDspl->name,tmpDspl->var,tmpDspl->status);
			}
		  pp[j].rzrv1[1]=pp[j].rzrv1[0]-1;		 
		}
	 else pp[j].rzrv1[1] -=1;
	}
}
//--------------------------------------------------------------------------
for(i=0;i<(INT)ChInData;i++)	// calculating values
	{
	 if(indata[i].TaktData[1]==0)
		{
		 indata[i].TaktData[1]=indata[i].TaktData[0]-1;	// set timer
		 INT cn=0; BOOL stat=1;
		 //ULOGW ("inP=%d",inP); 
 		 for(INT j=0 ;j<(INT)inP;j++)
			{
			 ///ULOGW ("cn =%d %d",cn,indata[i].Enabled);
			 ///ULOGW ("[%x] %x %x %x %x %x %x",pp[j].target,(UINT)indata[i].es.dp,(UINT)indata[i].es.p,(UINT)indata[i].es.t,(UINT)indata[i].es.d, (UINT)indata[i].es.dsu, (UINT)indata[i].es.roug);
			 if(cn>=3 && indata[i].Enabled!=2) break;
			 if(((UINT)indata[i].es.dp==pp[j].target || (UINT)indata[i].es.p==pp[j].target || (UINT)indata[i].es.t==pp[j].target // values in flow calculation
				 ||  ((UINT)indata[i].es.d==pp[j].target || (UINT)indata[i].es.dsu==pp[j].target || (UINT)indata[i].es.roug==pp[j].target) && indata[i].Enabled==2))
				{
				 // запрашиваем все значения, входящие в расчет, запрашиваем их, заносим их в буфер и подготавливаем вычисления
				 CopyMem((BYTE &)Ole1.target,(BYTE &)pp[j].target,sizeof(CycleDues));
				 cn+=1;
				 #ifdef SNS
					Ole1.forml=(INT)Ole1.rzrv2;
					DR = GetData(Ole1);
				 #endif				 
				 UINT ty=0;
				 if((UINT)indata[i].es.d==pp[j].target) ty=5;
				 if((UINT)indata[i].es.dsu==pp[j].target) ty=4;
				 if((UINT)indata[i].es.roug==pp[j].target)	ty=3;
				 if((UINT)indata[i].es.t==pp[j].target) ty=2;
				 if((UINT)indata[i].es.p==pp[j].target) ty=1;
				 if((UINT)indata[i].es.dp==pp[j].target) 
					{ ty=0;	if (indata[i].Enabled==0) cn=3; }
				 // corrector + flow
				 prr = (CycleBuffer *) PromRez ; prrt = (prr + ty)+(i*klvPrm);
				 //ULOGW ("val[%d]=Rez(%d*%d+%d)%f/SumRez(%d*%d+%d)%d = %f",ty,i,klvPrm,ty,Rez[i*klvPrm+ty],i,klvPrm,ty,SumRez[i*klvPrm+ty],val[ty]);
				 //ULOGW ("prrt->value = %f",prrt->value);
				 prrt->value=prrt->value+(Rez[i*klvPrm+ty]/(DWORD)SumRez[i*klvPrm+ty]);
				 if (!SumPromRez[i*klvPrm+ty]) prrt->status=1;
				 SumPromRez[i*klvPrm+ty]++;
				 //ULOGW ("val[%d]=Rez(%d*%d+%d)%f/SumRez(%d*%d+%d)%d = %f",ty,i,klvPrm,ty,Rez[i*klvPrm+ty],i,klvPrm,ty,SumRez[i*klvPrm+ty],val[ty]);
				 val[ty]=(Rez[i*klvPrm+ty]/(DWORD)SumRez[i*klvPrm+ty]);
				 prrt->trg&=(DR.device<<16)| (DR.nmodule<<8)|DR.kanal;
				 //prrt->status*=	DR.status;
				 stat=stat*DR.status;
				 //ULOGW (">[%d][%d] %f(%d) -> %f(%d) %d[%d]",i,j,Rez[i*klvPrm+ty],(DWORD)SumRez[i*klvPrm+ty],prrt->value,SumPromRez[i*klvPrm+ty],DR.status,prrt->status);
				 memcpy(&prrt->tNmame,&Ole1.name,6);
				 //--------
				 //if(MPointer>= (LONG) (kStrKB-50)*sizeof(CycleBuffer)) MPointer=0;
	  			 //p=(BYTE *) &MKB;
				 //MKB.trg = ((0x40000000|i)+1)|(j<<24); MKB.status = KB.status; MKB.value = Rez[i*klvPrm+ty]/(DWORD)SumRez[i*klvPrm+ty];
				 //dTime(&MKB.timestm); memcpy(&MKB.tNmame,&Ole1.name,6);
				 //for(INT n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);
				 //MPointer+=(LONG) sizeof(CycleBuffer);				 
				 //--------
				 Rez[i*klvPrm+ty]=0; SumRez[i*klvPrm+ty]=0;
				}
			}
		if(cn!=3 && indata[i].Enabled==1) ULOGW ("wrong configuration (%d,%d)",cn,indata[i].Enabled); // one or more parametr missed
		else 
			{
			 CopyMem((BYTE &) ED,(BYTE &) indata[i].es.d,sizeof(TENTRY_DATA));
			 ED.dp=val[0]; ED.p=val[1]; ED.t=val[2];
			 DOUBLE Qsk=0,Mkg=0; 
			 DOUBLE Tk=0;
			 if(indata[i].Enabled==1)
				{				
				 //ULOGW ("ED.d=%f ED.dsu=%f ED.type_su=%d SD.rnom=%f ED.pb=%f",ED.d,ED.dsu,ED.type_su,SD.rnom,ED.pb);
				 //u_kcal_m3 u_MJ_m3 30 31
				 ERC_OK1=_CalcGasCalor(SD,30,ac_gost,Tk);
				 // ULOGW ("CalcGasCalor (%d)",ERC_OK1);
				 //ULOGW ("CalculateRashod (%7.3f,%7.3f,%6.2f)",ED.dp,ED.p,ED.t);
				 CalculateRashod(ED.r_units); Qsk=RD.r;
				 CalculateRashod(u_kg_h); Mkg=RD.r;
				 printf ("Q=%7.3f | P=%7.3f | T=%6.2f |[Qs=%9.3f]\n",ED.dp,ED.p,ED.t,Qsk);
				 //ULOGW  ("Q=%7.3f\tP=%7.3f\tT=%6.2f\t[Qs=%9.3f]",ED.dp,ED.p,ED.t,Qsk);
				}
			 if(indata[i].Enabled==0)  RD.r=ED.dp;
			 //ULOGW ("indata[i].Enabled");
			 if(indata[i].Enabled==2)
				{
				 Rsh.Ttp1=val[0];
				 Rsh.Ttp2=val[1];
				 if((BOOL)indata[i].es.rad==TRUE)
					{
					 Rsh.Ptp1=val[2];
					 Rsh.Ptp2=val[3];
					}
				 else
					{
					 Rsh.Ptp1=indata[i].es.t;
					 Rsh.Ptp2=indata[i].es.d;
					 val[2]=indata[i].es.t;
					 val[3]=indata[i].es.d;
					}
				 Rsh.Qtp1=val[4];
				 Rsh.Qtp2=val[5];
				 //RD.r=EvaluateQ (Rsh); 
				}
			  //----------------------------------
			  prr = (CycleBuffer *) PromRez;
			  // if device is corrector we evaluate volume too
			  //if(Pointer>= (LONG) (kStrKB-50)*sizeof(CycleBuffer)) Pointer=0;
			  //if(MPointer>= (LONG) (kStrKB-50)*sizeof(CycleBuffer)) MPointer=0;
			  if (ED.type_su==su_vcorrector)
				{				 				 
				 prrt = (prr+7)+(i*klvPrm); prrt->value+=(DOUBLE)Mkg; SumPromRez[i*klvPrm+7]++; if (SumPromRez[i*klvPrm+7]>1) prrt->status=prrt->status*stat; else prrt->status=(1);
				 prrt = (prr+6)+(i*klvPrm); prrt->value+=(DOUBLE)Qsk; SumPromRez[i*klvPrm+6]++; if (SumPromRez[i*klvPrm+6]>1) prrt->status*=stat; else prrt->status=(1);
				 prrt = (prr+5)+(i*klvPrm); prrt->value+=(Qsk*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+5]++; if (SumPromRez[i*klvPrm+5]>1) prrt->status*=stat; else prrt->status=(1);
				 prrt = (prr+4)+(i*klvPrm); prrt->value+=(Mkg*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+4]++; if (SumPromRez[i*klvPrm+4]>1) prrt->status*=stat; else prrt->status=(1);
				 prrt = (prr+3)+(i*klvPrm); prrt->value+=Tk*(Qsk*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+3]++; if (SumPromRez[i*klvPrm+3]>1) prrt->status*=stat; else prrt->status=(1);
				 //ULOGW ("stat= %d / %d",stat,prrt->status);
			     //----------------------------------
			     tmpDspl= (DplExch *)RDspl+inP+i*5;			// 3
			     memcpy(tmpDspl->name,indata[i].Name,6); // name > display
			     tmpDspl->var=(Qsk);  tmpDspl->status=1; 
			     tmpDspl= (DplExch *)RDspl+1+inP+i*5;			// 4
			     sprintf (tmpDspl->name,"Q%d,kg",i+1);//strcpy(tmpDspl->name,"Q,kgh");
				 tmpDspl->var=(Mkg);  tmpDspl->status=1; 
 			     tmpDspl= (DplExch *)RDspl+2+inP+i*5;
			     sprintf (tmpDspl->name,"V%d,m3",i+1); //strcpy(tmpDspl->name,"V,m3");
			     tmpDspl->var=(Qsk*indata[i].TaktData[0]/3600);  tmpDspl->status=1;
			     tmpDspl= (DplExch *)RDspl+3+inP+i*5;
			     sprintf (tmpDspl->name,"K%d,kJ",i+1); //strcpy(tmpDspl->name,"K,kJ");
			     tmpDspl->var=(Tk*(Qsk*indata[i].TaktData[0]/3600)); tmpDspl->status=1;
			     tmpDspl= (DplExch *)RDspl+4+inP+i*5;
			     sprintf (tmpDspl->name,"M%d,kg",i+1); //strcpy(tmpDspl->name,"M,kg");
			     tmpDspl->var=(Mkg*indata[i].TaktData[0]/3600);  tmpDspl->status=1;
				 //ULOGW ("tmpDspl(%d) %s=%f(%d)",i+1+inP,tmpDspl->name,tmpDspl->var,tmpDspl->status);
	 			 //--------
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(3<<24); MKB.status = 1; MKB.value = (Tk*(Qsk*indata[i].TaktData[0]/3600));
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"K%d,kc",i+1); // strcpy (MKB.tNmame,"K,kJ");
				 //for(INT n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
		  		 //p=(BYTE *) &MKB;	//--------
				 //MKB.trg = ((0x40000000|i)+1)|(4<<24); MKB.status = 1; MKB.value = (Mkg*indata[i].TaktData[0]/3600);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"M%d,kg",i+1); // strcpy (MKB.tNmame,"M,kg"); 
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
				 //MKB.trg = ((0x40000000|i)+1)|(5<<24); MKB.status = 1; MKB.value = (Qsk*indata[i].TaktData[0]/3600);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"V%d,m3",i+1); // strcpy (MKB.tNmame,"V,m3");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(6<<24); MKB.status = 1; MKB.value = (Qsk);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"Q%d,m3h",i+1); // strcpy (MKB.tNmame,"Q,m3h");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);	 MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(7<<24); MKB.status = 1; MKB.value = (Mkg);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"Q%d,kgh",i+1); // strcpy (MKB.tNmame,"Q,kgh");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);	 MPointer+=(LONG) sizeof(CycleBuffer);
				 //--------
				}
			 else
				{
				 prrt = (prr+7)+(i*klvPrm); prrt->value+=(DOUBLE)Mkg; SumPromRez[i*klvPrm+7]++;  prrt->status*=stat;
				 prrt = (prr+6)+(i*klvPrm); prrt->value+=(DOUBLE)Qsk; SumPromRez[i*klvPrm+6]++;  prrt->status*=stat;
				 prrt = (prr+5)+(i*klvPrm); prrt->value+=(Qsk*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+5]++; prrt->status*=stat;
				 prrt = (prr+4)+(i*klvPrm); prrt->value+=(Mkg*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+4]++; prrt->status*=stat;
				 prrt = (prr+3)+(i*klvPrm); prrt->value+=Tk*(Qsk*indata[i].TaktData[0]/3600); SumPromRez[i*klvPrm+3]++; prrt->status*=stat;
			     //----------------------------------
			     tmpDspl= (DplExch *)RDspl+inP+i*5;			// 3
			     memcpy(tmpDspl->name,indata[i].Name,6);// name > display
			     tmpDspl->var=(Qsk);  tmpDspl->status=1; 
			     tmpDspl= (DplExch *)RDspl+1+inP+i*5;		// 4
			     sprintf (tmpDspl->name,"Q%d,kg",i);	//strcpy(tmpDspl->name,"Q,kgh");
				 tmpDspl->var=(Mkg);  tmpDspl->status=1; 
 			     tmpDspl= (DplExch *)RDspl+2+inP+i*5;
			     sprintf (tmpDspl->name,"V%d,m3",i);	//strcpy(tmpDspl->name,"V,m3");
			     tmpDspl->var=(Qsk*indata[i].TaktData[0]/3600);  tmpDspl->status=1;
			     tmpDspl= (DplExch *)RDspl+3+inP+i*5;
			     sprintf (tmpDspl->name,"K%d,kJ",i);	//strcpy(tmpDspl->name,"K,kJ");
			     tmpDspl->var=(Tk*(Qsk*indata[i].TaktData[0]/3600)); tmpDspl->status=1;
			     tmpDspl= (DplExch *)RDspl+4+inP+i*5;
			     sprintf (tmpDspl->name,"M%d,kg",i);	//strcpy(tmpDspl->name,"M,kg");
			     tmpDspl->var=(Mkg*indata[i].TaktData[0]/3600);  tmpDspl->status=1;
				 //ULOGW ("tmpDspl(%d) %s=%f(%d)",i+1+inP,tmpDspl->name,tmpDspl->var,tmpDspl->status);
	 			 //--------
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(3<<24); MKB.status = 1; MKB.value = (Tk*(Qsk*indata[i].TaktData[0]/3600));
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"K%d,kc",i+1); // strcpy (MKB.tNmame,"K,kJ");
				 //for(INT n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
		  		 //p=(BYTE *) &MKB;	//--------
				 //MKB.trg = ((0x40000000|i)+1)|(4<<24); MKB.status = 1; MKB.value = (Mkg*indata[i].TaktData[0]/3600);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"M%d,kg",i+1); // strcpy (MKB.tNmame,"M,kg"); 
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
				 //MKB.trg = ((0x40000000|i)+1)|(5<<24); MKB.status = 1; MKB.value = (Qsk*indata[i].TaktData[0]/3600);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"V%d,m3",i+1); // strcpy (MKB.tNmame,"V,m3");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n); MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(6<<24); MKB.status = 1; MKB.value = (Qsk);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"Q%d,m3h",i+1); // strcpy (MKB.tNmame,"Q,m3h");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);	 MPointer+=(LONG) sizeof(CycleBuffer);
				 //p=(BYTE *) &MKB;	//--------
  				 //MKB.trg = ((0x40000000|i)+1)|(7<<24); MKB.status = 1; MKB.value = (Mkg);
				 //dTime(&MKB.timestm); sprintf (MKB.tNmame,"Q%d,kgh",i+1); // strcpy (MKB.tNmame,"Q,kgh");
				 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);	 MPointer+=(LONG) sizeof(CycleBuffer);
				 //--------
				}
			}
		}
	else indata[i].TaktData[1]-=1;		// counter decrement
	//------------------------------------------------------------------------------
	//ULOGW("[%d][%d/%d]",i,indata[i].TaktCalc[1],indata[i].TaktCalc[0]);
	if(indata[i].TaktCalc[1]>=indata[i].TaktCalc[0]-1)
		{
		 p=(BYTE *) &KB;
		 //------------
		 prr = (CycleBuffer *) PromRez;
		 for(INT b=0;b<klvPrm;b++)
		 	{
		 	 if (indata[i].Enabled==0&&b==1) b=klvPrm-2;	// 0(0) -> 6
			 //if (indata[i].Enabled==2&&b==1) b=3;
			 p=(BYTE *) &KB; KB.trg = (0x80000000|i+1)|(b<<24);
		 	 dTime(&KB.timestm); 
		 	 prr = (CycleBuffer *) PromRez;	 prrt = prr + (i*klvPrm) + (b);
			 KB.status=prrt->status; 
			 memcpy(&KB.tNmame,&prrt->tNmame,6);
			 if (b==3) sprintf (KB.tNmame,"K%d,kc",i+1); //strcpy (KB.tNmame,"K,kJ");
			 if (b==4) sprintf (KB.tNmame,"M%d,kg",i+1); //strcpy (KB.tNmame,"M,kg");
			 if (b==5) sprintf (KB.tNmame,"V%d,m3",i+1); //strcpy (KB.tNmame,"V,m3");
			 if (b==6) sprintf (KB.tNmame,"Q%d,m3h",i+1); //strcpy (KB.tNmame,"Q,m3h");
			 if (b==7) sprintf (KB.tNmame,"Q%d,kh",i+1); //strcpy (KB.tNmame,"Q,kgh");
			 //if (indata[i].es.type_su==13 && (b!=5) && (b!=4) && (b!=3)) KB.value=prrt->value/SumPromRez[i*klvPrm+b];	// Qk,T,R,Qnk,K,Qkm
			 if ((b!=5) && (b!=4) && (b!=3)) KB.value=prrt->value/SumPromRez[i*klvPrm+b];
			 //if (indata[i].es.type_su==13 && (b==5 || b==4 || b==3)) KB.value=prrt->value;								// V,M
			 if ((b==5 || b==4 || b==3)) KB.value=prrt->value;
			 //ULOGW ("!%d/%d! %x (%d) (Q=%f)",b,klvPrm,KB.trg,KB.status,KB.value);
			 for(INT n=0; n< sizeof(CycleBuffer); n++) DataM[n+Pointer]= *(p+n);
		 	 Pointer+=(LONG) sizeof(CycleBuffer);
 			 //--------
	  		 //p=(BYTE *) &MKB;
			 //MKB.trg = KB.trg; MKB.status = 1; MKB.value = KB.value; //!!!
			 //dTime(&MKB.timestm); strcpy (MKB.tNmame,KB.tNmame);
			 //for(n=0; n<sizeof(CycleBuffer);n++) Data[n+MPointer]= *(p+n);
			 //MPointer+=(LONG) sizeof(CycleBuffer);
			 //--------
		 	 //if(MPointer>= (LONG) (kStrKB-50)*sizeof(CycleBuffer)) MPointer=0;
		 	}
		 if(Pointer>= (LONG) (kStrKB)*sizeof(CycleBuffer)) Pointer=0;	
		 indata[i].TaktCalc[1]=0;
		 indata[i].target=0;
		 calc_oth_data(i);
		 for(b=0;b<klvPrm;b++)
		 	{
		 	 if (indata[i].Enabled==0&&b==1) b=klvPrm-1;
		 	 //if (indata[i].Enabled==1&&b>=3&&b!=klvPrm-1)  b=klvPrm-2;
		 	 prrt = (CycleBuffer *) PromRez + (i*klvPrm) + (b);
			 prrt->value=0; prrt->status=TRUE;
			 SumPromRez[i*klvPrm+b]=0;
		 	}		 
		 prr = (CycleBuffer *) PromRez +(i); memset(prr,0,sizeof(CycleBuffer)*klvPrm);
		}
	else indata[i].TaktCalc[1]+=1;
	}
//--------------------------------------------------------------------------