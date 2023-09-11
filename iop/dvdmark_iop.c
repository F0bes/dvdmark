#include <stdio.h>
#include <cdvdman.h>
#include <thbase.h>
#include <sysmem.h>

#define IOP_T4_COUNT *(u32 *)0x1F801490
#define IOP_T4_MODE *(u16 *)0x1F801494
#define IOP_T4_TARGET *(u32 *)0x1F801498
void test_thread(void *arg)
{
	printf("[iop] Testing thread begin\n");

	printf("[iop] Firing up the timer. 1/16 scaler\n");
	IOP_T4_MODE = 2 << 13;
	u8 *megBlock = AllocSysMemory(0, 1024 * 1024, NULL);
	printf("[iop] Memory block is at %p\n", megBlock);

	printf("[iop] assuming 2_200_000 sectors are on the disc with a sector size of 2048 bytes\n");

	sceCdRMode readMode;
	readMode.trycount = 0;
	printf("[iop] setting up read mode (spinnom 1)\n");
	readMode.spindlctrl = 1;
	readMode.datapattern = 0;

	sceCdDiskReady(0);
	sceCdSync(0);

	sceCdSeek(0);
	sceCdSync(0);
	
	int sectorSize = 2048;
	int totalSectors = 2200000;
	int blockSize = (1024 * 1024) / sectorSize;
	for (int lbn = 0; lbn < totalSectors; lbn += blockSize)
	{
		IOP_T4_COUNT = 0;
		u32 cnt_start = IOP_T4_COUNT;
		sceCdRead(lbn,blockSize, megBlock, &readMode);
		sceCdSync(0);
		u32 cnt_end = IOP_T4_COUNT;
		u32 cnt_total = cnt_end - cnt_start;
		printf("%d,%d\n", lbn, cnt_total);
	}

	printf("[iop]finished\n");

	FreeSysMemory(megBlock);
}

int _start(int argc, char **argv)
{
	printf("[iop] Starting up\n");
	iop_thread_t thread;
	thread.attr = TH_C;
	thread.option = 0;
	thread.thread = test_thread;
	thread.priority = 100;
	thread.stacksize = 0x1000;

	int threadid = CreateThread(&thread);
	printf("[iop] created thread with id %d\n", threadid);
	StartThread(threadid, NULL);
	printf("[iop] returning\n");
	return 0;
}
