/******************************************************************************
// (C) Copyright 2010 Marvell International Ltd.
// All Rights Reserved
******************************************************************************/


#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>
#include <pthread.h>
//IPP voicepath lib include files
#include <misc.h>
#include <ippVP.h>
#include <cutils/properties.h>

//if open PLUGIN_DEBUG macro, please make dir under /data/ippvp to save dumped pcm files
//#define PLUGIN_DEBUG

#ifdef PLUGIN_DEBUG
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif



#include <utils/Log.h>
#define LOG_TAG "ALSA_PLUGIN--IPPVP"

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define  DOMAIN_SOCKET_NAME "/data/misc/ippvp/ippvp_plugin"
#define  VT_RECORD_START 1
#define  VT_RECORD_STOP 0
#define  VT_RECORD_CMD_TYPE 1
#define  VT_DEVICEINFO_CMD_TYPE 0



typedef struct NVM_Header
{
    unsigned long       StructSize;	// the size of the user structure below
    unsigned long       NumofStructs;	// >1 in case of array of structs (default is 1).
    char                StructName[64];	// the name of the user structure below
    char                Date[32];	// date updated by the ICAT when the file is saved. Filled by calibration SW.
    char                time[32];	// time updated by the ICAT when the file is saved. Filled by calibration SW.
    char                Version[64];	// user version - this field is updated by the SW eng. Every time they update the UserStruct.
    char                HW_ID[32];	// signifies the board number. Filled by calibration SW.
    char                CalibVersion[32];	// signifies the calibration SW version. Filled by calibration SW.
} NVM_Header_ts;

typedef struct {
    snd_pcm_extplug_t         ext;
    voicepath_cfg_t           parms;
    short                     *buf;		//TX/RX src buffer pointer, must 8 bytes alignment
    short                     *ref;		//TX reference buffer pointer, must 8 bytes alignment
    // srcbuf is used to keep vp process src buffer and stereo interleaved data to vt record plug-in
    short                     srcbuf[VOICEPATH_SAMPLEPERFRAME*2+7]; 
    short                     refbuf[VOICEPATH_SAMPLEPERFRAME+7];
/* running states */
    int                       numofchannel;
    MiscGeneralCallbackTable* pCallbackTable;
    int                       refGainArray[CFG_ECHO128MS_MODE+1];
#ifdef PLUGIN_DEBUG
    int                       totalBytes;
    FILE*                     srcFile;
    FILE*                     refFile;
    FILE*                     srcPostFile;
    int                       enablevp;
#endif
}snd_pcm_ippvp_t;

//global variabls 

typedef struct{
    void*                    pVPState;
    pthread_mutex_t*         pMutexLock;
    int                      aux_mode;
    int                      refCnt;
    int                      pipefd[2];
    int                      hSocketThread;
    int                      sk_record; 
}vp_handle_t;

static vp_handle_t*         g_vpHandle=NULL;

	


int threadCreate(void *phThread, int nPriority, void *pFunc, void *pParam)
{
    pthread_attr_t          attr;
    struct sched_param      param;
    if(!phThread || !pFunc) {
		    return -1;
    }
    if(nPriority) {
        pthread_attr_init(&attr);
        pthread_attr_getschedparam(&attr, &param);
        param.sched_priority=nPriority;
        pthread_attr_setschedparam(&attr, &param);
        return pthread_create((pthread_t*)phThread, &attr, pFunc, pParam);
    } else {
       return pthread_create((pthread_t*)phThread, NULL, pFunc, pParam);
    }
}

int threadDestroy(void *phThread, int bWait)
{
    if((!phThread) || !(*((pthread_t*)phThread)) ) {
        return -1;
    }
    if(bWait) {
        return pthread_join(*((pthread_t*)phThread), NULL);
    } else {
        return -1;
		//remove it because android does not support this func. pthread_cancel(*((pthread_t*)phThread));
    }
}

#define MAX_CLIENT_NUMBER 5

