#include "types.h"
#include "AssemblyUtility.h"
#include "keyboard.h"
#include "queue.h"

// keyboard controller & keyboard management

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

BOOL kWaitForACKAndPutOtherScanCode( void )
{
    int i, j;
    BYTE bData;
    BOOL bResult = FALSE;

    for( j = 0 ; j < 100 ; j++ )
    {
        for( i = 0 ; i < 0xFFFF ; i++ )
        {
            if( kIsOutputBufferFull() == TRUE )
            {
                break;
            }
        }

        bData = kInPortByte( 0x60 );
        if( bData == 0xFA )
        {
            bResult = TRUE;
            break;
        }
        else
        {
            kConvertScanCodeAndPutQueue( bData );
        }
    }
    return bResult;
}

BOOL kActivateKeyboard( void )
{
    int i;
    int j;
    BOOL bPreviousInterrupt;
    BOOL bResult;

    bPreviousInterrupt = kSetInterruptFlag( FALSE );

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
    bResult = kWaitForACKAndPutOtherScanCode();
    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}

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

BOOL kChangeKeyboardLED( BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn )
{
    int i, j;
    BOOL bPreviousInterrupt;
    BOOL bResult;
    BYTE bData;

    bPreviousInterrupt = kSetInterruptFlag( FALSE );

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

    bResult = kWaitForACKAndPutOtherScanCode();

    if(bResult == FALSE)
    {
        kSetInterruptFlag( bPreviousInterrupt );
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
    bResult = kWaitForACKAndPutOtherScanCode();

    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;

}

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
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }
    // 커맨드 레지스터(0x64)에 출력 포트 설정 커맨드(0xD1) 전달
    kOutPortByte(0x64, 0xD1);
    // 입력버퍼에(0x)
    kOutPortByte(0x60, bOutputPortData);
}

void kReboot( void )
{
    int i;

    for( i = 0; i < 0xFFFF; i++ )
    {
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }

    kOutPortByte( 0x64, 0xD1 );
    kOutPortByte( 0x64, 0x00 );

    while( 1 )
    {
        ;
    }
}

// convert scan code to ascii code functions

static KETBOARDMANAGER gs_stKeyboardManager = { 0, };

static QUEUE gs_stKeyQueue;
static KEYDATA gs_vstKeyQueueBuffer[ KEY_MAXQUEUECOUNT ];

