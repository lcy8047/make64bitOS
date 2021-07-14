#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include </usr/include/x86_64-linux-gnu/sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

/* get file, sourceSize return sector count */
int AdjustInSectorSize(int iFd, int iSourceSize);
/* write sector count in kernel */
void WriteKernelInformation(int iTargetFd, int iKernelSectorCount);
/* for copy file src(.bin) to target(.img) return file size */
int CopyFile(int iSourceFd, int iTargetFd);

int main(int argc, char *argv[])
{
    int iSourceFd;
    int iTargetFd;
    int iBootLoaderSize;
    int iKernel32SectorCount;
    int iSourceSize;

    if (argc < 3)
    {
        fprintf(stderr, "[ERROR] ImageMaker BookerLoader.bin Kernel32.bin\n");
        exit(-1);
    }

    if( (iTargetFd = open("Disk.img", O_RDWR | O_CREAT | O_TRUNC, S_IREAD | S_IWRITE)) == -1 )
    {
        fprintf(stderr, "[ERROR] Disk.img open fail\n");
        exit(-1);
    }

    // open bootloader, copy to disk image file
    printf( "[INFO] Copy boot loader to image file\n" );
    if( (iSourceFd = open(argv[1], O_RDONLY)) == -1 )
    {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[1]);
        exit(-1);
    }

    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // align sector size to Disk image
    iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf( "[INFO] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, iBootLoaderSize );

    // open kernel32, copy to Disk image
    printf( "[INFO] Copy protected mode kernel to image file\n" );
    if( (iSourceFd = open(argv[2], O_RDONLY)) == -1 )
    {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[2]);
        exit(-1);
    }

    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // align sector size to Disk image
    iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf( "[INFO] %s size = [%d] and sector count = [%d]\n", argv[2], iSourceSize, iKernel32SectorCount );

    // update sector count to Disk image
    printf( "[INFO] Start to write kernel information\n" );
    WriteKernelInformation(iTargetFd, iKernel32SectorCount);
    printf("[INFO] Image file create complete\n");

    close(iTargetFd);
    return 0;
}

int AdjustInSectorSize(int iFd, int iSourceSize)
{
    int i;
    int iAdjustSizeToSector;
    char cCh;
    int iSectorCount;

    iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
    cCh = 0x00;
    
    if (iAdjustSizeToSector != 0)
    {
        iAdjustSizeToSector = 512 - iAdjustSizeToSector;
        printf("[INFO] File size [%lu] and fill [%u] byte\n", iSourceSize,
               iAdjustSizeToSector);
        for (i = 0; i < iAdjustSizeToSector; i++)
        {
            write(iFd, &cCh, 1);
        }
    }
    else
    {
        printf("[INFO] File size is aligned 512 byte\n");
    }

    iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTESOFSECTOR;
    return iSectorCount;
}

void WriteKernelInformation(int iTargetFd, int iKernelSectorCount)
{
    unsigned short usData;
    long lPosition;
    // 파일의 시작에서 5바이트 떨어진 위치가 커널의 총 섹터 수 정보를 나타냄
    lPosition = lseek( iTargetFd, (off_t)5, SEEK_SET );
    if (lPosition == -1)
    {
        fprintf(stderr, "lseek fail. Return value = %d, errno = %d, %d\n", lPosition, errno, SEEK_SET);
        exit(-1);
    }

    // type cast 2bytes unsigned type because sector count is 2bytes
    usData = (unsigned short)iKernelSectorCount;
    write(iTargetFd, &usData, 2);
    printf("[INFO] Total sector count except boot loader [%d]\n", iKernelSectorCount);
}

int CopyFile(int iSourceFd, int iTargetFd)
{
    int iSourceFileSize;
    int iRead;
    int iWrite;
    char vcBuffer[BYTESOFSECTOR];
    iSourceFileSize = 0;
    while (1)
    {
        iRead = read(iSourceFd, vcBuffer, sizeof(vcBuffer));
        iWrite = write(iTargetFd, vcBuffer, iRead);
        if (iRead != iWrite)
        {
            fprintf(stderr, "[ERROR] iRead != iWrite.. \n");
            exit(-1);
        }
        iSourceFileSize += iRead;
        if (iRead != sizeof(vcBuffer))
        {
            break;
        }
    }
    // return read bytes(= file size)
    return iSourceFileSize;
}