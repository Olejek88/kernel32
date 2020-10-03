// u_conv.h
// Преобразование единиц измерения

double UConv(double AValue, int SrcUnit, int DstUnit, int &err);

char* GetUnitName(int UnitCode, char* Buf);