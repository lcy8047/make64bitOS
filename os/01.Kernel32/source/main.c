#include "types.h"

/* print string */
void kPrintString( int iX, int iY, const char* pcString );
/* set 0 Kernel64 area 0x100000~0x600000 */
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

void main( void )
{
	DWORD i;
	kPrintString(0, 3, "C Language Kernel Start.....................[Pass]");

	kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");
	if( kIsMemoryEnough() == FALSE)
	{
		kPrintString(45,4,"Fail");
		kPrintString(0,5,"Not Enough Memory. Require over 64MB");
		while(1);
	}
	else
	{
		kPrintString(45,4,"Pass");
	}

	kPrintString(0, 5, "IA-32e Kernel Area Initialization...........[    ]");
	if( kInitializeKernel64Area() == FALSE )
	{
		kPrintString(45, 5, "Fail");
		kPrintString(0, 6, "Kernel Area Initialize Fail");
		while(1);
	}
	kPrintString(0, 5, "Pass");

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

BOOL kIsMemoryEnough(void)
{
	DWORD *pdwCurrentAddress;
	// begin 0x100000(1MB)
	pdwCurrentAddress = ( DWORD* ) 0x100000;
	// to 0x4000000(64MB)
	while ((DWORD)pdwCurrentAddress < 0x4000000)
	{
		*pdwCurrentAddress = 0x12345678;
		// write 0x12345678
		// read again, if value is not 0x12345678, this address is invalid
		if( *pdwCurrentAddress != 0x12345678 )
		{
			return FALSE;
		}
		// check each 1MB
		pdwCurrentAddress += (0x100000 / 4);
	}
	return TRUE;
}