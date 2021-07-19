#include "types.h"

// functions
void kPrintString( int iX, int iY, const char* pcString);

void main(void)
{
    kPrintString( 0, 10, "Switch To IA-32e Mode Success" );
    kPrintString( 0, 11, "IA-32e C Language Kernel Start..............[Pass]" );
    while(1);
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