 if (strstr (pdest,"version"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 sprintf (buffer,"5 65 %s EKOM",ver_time);
  	 //sprintf(sendbuffer+strlen(sendbuffer),"Content-Length: %ld\r\n",strlen(buffer));
	 sprintf(sendbuffer+strlen(sendbuffer),"%s\r\n",buffer);
	 //strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 printf ("%s\n",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"gettime"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 SYSTEMTIME st; GetLocalTime(&st);
	 sprintf (buffer,"%02d-%02d-%04d %02d:%02d:%02d.%03d",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
	 sprintf(sendbuffer+strlen(sendbuffer),"%s\r\n",buffer);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 printf ("%s\n",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"settime"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 if (strstr (pdest+6,"&time="))
		{
		 strncpy (buffer,pdest+17,20);
		 SYSTEMTIME st;
		 //settime&time=YYYYNNDDHHMMSS[.mmm][z]
		 st.wDay=(buffer[6]-48)*10+buffer[7]-48;
		 st.wMonth=(buffer[4]-48)*10+(buffer[5]-48);
		 st.wYear=(buffer[0]-48)*1000+(buffer[1]-48)*100+(buffer[2]-48)*10+(buffer[3]-48);
		 
		 st.wHour=(buffer[8]-48)*10+(buffer[9]-48);
		 st.wMinute=(buffer[10]-48)*10+(buffer[11]-48);
		 st.wSecond=(buffer[12]-48)*10+(buffer[13]-48);
		 if (buffer[14]=='.') st.wMilliseconds=(buffer[15]-48)*100+(buffer[16]-48)*10+(buffer[17]-48);
		 else st.wMilliseconds=0;				    				
		 printf ("%02d-%02d-%04d %02d:%02d:%02d.%03d\n",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		 //ULOGW ("settime %02d-%02d-%04d %02d:%02d:%02d.%03d",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
		 if (st.wDay<=31 && st.wMonth<=12 && st.wYear>=2000 && st.wMinute<=59 && st.wSecond<=59 && st.wSecond<=999)
		 if (st.wDay>=1 && st.wMonth>=1 && st.wMinute>=0 && st.wSecond>=0 && st.wSecond>=0)
			{
			 ULOGW ("settime %02d-%02d-%04d %02d:%02d:%02d.%03d",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);
			 SetLocalTime(&st);
			 send(client_socket,sendbuffer,strlen(sendbuffer),0);
			}
		 else OutputHTTPError(client_socket, 400);
		}
	 else OutputHTTPError(client_socket, 400);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"getfile"))
	{				 
	 if (strstr (pdest+6,"&name="))
		{
		 rightreq=TRUE;
		 FILE *in;
		 CHAR *filebuffer;
		 LONG filesize;
		 if (strstr(buffer,".cfg") || strstr(buffer,".txt")) 
			 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
		 else strncat(sendbuffer,"Content-Type: data/octet-stream\r\n",COMM_BUFFER_SIZE);
		 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
		 sprintf (buffer,HOMEDIRFIL,pdest+17);
		 printf ("client request file: %s\n",buffer);
		 in = fopen(buffer,"rb");  // read binary
		 if (!in)
			{
			 printf ("file not found\n");
			 OutputHTTPError(client_socket, 404);   // 404 - not found
			}
		 else 
			{
			 // determine file size
			 fseek(in,0,SEEK_END);
 			 filesize = ftell(in);
			 fseek(in,0,SEEK_SET);
			 // allocate buffer and read in file contents
			 filebuffer = new char[filesize];
			 fread(filebuffer,sizeof(char),filesize,in);
			 fclose(in);
			 sprintf(sendbuffer+strlen(sendbuffer),"\r\nContent-Length: %ld\r\n",filesize);
			 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
 			 send(client_socket,sendbuffer,strlen(sendbuffer),0);
			 send(client_socket,filebuffer,filesize,0);
			 delete [] filebuffer;
			}
		}
	 else OutputHTTPError(client_socket, 400);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"reboot"))
	{
	 if (strstr (pdest+6,"&init="))
		{
		 rightreq=TRUE;
		 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
		 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
		 strncpy (buffer,pdest+17,20);					 
		 if (!strcmp (buffer,"yes"))
			{
			 printf ("reboot with archive erace no available\n");
			 sprintf (buffer,"reboot with archive erace no available");						 
			}
		 else
			{
			 printf ("recieve reboot command\n");
			 sprintf (buffer,"recieve reboot command");
			}					 
		 sprintf(sendbuffer+strlen(sendbuffer),"%s\r\n",buffer);
		 send(client_socket,sendbuffer,strlen(sendbuffer),0);
		 printf ("reboot after 5 sec\n"); Sleep (1000);
		 printf ("reboot after 4 sec\n"); Sleep (1000);
		 printf ("reboot after 3 sec\n"); Sleep (1000);
		 printf ("reboot after 2 sec\n"); Sleep (1000); 
		 printf ("reboot after 1 sec\n"); Sleep (1000);
		 printf ("reboot\n");
		 CreateProcess(TEXT(reboot),NULL,NULL,NULL,NULL,CREATE_NEW_CONSOLE,NULL,NULL,NULL,NULL); 
		}
	 else OutputHTTPError(client_socket, 400);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"dev_info"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 sprintf (buffer,"SerNum=%s\r\nSoftVer=%s\r\nMaxModule=%d\r\nA=%d\r\n",sernum,softver,maxmodule,kA+wkA);
	 send(client_socket,buffer,strlen(buffer),0);
	 printf ("%s",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"exit") || strstr (pdest,"quit"))
	{
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 sprintf (buffer,"now server end work. bye.\r\n");
	 send(client_socket,buffer,strlen(buffer),0);
	 ExitSignal=TRUE; rightreq=TRUE;
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"chan_info"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
	 strncat(sendbuffer,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\r\n",COMM_BUFFER_SIZE);\
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 for (UINT i=1,ci=1;i<devNum;i++)
	 if (DeviceRU[i].type>10)				 
		{
		 interval=88;
		 PollDeviceCOM (LPORT,0,0);
		 UINT maxc = (UINT)DeviceRU[DRL[i].device].tag_num;
		 //SumShift = смещение накопительного итога, если задано 
		 //Disp = признак индицирования канала (YES или NO) 
		 //Coeff = применяемый коэффициент для B-, C-, D-каналов 
		 UINT nch=0; DOUBLE MinLimit[5]; DOUBLE MaxLimit[5];
		 for (UINT r=1; r<=maxc; r++)
		 if (!Archive761[r-1].type && Archive761[r-1].quant>1)
			{
			 if (Archive761[r-1].adr==130) MinLimit[0]=atof(DRL[r-1].value);
			 if (Archive761[r-1].adr==131) MaxLimit[0]=atof(DRL[r-1].value);
			 if (Archive761[r-1].adr==133) MinLimit[1]=atof(DRL[r-1].value);
			 if (Archive761[r-1].adr==134) MaxLimit[1]=atof(DRL[r-1].value);
			 if (Archive761[r-1].adr==135) MinLimit[2]=atof(DRL[r-1].value);
			 if (Archive761[r-1].adr==136) MaxLimit[2]=atof(DRL[r-1].value);
			}
		 printf ("%f %f|%f %f|%f %f\n",MinLimit[0],MaxLimit[0],MinLimit[1],MaxLimit[1],MinLimit[2],MaxLimit[2]);
		 for (r=1; r<=maxc; r++)
		 if (!Archive761[r-1].type && Archive761[r-1].quant==1)
			{						 
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=%s\r\nUnits=%s\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,Archive761[r-1].name,Archive761[r-1].meas,i,r);
			 if (Archive761[r-1].adr==154) sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,MinLimit[1],MaxLimit[1]);
			 if (Archive761[r-1].adr==156) sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,MinLimit[0],MaxLimit[0]);
			 if (Archive761[r-1].adr==159) sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,MinLimit[2],MaxLimit[2]);
			 send(client_socket,buffer,strlen(buffer),0);
			 ci++;
			}
		 printf ("%s\n",buffer);
		 //SumShift = смещение накопительного итога, если задано 
		 //Disp = признак индицирования канала (YES или NO) 
		 //Coeff = применяемый коэффициент для B-, C-, D-каналов 
		 CycleDues	*pp;			// temporary sensors info for dispetcher	 
		 CHAR *pos;
		 INT inP=(TaskDues_length/sizeof(CycleDues)); // input parametrs
		 pp= (CycleDues *) TaskDues;		// link to recieved data for server
		 for(INT i=0;i<(INT)ChInData;i++)	// calculating values
		 for(INT j=0;j<(INT)inP;j++)		// sensors configurate
			{
			 sprintf (buffer,"[V%d]\r\n",ci);
			 pos = strstr(pp[j].name,",");
			 sprintf (buffer,"%sName=%s\r\nUnits=%s\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=NO\r\nSumShift=0\r\n",buffer,pp[j].name,pos+1,ci,pp[j].target&0xf);
			 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,pp[j].rz_n_gr,pp[j].rz_v_gr);
			 send(client_socket,buffer,strlen(buffer),0);
			 ci++;
			}
		 for(i=0;i<(INT)ChInData;i++)	// calculating values
			{
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=%s\r\nUnits=m3/h\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,indata[i].Name,ci,4);
			 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,12300);
			 send(client_socket,buffer,strlen(buffer),0);
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=Q%d,kgh\r\nUnits=kg/h\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,4);
			 sprintf (buffer,"%sMinLimit=0\r\nMaxLimit=0\r\nKbdControl=NO\r\n",buffer);
			 send(client_socket,buffer,strlen(buffer),0);
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=V%d,m3\r\nUnits=m3\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,5);
			 sprintf (buffer,"%sMinLimit=0\r\nMaxLimit=0\r\nKbdControl=NO\r\n",buffer);
			 send(client_socket,buffer,strlen(buffer),0);
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=K%d,kJ\r\nUnits=kJ\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,6);
			 sprintf (buffer,"%sMinLimit=0\r\nMaxLimit=0\r\nKbdControl=NO\r\n",buffer);
			 send(client_socket,buffer,strlen(buffer),0);
			 sprintf (buffer,"[V%d]\r\n",ci);
			 sprintf (buffer,"%sName=M%d,kg\r\nUnits=kg\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,7);
			 sprintf (buffer,"%sMinLimit=0\r\nMaxLimit=0\r\nKbdControl=NO\r\n",buffer);
			 send(client_socket,buffer,strlen(buffer),0);
			 ci++;
			}
		}
