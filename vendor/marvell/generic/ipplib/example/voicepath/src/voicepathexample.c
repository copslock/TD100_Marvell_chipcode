#include "misc.h"
#include "ippVP.h"
#include "stdio.h"

#ifdef LINUX_VP
#include <sys/time.h>
#endif

#define	VOICEPATH_ALIGH_BUF(buf, align)	(((unsigned int)(buf)+(align)-1)&~((unsigned int)(align)-1))

#define CMD_FIRE_AT_FRAMEIDX	-1
#define CMD_FIRE_AT_FRAMEIDX2	-1
#define CMD_FIRE_AT_FRAMEIDX3	-1

int runmode_TxOnly(char* SinFileName, char* RefFileName, char* SoutFileName)
{
	IppCodecStatus ret;
	FILE * fpSin, * fpRef, * fpSout;

	int szFileSin, szFileRef, szFile;
	int cnt,i;
	
	Ipp16s bufSinSout[VOICEPATH_SAMPLEPERFRAME+VOICEPATH_INPUTOUTPUTALIGN/sizeof(Ipp16s)];
	Ipp16s bufRef[VOICEPATH_SAMPLEPERFRAME+VOICEPATH_INPUTOUTPUTALIGN/sizeof(Ipp16s)];
	Ipp16s* pSinSout  = (Ipp16s*)VOICEPATH_ALIGH_BUF(bufSinSout, VOICEPATH_INPUTOUTPUTALIGN);
	Ipp16s* pRef  = (Ipp16s*)VOICEPATH_ALIGH_BUF(bufRef, VOICEPATH_INPUTOUTPUTALIGN);
	

	MiscGeneralCallbackTable* pMemFunsTbl = NULL;

	//voice path object
	void* pVoicePathState;
#ifdef LINUX_VP
	struct timeval g_tv;
		struct timezone g_tz;
		long long int at_start;
		long long int at_stop;
		long long int at = 0;
#endif

	//voice path user config
	voicepath_cfg_t vpcfg;
	vpcfg.auxaec_mode = CFG_SP_MODE;//CFG_ESNOECHO_MODE;//CFG_ES_MODE;
	vpcfg.path = CFG_TX_PATH;

	if( !SinFileName || !RefFileName || !SoutFileName ) {
		printf("file name error!\n");
		return -1;
	}
	if( NULL == (fpSin = fopen(SinFileName,"rb")) )
	{
		printf("open file %s fail!\n", SinFileName);
		return -1;
	}
	if( NULL == (fpRef = fopen(RefFileName,"rb")) )
	{
		printf("open file %s fail!\n", RefFileName);
		return -1;
	}
	if( NULL == (fpSout = fopen(SoutFileName,"wb")) )
	{
		printf("open file %s fail!\n", SoutFileName);
		return -1;
	}

	fseek(fpSin, 0, SEEK_END);
	szFileSin = ftell(fpSin);
	fseek(fpSin, 0, SEEK_SET);
	
	fseek(fpRef, 0, SEEK_END);
	szFileRef = ftell(fpRef);
	fseek(fpRef, 0, SEEK_SET);

	szFile = szFileSin < szFileRef ? szFileSin : szFileRef;
	cnt = szFile/(sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);

	if( 0 != miscInitGeneralCallbackTable(&pMemFunsTbl) ) {
		return -1;
	}

//	pMemFunsTbl->fFileOpen = fopen;
//	pMemFunsTbl->fFileClose = fclose;

	
	ret = VoicePathInit(&vpcfg, &pVoicePathState, pMemFunsTbl);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}
	

	newMode = vpcfg.auxaec_mode; //CFG_ESNOECHO_MODE;
	if (newMode != vpcfg.auxaec_mode){
		VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_CHANGEAUXAECMODE, &newMode, NULL);
	}


	printf("\n");

	for(i=0; i<cnt; i++) {
		fread(pSinSout, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpSin);
		fread(pRef, sizeof(Ipp16s),VOICEPATH_SAMPLEPERFRAME, fpRef);

#ifdef _IPP_X86
		printf("\rframe=%-8ld",i);
#endif

	/*	if( CMD_FIRE_AT_FRAMEIDX == i ) {
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_ENABLENS, (void*)0, (void*)1);
		}
		if( CMD_FIRE_AT_FRAMEIDX2 == i ) {
			CFG_AUXAEC_MODE	newMode = CFG_ESNOECHO_MODE;
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_CHANGEAUXAECMODE, &newMode, NULL);
		}*/
		
#ifdef LINUX_VP
		gettimeofday(&g_tv, &g_tz);
		at_start = g_tv.tv_sec * 1000000 + g_tv.tv_usec;
#endif
		ret = VoicePathTx(pVoicePathState, pSinSout, pRef, VOICEPATH_SAMPLEPERFRAME);
#ifdef LINUX_VP
		gettimeofday(&g_tv, &g_tz);
		at_stop = g_tv.tv_sec * 1000000 + g_tv.tv_usec;
		at+=(at_stop - at_start);
#endif
		if( IPP_STATUS_NOERR != ret ) {
			return -1;
		}

		fwrite(pSinSout, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpSout);
	}