void socket_server_handler()
{
    int sockfd, newsockfd, servlen, clilen, ret;
    struct sockaddr_un  cli_addr, serv_addr;
    int i=0;
    int conn_count=0;
    fd_set fdsr;
    int maxsock;
    int	fd_conn[MAX_CLIENT_NUMBER];
    int dataBuf[2];
    
    LOGD("enter socket_server_handler\n");
    for(i = 0; i < MAX_CLIENT_NUMBER; i++){
        fd_conn[i] = 0;
    }

    
    if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0){
       LOGE("Error--creating socket\n");
       return;
    }
    unlink(DOMAIN_SOCKET_NAME);
    memset(&serv_addr, 0x00, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, DOMAIN_SOCKET_NAME);
    servlen=strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen)<0){
       LOGE("Error--binding socket\n");    
       return;
    }
    listen(sockfd,MAX_CLIENT_NUMBER);
    if( sockfd > g_vpHandle->pipefd[0] )
        maxsock = sockfd;
    else
        maxsock = g_vpHandle->pipefd[0];               
    while (1) {
    	  FD_ZERO(&fdsr);
    	  FD_SET(sockfd,&fdsr);
    	  FD_SET(g_vpHandle->pipefd[0], &fdsr);
    	  for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
            if (fd_conn[i] != 0) {
                FD_SET(fd_conn[i], &fdsr);
            }
        }
        LOGD("begin select.....\n");
    	  ret = select(maxsock+1, &fdsr, NULL, NULL, NULL);
    	  if(ret < 0){
    	      LOGE("Error--select accept socket\n");  
    	      break;
    	  }else if(ret == 0){
    	      LOGW("Warning--select nothing\n");  
    	      continue;
    	  } 
    	   
        if(FD_ISSET(g_vpHandle->pipefd[0], &fdsr)){
            //receive bye msg from leader thread
            LOGD("Receive bye--exit select thread\n");
            break;
        }
        for(i = 0; i <MAX_CLIENT_NUMBER ; i++){
    	      if(FD_ISSET(fd_conn[i], &fdsr)){
    	          ret = recv(fd_conn[i], dataBuf, 8, 0);
    	          if(ret < 0){
    	              LOGW("Warning--recv <0 bytes, socket error, i:%d, error:%d\n", i,errno); 	
    	          }else if(ret == 0){
    	              close(fd_conn[i]);
    	              if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){	
                        if(g_vpHandle->sk_record == fd_conn[i]){
                            g_vpHandle->sk_record = -1;
                        }
                        pthread_mutex_unlock(g_vpHandle->pMutexLock); 
                    }
    	            fd_conn[i] = 0;
    	            conn_count--;
    	            LOGW("Warning--recv 0 bytes, close socket, i:%d\n", i);
    	          }else if(ret == 8){
                    if(dataBuf[0] == VT_DEVICEINFO_CMD_TYPE){
                        if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
                            switch (dataBuf[1]){
                                case 0x01: //earpiece;
                                    g_vpHandle->aux_mode = CFG_ES_MODE;
                                    break;
                                case 0x02: //speaker
                                    g_vpHandle->aux_mode = CFG_SP_MODE;
                                    break;
                                case 0x04: //headset
                                    g_vpHandle->aux_mode = CFG_ES_MODE;
                                    break;
                                default:
                                    break;   
                            }
                            LOGD("audiopath update to: %d\n", g_vpHandle->aux_mode);  
                            if(!g_vpHandle->pVPState)
                                VoicePathSendCmd(g_vpHandle->pVPState, VOICEPATH_CMD_CHANGEAUXAECMODE, &g_vpHandle->aux_mode);
             	              pthread_mutex_unlock(g_vpHandle->pMutexLock); 
             	          }
                    }else if(dataBuf[0] == VT_RECORD_CMD_TYPE){
                        if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){	
                            if(	dataBuf[1] == VT_RECORD_START){
                                g_vpHandle->sk_record = fd_conn[i];
                                LOGD("record socket is connected, i:%d\n",i);
                            }else if(dataBuf[1] == VT_RECORD_STOP){
                                LOGD("record socket is disconnected\n");	
                                g_vpHandle->sk_record = -1;
                            }
                            pthread_mutex_unlock(g_vpHandle->pMutexLock); 
                        }
                    }//other header type can be extended here
                }     	           
    	    }	
    	}
    	   
    	  if(FD_ISSET(sockfd, &fdsr)){
    	      clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
            if (newsockfd < 0){ 
                LOGE("Error--accepting\n");
                continue;
            }
            
            LOGE("accept socket, conn_count:%d",conn_count);
            if(conn_count < MAX_CLIENT_NUMBER){
                for(i = 0; i < MAX_CLIENT_NUMBER; i++){
                    if(fd_conn[i] == 0){
                       fd_conn[i] =  newsockfd;
                       conn_count++;
                        LOGE("put socket to location i:%d",i);
                       break;
                    }
                }
                if(newsockfd > maxsock)
                    maxsock = newsockfd;
            }else{
                LOGE("Error--max connections, exit\n");
                close(newsockfd);
            }
    	  }
    }
    
    for (i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if (fd_conn[i] != 0) {
            close(fd_conn[i]);
        }
    }
    
    close(sockfd);
    unlink(DOMAIN_SOCKET_NAME);
    LOGE("exit socket_server_handler\n");
    
}


