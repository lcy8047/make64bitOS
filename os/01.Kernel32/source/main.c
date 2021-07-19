#include "types.h"
#include "page.h"
#include "ModeSwitch.h"

/* print string */
void kPrintString( int iX, int iY, const char* pcString );
/* set 0 Kernel64 area 0x100000~0x600000 */
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );
void kCopyKernel64ImageTo2Mbyte(void);

void main( void )
{
	DWORD i;
	DWORD dwEAX, dwEBX, dwECX, dwEDX;
	char vcVendorString[ 13 ] = {0, };
	kPrintString(0, 3, "Protected Mode C Language Kernel Start......[Pass]");

	// check minimum memory size
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

	// initialize IA-32e kernel area
	kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
	if( kInitializeKernel64Area() == FALSE )
	{
		kPrintString(45, 5, "Fail");
		kPrintString(0, 6, "Kernel Area Initialize Fail");
		while(1);
	}
	kPrintString(45, 5, "Pass");

	// create page table for IA-32e mode Kernel
	kPrintString(0, 6, "IA-32e Page Tables Initialize...............[    ]");
	kInitializePageTables();
	kPrintString(45, 6, "Pass");

	// read vendor info
	kReadCPUID( 0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
	*( DWORD* ) vcVendorString = dwEBX;
	*(( DWORD* ) vcVendorString + 1) = dwECX;
	*(( DWORD* ) vcVendorString + 2) = dwEDX;
	kPrintString(0, 7, "Processor Vendor String.....................[             ]" );
	kPrintString( 45, 7, vcVendorString );

	// check support 64bit
	kReadCPUID( 0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX );
	kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
	if( dwEDX & ( 1 << 29 ) )
	{
		kPrintString(45, 8, "Pass");
	}
	else
	{
		kPrintString(45, 8, "Fail");
		kPrintString(0, 9, "This processor does not support 64bit mode~!!");
		while (1)
			;
	}

	// IA-32e mode kernel move to 0x200000
	kPrintString(0, 9, "Copy IA-32e Kernel To 2M Address............[    ]");
	kCopyKernel64ImageTo2Mbyte();
	kPrintString( 45, 9, "Pass");

	// switch to IA-32e Mode
	kPrintString(0, 10, "Switch To IA-32e Mode");
	kSwitchAndExecute64bitKernel();

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

void kCopyKernel64ImageTo2Mbyte(void)
{
	WORD wKernel32SectorCount, wTotalKernelSectorCount;
	DWORD *pdwSourceAddress, *pdwDestinationAddress;
	int i;

	// 0x7C05에 총 커널 섹터 수, 0x7C07에 보호 모드 커널 섹터 수가 들어 있음
	wTotalKernelSectorCount = *( ( WORD* ) 0x7C05 );
	wKernel32SectorCount = *( ( WORD* ) 0x7C07 );
	pdwSourceAddress = (DWORD *)(0x10000 + (wKernel32SectorCount * 512));
	pdwDestinationAddress = (DWORD *)0x200000;

	// IA-32e 모드 커널 섹터 크기만큼 복사
	for (i = 0; i < 512 * (wTotalKernelSectorCount - wKernel32SectorCount) / 4; i++)
	{
		*pdwDestinationAddress = *pdwSourceAddress;
		pdwDestinationAddress++;
		pdwSourceAddress++;
	}
}