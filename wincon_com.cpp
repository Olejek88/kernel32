 if (strstr (pdest,"version"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 sprintf (buffer,"%d %d %s %s",ver_major,ver_minor,ver_time,ver_desc);
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
		 if (st.wDay<=31 && st.wMonth<=12 && st.wYear>=2000 && st.wMinute<=59 && st.wSecond<=59 && st.wSecond<=999)
		 if (st.wDay>=1 && st.wMonth>=1 && st.wMinute>=0 && st.wSecond>=0 && st.wSecond>=0)
			{
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
	 sprintf (buffer,"SerNum=%s\r\nSoftVer=%s\r\nMaxModule=%d\r\nA=%d\r\n",sernum,wsoftver,maxmodule,RDspl_length);
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
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 //SumShift = смещение накопительного итога, если задано 
	 //Disp = признак индицирования канала (YES или NO) 
	 //Coeff = применяемый коэффициент для B-, C-, D-каналов 
	 UINT nch=0; UINT ci=1;
	 CycleDues	*pp;			// temporary sensors info for dispetcher	 
	 INT inP=(TaskDues_length/sizeof(CycleDues)); // input parametrs
	 pp= (CycleDues *) TaskDues;	// link to recieved data for server
	 for(INT i=0;i<(INT)ChInData;i++)	// calculating values
	 for(INT j=0;j<(INT)inP;j++)		// sensors configurate
		{
		 sprintf (buffer,"[A%d]\r\n",ci);
		 sprintf (buffer,"%sName=%s\r\nUnits=%s\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=NO\r\nSumShift=0\r\n",buffer,pp[j].name,pp[j].name,ci,pp[j].target&0xf);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,pp[j].rz_n_gr,pp[j].rz_v_gr);
		 send(client_socket,buffer,strlen(buffer),0);
		 ci++;
		}
	 for(i=0;i<(INT)ChInData;i++)	// calculating values
		{
		 sprintf (buffer,"[M%d]\r\n",ci);
		 sprintf (buffer,"%sName=%s\r\nUnits=m3/h\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,indata[i].Name,ci,4);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,12300);
		 send(client_socket,buffer,strlen(buffer),0);
		 sprintf (buffer,"[M%d]\r\n",ci);
		 sprintf (buffer,"%sName=Q%d,kgh\r\nUnits=kg/h\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,4);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,0);
		 send(client_socket,buffer,strlen(buffer),0);
		 sprintf (buffer,"[M%d]\r\n",ci);
		 sprintf (buffer,"%sName=V%d,m3\r\nUnits=m3\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,5);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,0);
		 send(client_socket,buffer,strlen(buffer),0);
		 sprintf (buffer,"[M%d]\r\n",ci);
		 sprintf (buffer,"%sName=K%d,kJ\r\nUnits=kJ\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,6);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,0);
		 send(client_socket,buffer,strlen(buffer),0);
		 sprintf (buffer,"[M%d]\r\n",ci);
		 sprintf (buffer,"%sName=M%d,kg\r\nUnits=kg\r\nModule=%d\r\nNumInModule=%d\r\nFill=Fixed\r\nArc=YES\r\nSumShift=0\r\n",buffer,i+1,ci,7);
		 sprintf (buffer,"%sMinLimit=%f\r\nMaxLimit=%f\r\nKbdControl=NO\r\n",buffer,0,0);
		 send(client_socket,buffer,strlen(buffer),0);
		 ci++;
		}
	 printf ("%s\n",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"module_info"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 sprintf (buffer,"[MODULE 1]\r\n");
	 sprintf (buffer,"%sType=ICPDAS Wincon-8031\r\n",buffer);
	 sprintf (buffer,"%sPort=0\r\n",buffer);
	 sprintf (buffer,"%sAddr=192.1.95.88\r\n",buffer);
	 sprintf (buffer,"%sSerNum=??\r\n",buffer);
	 sprintf (buffer,"%sState=1\r\n",buffer);
	 sprintf (buffer,"%sLastSession=%02d-%02d-%04d %02d:%02d:%02d\r\n",buffer,time3.wDay,time3.wMonth,time3.wYear,time3.wHour,time3.wMinute,time3.wSecond);
	 sprintf (buffer,"%sArcPtr=%02d-%02d-%04d %02d:%02d:%02d\r\n",buffer,time3.wDay,time3.wMonth,time3.wYear,time3.wHour,time3.wMinute,time3.wSecond);
	 send(client_socket,buffer,strlen(buffer),0);
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
	 return;
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"current"))
	{
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 sprintf (buffer,"ShortChanName,Value,State,\r\n");
	 // ShortChanName - краткое имя канала 
	 send(client_socket,buffer,strlen(buffer),0);				 
	 printf ("%s\n",buffer);
	}
 //-----------------------------------------------------------------------------------
 if (strstr (pdest,"archive") || strstr (pdest,"total") || strstr (pdest,"sys_events") || strstr (pdest,"events") || strstr (pdest,"last_event"))
	{
	 // /crw?req=archive&type=A&n1=1&n2=4
	 // /crw?req=archive&g1=a1&g2=a2&g3=a3&g4=a4				 
	 // t1 - время начала периода (по умолчанию - начало последнего имеющегося в архиве интервала) 
	 // t2 - время окончания периода (по умолчанию - конец последнего имеющегося в архиве интервала) 
	 // interval - тип архивных интервалов (по умолчанию - main) 
	 // short - короткие main - основные day - суточные month - помесячные year - годовые
	 CHAR type=0; CHAR *ddest; CHAR *dest;
	 SHORT n1=0,n2=0; SHORT g[20]={0};
	 CHAR t1[20],t2[20];
	 if (strstr (pdest+6,"&type=") || strstr (pdest+6,"&n1=") || strstr (pdest+6,"&n2=")) 
		{
		 printf ("archive values: diapason\n"); ULOGW ("archive values: diapason");
		 ddest = strstr (pdest+7,"&type="); 
		 if (ddest) type=ddest[6];
		 ddest = strstr (pdest+7,"&n1=");
		 if (ddest) n1=ddest[4];
		 ddest = strstr (pdest+7,"&n2="); 
		 if (ddest) n2=ddest[4];
		}
	 if (strstr (pdest+6,"&g"))
		{
		 printf ("archive values: sequence\n"); ULOGW ("archive values: sequence");
		 for (UINT pp=1; pp<20; pp++)
			{
			 sprintf (buffer,"&g%d",pp);
			 ddest = strstr (pdest+2,buffer);
			 //ULOGW ("%s\n",ddest);
			 if (ddest)		//&g1=a1
				{							 
				 if (*(ddest+5)>='0' && *(ddest+5)<='9')
					{
					 buffer[0]=*(ddest+5);
					 buffer[1]=0;
					 if (*(ddest+6)>='0' && *(ddest+6)<='9') 
						buffer[1]=*(ddest+6);
					 buffer[2]=0;
					}
				 g[pp]=(SHORT)atoi(buffer);
				}
			}
		}
	 if ((dest=strstr (pdest+6,"&t1="))!=NULL)
		{
		 for (UINT pp=0; pp<20; pp++) 
			 if (dest[4+pp]!='&' && dest[4+pp])
				 t1[pp]=dest[4+pp];							 						
			 else break;
		 printf ("archive values: t1=%s\n",t1); ULOGW ("archive values: t1=%s",t1);
		}
	 if ((dest=strstr (pdest+6,"&t2="))!=NULL)
		{
		 for (UINT pp=0; pp<20; pp++) 
			 if (dest[4+pp]!='&' && dest[4+pp])
				 t2[pp]=dest[4+pp];
			 else break;
		 printf ("archive values: t2=%s\n",t2); ULOGW ("archive values: t2=%s",t2);
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

	 printf ("%02d-%02d-%04d %02d:%02d:%02d\n",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond);
	 printf ("%02d-%02d-%04d %02d:%02d:%02d\n",fn.wDay,fn.wMonth,fn.wYear,fn.wHour,fn.wMinute,fn.wSecond);
	 if ((st.wDay<=31 && st.wMonth<=12 && st.wYear>=2000 && st.wMinute<=59 && st.wSecond<=59 && st.wSecond<=999)&&(st.wDay>=1 && st.wMonth>=1 && st.wMinute>=0 && st.wSecond>=0 && st.wSecond>=0))
		 ULOGW ("t1 in diapason");
	 else 
		{
		 OutputHTTPError(client_socket, 400);
		 printf ("incorrect t1 recieve\n"); ULOGW ("incorrect t1 recieve");
		 closesocket(client_socket);
		 return;
		}

	 if ((fn.wDay<=31 && fn.wMonth<=12 && fn.wYear>=2000 && fn.wMinute<=59 && fn.wSecond<=59 && fn.wSecond<=999)&&(fn.wDay>=1 && fn.wMonth>=1 && fn.wMinute>=0 && fn.wSecond>=0 && fn.wSecond>=0))
		ULOGW ("t2 in diapason");
	 else 
		{
		 OutputHTTPError(client_socket, 400);
		 printf ("incorrect t2 recieve\n"); ULOGW ("incorrect t2 recieve");
		 closesocket(client_socket);
		 return;
		}
	 SystemTimeToFileTime (&st,&fst);
	 SystemTimeToFileTime (&fn,&ffn);
	 interval=1;	// ????????
	 if ((dest=strstr (pdest+6,"&interval="))!=NULL)
		{
		 buffer[0]=0;
		 for (UINT pp=0; pp<20; pp++)
			 if (*(dest+10+pp)!='&' && *(dest+10+pp))
				 buffer[pp]=*(dest+10+pp);
			 else break;
		 buffer[pp]=0;
		 if (strstr (buffer,"short")) interval=5;
		 if (strstr (buffer,"main")) interval=1;
		 if (strstr (buffer,"day")) interval=2;
		 if (strstr (buffer,"month")) interval=4;
		 if (strstr (buffer,"year")) interval=3;
		}
	 if (!interval)
		{
		 OutputHTTPError(client_socket, 400);
		 printf ("not defined interval\n"); ULOGW ("not defined interval\n");
		 closesocket(client_socket);
		 return;
		}
	 if (strstr (pdest,"sys_events"))
		{
		 sprintf (buffer,"Time,Value,Ipar,Fpar,\r\n");
		 noStaff((TYPE_EVENTS|MPrn.pipe),0,0);		 
		 CHAR MessageText[100];
		 SYSTEMTIME st;
		 for (UINT tim=0;tim<60;tim++) 
			{
			 if (MEO.event[tim]>0)
				{			 
				 if ((UINT)MEO.event[tim]!=(TYPE_EVENTS+MODULE_KERNEL+EVENTS_POWER_ON) && (UINT)MEO.event[tim]!=(TYPE_EVENTS+MODULE_KERNEL+EVENTS_POWER_OFF))
					{
					 FormError ((UINT)MEO.event[tim], MessageText);
					 FileTimeToSystemTime (&MEO.fevnt[tim],&st);
					 sprintf (buffer,"%s%02d-%02d-%04d %02d:%02d:%02d,%s,0,0.0\r\n",buffer,st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond,MessageText);
					}
				}
			}
		}
	 if (strstr (pdest,"=events") || strstr (pdest,"last_event"))
		{
		 sprintf (buffer,"ShortChanName,Time,Value,Ipar,Fpar,Comment,\r\n");
		 BOOL chev[10]={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
		 if (g[1])
			{
			 PollDeviceCOM (LPORT,g[1],12);
			 for (UINT pp=1; pp<20; pp++)
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
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==1 && strstr(alog.data[j],"т1-00-"))
					if (!chev[1] && pp==1)
						{
						 if (strstr (pdest,"last_event")) chev[1]=TRUE;
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==2 && strstr(alog.data[j],"т1-00-"))
					if (!chev[2] && pp==2)
						{
						 if (strstr (pdest,"last_event")) chev[2]=TRUE;
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==7 && strstr(alog.data[j],"т2-03-"))
					if (!chev[7] && pp==7)
						{
						 if (strstr (pdest,"last_event")) chev[1]=TRUE;
							 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					if (pp==8 && strstr(alog.data[j],"т2-04-"))
					if (!chev[8] && pp==8)
						{
						 if (strstr (pdest,"last_event")) chev[2]=TRUE;
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,g[pp],alog.time[j],alog.data[j],alog.type[j]);
						}
					}
				}
			}
		 // диапазон
		 else
			{
			 PollDeviceCOM (LPORT,1,12);
			 if (n1==0) n1=1;
			 if (n2==0) n2=4*(devNum-1);						 
			 for (SHORT pp=n1; pp<n2; pp++)
				{
				 interval=12;
				 if (alog.quant_param)
				 for (UINT j=0; j<alog.quant_param; j++)
					{
					 // т*-00-00:000 т*-00-07:111 Q
					 // т*-03-00:000 т*-03-07:111 P
					 // т*-04-00:000 т*-04-06:111 T
					 ULOGW ("alog.data[%d]=%s alog.type[%d]=%s pp=%d",j,alog.data[j],j,alog.data[j],pp); 
					 if ((pp==3 || pp==4) && (strstr(alog.data[j],"т1-00-") || strstr(alog.data[j],"т1-05-") || strstr(alog.data[j],"в1-00-") || strstr(alog.data[j],"в1-05-")))
						sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==1 && (strstr(alog.data[j],"т1-03-")||strstr(alog.data[j],"в1-03-")))
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==2 && (strstr(alog.data[j],"т1-04-")||strstr(alog.data[j],"в1-04-")))
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if ((pp==7 || pp==8) && (strstr(alog.data[j],"т2-00-")||strstr(alog.data[j],"в1-00-")))
						sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==5 && (strstr(alog.data[j],"т2-03-")||strstr(alog.data[j],"в2-03-")))
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					 if (pp==6 && (strstr(alog.data[j],"т2-04-")||strstr(alog.data[j],"в2-04-")))
						 sprintf (buffer,"%sA%d,%s,%s,0,0.0,%s\r\n",buffer,pp,alog.time[j],alog.data[j],alog.type[j]);
					}
				}
			}				 
		}
	 if (strstr (pdest,"archive") || strstr (pdest,"total"))
		{
		 sprintf (buffer,"ShortChanName,Time,Value,State,\r\n");					 
		 // перечисление
		 if (g[1])
			{
			 printf ("sequence %d %d %d %d\n",g[1],g[2],g[3],g[4]);
			 for (UINT pp=1; pp<20; pp++)
				if (g[pp])
					{
					 PollDeviceCOM (LPORT,g[pp],1);
					 if (alog.quant_param)
					 for (UINT j=0; j<alog.quant_param; j++)
						 sprintf (buffer,"%sA%d,%s,%s,%d,\r\n",buffer,g[pp],alog.time[j],alog.data[j],1);
					}
			}
		 // диапазон
		 else
			{
			 printf ("diapason %d %d\n",n1,n2);
			 if (n1==0) n1=1;
			 if (n2==0) n2=4*(devNum-1);
			 for (SHORT pp=n1; pp<=n2; pp++)
				{
				 PollDeviceCOM (LPORT,pp,1);
				 ULOGW ("PollDeviceCOM (%d)",alog.quant_param);
				 if (alog.quant_param)
				 for (UINT j=0; j<alog.quant_param; j++)
					 sprintf (buffer,"%sA%d,20%c%c%c%c%c%c%c%c0000,%s,1,\r\n",buffer,pp,alog.time[j][6],alog.time[j][7],alog.time[j][3],alog.time[j][4],alog.time[j][0],alog.time[j][1],alog.time[j][9],alog.time[j][10],alog.data[j]);
				}
			}				 
		}
	 rightreq=TRUE;
	 strncat(sendbuffer,"Content-Type: text/plain\r\n",COMM_BUFFER_SIZE);
 	 strncat(sendbuffer,"\r\n",COMM_BUFFER_SIZE);
	 send(client_socket,sendbuffer,strlen(sendbuffer),0);
	 send(client_socket,buffer,strlen(buffer),0);
	 printf ("%s\n",buffer);				 
	}