int ReadNVMTable(char* nvmFileName, int tableType)
{
    int             fileReadBytes;
    NVM_Header_ts   header;
    FILE*           tableFileID;
    ACM_NSParam     nsParam;
    ACM_EC_AVCParam ecAVCParam;
    int             ecavc_mode;
    NVM_Header_ts*  pHeader = &header;

    LOGE("nvmfilename:%s", nvmFileName);
    tableFileID = fopen(nvmFileName,"rb");
    //if the NVM file do not exist, use default params.
    if(!tableFileID) return IPP_STATUS_NOERR;
    fread(pHeader, 1, sizeof(NVM_Header_ts), tableFileID );
    LOGE("pHeader.StructName:%s, Date:%s\n", pHeader->StructName, pHeader->Date);
    switch(tableType){
        case DEVICE_TABLE:
            break;
        case EQ_TABLE:
            break;
        case NS_TABLE:
        //file format: Rx / Tx/ Default
            fread(&nsParam, 1, sizeof(nsParam), tableFileID );
            VoicePathSetParams(tableType, 0, CFG_RX_PATH, &nsParam);
            fread(&nsParam, 1, sizeof(nsParam), tableFileID );
            VoicePathSetParams(tableType, 0, CFG_TX_PATH, &nsParam);
            break;
        case SIDE_TONE_TABLE:
            break;
        case EC_AVC_TABLE:
            while(1){
                fileReadBytes = fread(&ecAVCParam, 1, sizeof(ecAVCParam), tableFileID );
                if(fileReadBytes < sizeof(ecAVCParam))
                    break;
                if(!strcmp(ecAVCParam.description, "Handset-TX")){
                   LOGE("NVM, ES mode");
                   ecavc_mode = CFG_ES_MODE;
                }else if(!strcmp(ecAVCParam.description, "Handsfree-TX")){ 
                    LOGE("NVM, HF mode");
                    ecavc_mode = CFG_HF_MODE;
                }else{
                    continue;
                }
                //NVM table does not include SP mode parameters, use HF mode parameter instead.
                if(ecavc_mode == CFG_HF_MODE){ 
                    VoicePathSetParams(tableType, CFG_SP_MODE, 0, &ecAVCParam);
                }
                VoicePathSetParams(tableType, ecavc_mode, 0, &ecAVCParam);
            }
            break;
        case EC_FIR_TAPS_SP_TABLE:
            break;
        case CTM_TABLE:
            break;
    }	
    fclose(tableFileID);
    return IPP_STATUS_NOERR;
}


static inline void *area_addr(const snd_pcm_channel_area_t *area,snd_pcm_uframes_t offset)
{
    unsigned int bitofs = area->first + area->step * offset;
    return (char *) area->addr + bitofs / 8;
}


