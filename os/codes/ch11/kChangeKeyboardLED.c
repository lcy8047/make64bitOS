BOOL kChangeKeyboardLED( BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn )
{
    int i, j;

    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }
    // 입력 버퍼(포트 0x60)로 LED상태 변경 커맨드(0xED) 전송
    kOutPortByte(0x60, 0xED);
    
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    for(j=0; j<100; j++)
    {
        for(i=0; i<0xFFFF; i++)
        {
            if(kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }
        // check ACK
        if(kInPortByte( 0x60 ) == 0xFA)
        {
            break;
        }
    }
    if( j >= 100 )
    {
        return FALSE;
    }

    // 
    kOutPortByte(0x60, (bCapsLockOn << 2) | ( bNumLockOn << 1) | bScrollLockOn );
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    for( j=0; j<100; j++)
    {
        for( i=0; i<0xFFFF; i++)
        {
            if( kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }
        if(kInPortByte( 0x60 ) == 0xFA)
        {
            break;
        }
    }

    if(j >= 100)
    {
        return FALSE;
    }

    return TRUE;
}