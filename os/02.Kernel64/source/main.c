#include "types.h"
#include "keyboard.h"
#include "descriptor.h"
#include "pic.h"

// functions
void kPrintString( int iX, int iY, const char* pcString);

void main(void)
{
    /* for simple shell */
    char vcTemp[ 2 ] = { 0, };
    BYTE bTemp;
    int i = 0;
    KEYDATA stData;

    kPrintString( 0, 10, "Switch To IA-32e Mode Success" );
    kPrintString( 0, 11, "IA-32e C Language Kernel Start..............[Pass]" );
    
    kPrintString( 0, 12, "GDT Initialize And Switch For IA-32e Mode...[    ]" );
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kPrintString( 45, 12, "Pass" );

    kPrintString( 0, 13, "TSS Segment Load............................[    ]" );
    kLoadTR( GDT_TSSSEGMENT );
    kPrintString( 45, 13, "Pass" );

    kPrintString( 0, 14, "IDT Initialize..............................[    ]" );
    kInitializeIDTTables();
    kLoadIDTR( IDTR_STARTADDRESS );
    kPrintString( 45, 14, "Pass" );

    kPrintString( 0, 15, "Keyboard Activate And Queue Initialize......[    ]" );

    if( kInitializeKeyboard() == TRUE )
    {
        kPrintString( 45, 15, "Pass" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
        kPrintString( 45, 15, "Fail" );
        while(1);
    }

    kPrintString( 0, 16, "PIC Controller And Interrupt Initialize.....[    ]" );
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kPrintString( 45, 16, "Pass" );

    while(1)
    {
        if( kGetKeyFromKeyQueue( &stData ) == TRUE )
        {
            bTemp = kGetKeyboardScanCode();

            if( stData.bFlags & KEY_FLAGS_DOWN )
            {
                vcTemp[ 0 ] = stData.bASCIICode;
                kPrintString( i++, 17, vcTemp );

                // 0을 입력하면 0으로 나누는 연산을 수행해 divide error 예외 발생
                if( vcTemp[0] == '0')
                {
                    bTemp = bTemp / 0;
                }
            }
        }
    }
    

}

void kPrintString( int iX, int iY, const char* pcString)
{
    CHARACTER *pstScreen = (CHARACTER *)0xB8000;
    int i;
    
    pstScreen += ( iY * 80 ) + iX;

    for (i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].bCharacter = pcString[i];
    }
}