#include <stdio.h>
#include <kernel.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libcdvd.h>
#include <map>

extern u8 dvdmark[];
extern u32 size_dvdmark;

std::map<int, const char*> diskTypeMap
{
	{SCECdGDTFUNCFAIL, "FUNC FAIL"},
	{SCECdNODISC,"NO DISC"},
	{SCECdDETCT,"STILL DETECTING"},
	{SCECdDETCTCD,"CD"},
	{SCECdDETCTDVDS,"DVDS"},
	{SCECdDETCTDVDD,"DVDD"},
	{SCECdUNKNOWN,"UNKNOWN"},
	{SCECdPSCD,"PS1 CD NO CDDA"},
	{SCECdPSCDDA,"PS1 CD WITH CDDA"},
	{SCECdPS2CD,"PS2 CD NO CDDA"},
	{SCECdPS2CDDA,"PS2 CD WITH CDDA"},
	{SCECdPS2DVD,"PS2 DVD"},

	{SCECdCDDA,"CDDA"},
	{SCECdDVDV,"DVD VIDEO"},
	{SCECdIllegalMedia,"ILLEGAL MEDIA"}
};

void _libcglue_timezone_update(void){}

int main(int argc, char* argv[])
{
	SifInitRpc(0);
	SifLoadFileInit();
	sceCdInit(SCECdINIT);
	while(1)
	{
		int mediaType = sceCdGetDiskType();
		if(mediaType == 0)
		{
			printf("No media found. Please insert a disc\n");
			for(int i = 0; i < 75; i++)
				nopdelay();
		}
		else if (mediaType == 1)
		{
			printf("Detecting media type...\n");
			for(int i = 0; i < 75; i++)
				nopdelay();
		}
		else
		{
			printf("Media found (%s)\n",diskTypeMap.at(mediaType));
			break;
		}
	}

	printf("Going to load our IOP module..\n");

	sbv_patch_enable_lmb();
	int mod_result = 0;
	int loadResult = SifExecModuleBuffer(dvdmark, size_dvdmark, 0, NULL, &mod_result);
	printf("SifLoadModuleBuffer resulted in %d (module result %d)\n", loadResult, mod_result);
	SleepThread();
}
