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