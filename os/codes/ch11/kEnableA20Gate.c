// 키보드 컨트롤러의 출력포트에 A20게이트와 프로세서 리셋과 연결되어 있음
// 관련 기능 수행가능
// 출력포트 비트 0 - 프로세서 리셋
// 출력포트 비트 1 - A20 gate 
// 컨트롤러의 출력포트는 0xD0, 0xD1 커맨드로 접근 가능

void kEnableA20Gate( void )
{
    BYTE bOutputPortData;
    int i;

    // 컨트롤 레지스터(포트 0x64)에 출력 포트 read 커맨드(0xD0) 전송
    kOutPortByte( 0x64, 0xD0 );

    for(i=0; i<0xFFFF; i++)
    {
        if(kIsOutputBufferFull() == TRUE)
        {
            break;
        }
    }

    bOutputPortData = kInPortByte( 0x60 );

    // set A20 gate bit
    bOutputPortData != 0x02;

    // 
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsIputBufferFull() == FALSE)
        {
            break;
        }
    }
    // 커맨드 레지스터(0x64)에 출력 포트 설정 커맨드(0xD1) 전달
    kOutPortByte(0x64, 0xD1);
    // 입력버퍼에(0x)
    kOutPortByte(0x60, bOutputPortData);
}