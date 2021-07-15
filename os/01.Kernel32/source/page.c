#include "Page.h"

// cretae page table for IA-32e mode kernel
void kInitializePageTables( void )
{
    PML4TENTRY *pstPML4TEntry;
    PDPTENTRY *pstPDPTEntry;
    PDENTRY *pstPDEntry;
    DWORD dwMappingAddress;
    int i;
    // PML4 테이블 생성
    // 첫 번째 엔트리 외에 나머지는 모두 0으로 초기화
    pstPML4TEntry = (PML4TENTRY *)0x100000;
    kSetPageEntryData(&(pstPML4TEntry[0]), 0x00,
                        0x101000, PAGE_FLAGS_DEFAULT, 0);
    for (i = 1; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData(&(pstPML4TEntry[i]), 0, 0, 0, 0);
    }
    // 페이지 디렉터리 포인터 테이블 생성
    // 하나의 PDPT로 512GB까지 매핑 가능하므로 하나로 충분함
    // 64개의 엔트리를 설정하여 64GB까지 매핑함
    pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
    for (i = 0; i < 64; i++)
    {
        kSetPageEntryData(&(pstPDPTEntry[i]), 0,
                            0x102000 + (i * PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
    }

    for (i = 64; i < PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
    }

    // 페이지 디렉터리 테이블 생성
    // 하나의 페이지 디렉터리가 1GB까지 매핑 가능
    // 여유있게 64개의 페이지 디렉터리를 생성하여 총 64GB까지 지원
    pstPDEntry = ( PDENTRY* ) 0x102000;
    dwMappingAddress = 0;
    for (i = 0; i < PAGE_MAXENTRYCOUNT * 64; i++)
    {
        // 32비트로는 상위 어드레스를 표현할 수 없으므로, MB 단위로 계산한 다음
        // 최종 결과를 다시 4KB로 나누어 32비트 이상의 어드레스를 계산함
        kSetPageEntryData(&(pstPDEntry[i]), (i * (PAGE_DEFAULTSIZE >> 20)) >> 12,
                            dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
        dwMappingAddress += PAGE_DEFAULTSIZE;
    }
}
// 페이지 엔트리에 기준 주소와 속성 플래그를 설정
void kSetPageEntryData(PTENTRY *pstEntry, DWORD dwUpperBaseAddress,
                       DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{
    pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}