//	 else OutputHTTPError(client_socket, 400);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"module_info"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 for (UINT i=0;i<devNum;i++)
	 if (DeviceRU[1].type>10) 
		{					 
		 sprintf (buffer,"[MODULE %d]\r\n",i+1);
		 sprintf (buffer,"%sType=%s\r\n",buffer,DeviceRU[i].name);
		 sprintf (buffer,"%sPort=Com%d\r\n",buffer,DeviceRU[i].com);
		 sprintf (buffer,"%sAddr=%d\r\n",buffer,DeviceRU[i].device);
		 sprintf (buffer,"%sSerNum=??\r\n",buffer);
		 sprintf (buffer,"%sState=%d\r\n",buffer,DeviceRU[i].status);
		 sprintf (buffer,"%sLastSession=%02d-%02d-%04d %02d:%02d:%02d\r\n",buffer,time1.wDay,time1.wMonth,time1.wYear,time1.wHour,time1.wMinute,time1.wSecond);
		 sprintf (buffer,"%sArcPtr=%02d-%02d-%04d %02d:%02d:%02d\r\n",buffer,time2.wDay,time2.wMonth,time2.wYear,time2.wHour,time2.wMinute,time2.wSecond);
		 send(client_socket,buffer,strlen(buffer),0);
		}
	 printf ("%s\n",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"change") || strstr (pdest,"rights_info") || strstr (pdest,"start_time") || strstr (pdest," modules"))
	{
	 rightreq=TRUE;
	 printf ("recieve not supported command\n");
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 sprintf (buffer,"command not supported\r\n");
	 send(client_socket,buffer,strlen(buffer),0);
	 //OutputHTTPError(client_socket, 400);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"connect"))
	{
	 rightreq=TRUE;
	 printf ("recieve connect command\n");
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);	
	 sprintf(buffer,"Ok\r\n");
	 send(client_socket,buffer,strlen(buffer),0);
	 closesocket(client_socket);				 
	 GetLocalTime(&st); GetLocalTime(&fn);
	 SystemTimeToFileTime (&st,&fst);
	 SystemTimeToFileTime (&fn,&ffn);				 
	 printf ("poll device for instant data\n"); PollDeviceCOM (LPORT,0,0);
	 ULOGW("poll device for instant data");
	 printf ("poll device for archive data\n"); PollDeviceCOM (LPORT,0,1);
	 ULOGW("poll device for archive data");
	 return;
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"current"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 // /crq?req=current&type=A&n1=1&n2=4
	 // /crq?req=current&g1=a1&g2=a2&g3=a3&g4=a4
	 CHAR type=0; CHAR *ddest;
	 UINT n1=0,n2=0; UINT g[20]={0};
	 if (strstr (pdest+7,"&type=") || strstr (pdest+6,"&n1=") || strstr (pdest+6,"&n2=")) 
		{					 
		 //printf ("current values: diapason\n"); ULOGW ("current values: diapason");
		 ddest = strstr (pdest+7,"&type=");
		 if (ddest) type=ddest[6];
		 ddest = strstr (pdest+7,"&n1=");
		 if (ddest) 
			{
			 if (*(ddest+4)>='0' && *(ddest+4)<='9')
				{
				 buffer[0]=*(ddest+4); buffer[1]=0;
				 if (*(ddest+5)>='0' && *(ddest+5)<='9') buffer[1]=*(ddest+5);
				 buffer[2]=0;
				}
			 else
			 if (*(ddest+5)>='0' && *(ddest+5)<='9')
				{
				 buffer[0]=*(ddest+5); buffer[1]=0;
				 if (*(ddest+6)>='0' && *(ddest+6)<='9') buffer[1]=*(ddest+6);
				 buffer[2]=0;
				}
			 n1=(SHORT)atoi(buffer);
			}
		 ddest = strstr (pdest+7,"&n2=");
		 if (ddest) 
			{
			 if (*(ddest+4)>='0' && *(ddest+4)<='9')
				{
				 buffer[0]=*(ddest+4); buffer[1]=0;
				 if (*(ddest+5)>='0' && *(ddest+5)<='9') buffer[1]=*(ddest+5);
				 buffer[2]=0;
				}
			 else
			 if (*(ddest+5)>='0' && *(ddest+5)<='9')
				{
				 buffer[0]=*(ddest+5); buffer[1]=0;
				 if (*(ddest+6)>='0' && *(ddest+6)<='9') buffer[1]=*(ddest+6);
				 buffer[2]=0;
				}
			 n2=(SHORT)atoi(buffer);
			}
		}
	 if (strstr (pdest+6,"&g"))
		{
		 //printf ("current values: sequence\n"); ULOGW ("current values: sequence");
		 for (UINT pp=1; pp<20; pp++)
			{
			 sprintf (buffer,"&g%d",pp);
			 ddest = strstr (pdest+2,buffer);
			 if (ddest && pp<10)		//&g1=a1
				{				 
				 if (*(ddest+4)>='0' && *(ddest+4)<='9')
					{
					 buffer[0]=*(ddest+4); buffer[1]=0;
					 if (*(ddest+5)>='0' && *(ddest+5)<='9') 
						buffer[1]=*(ddest+5);
					 buffer[2]=0;
					}
				 g[pp]=(SHORT)atoi(buffer);
				 printf ("%d ",g[pp]); 
				}
			 if (ddest && pp>9)		//&g1=a1
				{				 
				 if (*(ddest+5)>='0' && *(ddest+5)<='9')
					{
					 buffer[0]=*(ddest+5); buffer[1]=0;
					 if (*(ddest+6)>='0' && *(ddest+6)<='9') 
						buffer[1]=*(ddest+6);
					 buffer[2]=0;
					}
				 g[pp]=(SHORT)atoi(buffer);
				 printf ("%d ",g[pp]); 
				}
			}
		}
	 sprintf (buffer,"ShortChanName,Value,State\r\n");
	 if (g[1])	// перечисление
		{					
		 if (type!='j' || type==0)
		 for (UINT pp=1; pp<=(4*(devNum-1)); pp++)
			if (g[pp] && g[pp]!=3 && g[pp]!=7 && ((g[pp]<9 && max==8) || (g[pp]<5 && max==4))) 
				{
				 //PollDeviceCOM (LPORT,g[pp],0);
				 //logik[pp]=atof(alog.data[0]);
				 //sprintf (buffer,"%sV%d,%s,%d\r\n",buffer,g[pp],alog.data[0],0);
				}
         for (UINT pp=1; pp<=19; pp++)
		 if ((max==8 && g[pp]>16) || (max==4 && g[pp]>12))
			{
	 		 Dis.forml=0; Dis.rz_n_gr=-1; Dis.rz_v_gr=10;
	         if ((max==8 && g[pp]==17) || (max==4 && g[pp]==13)) Dis.target=0x201;
			 if ((max==8 && g[pp]==18) || (max==4 && g[pp]==14)) Dis.target=0x202;
			 if ((max==8 && g[pp]==19) || (max==4 && g[pp]==15)) Dis.target=0x203;
			 DisR = GetData(Dis);
			 printf ("A%d=%f\n",g[pp],DisR.value);
			 if (DisR.value>10) sprintf (buffer,"%sV%d,1,0\r\n",buffer,g[pp]);
			 else sprintf (buffer,"%sV%d,0,0\r\n",buffer,g[pp]);
			}
		}
	 else		// диапазон
		{
		 if (n1==0) n1=1;
		 if (n2==0) n2=4*(devNum-1);
		 if (type!='j' || type==0)
		 for (UINT pp=n1,ci=1; pp<=n2; pp++)	
			{
			 if (pp<=(4*(devNum-1))) 
				{
				 PollDeviceCOM (LPORT,pp,0);
 				 //logik[pp]=atof(alog.data[0]);
				 sprintf (buffer,"%sV%d,%s,%d\r\n",buffer,pp,alog.data[0],0);
				}
		  	 //sprintf (buffer,"");
			 //send(client_socket,buffer,strlen(buffer),0);
			 g[ci]=pp; ci++;
			}
		 for (UINT pp=n1,ci=1; pp<=n2; pp++)	
		 if ((max==8 && pp>16) || (max==4 && pp>12))
			{
	 		 Dis.forml=0; Dis.rz_n_gr=-1; Dis.rz_v_gr=100;
	         if ((max==8 && pp==17) || (max==4 && pp==13)) Dis.target=0x201;
			 if ((max==8 && pp==18) || (max==4 && pp==14)) Dis.target=0x202;
			 if ((max==8 && pp==19) || (max==4 && pp==15)) Dis.target=0x203;
			 DisR = GetData(Dis);
			 printf ("A%d=%f\n",pp,DisR.value);
			 if (DisR.value>10) sprintf (buffer,"%sV%d,1,0\r\n",buffer,pp);
			 else sprintf (buffer,"%sV%d,0,0\r\n",buffer,pp);
			}
		}
	UINT ci=1;
    send(client_socket,sendbuffer,strlen(sendbuffer),0);
	send(client_socket,buffer,strlen(buffer),0);
	printf ("%s\n",buffer);
	 if (WorkModem)
		{
		 //BOOL send= senddata (buffer);
		 //ULOGW ("send gprs current data");
		 //BOOL send=SendGPRSdata ("grp2\\index.html",buffer);
		 //if (!send) ULOGW ("[gprs] error send current data");
		}
	htpDspl= (DplExch *)RDspl;
	memset(&DsplH,0,sizeof(DplExchO));
	for(UINT iu=0;iu<RDspl_length;iu++)
		{
		 if(htpDspl->name[0]==0) strcpy(htpDspl->name,"empty");
		 memcpy(DsplH.name[0],htpDspl->name,7);
		 DsplH.var[0]=htpDspl->var;
		 DsplH.status[0]=htpDspl->status;
		 SHORT stat=0;
		 if (!DsplH.status[0]) stat=1;
		 //sprintf (buffer,"V%d,%f,%d\r\n",ci,DsplH.var[0],stat);
		 if (iu==0) sprintf (buffer,"V%d,%f,%d\r\n",5+4*(devNum-2),DsplH.var[0],stat); //7
		 if (iu==1) sprintf (buffer,"V%d,%f,%d\r\n",6+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==2) sprintf (buffer,"V%d,%f,%d\r\n",7+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==3) sprintf (buffer,"V%d,%f,%d\r\n",11+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==4) sprintf (buffer,"V%d,%f,%d\r\n",9+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==5) sprintf (buffer,"V%d,%f,%d\r\n",10+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==6) sprintf (buffer,"V%d,%f,%d\r\n",8+4*(devNum-2),DsplH.var[0],stat);
		 if (iu==7) sprintf (buffer,"V%d,%f,%d\r\n",12+4*(devNum-2),DsplH.var[0],stat);
		 printf ("%s\n",buffer);
		 if (type!='j' || type==0) send(client_socket,buffer,strlen(buffer),0);
		 htpDspl+=1; ci++;
		}	 
	 //ULOGW ("send buffer");
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"archive") || strstr (pdest,"total") || strstr (pdest,"sys_events") || strstr (pdest,"events") || strstr (pdest,"last_event"))
	{
	 // /crq?req=archive&type=A&n1=1&n2=4
	 // /crq?req=archive&g1=a1&g2=a2&g3=a3&g4=a4				 
	 // t1 - время начала периода (по умолчанию - начало последнего имеющегося в архиве интервала) 
	 // t2 - время окончания периода (по умолчанию - конец последнего имеющегося в архиве интервала) 
	 // interval - тип архивных интервалов (по умолчанию - main) 
	 // short - короткие main - основные day - суточные month - помесячные year - годовые
	 SYSTEMTIME rt,st1,fn1; FILETIME tm;
	 BOOL equal=FALSE; BOOL est=FALSE; BOOL gprs=FALSE;
	 CHAR type=0; CHAR *ddest; CHAR *dest; 
	 UINT n1=0,n2=0; UINT g[20]={0};
	 CHAR t1[20],t2[20]; ULARGE_INTEGER uon;
	 if (!WorkModem) RecHour=TRUE;
	 if (strstr (pdest+6,"&type=") || strstr (pdest+6,"&n1=") || strstr (pdest+6,"&n2=")) 
		{
		 //ULOGW ("archive values: diapason");
		 ddest = strstr (pdest+7,"&type=");
		 if (ddest) type=ddest[6];
		 ddest = strstr (pdest+7,"&n1=");
		 if (ddest) 
			{
			 if (*(ddest+4)>='0' && *(ddest+4)<='9')
				{
				 buffer[0]=*(ddest+4); buffer[1]=0;
				 if (*(ddest+5)>='0' && *(ddest+5)<='9') buffer[1]=*(ddest+5);
				 buffer[2]=0;
				}
			 else
			 if (*(ddest+5)>='0' && *(ddest+5)<='9')
				{
				 buffer[0]=*(ddest+5); buffer[1]=0;
				 if (*(ddest+6)>='0' && *(ddest+6)<='9') buffer[1]=*(ddest+6);
				 buffer[2]=0;
				}			 
			 n1=(SHORT)atoi(buffer);
			}
		 ddest = strstr (pdest+7,"&n2=");
		 if (ddest) 
			{
			 if (*(ddest+4)>='0' && *(ddest+4)<='9')
				{
				 buffer[0]=*(ddest+4); buffer[1]=0;
				 if (*(ddest+5)>='0' && *(ddest+5)<='9') buffer[1]=*(ddest+5);
				 buffer[2]=0;
				}
			 else
			 if (*(ddest+5)>='0' && *(ddest+5)<='9')
				{
				 buffer[0]=*(ddest+5); buffer[1]=0;
				 if (*(ddest+6)>='0' && *(ddest+6)<='9') buffer[1]=*(ddest+6);
				 buffer[2]=0;
				}			 
			 n2=(SHORT)atoi(buffer);
			}
		}
	 if (strstr (pdest+6,"&g"))
		{
		 //printf ("archive values: sequence "); ULOGW ("archive values: sequence");
		 for (UINT pp=1; pp<20; pp++)
			{
			 sprintf (buffer,"&g%d",pp); // &g1=v11
			 ddest = strstr (pdest+2,buffer);
			 //ULOGW ("%s\n",ddest);
			 if (ddest && pp<10)		//&g1=a1
				{
				 //ULOGW ("%c %c",*(ddest+4),*(ddest+5));
				 if (*(ddest+5)>='0' && *(ddest+5)<='9')
					{
					 buffer[0]=*(ddest+5); buffer[1]=0;
					 if (*(ddest+6)>='0' && *(ddest+6)<='9') 
						buffer[1]=*(ddest+6);
					 buffer[2]=0;
					}
				 g[pp]=(SHORT)atoi(buffer);
				 printf ("%d ",g[pp]); 
				}
			 if (ddest && pp>9)		//&g1=a11
				{
				 //ULOGW ("%c %c",*(ddest+5),*(ddest+6));
				 if (*(ddest+6)>='0' && *(ddest+6)<='9')
					{
					 buffer[0]=*(ddest+6); buffer[1]=0;
					 if (*(ddest+7)>='0' && *(ddest+7)<='9') 
						buffer[1]=*(ddest+7);
					 buffer[2]=0;
					}
				 g[pp]=(SHORT)atoi(buffer);
				 printf ("%d ",g[pp]); 
				}
			}
		 printf ("\n");
		}
	 if (!g[1])
		{
		 if (n1==0) n1=1;
		 if (n2==0) n2=4*(devNum-1)+8*(devNum-1);
		 for (UINT i=n1; i<=n2; i++)
			 g[i-n1+1]=i;
		}
	 if ((dest=strstr (pdest+6,"&t1="))!=NULL)
		{
		 for (UINT pp=0; pp<20; pp++) 
			 if (dest[4+pp]!='&' && dest[4+pp])
				 t1[pp]=dest[4+pp];							 						
			 else break;
		 printf ("archive values: t1=%s\n",t1); //ULOGW ("archive values: t1=%s",t1);
		}
	 if ((dest=strstr (pdest+6,"&t2="))!=NULL)
		{
		 for (UINT pp=0; pp<20; pp++) 
			 if (dest[4+pp]!='&' && dest[4+pp])
				 t2[pp]=dest[4+pp];
			 else break;
		 printf ("archive values: t2=%s\n",t2); //ULOGW ("archive values: t2=%s",t2);
		}				 
	 //YYYYNNDDHHMMSS[.mmm][z]
	 st.wDay=(t1[6]-48)*10+t1[7]-48;
	 st.wMonth=(t1[4]-48)*10+(t1[5]-48);
	 st.wYear=(t1[0]-48)*1000+(t1[1]-48)*100+(t1[2]-48)*10+(t1[3]-48);				 
	 st.wHour=(t1[8]-48)*10+(t1[9]-48);
	 st.wMinute=(t1[10]-48)*10+(t1[11]-48);
	 st.wSecond=(t1[12]-48)*10+(t1[13]-48);
	 
	 fn.wDay=(t2[6]-48)*10+t2[7]-48;
	 fn.wMonth=(t2[4]-48)*10+(t2[5]-48);
	 fn.wYear=(t2[0]-48)*1000+(t2[1]-48)*100+(t2[2]-48)*10+(t2[3]-48);
	 fn.wHour=(t2[8]-48)*10+(t2[9]-48);
	 fn.wMinute=(t2[10]-48)*10+(t2[11]-48);
	 fn.wSecond=(t2[12]-48)*10+(t2[13]-48);

	 //printf ("%02d-%02d-%04d %02d:%02d:%02d\n",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond);
	 //printf ("%02d-%02d-%04d %02d:%02d:%02d\n",fn.wDay,fn.wMonth,fn.wYear,fn.wHour,fn.wMinute,fn.wSecond);
	 if ((st.wDay<=31 && st.wMonth<=12 && st.wYear>=2000 && st.wMinute<=59 && st.wHour<=24 && st.wSecond<=59)&&(st.wDay>=1 && st.wMonth>=1 && st.wMinute>=0 && st.wHour>=0 && st.wSecond>=0)) ;
		 //printf ("t1 in diapason");
	 else 
		{
		 st.wDay=13; st.wMonth=10; st.wYear=2006; st.wMinute=59; st.wHour=10; st.wSecond=59;
		}

	 if ((fn.wDay<=31 && fn.wMonth<=12 && fn.wYear>=2000 && fn.wMinute<=59 && fn.wHour<=59 && fn.wSecond<=59)&&(fn.wDay>=1 && fn.wMonth>=1 && fn.wMinute>=0 && fn.wHour>=0 && fn.wSecond>=0)) ;
		//ULOGW ("t2 in diapason");
	 else 
		{
		 GetLocalTime(&fn);
		 //st.wDay=13; st.wMonth=10; st.wYear=2006; st.wMinute=59; st.wHour=10; st.wSecond=99;
		}
	 interval=1;	// ????????
	 if ((dest=strstr (pdest+6,"&interval="))!=NULL)
		{
		 buffer[0]=0;
		 for (UINT pp=0; pp<20; pp++)
			 if (*(dest+10+pp)!='&' && *(dest+10+pp))
				 buffer[pp]=*(dest+10+pp);
			 else break;
		 buffer[pp]=0;
		 if (strstr (buffer,"short")) 
			{ 
			 interval=5;
			 MPrn.quant=0;
			 //GetLocalTime(&fn);
			}
		 if (strstr (buffer,"main")) 
			{ 
			 interval=1; 
			 MPrn.quant=1;
			}
		 if (strstr (buffer,"day")) { interval=2; MPrn.quant=2; } 
		 if (strstr (buffer,"month")) { interval=4; MPrn.quant=4; } 
		 if (strstr (buffer,"year")) { interval=3; MPrn.quant=8; }
		 
		 ULARGE_INTEGER uon,uoff;
		 SystemTimeToFileTime (&st,&tm);
		 uon.QuadPart= tm.dwLowDateTime + tm.dwHighDateTime * 0x100000000L;
		 uon.QuadPart= uon.QuadPart+600000000L;
		 tm.dwLowDateTime=uon.LowPart; tm.dwHighDateTime=uon.HighPart;
		 FileTimeToSystemTime (&tm,&st1);

		 SystemTimeToFileTime (&fn,&tm);
		 uoff.QuadPart= tm.dwLowDateTime + tm.dwHighDateTime * 0x100000000L;
		 uoff.QuadPart= uoff.QuadPart+600000000L;
		 tm.dwLowDateTime=uoff.LowPart; tm.dwHighDateTime=uoff.HighPart;
		 FileTimeToSystemTime (&tm,&fn1);
	 	 GetLocalTime (&rt);
		 if (interval==5 && (uoff.QuadPart>(uon.QuadPart+36000000000L))) 
			{
			 printf ("big interval\n");
			 uoff.QuadPart=uon.QuadPart+34200000000L;
			 tm.dwLowDateTime=uoff.LowPart; tm.dwHighDateTime=uoff.HighPart;
			 FileTimeToSystemTime (&tm,&fn);
			}
		 if (interval==1 && (uoff.QuadPart>(uon.QuadPart+536000000000L))) 
			{
			 printf ("big interval\n");
			 uoff.QuadPart=uon.QuadPart+534200000000L;
			 tm.dwLowDateTime=uoff.LowPart; tm.dwHighDateTime=uoff.HighPart;
			 FileTimeToSystemTime (&tm,&fn);
			}
		 if (prevHTime.wHour!=rt.wHour && interval==1) 
			{
			 RecHour=FALSE;
			 prevHTime.wHour=rt.wHour;
			 ULOGW ("new hour");
			}
		 if (!RecHour && interval==1)
			{
			 interval=1;
			 MPrn.quant=1;
			 SystemTimeToFileTime (&st,&tm);
			 uon.QuadPart= tm.dwLowDateTime + tm.dwHighDateTime * 0x100000000L;
			 uon.QuadPart= uon.QuadPart-24*36000000000L;
			 tm.dwLowDateTime=uon.LowPart; tm.dwHighDateTime=uon.HighPart;
			 FileTimeToSystemTime (&tm,&st); 
			 GetSlaveVal=1;
			 gprsbuf[0]=0;
			 ULOGW ("new hour rec");
			}
		}
	 if (!SystemTimeToFileTime (&st,&fst)) ULOGW ("SystemTimeToFileTime (st) failure");
	 SystemTimeToFileTime (&fn,&ffn);
	 if (CompareFileTime(&fst,&ffn)==0) 
		{
		 equal=TRUE;
		 uon.QuadPart= fst.dwLowDateTime + fst.dwHighDateTime * 0x100000000L;
		 if (interval==5) uon.QuadPart=uon.QuadPart+34200000000L;  // !!!
		 if (interval==1) uon.QuadPart=uon.QuadPart+2*36000000000L;
		 if (interval==2) uon.QuadPart=uon.QuadPart+2*24*36000000000L;
		 if (interval==3) uon.QuadPart=uon.QuadPart+70*24*36000000000L;
		 ffn.dwLowDateTime=uon.LowPart;
		 ffn.dwHighDateTime=uon.HighPart;
		}
	 else
		{
		 uon.QuadPart= ffn.dwLowDateTime + ffn.dwHighDateTime * 0x100000000L;
		 if (interval==1) uon.QuadPart=uon.QuadPart+3*36000000000L;
		 ffn.dwLowDateTime=uon.LowPart;	 ffn.dwHighDateTime=uon.HighPart;
		}
	 //ULOGW ("archivData(%x %x)-(%x %x)",fst.dwLowDateTime,fst.dwHighDateTime,ffn.dwLowDateTime,ffn.dwHighDateTime); 

	 if (strstr (pdest,"sys_events"))
		{
		 sprintf (buffer,"Time,Value,Ipar,Fpar,\r\n");
		 //PollDeviceCOM (LPORT,12,12);
		 //for (UINT j=0; j<alog.quant_param; j++)	sprintf (buffer,"%s%s,%s,0,0.0\r\n",buffer,alog.time[j],alog.data[j]);
		}
	 if (strstr (pdest,"=events") || strstr (pdest,"last_event"))
		{
		 sprintf (buffer,"ShortChanName,Time,Value,Ipar,Fpar,Comment,\r\n");
		 /*BOOL chev[10]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
		 if (g[1])
			{
			 PollDeviceCOM (LPORT,g[1],12);
			 UINT maxc=devNum*4;
			 for (UINT pp=1; pp<=maxc; pp++) //!!!
			 if (g[pp])
				{								 
				 if (alog.quant_param)
				 for (UINT j=0; j<alog.quant_param; j++)
					{									 
					 if ((pp==3 || pp==4 || pp==7 || pp==8) && strstr(alog.data[j],"т1-00-"))
					 if ((!chev[3] && pp==3) || (!chev[4] && pp==4) || (!chev[7] && pp==7) || (!chev[8] && pp==8))
						{
						 if (strstr (pdest,"last_event")) 
							{
							 if (pp==3) chev[3]=TRUE;
							 if (pp==4) chev[4]=TRUE;
							 if (pp==7) chev[7]=TRUE;
							 if (pp==8) chev[8]=TRUE;
							}
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==1 && strstr(alog.data[j],"т1-00-"))
					if (!chev[1] && pp==1)
						{
						 if (strstr (pdest,"last_event")) chev[1]=TRUE;
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==2 && strstr(alog.data[j],"т1-00-"))
					if (!chev[2] && pp==2)
						{
						 if (strstr (pdest,"last_event")) chev[2]=TRUE;
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==7 && strstr(alog.data[j],"т2-03-"))
					if (!chev[7] && pp==7)
						{
						 if (strstr (pdest,"last_event")) chev[1]=TRUE;
							 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==8 && strstr(alog.data[j],"т2-04-"))
					if (!chev[8] && pp==8)
						{
						 if (strstr (pdest,"last_event")) chev[2]=TRUE;
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					}
				}
			}
		 // диапазон
		 else
			{
			 PollDeviceCOM (LPORT,1,12);
			 if (n1==0) n1=1;
			 if (n2==0) n2=4*(devNum-1)+8*(devNum-1);						 
			 for (UINT pp=n1; pp<=n2; pp++)
				{
				 interval=12;
				 if (alog.quant_param)
				 for (UINT j=0; j<alog.quant_param; j++)
					{
					 // т*-00-00:000 т*-00-07:111 Q
					 // т*-03-00:000 т*-03-07:111 P
					 // т*-04-00:000 т*-04-06:111 T
					 // ULOGW ("alog.data[%d]=%s alog.type[%d]=%s pp=%d",j,alog.data[j],j,alog.data[j],pp); 
					 if ((pp==3 || pp==4) && (strstr(alog.data[j],"т1-00-") || strstr(alog.data[j],"т1-05-") || strstr(alog.data[j],"в1-00-") || strstr(alog.data[j],"в1-05-")))
						sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==1 && (strstr(alog.data[j],"т1-03-")||strstr(alog.data[j],"в1-03-")))
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==2 && (strstr(alog.data[j],"т1-04-")||strstr(alog.data[j],"в1-04-")))
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if ((pp==7 || pp==8) && (strstr(alog.data[j],"т2-00-")||strstr(alog.data[j],"в1-00-")))
						sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==5 && (strstr(alog.data[j],"т2-03-")||strstr(alog.data[j],"в2-03-")))
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==6 && (strstr(alog.data[j],"т2-04-")||strstr(alog.data[j],"в2-04-")))
						 sprintf (buffer,"%sV%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					}
				}
			}*/				 
		}
	 if (strstr (pdest,"total") && type!='v') sprintf (buffer,"ShortChanName,Time,Value,State\r\n"); 
	 if (strstr (pdest,"total") && (type=='v' || type==0))
		{
		 sprintf (buffer,"ShortChanName,Time,Value,State\r\n");
		 /*for (INT pp=1; pp<=20; pp++)
		 if (g[pp]==4)
			{
			 PollDeviceCOM (LPORT,24,0);
			 ULOGW ("PollDeviceCOM (%d)",alog.quant_param);
			 GetLocalTime(&fn);
			 if (alog.quant_param)
			 for (INT j=0; j<alog.quant_param; j++)
				sprintf (buffer,"%sV4,%02d-%02d-%04d %02d:%02d:%02d,%s,0\r\n",buffer,fn.wDay,fn.wMonth,fn.wYear,fn.wHour,fn.wMinute,fn.wSecond,alog.data[j]);
			}*/
		} 
	 if (strstr (pdest,"archive") && type!='v') sprintf (buffer,"ShortChanName,Time,Value,State\r\n"); 
	 if (strstr (pdest,"archive") && (type=='v' || type==0))
		{
		 sprintf (buffer,"ShortChanName,Time,Value,State\r\n");
		 // перечисление
		 //max=4;
		 //ULOGW ("g[1]=%d | inter=%d (%d) ",g[1],interval,MPrn.quant);
		 if (g[1]<=max || g[2]<=max || g[3]<=max || g[4]<=max || g[5]<=max)
		 for (INT pp=1; pp<=20; pp++)
		 if (g[pp] && g[pp]<=max)
			{
			 est=FALSE;
			 alog.quant_param=0;
			 if (max==4) if (g[pp]==4 || g[pp]==1 || g[pp]==2) PollDeviceCOM (LPORT,g[pp],1);
						 else alog.quant_param=250;
			 if (max==8) if (g[pp]==8 || g[pp]==5 || g[pp]==6 || g[pp]==4) PollDeviceCOM (LPORT,g[pp],1);
						 else alog.quant_param=250;
			 if (alog.quant_param<100)
			 for (UINT j=0; j<alog.quant_param; j++)
				{
				 SYSTEMTIME ty;
				 ty.wSecond=0;
				 ty.wMinute=0;
				 ty.wHour=(alog.time[j][9]-48)*10+(alog.time[j][10]-48);
				 ty.wDay=(alog.time[j][0]-48)*10+(alog.time[j][1]-48);
				 ty.wMonth=(alog.time[j][3]-48)*10+(alog.time[j][4]-48);
				 ty.wYear=2000+(alog.time[j][6]-48)*10+(alog.time[j][7]-48);
				 tmy=ty.wHour+ty.wDay*100+ty.wMonth*100*100+ty.wYear*100*100*100;
				 //ULOGW ("[s:%d|[%d]|w:%d]",tmy_l,tmy,tmy_w);
				 if (tmy>tmy_l && tmy<tmy_w) 
					{
					 //ULOGW ("[%02d-%02d-%02d %02d:00:00][%s]",ty.wDay,ty.wMonth,ty.wYear,ty.wHour,alog.data[j]);
					 FILETIME	ftf; ULARGE_INTEGER un;
					 SystemTimeToFileTime (&ty,&ftf);
					 un.QuadPart= ftf.dwLowDateTime + ftf.dwHighDateTime * 0x100000000L;
					 un.QuadPart= un.QuadPart-600000000L;
					 ftf.dwLowDateTime=un.LowPart; ftf.dwHighDateTime=un.HighPart;
					 FileTimeToSystemTime (&ftf,&ty);
					 //ULOGW ("[%02d-%02d-%02d %02d:00:00]",ty.wDay,ty.wMonth,ty.wYear,ty.wHour);
					}

				 if (ty.wHour==st1.wHour && ty.wDay==st1.wDay && ty.wMonth==st1.wMonth) est=TRUE;		
				 if (RecHour && (strlen(buffer)<12000))
					{
					 //if (interval!=2) sprintf (buffer,"%sV%d,%c%c-%c%c-20%c%c %c%c:00:00,%s,0\r\n",buffer,g[pp],alog.time[j][0],alog.time[j][1],alog.time[j][3],alog.time[j][4],alog.time[j][6],alog.time[j][7],alog.time[j][9],alog.time[j][10],alog.data[j]);
					 //if (interval!=2) 
						 sprintf (buffer,"%sV%d,%02d-%02d-%02d %02d:00:00,%s,0\r\n",buffer,g[pp],ty.wDay,ty.wMonth,ty.wYear,ty.wHour,alog.data[j]);
					 //else sprintf (buffer,"%sV%d,%c%c-%c%c-20%c%c %c%c:00:00,%s,0\r\n",buffer,g[pp],alog.time[j][0],alog.time[j][1],alog.time[j][3],alog.time[j][4],alog.time[j][6],alog.time[j][7],alog.time[j][9],alog.time[j][10],alog.data[j]);
					}
				 if ((WorkModem && !RecHour) || (Wincon_mode==2 && !RecHour))
				 if (interval==1) //GRP2
					{
					 //GetSlaveVal=1;
					 gprs=TRUE;
					 if (strlen (gprsbuf)<15000) sprintf (gprsbuf,"%sV%d,%c%c-%c%c-20%c%c %c%c:00:00,%s\r\n",gprsbuf,g[pp],alog.time[j][0],alog.time[j][1],alog.time[j][3],alog.time[j][4],alog.time[j][6],alog.time[j][7],alog.time[j][9],alog.time[j][10],alog.data[j]);
					}
				}			 
			 if (alog.quant_param==250)
				{
				 if (equal)
					{
					 if (interval==1) sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],fn1.wDay,fn1.wMonth,fn1.wYear,fn1.wHour);
					 if (interval==2) sprintf (buffer,"%sV%d,%02d-%02d-%04d 00:00:00,0,1\r\n",buffer,g[pp],fn1.wDay,fn1.wMonth,fn1.wYear);
					 if (interval==3) sprintf (buffer,"%sV%d,01-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],fn1.wMonth,fn1.wYear);
					 if (interval==4) sprintf (buffer,"%sV%d,01-01-%04d 00:00:00,0,1\r\n",buffer,g[pp],fn1.wYear);
					}
				 else
					{
					 if (interval==1) sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear,st1.wHour);
					 if (interval==2) sprintf (buffer,"%sV%d,%02d-%02d-%04d 00:00:00,0,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear);
					 if (interval==3) sprintf (buffer,"%sV%d,01-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],st1.wMonth,st1.wYear);
					 if (interval==4) sprintf (buffer,"%sV%d,01-01-%04d 00:00:00,0,1\r\n",buffer,g[pp],st1.wYear);
					}
				}
			}
		 if (g[1]>max || g[2]>max || g[3]>max || g[4]>max || g[5]>max || g[9]>max)
		 if (!workwithbuf)
		 for(UINT i=0;i<(INT)ChInData;i++)
			{				 
			 MPrn.type=ARCHIVE_DATA;
			 MPrn.start=fst;
	 		 if (interval==1 && equal==1) //!!!! 
				{
				 uon.QuadPart=uon.QuadPart-4*36000000000L;
				 MPrn.start.dwLowDateTime=uon.LowPart;
				 MPrn.start.dwHighDateTime=uon.HighPart;
				 equal=0;
				}
			 MPrn.end=ffn;
			 MPrn.pipe=i+1;
			 //if (interval!=5) 
			 SYSTEMTIME ty; GetLocalTime (&ty);
			 archivData(TRUE);
			 for (INT pp=1; pp<=18; pp++)
			 if (g[pp]>max && g[pp]<18)
				{
				 est=FALSE;				 				 
				 UINT nParam=0;
				 if ((g[pp]-max)==1) nParam=2; 
				 if ((g[pp]-max)==2) nParam=1;
				 if ((g[pp]-max)==3) nParam=0;
				 if ((g[pp]-max)==4) nParam=3;
				 if ((g[pp]-max)==5) nParam=4;
				 if ((g[pp]-max)==6) nParam=5;
				 if ((g[pp]-max)==7) nParam=6;
				 if ((g[pp]-max)==8) nParam=7;

				 if (ty.wHour==st1.wHour && ty.wDay==st1.wDay && ty.wMonth==st1.wMonth && ty.wMinute==st1.wMinute) 
					{
					 est=TRUE; PrnVl.time[0].dwHighDateTime=0x1;
					 //ULOGW ("[data not ready yet] %02d-%02d-%02d %02d:%02d (%02d-%02d-%02d %02d:%02d)",st1.wDay,st1.wMonth,st1.wYear,st1.wHour,st1.wMinute,ty.wDay,ty.wMonth,ty.wYear,ty.wHour,ty.wMinute);
					}
				 else
				 for (INT tim=59;tim>=0;tim--)
					{
					 //ULOGW ("(%d) [%x %x]",tim,PrnVl.time[tim].dwLowDateTime,PrnVl.time[tim].dwHighDateTime);
					 if (PrnVl.time[tim].dwHighDateTime!=0x0)
						{
						 FileTimeToSystemTime (&PrnVl.time[tim],&rt);
						 //ULOGW ("tim=%d inter=%d, g[pp]=%d, g[pp]-max=%d, nParam=%d [%s]",tim,interval,g[pp],g[pp]-max,nParam,PrnVl.name[nParam]);
						 if (PrnVl.name[nParam][0]!=0)
							{
							 //if (tim>185 || tim<7) ULOGW ("V%d,%02d-%02d-%02d %02d:%d:00,%11.4f,0",g[pp],rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute,PrnVl.value[nParam][tim]);
							 if (RecHour)
								{
								 //??!
								 if (interval==5 && PrnVl.value[nParam][tim]!=0) PrValues[nParam]=PrnVl.value[nParam][tim];
								 if (interval==1) rt.wMinute=0;
								 if (!PrnVl.status[nParam][tim]) // !!!
									//sprintf (buffer,"%sV%d,%02d-%02d-%02d %02d:%02d:00,%11.4f,0\r\n",buffer,g[pp],rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute,PrnVl.value[nParam][tim]);
									sprintf (buffer,"%sV%d,%02d-%02d-%02d %02d:%02d:00,%11.4f,0\r\n",buffer,g[pp],rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute,PrnVl.value[nParam][tim]);
								 else 
									sprintf (buffer,"%sV%d,%02d-%02d-%02d %02d:%02d:00,%11.4f,0\r\n",buffer,g[pp],rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute,PrnVl.value[nParam][tim]);
								}
			 				 if (WorkModem) if (interval==1 && !RecHour) //GRP2
								{
								 gprs=TRUE;
								 //sprintf (gprsbuf,"%sV%d,%02d-%02d-%02d %02d:%02d:00,%11.4f,0\r\n",gprsbuf,g[pp],rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute,PrnVl.value[nParam][tim]);
								}
							}
						 FileTimeToSystemTime (&PrnVl.time[tim],&rt);
						 //ULOGW ("%02d-%02d-%02d %02d:%02d (%02d-%02d-%02d %02d:%02d)",st1.wDay,st1.wMonth,st1.wYear,st1.wHour,st1.wMinute,rt.wDay,rt.wMonth,rt.wYear,rt.wHour,rt.wMinute);
						 //SystemTimeToFileTime (&st1,&tm);
						 if (rt.wHour==st1.wHour && rt.wDay==st1.wDay && rt.wMonth==st1.wMonth && rt.wMinute==st1.wMinute) { countint[interval][nParam]=0; est=TRUE;}
						 //if (CompareFileTime(&tm,&PrnVl.time[tim])==0) est=TRUE;
						 if (equal && interval!=5) tim=0; //!!!!
						}
					}				 
				 //ULOGW ("=---%d %d",PrnVl.time[0].dwHighDateTime,est);
				 if ((PrnVl.time[0].dwHighDateTime==0x0) || (!est)) 
				 if (interval==5 || countint[interval][nParam]>10)
					{
					 if (interval==1 && nParam==2) PrValues[nParam]=0;
					 if (interval==1 && nParam==1) PrValues[nParam]=0;
					 if (interval==1 && nParam==0) PrValues[nParam]=0;
 					 if (interval==1 && nParam==6) PrValues[nParam]=0;
					 if (equal)
						{
						 if (interval==1) sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:00:00,%f,1\r\n",buffer,g[pp],fn1.wDay,fn1.wMonth,fn1.wYear,fn1.wHour,PrValues[nParam]);
						 if (interval==2) sprintf (buffer,"%sV%d,%02d-%02d-%04d 00:00:00,0,1\r\n",buffer,g[pp],fn1.wDay,fn1.wMonth,fn1.wYear);
						 if (interval==3) sprintf (buffer,"%sV%d,01-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],fn1.wMonth,fn1.wYear);
						 if (interval==4) sprintf (buffer,"%sV%d,01-01-%04d 00:00:00,0,1\r\n",buffer,g[pp],fn1.wYear);
						}
					 else
						{
						 if (interval==1) sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:00:00,%f,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear,st1.wHour,PrValues[nParam]);
						 if (interval==2) sprintf (buffer,"%sV%d,%02d-%02d-%04d 00:00:00,0,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear);
						 if (interval==3) sprintf (buffer,"%sV%d,01-%02d-%04d %02d:00:00,0,1\r\n",buffer,g[pp],st1.wMonth,st1.wYear);
						 if (interval==4) sprintf (buffer,"%sV%d,01-01-%04d 00:00:00,0,1\r\n",buffer,g[pp],st1.wYear);
						}
					 if (interval==5 && ty.wSecond>9)
						{
						 //ULOGW ("PrValues[nParam]=%f",PrValues[nParam]);
						 sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:%02d:00,%f,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear,st1.wHour,st1.wMinute,PrValues[nParam]);
						 //if (st1.wMinute<59) sprintf (buffer,"%sV%d,%02d-%02d-%04d %02d:%02d:00,%f,1\r\n",buffer,g[pp],st1.wDay,st1.wMonth,st1.wYear,st1.wHour,st1.wMinute+1,PrValues[nParam]);
						}
					}
				 else { countint[interval][nParam]++; printf ("countint[%d][%d]=%d\n",interval,nParam,countint[interval][nParam]); }
				}
			}
		}
	 reqend=FALSE;
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 //sprintf(sendbuffer,"Content-Type: text/plain\r\n\r\n");
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 send(client_socket,buffer,strlen(buffer),0);
	 //ULOGW ("send buffer (%d)(%d)",strlen (sendbuffer),strlen (buffer));
	 //ULOGW ("send buffer (%d)",strlen (buffer));
	 printf ("%s\n",buffer); buffer[499]=0;
	 //ULOGW ("%s",buffer);
	 /*if (WorkModem && gprs)
		{
		 //sendsms ("+79227000293",gprsbuf);
		 //BOOL send=AppendGPRSdata (interval,gprsbuf);
		 RecHour=TRUE;
		 ULOGW ("gprsbuf= %d",strlen(gprsbuf));
		 BOOL send= sendhdata (gprsbuf);
		 if (!send) 
			{
			 ULOGW ("[gprs] error send data");
		//	 //BOOL res = OpenGPRSConnection ("", "tpchel-eco.ru", "ptfiber@tpchel-co.ru", "ptfiber");
			 //if (!res) ULOGW ("OpenGPRSConnection () failure");
			}
		}*/
	}         