#ifdef LINUX_VP
	printf("cost %lld us, totol is %d ms\n", at, cnt*20);
#endif
	ret = VoicePathFree(&pVoicePathState);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}

	

	if(fpSin) {
		fclose(fpSin);
		fpSin = NULL;
	}
	if(fpRef) {
		fclose(fpRef);
		fpRef = NULL;
	}
	if(fpSout) {
		fclose(fpSout);
		fpSout = NULL;
	}

//	miscFreeGeneralCallbackTable(&pMemFunsTbl);

	return 0;
}

int runmode_RxOnly(char* RinFileName, char* NoiseLevelFileName, char* RoutFileName)
{
	IppCodecStatus ret;
	FILE * fpRin, * fpRout;
	FILE * fpNL = NULL; 

	int szFileRin, szFileNL, cntFrameRin, cntFrameNL;
	int cnt,i;
	
	Ipp16s bufRinRout[VOICEPATH_SAMPLEPERFRAME+VOICEPATH_INPUTOUTPUTALIGN/sizeof(Ipp16s)];
	Ipp32s NoiseLevel;
	Ipp16s* pRinRout  = (Ipp16s*)VOICEPATH_ALIGH_BUF(bufRinRout, VOICEPATH_INPUTOUTPUTALIGN);
	

	MiscGeneralCallbackTable* pMemFunsTbl = NULL;

	//voice path object
	void* pVoicePathState;

	//voice path user config
	voicepath_cfg_t vpcfg;
	vpcfg.auxaec_mode = CFG_HF_MODE;//CFG_ESNOECHO_MODE;//CFG_ES_MODE;
	vpcfg.path = CFG_RX_PATH;
	

	if( !RinFileName || !RoutFileName ) {
		printf("file name error!\n");
		return -1;
	}
	if( NULL == (fpRin = fopen(RinFileName,"rb")) )
	{
		printf("open file %s fail!\n", RinFileName);
		return -1;
	}
	if( NULL != NoiseLevelFileName && NULL == (fpNL = fopen(NoiseLevelFileName,"rb")) )
	{
		printf("open file %s fail!\n", NoiseLevelFileName);
		return -1;
	}
	if( NULL == (fpRout = fopen(RoutFileName,"wb")) )
	{
		printf("open file %s fail!\n", RoutFileName);
		return -1;
	}

	fseek(fpRin, 0, SEEK_END);
	szFileRin = ftell(fpRin);
	fseek(fpRin, 0, SEEK_SET);
	cntFrameRin = szFileRin/(sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);
	
	if(fpNL) {
		fseek(fpNL, 0, SEEK_END);
		szFileNL = ftell(fpNL);
		fseek(fpNL, 0, SEEK_SET);
		cntFrameNL = szFileNL/sizeof(Ipp32s);
	}else{
		cntFrameNL = cntFrameRin;
	}

	cnt = cntFrameRin < cntFrameNL ? cntFrameRin : cntFrameNL ;

	if( 0 != miscInitGeneralCallbackTable(&pMemFunsTbl) ) {
		return -1;
	}
	ret = VoicePathInit(&vpcfg, &pVoicePathState, pMemFunsTbl);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}

	printf("\n");