int ippvp_hw_params(snd_pcm_extplug_t *ext, snd_pcm_hw_params_t *params) 
{
    int access;
    int chunk_size;
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;
    if(!snd_pcm_hw_params_get_access(params, &access)){
        if( (access != SND_PCM_ACCESS_MMAP_INTERLEAVED) && (access != SND_PCM_ACCESS_RW_INTERLEAVED )){
              return -EINVAL;
        }
        LOGD("ippvp_hw_params, access is %d", access);
    }else{
        return -EINVAL;
    }
    if(snd_pcm_hw_params_get_channels(params, &(ippvp->numofchannel))){
        return -EINVAL;
    }
    LOGD("ippvp_hw_params, channels:%d", ippvp->numofchannel);
    if(!snd_pcm_hw_params_get_period_size(params, &chunk_size, 0)){
        LOGE("period_size is %d", chunk_size);
        if((chunk_size % VOICEPATH_SAMPLEPERFRAME)){
            return -EINVAL;    
        }
        LOGD("ippvp_hw_params, chunk_size:%d", chunk_size);
    }else{
        return -EINVAL;
    } 
    return 0;
} 
//input maybe mono or stereo(interleave), output is always mono
static snd_pcm_sframes_t ippvprx_transfer(snd_pcm_extplug_t *ext, const snd_pcm_channel_area_t *dst_areas,\
                                          snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t *src_areas,\
                                          snd_pcm_uframes_t src_offset,snd_pcm_uframes_t size)
{
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;
    short *src = area_addr(src_areas, src_offset);
    short *dst = area_addr(dst_areas, dst_offset);
    int i;
    int chunk = 0;
    int auxaec_mode, cmdValue;

#ifdef PLUGIN_DEBUG
    ippvp->totalBytes += size;
#endif	
    while(size >= VOICEPATH_SAMPLEPERFRAME){
        //if src is not 8-byte aligned, or there're remain bytes in ippvp->buf, fill-in the src data to ippvp->buf, 
        //or else, use src directly 
        if(ippvp->numofchannel == 1){
            memcpy(ippvp->buf , src, VOICEPATH_SAMPLEPERFRAME*2);	
        }else{//take left channel of stereo for VP processing
            for(i=0; i<VOICEPATH_SAMPLEPERFRAME; i++){
                ippvp->buf[i] = src[i*2];
            }
        }
       // LOGD("ippvprx_transfer, size:%d", size);
        src += VOICEPATH_SAMPLEPERFRAME*ippvp->numofchannel ;
        size -= VOICEPATH_SAMPLEPERFRAME;		
        if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){   
#ifdef PLUGIN_DEBUG
            fwrite(ippvp->buf, 2, VOICEPATH_SAMPLEPERFRAME, ippvp->refFile);
#endif    
#ifdef PLUGIN_DEBUG
            if(ippvp->enablevp)
#endif                
            VoicePathRx(g_vpHandle->pVPState, ippvp->buf, VOICEPATH_SAMPLEPERFRAME);
#ifdef PLUGIN_DEBUG			
            fwrite(ippvp->buf, 2, VOICEPATH_SAMPLEPERFRAME, ippvp->srcFile);		
#endif          	
            pthread_mutex_unlock(g_vpHandle->pMutexLock); 
        }        
        chunk += VOICEPATH_SAMPLEPERFRAME;
        memcpy(dst, ippvp->buf, VOICEPATH_SAMPLEPERFRAME*2);		
        dst += VOICEPATH_SAMPLEPERFRAME;
        ippvp->buf = (short*)((char*)ippvp->srcbuf + ((8 - ((int)ippvp->srcbuf & 0x7)) % 8));
    }
    if(size > 0){
        LOGE("rx_transfer, size not multiples of 160:%d\n", size);
        if(ippvp->numofchannel == 1){
            memcpy(dst, src, size*2);
        }else{
            for(i=0; i<size; i++){
	              dst[i] = src[i*2];
            }
        }
        chunk += size; 
    }
    return chunk;	
}

static int ippvprx_init(snd_pcm_extplug_t *ext)
{
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;	
    
#ifdef PLUGIN_DEBUG		
    if(!ippvp->srcFile)  ippvp->srcFile = fopen("/data/ippvp/src_rx.pcm", "wb");		
    if(ippvp->srcFile == NULL){
        LOGE("rx_init:open rx src file failed\n");
    }
    if(!ippvp->refFile)  ippvp->refFile = fopen("/data/ippvp/src_rx_ori.pcm", "wb");            

    if(ippvp->refFile == NULL){

        LOGE("rx_init:open rx ref file failed\n");

    }
    ippvp->totalBytes = 0;
#endif	
    LOGD( "ippvprx_init complete g_pVPState:%0x\n", g_vpHandle->pVPState); 

    return 0;
}

