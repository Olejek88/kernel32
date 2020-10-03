#ifndef __UARTCE_H__

#define __UARTCE_H__
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UARTCE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UARTCE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef UARTCE_EXPORTS
   #define UARTCE_API extern "C" __declspec(dllexport)
#else
   #define UARTCE_API extern "C" __declspec(dllimport)
#endif

//By Troy---------disable  
// This class is exported from the UARTCE.dll
/*
class UARTCE_API CUARTCE {
public:
	CUARTCE(void);
	// TODO: add your methods here.
};
*/

//extern UARTCE_API int nUARTCE;
UARTCE_API int nUARTCE;

UARTCE_API int fnUARTCE(void);

#include <VXCOMM.H>
#include <UART.H>

#endif // __UARTCE_H__