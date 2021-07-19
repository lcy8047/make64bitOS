// declaration kReadCPUID()
// define DWORD unsigned int in types.h
void kReadCPUID( DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX );

DWORD dwEAX, dwEBX, dwECX, dwEDX;
char vcVendorString[ 13 ] = {0, };

// read vendor info
kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
*( DWORD* ) vcVendorString = dwEBX;
*(( DWORD* ) vcVendorString + 1) = dwECX;
*(( DWORD* ) vcVendorString + 2) = dwEDX;

// print vendor info
// kPrintString define 01.Kernel32/source/main.c
kPrintString(0, 0, vcVendorString);