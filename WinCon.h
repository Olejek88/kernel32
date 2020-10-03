#ifndef __WINCON_H__

#define __WINCON_H__

#include <WinconSDK.h>


WINCONSDK_API void DO_8(int slot, unsigned char cdata);

WINCONSDK_API void DO_16(int slot, unsigned int cdata);

WINCONSDK_API void DO_32(int slot, unsigned long cdata);

WINCONSDK_API unsigned char DI_8(int slot);

WINCONSDK_API unsigned int DI_16(int slot);

WINCONSDK_API unsigned long DI_32(int slot);

WINCONSDK_API void DIO_DO_8(int slot, unsigned char data);

WINCONSDK_API void DIO_DO_16(int slot, unsigned int data);


WINCONSDK_API int GetSystemSerialNumber(char *string1);

WINCONSDK_API int GetNameOfModule(int slot, char *string1);

WINCONSDK_API int GetModuleType(int slot);

WINCONSDK_API DWORD GetSlotAddr(int slot);


WINCONSDK_API DWORD GetTimeTicks(void);

WINCONSDK_API DWORD GetDIOData32(int slot);

WINCONSDK_API void WriteDIOData32(int slot, DWORD dw);


WINCONSDK_API unsigned char IoReadByte(DWORD addr);

WINCONSDK_API void IoWriteByte(DWORD addr, unsigned char cdata);

WINCONSDK_API void ShowLED16(int slot, unsigned int cdata);

WINCONSDK_API void ShowLED32(int slot, unsigned long cdata);

WINCONSDK_API void AutoScan(int flag, unsigned long delay, int caller);


WINCONSDK_API int ProgramMAC(unsigned char *MAC);
WINCONSDK_API int ReadMAC(unsigned char *MAC);
/*
WINCONSDK_API int ProgramMAC_DM9000SADU(int port,unsigned char *MAC);
WINCONSDK_API int ReadMAC_DM9000SADU(int port,unsigned int *MAC);
*/

WINCONSDK_API unsigned char ReadEEP(int block, int offset);
WINCONSDK_API void WriteEEP(int block, int offset, unsigned char ucData);


WINCONSDK_API DWORD FlashWriteString(unsigned int segment, DWORD offset, LPBYTE data, DWORD len);
WINCONSDK_API void FlashErase(unsigned int segment);
WINCONSDK_API DWORD GetFlashSize(DWORD* pDwSegmentSize);

WINCONSDK_API void GetOSversion(LPTSTR lpOSversion);
WINCONSDK_API void GetSDKversion(LPTSTR lpSDKversion);

WINCONSDK_API void ChangeSlotTo87K(unsigned char slotNo);

// 20031009 Vincent added for read back output
WINCONSDK_API unsigned char DO_8_RB(int slot);
WINCONSDK_API unsigned int DO_16_RB(int slot);
WINCONSDK_API unsigned long DO_32_RB(int slot);
WINCONSDK_API unsigned char DIO_DO_8_RB(int slot);
WINCONSDK_API unsigned int DIO_DO_16_RB(int slot);
// 20031009 Vincent added for bitwise R/W
WINCONSDK_API void DO_8_BW(int slot, int bit, BOOL data);
WINCONSDK_API void DO_16_BW(int slot, int bit, BOOL data);
WINCONSDK_API void DO_32_BW(int slot, int bit, BOOL data);
WINCONSDK_API void DIO_DO_8_BW(int slot, int bit, BOOL data);
WINCONSDK_API void DIO_DO_16_BW(int slot, int bit, BOOL data);
WINCONSDK_API BOOL DI_8_BW(int slot, int bit);
WINCONSDK_API BOOL DI_16_BW(int slot, int bit);
WINCONSDK_API BOOL DI_32_BW(int slot, int bit);

// unsigned int V_DI_16(int slot);

// void V_DO_16(int slot, unsigned int cdata);

// 20040624 Stephen add for WatchDog function

WINCONSDK_API void EnableWDT(DWORD msecond);
WINCONSDK_API void DisableWDT(void);
WINCONSDK_API void WatchDogReset(void);

WINCONSDK_API void EnableOSWDT(DWORD msecond);
WINCONSDK_API void DisableOSWDT(void);
WINCONSDK_API void ClearWDTSWEven(void);
WINCONSDK_API void WatchDogTimeOutType(int type);
WINCONSDK_API int WatchDogSWEven(void);
//======== 2005/8/10 stanley add ============================
WINCONSDK_API void Reset_WinCon();
WINCONSDK_API int ProgramMAC_DM9000SADU(int port,unsigned char *MAC);
WINCONSDK_API int ReadMAC_DM9000SADU(int port,unsigned int *MAC);
//////////Stanley Add  2005/08/18//////////////////
WINCONSDK_API unsigned short UDIO_WriteConfig_16(int slot,unsigned short config);

WINCONSDK_API unsigned short UDIO_ReadConfig_16(int slot);

WINCONSDK_API void UDIO_DO16(int slot,unsigned short config);

WINCONSDK_API unsigned short UDIO_DI16(int slot);

//////////Stanley Add  2005/11/10//////////////////
WINCONSDK_API void FRNET_SA(BYTE Slot,BYTE Port,BYTE wSA,WORD Data);

WINCONSDK_API WORD FRNET_RA(BYTE Slot,BYTE Port,BYTE wRA);
//////////Benson Add  2005/12/13//////////////////
WINCONSDK_API BYTE GET_ROTARYSW();
///////////////////////////////////////////////////
//#include <WinCon1.h>

#endif // __WINCON_H__
