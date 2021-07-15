#ifndef __PAGE_H__
#define __PAGE_H__

#include "types.h"

// 매크로
#define PAGE_FLAGS_P        0x00000001     // Present
#define PAGE_FLAGS_RW       0x00000002 // Read/Write
#define PAGE_FLAGS_US       0x00000004 // User/Supervisor(플래그 설정 시 유저 레벨) 
#define PAGE_FLAGS_PWT      0x00000008 // Page Level Write-through
#define PAGE_FLAGS_PCD      0x00000010 // Page Level Cache Disable
#define PAGE_FLAGS_A        0x00000020 // Accessed
#define PAGE_FLAGS_D        0x00000040 // Dirty
#define PAGE_FLAGS_PS       0x00000080 // Page Size
#define PAGE_FLAGS_G        0x00000100 // Global
#define PAGE_FLAGS_PAT      0x00001000 // Page Attribute Table Index
#define PAGE_FLAGS_EXB      0x80000000 // Execute Disable 비트
#define PAGE_FLAGS_DEFAULT  ( PAGE_FLAGS_P | PAGE_FLAGS_RW )
#define PAGE_TABLESIZE      0x1000
#define PAGE_MAXENTRYCOUNT  512
#define PAGE_DEFAULTSIZE    0x200000

// 구조체
#pragma pack(push, 1)

// 페이지 엔트리에 대한 자료구조
typedef struct kPageTableEntryStruct
{
    // PML4T와 PDPTE의 경우
    // 1비트 P, RW, US, PWT, PCD, A, D, PS, G, 3비트 Avail, 1비트 PAT, 8비트 Reserved,
    // 20비트 Base Address
    // PDE의 경우
    // 1비트 P, RW, US, PWT, PCD, A, D, 1, G, 3비트 Avail, 1비트 PAT, 8비트 Avail,
    // 11비트 Base Address
    DWORD dwAttributeAndLowerBaseAddress;
    // 8비트 Upper BaseAddress, 12비트 Reserved, 11비트 Avail, 1비트 EXB
    DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;
#pragma pack(pop)
// 함수
void kInitializePageTables(void);
void kSetPageEntryData(PTENTRY *pstEntry, DWORD dwUpperBaseAddress,
                       DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags);
#endif /*__PAGE_H__*/