static int ippvp_close(snd_pcm_extplug_t *ext)
{
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;
    
    LOGD("enter vp_close,ippvp:%p \n",ippvp);
    if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){  
        g_vpHandle->refCnt--;  
        if(g_vpHandle->refCnt== 0){     
			LOGD("try to free vplib resource");
            if(VoicePathFree(&g_vpHandle->pVPState)){
                LOGE("close:VoicePath Free Error\n");
		            return -1;
            }
            pthread_mutex_unlock(g_vpHandle->pMutexLock);   
		LOGD("free vplib resource--done");
            write(g_vpHandle->pipefd[1], "bye", 4); 
            threadDestroy(&g_vpHandle->hSocketThread,1);  
            close(g_vpHandle->pipefd[0]);
            close(g_vpHandle->pipefd[1]); 
            pthread_mutex_destroy(g_vpHandle->pMutexLock);
            free(g_vpHandle->pMutexLock);
            g_vpHandle->pMutexLock = NULL; 
      
            free(g_vpHandle);
	    g_vpHandle = NULL;	
            LOGD("free vpHandle and close log file\n");
        }else{
            pthread_mutex_unlock(g_vpHandle->pMutexLock);
        } 
    }

    if(ippvp->pCallbackTable != NULL){
        miscFreeGeneralCallbackTable(&ippvp->pCallbackTable);
    } 
#ifdef PLUGIN_DEBUG		
    if(ippvp->srcFile) fclose(ippvp->srcFile);	
    if(ippvp->refFile) fclose(ippvp->refFile);	
	if(ippvp->srcPostFile) fclose(ippvp->srcPostFile);	
#endif

    free(ippvp);   

LOGD("leave vp_close---done \n");
   	    
    return 0;
}

static const snd_pcm_extplug_callback_t ippvprx_callback = {
    .transfer = ippvprx_transfer,
    .init = ippvprx_init,
    .close = ippvp_close,
    .hw_params = ippvp_hw_params,
};

/* Note: 1) if the transfer size < 160, by pass the data directly. buffers the data to 160 
  will impact the time-sequence, possible buffer overrun/underrun happens.
2) the input stereo data is always interleaved format. 
3) input stereo data format: LRLRLRLR.....L: source signal R: ec-path signal
4) output stereo data format: LRLRLRLR....L:source signal with vp tx  processing  R: ec-path signal 
5) socket send stereo data format: LRLRLRLR....L:source signal with vp tx  processinga R:ec-path signal
*/
static snd_pcm_sframes_t ippvptx_transfer(snd_pcm_extplug_t *ext,const snd_pcm_channel_area_t *dst_areas,\
                                          snd_pcm_uframes_t dst_offset,const snd_pcm_channel_area_t *src_areas,\
                                          snd_pcm_uframes_t src_offset,snd_pcm_uframes_t size)
{
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;
    short *src = area_addr(src_areas, src_offset);
    short *dst = area_addr(dst_areas, dst_offset);
    int i, packet_size;
    int chunk = 0;
    int auxaec_mode, cmdValue;
    short *srcL;
    short *srcR;
    int  tmp;

#ifdef PLUGIN_DEBUG
    ippvp->totalBytes += size;
#endif	

    srcL = src;
    srcR = src + 1;
    


    while(size >= VOICEPATH_SAMPLEPERFRAME){
        for(i=0; i<VOICEPATH_SAMPLEPERFRAME; i++){
            ippvp->buf[i] = srcL[2*i];
            tmp = ippvp->refGainArray[g_vpHandle->aux_mode] * srcR[2*i];
            if(tmp > 0x7fff){
                LOGE("ref is greater than max");
                ippvp->ref[i] = 0x7fff;
            }else if(tmp < -0x8000){
               LOGE("ref is less than min");
               ippvp->ref[i] = 0x8000;
            }else{
               ippvp->ref[i] = tmp;
            }
           // ippvp->ref[i] = ippvp->refGainArray[g_vpHandle->aux_mode] * srcR[2*i];
        }
      //  LOGD("ippvptx_transfer, size:%d", size);
        srcL += 2*VOICEPATH_SAMPLEPERFRAME;
        srcR += 2*VOICEPATH_SAMPLEPERFRAME;
        size -= VOICEPATH_SAMPLEPERFRAME;
#ifdef PLUGIN_DEBUG			
      if(ippvp->srcFile) fwrite(ippvp->buf, 2, VOICEPATH_SAMPLEPERFRAME, ippvp->srcFile);
       if(ippvp->refFile) fwrite(ippvp->ref, 2, VOICEPATH_SAMPLEPERFRAME, ippvp->refFile);	
#endif	

        if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){ 
#ifdef PLUGIN_DEBUG
            if(ippvp->enablevp)
#endif            
               VoicePathTx(g_vpHandle->pVPState, ippvp->buf, ippvp->ref, VOICEPATH_SAMPLEPERFRAME);	
            pthread_mutex_unlock(g_vpHandle->pMutexLock); 
        }   
	//	LOGD("des buffer :%p,channel:%d",dst,ippvp->numofchannel );
#ifdef PLUGIN_DEBUG			
        if(ippvp->srcPostFile) fwrite(ippvp->buf, 2, VOICEPATH_SAMPLEPERFRAME, ippvp->srcPostFile);
#endif
        chunk += VOICEPATH_SAMPLEPERFRAME;	
        //support mono and stereo output
        if(ippvp->numofchannel == 1){
            memcpy(dst, ippvp->buf, VOICEPATH_SAMPLEPERFRAME*2); 
        }else if(	ippvp->numofchannel == 2){
            for(i=0; i<VOICEPATH_SAMPLEPERFRAME; i++){
                dst[2*i] = ippvp->buf[i];
                dst[2*i+1] = ippvp->ref[i];
            }
        }
        dst += (VOICEPATH_SAMPLEPERFRAME*ippvp->numofchannel);
    }
    if (size > 0){
        LOGW("tx_transfer, size not multiples of 160:%d\n", size);
        if(ippvp->numofchannel == 1){
            for(i=0; i<size; i++){
                dst[i] = srcL[2*i];
            }    
        }else if(	ippvp->numofchannel == 2){
            memcpy(dst, srcL, size*4);
        }
        	
        chunk += size;
    } 
    
    size = chunk;
    dst = area_addr(dst_areas, dst_offset);
