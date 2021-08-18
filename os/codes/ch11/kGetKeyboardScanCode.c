BYTE kGetKeyboardScanCode( void )
{
    // 출력 버퍼 (포트 0x60)에 데이터가 있을 때까지 대기
    while(kIsOutputBufferFull() == FALSE)
    {
        ;
    }
    // 출력 버퍼에서 읽어서 반환
    return kInPortByte( 0x60 );
}