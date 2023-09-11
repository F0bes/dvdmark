#include <stdio.h>
#include <cdvdman.h>
#include <thbase.h>
#include <sysmem.h>

#define IOP_T4_COUNT *(u32 *)0x1F801490
#define IOP_T4_MODE *(u16 *)0x1F801494
#define IOP_T4_TARGET *(u32 *)0x1F801498

unsigned char  ring_buf[2048 * 200] __attribute__((aligned (8)));
unsigned char  buf[2048 * 200] __attribute__((aligned (8)));
void test_thread(void *arg)
{
	printf("[iop] Testing thread begin\n");

	printf("[iop] Firing up the timer. 1/16 scaler\n");
	IOP_T4_MODE = 2 << 13;

	printf("[iop] assuming 2_200_000 sectors are on the disc with a sector size of 2048 bytes\n");

	sceCdRMode readMode __attribute__((aligned (8)));
	readMode.trycount = 0;
	printf("[iop] setting up read mode (SCECdSpinNom 1)\n");
	readMode.spindlctrl = SCECdSpinNom;
	readMode.datapattern = 0;
	readMode.pad = 0;

	sceCdInit(SCECdINIT);
	sceCdSync(0);

	sceCdMmode(SCECdMmodeDvd);
	sceCdSync(0);

	printf("Waiting for the disk to be ready\n");
	while(sceCdDiskReady(0) == SCECdNotReady)
	{
		asm volatile("nop");
	}
	printf("Disk is ready\n");

	sceCdSeek(0);
	sceCdSync(0);
	
	sceCdStInit(200, 5, ring_buf);
	sceCdSync(0);

	printf("Starting stream\n");
	sceCdStStart(0 , &readMode);
//	printf("syncing\n");
//	sceCdSync(0);
	printf("Manually waiting...\n");
	for(int i = 0; i < 10000000; i++)
		asm volatile("nop");

	printf("Begin read block\n");
	int totalSectors = 2200000;
	int blockSize = 200;
	for (int lbn = 0; lbn < totalSectors;)
	{
		IOP_T4_COUNT = 0;
		u32 cnt_start = IOP_T4_COUNT;
		u32 error;
		u32 sectors_read = sceCdStRead(blockSize, (u32*)buf, STMNBLK, &error);
		
		if(sectors_read != blockSize || error)
			printf("Read %d sectors (err %d) at lbn %d\n", sectors_read, error, lbn);
		u32 cnt_end = IOP_T4_COUNT;
		u32 cnt_total = cnt_end - cnt_start;
		printf("%d,%d\n", lbn, cnt_total);
		lbn += sectors_read;
	}

	printf("[iop]finished\n");
	sceCdStStop();
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