//LOGD("dst add:%p, sk_record:%d",dst, g_vpHandle->sk_record);
    if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){
        if(g_vpHandle->sk_record >= 0){         
            while(size > 0){
                packet_size = (size>VOICEPATH_SAMPLEPERFRAME? VOICEPATH_SAMPLEPERFRAME:size);
                for(i=0; i<packet_size; i++){
                    ippvp->buf[i*2] =  dst[ippvp->numofchannel*(i+1)-1];
                    tmp = ippvp->refGainArray[g_vpHandle->aux_mode] * src[i*2+1];
                    if(tmp > 0x7fff){
                        ippvp->buf[i*2+1] = 0x7fff;
                    }else if(tmp < -0x8000){
                        ippvp->buf[i*2+1] = 0x8000;
                    }else{
                        ippvp->buf[i*2+1] = tmp;
                    }
                }		
                size -= packet_size;
                send(g_vpHandle->sk_record, ippvp->buf, packet_size*4, 0);
            }	
        }
        pthread_mutex_unlock(g_vpHandle->pMutexLock);          
    }
     
    return chunk;		
}

static int ippvptx_init(snd_pcm_extplug_t *ext)
{
    snd_pcm_ippvp_t *ippvp = (snd_pcm_ippvp_t *)ext;	
       
#ifdef PLUGIN_DEBUG		
    if(!ippvp->srcFile)  ippvp->srcFile = fopen("/data/ippvp/src.pcm", "wb");	
    if(!ippvp->refFile)  ippvp->refFile = fopen("/data/ippvp/ref.pcm", "wb");
	 if(!ippvp->srcPostFile)  ippvp->srcPostFile = fopen("/data/ippvp/srcPostFile.pcm", "wb");
    if((ippvp->srcFile == NULL) || (ippvp->refFile == NULL) || (ippvp->srcPostFile == NULL) ){
        LOGE("tx_init:open src, ref file failed\n");
    }
    ippvp->totalBytes = 0;
#endif   
    LOGD("ippvptx_init complete, g_pVPState: %0x\n", g_vpHandle->pVPState); 
    return 0;
}


/*the hw plug-in(default plug-in) always use SND_PCM_ACCESS_MMAP_INTERLEAVED access type,
no matter what kind of access type user sets, this callback is kept for future stereo 
output of tx path.
*/
    
       
     