static KEYMAPPINGENTRY gs_vstKeyMappingTable[ KEY_MAPPINGTABLEMAXCOUNT ] =
{
    /*  0   */  {   KEY_NONE        ,   KEY_NONE        },
    /*  1   */  {   KEY_ESC         ,   KEY_ESC         },
    /*  2   */  {   '1'             ,   '!'             },
    /*  3   */  {   '2'             ,   '@'             },
    /*  4   */  {   '3'             ,   '#'             },
    /*  5   */  {   '4'             ,   '$'             },
    /*  6   */  {   '5'             ,   '%'             },
    /*  7   */  {   '6'             ,   '^'             },
    /*  8   */  {   '7'             ,   '&'             },
    /*  9   */  {   '8'             ,   '*'             },
    /*  10  */  {   '9'             ,   '('             },
    /*  11  */  {   '0'             ,   ')'             },
    /*  12  */  {   '-'             ,   '_'             },
    /*  13  */  {   '='             ,   '+'             },
    /*  14  */  {   KEY_BACKSPACE   ,   KEY_BACKSPACE   },
    /*  15  */  {   KEY_TAB         ,   KEY_TAB         },
    /*  16  */  {   'q'             ,   'Q'             },
    /*  17  */  {   'w'             ,   'W'             },
    /*  18  */  {   'e'             ,   'E'             },
    /*  19  */  {   'r'             ,   'R'             },
    /*  20  */  {   't'             ,   'T'             },
    /*  21  */  {   'y'             ,   'Y'             },
    /*  22  */  {   'u'             ,   'U'             },
    /*  23  */  {   'i'             ,   'I'             },
    /*  24  */  {   'o'             ,   'O'             },
    /*  25  */  {   'p'             ,   'P'             },
    /*  26  */  {   '['             ,   '{'             },
    /*  27  */  {   ']'             ,   '}'             },
    /*  28  */  {   '\n'            ,   '\n'            },
    /*  29  */  {   KEY_CTRL        ,   KEY_CTRL        },
    /*  30  */  {   'a'             ,   'A'             },
    /*  31  */  {   's'             ,   'S'             },
    /*  32  */  {   'd'             ,   'D'             },
    /*  33  */  {   'f'             ,   'F'             },
    /*  34  */  {   'g'             ,   'G'             },
    /*  35  */  {   'h'             ,   'H'             },
    /*  36  */  {   'j'             ,   'J'             },
    /*  37  */  {   'k'             ,   'K'             },
    /*  38  */  {   'l'             ,   'L'             },
    /*  39  */  {   ';'             ,   ':'             },
    /*  40  */  {   '\''            ,   '\"'            },
    /*  41  */  {   '`'             ,   '~'             },
    /*  42  */  {   KEY_LSHIFT      ,   KEY_LSHIFT      },
    /*  43  */  {   '\\'            ,   '|'             },
    /*  44  */  {   'z'             ,   'Z'             },
    /*  45  */  {   'x'             ,   'X'             },
    /*  46  */  {   'c'             ,   'C'             },
    /*  47  */  {   'v'             ,   'V'             },
    /*  48  */  {   'b'             ,   'B'             },
    /*  49  */  {   'n'             ,   'N'             },
    /*  50  */  {   'm'             ,   'M'             },
    /*  51  */  {   ','             ,   '<'             },
    /*  52  */  {   '.'             ,   '>'             },
    /*  53  */  {   '/'             ,   '?'             },
    /*  54  */  {   KEY_RSHIFT      ,   KEY_RSHIFT      },
    /*  55  */  {   '*'             ,   '*'             },
    /*  56  */  {   KEY_LALT        ,   KEY_LALT        },
    /*  57  */  {   ' '             ,   ' '             },
    /*  58  */  {   KEY_CAPSLOCK    ,   KEY_CAPSLOCK    },
    /*  59  */  {   KEY_F1          ,   KEY_F1          },
    /*  60  */  {   KEY_F2          ,   KEY_F2          },
    /*  61  */  {   KEY_F3          ,   KEY_F3          },
    /*  62  */  {   KEY_F4          ,   KEY_F4          },
    /*  63  */  {   KEY_F5          ,   KEY_F5          },
    /*  64  */  {   KEY_F6          ,   KEY_F6          },
    /*  65  */  {   KEY_F7          ,   KEY_F7          },
    /*  66  */  {   KEY_F8          ,   KEY_F8          },
    /*  67  */  {   KEY_F9          ,   KEY_F9          },
    /*  68  */  {   KEY_F10         ,   KEY_F10         },
    /*  69  */  {   KEY_NUMLOCK     ,   KEY_NUMLOCK     },
    /*  70  */  {   KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK  },
    /*  71  */  {   KEY_HOME        ,   '7'             },
    /*  72  */  {   KEY_UP          ,   '8'             },
    /*  73  */  {   KEY_PAGEUP      ,   '9'             },
    /*  74  */  {   '-'             ,   '-'             },
    /*  75  */  {   KEY_LEFT        ,   '4'             },
    /*  76  */  {   KEY_CENTER      ,   '5'             },
    /*  77  */  {   KEY_RIGHT       ,   '6'             },
    /*  78  */  {   '+'             ,   '+'             },
    /*  79  */  {   KEY_END         ,   '1'             },
    /*  80  */  {   KEY_DOWN        ,   '2'             },
    /*  81  */  {   KEY_PAGEDOWN    ,   '3'             },
    /*  82  */  {   KEY_INS         ,   '0'             },
    /*  83  */  {   KEY_DEL         ,   '.'             },
    /*  84  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  85  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  86  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  87  */  {   KEY_F11         ,   KEY_F11         },
    /*  88  */  {   KEY_F12         ,   KEY_F12         }
};

BOOL kIsAlphabetScanCode( BYTE bScanCode )
{
    if( ( 'a' <= gs_vstKeyMappingTable[ bScanCode ].bNormalCode ) &&
        ( gs_vstKeyMappingTable[ bScanCode ].bNormalCode <= 'z' ) )
    {
        return TRUE;
    }
    return FALSE;
}

BOOL kIsNumberOrSymbolScanCode( BYTE bScanCode )
{
    if( ( 2 <= bScanCode ) && ( bScanCode <= 53 ) &&
    ( kIsAlphabetScanCode( bScanCode ) == FALSE ) ) 
    {
        return TRUE;
    }
    return FALSE;
}

BOOL kIsNumberPadScanCode( BYTE bScanCode ) 
{
// 숫자 패드는 스캔 코드의 71~83에 있음
    if( ( 71 <= bScanCode ) && ( bScanCode <= 83 ) ) 
    {
        return TRUE;
    }
    return FALSE;
}

