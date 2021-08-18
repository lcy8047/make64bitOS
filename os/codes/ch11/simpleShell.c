char vcTemp[2] = { 0, };
BYTE bFlags;
BYYTE bTemp;
int t = 0;

while(1)
{
    if( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();

        if( kConvertScanCodeToASCIICode ( bTemp, &( vcTemp[0] ), &bFlags ) == TRUE )
        {
            if( bFlags & KEY_FLAGS_DOWN )
            {
                kPrintString( i++, 13, vcTemp );
            }
        }
    }
}