static const snd_pcm_extplug_callback_t ippvptx_callback = {
    .transfer = ippvptx_transfer,
    .init = ippvptx_init,
    .close = ippvp_close,
    .hw_params = ippvp_hw_params,
};

static int get_bool_parm(snd_config_t *n, const char *id, const char *str,int *val_ret)
{
    int val;
    if (strcmp(id, str))
        return 0;

    val = snd_config_get_bool(n);
    if (val < 0) {
        SNDERR("Invalid value for %s", id);
        return val;
    }
    *val_ret = val;
    return 1;
}

static int get_int_parm(snd_config_t *n, const char *id, const char *str,int *val_ret)
{
    long val;
    int err;
    if (strcmp(id, str))
        return 0;
    err = snd_config_get_integer(n, &val);
    if (err < 0) {
        SNDERR("Invalid value for %s parameter", id);
        return err;
    }
    *val_ret = val;
    return 1;
}

static int get_string_parm(snd_config_t *n, const char *id, const char *str,char *val_ret)
{
    int err;
    char* val;
    if (strcmp(id, str))
        return 0;
    err = snd_config_get_string(n, &val);
    if (err < 0) {
        SNDERR("Invalid value for %s parameter", id);
        return err;
    }
    strcpy(val_ret, val);
    return 1;
}


static int get_float_parm(snd_config_t *n, const char *id, const char *str,float *val_ret)
{
    double val;
    int err;

    if (strcmp(id, str))
        return 0;
    err = snd_config_get_ireal(n, &val);
    if (err < 0) {
        SNDERR("Invalid value for %s", id);
        return err;
    }
    *val_ret = val;
    return 1;
}

