// 출력버퍼(포트 0x60)에 수신된 데이터가 존재하는지 확인
BOOL kIsOutputBufferFull( void )
{
    // 상태 레지스터(포트 0x64)에서 상태확인
    // 출력 버퍼 상태 비트(비트 0)가 1이면 데이터 존재
    if( kInPortByte( 0x64 ) & 0x01 )
    {
        return TRUE;
    }
    return FALSE;
}

// 입력 버퍼(포트 0x60)에 프로세서가 쓴 데이터가 남아있는지 확인
BOOL kIsInputBufferFull( void )
{
    // 상태 레지스터(포트 0x64)에서
    // 입력 버퍼 상태 비트(비트 1)가 1이면 키보드가 데이터를 가져가지 않음
    if(kInPortByte(0x64) & 0x02)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL kActivateKeyboard( void )
{
    int i;
    int j;

    // 컨트롤 레지스터(포트 0x64)에 키보드 활성화 커맨드(0xAE) 전달해서 활성화
    kOutPortByte( 0x64, 0xAE );

    // 입력버퍼(포트 0x60)가 빌 떄까지 기다린후 활성화 커맨드 전송
    // 0xffff만큼 기다림 - 저자 생각으로 전송되는데 충분한 시간
    // 이후 무시하고 전송함
    for(i=0; i<0xFFFF; i++)
    {
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }

    // 입력버퍼 (포트 0x60)로 키보드 활성화 커맨드(0xF4) 전송
    kOutPortByte( 0x60, 0xF4 );

    // 키보드에 커맨드를 쓰면 응답으로 ACK(0xFA)를 보냄
    // ACK 올 떄 까지 대기
    // ACK가 오기 전까지 출력 버퍼에 데이터가 저장될 수 있으므로
    // 100번 정도 반복하여 데이터 수신 확인 - 저자의 임의대로 충분한 횟수 수행
    for(j=0; j<100; j++)
    {
        // 0xffff까지 대기 후 무시하고 읽음
        for(i=0; i<0xffff; i++)
        {
            if(kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }

        // 출력 버퍼(포트 0x60)에서 읽은 데이터가 ACK면 성공
        if(kInPortByte( 0x60 ) == 0xFA)
        {
            return TRUE;
        }
    }

    return FALSE;

}