/*	if( !fpNL ) {
		//disable the dependence between RX AVC and local envoirnment noise
		//Defaultly, RX AVC component could adjusts its behavior according to local environment noise level.
		//When and only when application couldn't provide noise level information to voice path RX API, it should shut off the relationship between RX AVC and noise by sending following command and setting the pPar1 to 0.
		//Note: Voice path TX API will provide environment noise information, therefore, if application has called TX API, it needn't to provide noise level information additionally.
		VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_ENABLEAVCRESPONSENOISE, 0, NULL);
	}*/

	for(i=0; i<cnt; i++) {
		fread(pRinRout, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpRin);
		if(fpNL) {
			fread(&NoiseLevel, sizeof(Ipp32s), 1, fpNL);
			//VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_SETNOISELEVLETORX, &NoiseLevel, NULL);
		}

#ifdef _IPP_X86
		printf("\rframe=%-8ld",i);
#endif

	/*	if( CMD_FIRE_AT_FRAMEIDX == i ) {
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_ENABLEAVC, (void*)0, (void*)0);
		}
		if( CMD_FIRE_AT_FRAMEIDX2 == i ) {
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_ENABLEAVC, (void*)1, (void*)0);
		}*/

		ret = VoicePathRx(pVoicePathState, pRinRout, VOICEPATH_SAMPLEPERFRAME);
		if( IPP_STATUS_NOERR != ret ) {
			return -1;
		}

		fwrite(pRinRout, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpRout);
	}

	ret = VoicePathFree(&pVoicePathState);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}


	miscFreeGeneralCallbackTable(&pMemFunsTbl);

	if(fpRin) {
		fclose(fpRin);
		fpRin = NULL;
	}
	if(fpNL) {
		fclose(fpNL);
		fpNL = NULL;
	}
	if(fpRout) {
		fclose(fpRout);
		fpRout = NULL;
	}

	return 0;
}