BOOL kIsUseCombinedCode(BYTE bScanCode)
{
    BYTE bDownScanCode;
    BOOL bUseCombinedKey = FALSE;

    bDownScanCode = bScanCode & 0x7F;
 
    // 알파벳이면 shift, capslock의 영향읇 받음
    if( kIsAlphabetScanCode( bDownScanCode ) == TRUE )
    {
        if( gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn )
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    // 숫자 or 기호는 shift의 영향만 받음
    else if( kIsNumberOrSymbolScanCode( bDownScanCode ) == TRUE )
    {
        if( gs_stKeyboardManager.bShiftDown )
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    else if( ( kIsNumberPadScanCode( bDownScanCode ) == TRUE ) && 
             ( gs_stKeyboardManager.bExtendedCodeIn == FALSE ) )
    {
        if( gs_stKeyboardManager.bNumLockOn == TRUE )
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    return bUseCombinedKey;
}

void UpdateCombinationKeyStatusAndLED( BYTE bScanCode )
{
    BOOL bDown;
    BYTE bDownScanCode;
    BOOL bLEDStatusChanged = FALSE;

    // 눌림 또는 떨어짐 상태처리, 최상위 비트(비트 7)가 1이면 키가 떨어졌음을 의미하고
    // 0이면 떨어졌음을 의미함
    if( bScanCode & 0x80 )
    {
        bDown = FALSE;
        bDownScanCode = bScanCode & 0x7F;
    }
    else
    {
        bDown = TRUE;
        bDownScanCode = bScanCode;
    }

    // 조합 키 검색
    // Shift 키의 스캔 코드(42 or 54)이면 Shift 키의 상태 갱신
    if( ( bDownScanCode == 42 ) || ( bDownScanCode == 54 ) )
    {
        gs_stKeyboardManager.bShiftDown = bDown;
    }
    // Caps Lock 키의 스캔 코드(58)이면 Caps Lock의 상태 갱신하고 LED 상태 변경
    else if( ( bDownScanCode == 58 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bCapsLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Num Lock 키의 스캔 코드(69)이면 Num Lock의 상태를 갱신하고 LED 상태 변경
    else if( ( bDownScanCode == 69 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bNumLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Scroll Lock 키의 스캔 코드(70)이면 Scroll Lock의 상태를 갱신하고 LED 상태 변경
    else if( ( bDownScanCode == 70 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bScrollLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }

    // LED 상태가 변했으면 키보드로 커맨드를 전송하여 LED를 변경
    if( bLEDStatusChanged == TRUE )
    {
        kChangeKeyboardLED( gs_stKeyboardManager.bCapsLockOn,
            gs_stKeyboardManager.bNumLockOn, gs_stKeyboardManager.bScrollLockOn );
    }
}

BOOL kConvertScanCodeToASCIICode( BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags )
{
    BOOL bUseCombinedKey;

    // 이전에 Pause 키가 수신되었다면, Pause의 남은 스캔 코드를 무시
    if( gs_stKeyboardManager.iSkipCountForPause > 0 )
    {
        gs_stKeyboardManager.iSkipCountForPause--;
        return FALSE;
    }

    // Pause 키는 특별히 처리
    if( bScanCode == 0xE1 )
    {
        *pbASCIICode = KEY_PAUSE;
        *pbFlags = KEY_FLAGS_DOWN;
        gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
        return TRUE;
    }
    // 확장 키 코드가 들어왔을 때, 실제 키 값은 다음에 들어오므로 플래그 설정만 하고 종료
    else if( bScanCode == 0xE0 )
    {
        gs_stKeyboardManager.bExtendedCodeIn = TRUE;
        return FALSE;
    }

    // 조합된 키를 반환해야 하는가?
    bUseCombinedKey = kIsUseCombinedCode( bScanCode );

    // 키 값 설정
    if( bUseCombinedKey == TRUE )
    {
        *pbASCIICode = gs_vstKeyMappingTable[ bScanCode & 0x7F ].bCombinedCode;
    }
    else
    {
        *pbASCIICode = gs_vstKeyMappingTable[ bScanCode & 0x7F ].bNormalCode;
    }

    // 확장 키 유무 설정
    if( gs_stKeyboardManager.bExtendedCodeIn == TRUE )
    {
        *pbFlags = KEY_FLAGS_EXTENDEDKEY;
        gs_stKeyboardManager.bExtendedCodeIn = FALSE;
    }
    else
    {
        *pbFlags = 0;
    }

    // 눌러짐 또는 떨어짐 유무 설정
    if( ( bScanCode & 0x80 ) == 0 )
    {
        *pbFlags |= KEY_FLAGS_DOWN;
    }

    // 조합 키 눌림 또는 떨어짐 상태를 갱신
    UpdateCombinationKeyStatusAndLED( bScanCode );
    return TRUE;
}

BOOL kInitializeKeyboard( void )
{
    kInitializeQueue( &gs_stKeyQueue, gs_vstKeyQueueBuffer, KEY_MAXQUEUECOUNT, sizeof( KEYDATA ));
    return kActivateKeyboard();
}

BOOL kConvertScanCodeAndPutQueue( BYTE bScanCode )
{
    KEYDATA stData;
    BOOL bResult = FALSE;
    BOOL bPreviousInterrupt;

    stData.bScanCode = bScanCode;

    if( kConvertScanCodeToASCIICode( bScanCode, &( stData.bASCIICode ),
            &( stData.bFlags ) ) == TRUE )
    {
        bPreviousInterrupt = kSetInterruptFlag( FALSE );

        bResult = kPutQueue( &gs_stKeyQueue, &stData );

        kSetInterruptFlag( bPreviousInterrupt );
    }

    return bResult;
}

BOOL kGetKeyFromKeyQueue( KEYDATA* pstData )
{
    BOOL bResult;
    BOOL bPreviousInterrupt;

    if( kIsQueueEmpty( &gs_stKeyQueue ) == TRUE )
    {
        return FALSE;
    }
    bPreviousInterrupt = kSetInterruptFlag( FALSE );

    bResult = kGetQueue( &gs_stKeyQueue, pstData );

    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}