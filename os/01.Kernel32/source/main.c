#include "types.h"

/* print string */
void kPrintString( int iX, int iY, const char* pcString );
/* set 0 Kernel64 area 0x100000~0x600000 */
BOOL kInitializeKernel64Area(void);

void main(void)
{
	DWORD i;
	kPrintString(0, 3, "C Language Kernel Started");

	kInitializeKernel64Area();
	kPrintString(0, 4, "IA-32e Kernel Area Initialization Complete");

	while(1);
}

void kPrintString( int iX, int iY, const char* pcString )
{
	CHARACTER* pstScreen = (CHARACTER *) 0xB8000;
	int i;

	pstScreen += ( iY * 80 ) + iX;
	for( i = 0; pcString[i] != 0; i++ )
	{
		pstScreen[i].bCharacter = pcString[i];
	}
}

BOOL kInitializeKernel64Area(void)
{
	DWORD* pdwCurrentAddress;

	pdwCurrentAddress = (DWORD*) 0x100000;

	while( (DWORD) pdwCurrentAddress < 0x600000 )
	{
		*pdwCurrentAddress = 0x00;

		// recheck. If not 0, that address have a problem
		if(*pdwCurrentAddress != 0)
			return FALSE;
		
		pdwCurrentAddress ++;
	}
	return TRUE;
}