//Under LOOP mode, output of TX path is the input of RX path. In other words, a short route exists between TX's output and RX's input.
int runmode_Loop(char* SinFileName, char* RefFileName, char* RoutFileName)
{
	IppCodecStatus ret;
	FILE * fpSin, * fpRout;
	FILE * fpRef = NULL; 

	int szFileSin, szFileRef, cntFrameSin, cntFrameRef;
	int cnt,i;
	
	Ipp16s bufSpeech[VOICEPATH_SAMPLEPERFRAME+VOICEPATH_INPUTOUTPUTALIGN/sizeof(Ipp16s)];
	Ipp16s* pSpeech  = (Ipp16s*)VOICEPATH_ALIGH_BUF(bufSpeech, VOICEPATH_INPUTOUTPUTALIGN);
	Ipp16s bufRef[VOICEPATH_SAMPLEPERFRAME+VOICEPATH_INPUTOUTPUTALIGN/sizeof(Ipp16s)];
	Ipp16s* pRef  = (Ipp16s*)VOICEPATH_ALIGH_BUF(bufRef, VOICEPATH_INPUTOUTPUTALIGN);

	MiscGeneralCallbackTable* pMemFunsTbl = NULL;

	//voice path object
	void* pVoicePathStateTx;
	void* pVoicePathStateRx;

	//voice path user config
	voicepath_cfg_t vpcfgTx;
	voicepath_cfg_t vpcfgRx;

	vpcfgTx.auxaec_mode = CFG_ECHO64MS_MODE;//CFG_ECHO64MS_MODE;//CFG_ESNOECHO_MODE;//CFG_ES_MODE;
	vpcfgTx.path = CFG_TX_PATH;

	vpcfgRx.auxaec_mode = CFG_ECHO64MS_MODE;//CFG_ECHO64MS_MODE;//CFG_ESNOECHO_MODE;//CFG_ES_MODE;
	vpcfgRx.path = CFG_RX_PATH;

	memset(pSpeech, 0, sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);
	memset(pRef, 0, sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);

	if( !SinFileName || !RoutFileName ) {
		printf("file name error!\n");
		return -1;
	}
	if( NULL == (fpSin = fopen(SinFileName,"rb")) )
	{
		printf("open file %s fail!\n", SinFileName);
		return -1;
	}
	if( NULL != RefFileName && NULL == (fpRef = fopen(RefFileName,"rb")) )
	{
		printf("open file %s fail!\n", RefFileName);
		return -1;
	}
	if( NULL == (fpRout = fopen(RoutFileName,"wb")) )
	{
		printf("open file %s fail!\n", RoutFileName);
		return -1;
	}

	fseek(fpSin, 0, SEEK_END);
	szFileSin = ftell(fpSin);
	fseek(fpSin, 0, SEEK_SET);
	cntFrameSin = szFileSin/(sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);
	
	if(fpRef) {
		fseek(fpRef, 0, SEEK_END);
		szFileRef = ftell(fpRef);
		fseek(fpRef, 0, SEEK_SET);
		cntFrameRef = szFileRef/(sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);
	}else{
		cntFrameRef = cntFrameSin;
	}

	cnt = cntFrameSin < cntFrameRef ? cntFrameSin : cntFrameRef ;

	if( 0 != miscInitGeneralCallbackTable(&pMemFunsTbl) ) {
		return -1;
	}
	ret = VoicePathInit(&vpcfgTx, &pVoicePathStateTx, pMemFunsTbl);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}

	ret = VoicePathInit(&vpcfgRx, &pVoicePathStateRx, pMemFunsTbl);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}
	printf("\n");

	

	for(i=0; i<cnt; i++) {

		if(fpRef) {
			fread(pRef, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpRef);
		}else{
			memcpy(pRef, pSpeech, sizeof(Ipp16s)*VOICEPATH_SAMPLEPERFRAME);
		}

		fread(pSpeech, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpSin);
		
#ifdef _IPP_X86
		printf("\rframe=%-8ld",i);
#endif

/*		if( CMD_FIRE_AT_FRAMEIDX == i ) {
			CFG_AUXAEC_MODE	newMode = CFG_ESNOECHO_MODE;//CFG_ES_MODE;
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_CHANGEAUXAECMODE, &newMode, NULL);
		}
		if( CMD_FIRE_AT_FRAMEIDX2 == i ) {
			VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_ENABLEAVC, (void*)0, (void*)0);
		}*/

		ret = VoicePathTx(pVoicePathStateTx, pSpeech, pRef, VOICEPATH_SAMPLEPERFRAME);
		if( IPP_STATUS_NOERR != ret ) {
			return -1;
		}

		if( CMD_FIRE_AT_FRAMEIDX3 == i ) {
			CFG_AUXAEC_MODE	newMode = CFG_ES_MODE;//CFG_ES_MODE;
			//VoicePathSendCmd(pVoicePathState, VOICEPATH_CMD_CHANGEAUXAECMODE, &newMode, NULL);
		}

		ret = VoicePathRx(pVoicePathStateTx, pSpeech, VOICEPATH_SAMPLEPERFRAME);
		if( IPP_STATUS_NOERR != ret ) {
			return -1;
		}

		fwrite(pSpeech, sizeof(Ipp16s), VOICEPATH_SAMPLEPERFRAME, fpRout);
	}

	ret = VoicePathFree(&pVoicePathStateTx);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}

	ret = VoicePathFree(&pVoicePathStateRx);
	if( IPP_STATUS_NOERR != ret ) {
		return -1;
	}

	miscFreeGeneralCallbackTable(&pMemFunsTbl);

	if(fpSin) {
		fclose(fpSin);
		fpSin = NULL;
	}
	if(fpRef) {
		fclose(fpRef);
		fpRef = NULL;
	}
	if(fpRout) {
		fclose(fpRout);
		fpRout = NULL;
	}

	return 0;
}

#ifdef WINCE
int wmain()
#else
int main()
#endif
{

	runmode_TxOnly("src.pcm","ref.pcm","aaaaaaaaaaaaaaaaout_es.pcm");
	//runmode_TxOnly("c:\\sin_align.pcm","c:\\rin_align.pcm","Out.pcm");
//	runmode_RxOnly("ref.pcm", NULL, "Rout.pcm");
	//runmode_RxOnly("c:\\tv4.inp", "c:\\tv4.dat", "Out.pcm");
	//runmode_Loop("sin_align.pcm", "rin_align.pcm", "Out.pcm");
	//runmode_Loop("c:\\tv6.inp", NULL, "Rout.pcm");

	return 0;
}




