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