SND_PCM_PLUGIN_DEFINE_FUNC(ippvp)
{
    snd_config_iterator_t i, next;
    snd_pcm_ippvp_t          *ippvp;
    snd_config_t             *sconf = NULL;
    int                      err;
    pthread_mutexattr_t      oAttr;
    pthread_mutexattr_t      *pAttr = NULL;

    char nvmFile[256];
    char nvmFileDir[128];
    
    
  
    
    ippvp = calloc(1, sizeof(*ippvp));
    if (!ippvp)
        return -ENOMEM;
    //parser <asound.conf >   
    snd_config_for_each(i, next, conf) {
        snd_config_t *n = snd_config_iterator_entry(i);
        const char *id;
        if (snd_config_get_id(n, &id) < 0)
            continue;
        if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
            continue;
        if (strcmp(id, "slave") == 0) {
            sconf = n;
            continue;
        }
        
        err = get_int_parm(n, id, "refGainSpk", &ippvp->refGainArray[CFG_SP_MODE]);
	if (err)  goto ok;
        err = get_int_parm(n, id, "refGainHeadset", &ippvp->refGainArray[CFG_ES_MODE]);
        if (err)  goto ok;
        err = get_string_parm(n, id, "nvmFileDir", nvmFileDir);
        if (err) goto ok;		
ok:
        if (err < 0){
            free(ippvp);
            LOGE("parser configure parameter error\n");
            return err;	
        }
    }
    if (!sconf) {
        SNDERR("No slave configuration for VP pcm");
        return -EINVAL;
    }
   
    //init ippvp
    ippvp->ext.version = SND_PCM_EXTPLUG_VERSION;
    ippvp->ext.private_data = ippvp;  
    if(stream == SND_PCM_STREAM_PLAYBACK){      
        ippvp->ext.callback = &ippvprx_callback;
        ippvp->ext.name = "IPPVP RX Plugin";
    }else {
        ippvp->ext.callback = &ippvptx_callback;
        ippvp->ext.name = "IPPVP TX Plugin";	
    }        	
    ippvp->buf = (short*)((char*)ippvp->srcbuf + ((8 - ((int)ippvp->srcbuf & 0x7)) % 8));
    ippvp->ref = (short*)((char*)ippvp->refbuf + ((8 - ((int)ippvp->refbuf & 0x7)) % 8));
    ippvp->parms.auxaec_mode = CFG_SP_MODE;     
    ippvp->parms.path = CFG_BOTH_PATH; 
    //Inin misc callback Table     
    if(miscInitGeneralCallbackTable(&ippvp->pCallbackTable)){
        LOGE("misc init error\n");
        return -ENOMEM;
    }
    err = snd_pcm_extplug_create(&ippvp->ext, name, root, sconf,stream, mode);
    if (err < 0) {
        free(ippvp);
        return err;
    }
    
    //set format
    snd_pcm_extplug_set_param_minmax (&ippvp->ext, SND_PCM_EXTPLUG_HW_CHANNELS, 1, 2);
    if(stream == SND_PCM_STREAM_PLAYBACK){
        snd_pcm_extplug_set_slave_param(&ippvp->ext,SND_PCM_EXTPLUG_HW_CHANNELS, 1);
        LOGD("playback channel,ippvp%p");		
    }else{
     //   snd_pcm_extplug_set_param(&ippvp->ext, SND_PCM_EXTPLUG_HW_CHANNELS,1);
        snd_pcm_extplug_set_slave_param(&ippvp->ext,SND_PCM_EXTPLUG_HW_CHANNELS, 2);
	 LOGD("capture channel,ippvp%p");
    }
    snd_pcm_extplug_set_param(&ippvp->ext, SND_PCM_EXTPLUG_HW_FORMAT,  SND_PCM_FORMAT_S16_LE);
    snd_pcm_extplug_set_slave_param(&ippvp->ext, SND_PCM_EXTPLUG_HW_FORMAT, SND_PCM_FORMAT_S16_LE);
        
    *pcmp = ippvp->ext.pcm;
    
#ifdef PLUGIN_DEBUG
    char value[5];
    property_get("ippvp.tx.enable",value, "1");
    ippvp->enablevp = atoi(value);
    LOGE("enable vp:%d", ippvp->enablevp);
#endif    
    
    LOGD("begin init vp processing,g_vpHandle:%p, refGainSpk:%d, refGainHeadset:%d",g_vpHandle,ippvp->refGainArray[CFG_SP_MODE], ippvp->refGainArray[CFG_ES_MODE]);
    //init vp processing    
    if(!g_vpHandle){ 
        g_vpHandle = calloc(1,sizeof(*g_vpHandle));
        if(!g_vpHandle) LOGE("malloc g_vpHandle Error\n");
        g_vpHandle->sk_record = -1;
		LOGD("begincreatemutex ");
        g_vpHandle->pMutexLock = calloc(1, sizeof(*g_vpHandle->pMutexLock));
        if (!pthread_mutexattr_init(&oAttr) && !pthread_mutexattr_settype(&oAttr, PTHREAD_MUTEX_RECURSIVE_NP)) {
            pAttr = &oAttr;
        }
		LOGD("initmutxt");
		
        if (pthread_mutex_init(g_vpHandle->pMutexLock, pAttr)) {
        }
        strcpy(nvmFile, nvmFileDir);
        strcat(nvmFile, NS_TABLE_FILENAME);
		LOGD("readnvm table NS");
        ReadNVMTable(nvmFile, NS_TABLE);
        strcpy(nvmFile, nvmFileDir);
        strcat(nvmFile, EC_AND_AVC_FILENAME);
		LOGD("read nvmtable AVC");
        ReadNVMTable(nvmFile,EC_AVC_TABLE);    
        g_vpHandle->aux_mode = ippvp->parms.auxaec_mode;  
		LOGD("vp init");
        if(VoicePathInit(&ippvp->parms, &g_vpHandle->pVPState, ippvp->pCallbackTable)){
            LOGE("VoicePathInit Error\n");
            pthread_mutex_destroy(g_vpHandle->pMutexLock);
            free(g_vpHandle->pMutexLock);
            free(g_vpHandle);
            return -1;	
        }
    } 	    
    
    if(!pthread_mutex_lock(g_vpHandle->pMutexLock)){  
        g_vpHandle->refCnt++;  
        pthread_mutex_unlock(g_vpHandle->pMutexLock);
    } 
          
    if(!g_vpHandle->hSocketThread){
        //init server endpoint of socket to receive device switch msgs from AudioPolicyManager
        if(pipe(g_vpHandle->pipefd) < 0){
            LOGE("Error--pipe error\n"); 
        }
        threadCreate(&g_vpHandle->hSocketThread, 0, socket_server_handler, NULL);
    }
    
    return 0;  
}

SND_PCM_PLUGIN_SYMBOL(ippvp);
