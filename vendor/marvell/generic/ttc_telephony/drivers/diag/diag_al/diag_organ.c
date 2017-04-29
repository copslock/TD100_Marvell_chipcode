#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include "diag_API.h"
#include "diag_organ.h"
#include "pxa_dbg.h"

#define MAX_SLOPE_OFFSET_SIZE 8000

int Organ2Path(unsigned short paramType, unsigned short device,
    unsigned short streamType, unsigned short format, int* path);
int Path2Organ(int path, unsigned short* paramType,
    unsigned short* device, unsigned short* streamType, unsigned short* format);

/*******************************************************************************
* Function: GetPathsStatus
*******************************************************************************
* Description: Send pathID/state/volume/mute to CTAudio
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_GetPathsStatus
void GetPathsStatus(void *data)
{
    int fd;
    struct sockaddr_un unix_addr;
    int command = GET_VOLUME;
    char * streamStatus;
    char * pathStatus;
    int len, pathID;
    int i;
    int pathNum = sizeof(hifi_mapping) / sizeof(hifiMapping);
    int transResult;
    UNUSEPARAM(data);

    DPRINTF("***** Recv AUDIO_HW_GetPathsStatus...");

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        DPRINTF("***** Fail to create socket!");
        return;
    }

    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, ORGAN_DAEMON_SOCKET);
    len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

    if (connect(fd, (struct sockaddr *)&unix_addr, len) < 0) {
        DPRINTF("***** Fail to connect to server!");
        close(fd);
        return;
    }

    write(fd, &command, sizeof(command));

    len = pathNum * 3;
    streamStatus = (char *)malloc(len);
    if (NULL == streamStatus) {
        DPRINTF("***** Unable to allocate buffer for streamStatus!");
        close(fd);
        return;
    }

    if (read(fd, streamStatus, len) < 0) {
        DPRINTF("***** Fail to read server response!");
        close(fd);
        free(streamStatus);
        return;
    }

    len = ORGANPATHNUMB * 4 + 3;
    pathStatus = (char *)malloc(len);
    if (NULL == pathStatus) {
        DPRINTF("***** Unable to allocate buffer for pathStatus!");
        close(fd);
        free(streamStatus);
        return;
    }

    // Initialize path status
    memset(pathStatus, 0, len);
    for (i = 0; i < ORGANPATHNUMB; i++) {
        pathStatus[i * 4 + 3] = i; //path ID
    }

    // Component number
    pathStatus[0] = 1;
    // Organ type code
    pathStatus[1] = 0x0A;
    // Path number
    pathStatus[2] = (char)pathNum;

    for (i = 0; i < pathNum; i++) {
        transResult = Organ2Path(hifi_mapping[i].paramType,
            hifi_mapping[i].deviceType,
            hifi_mapping[i].streamType,
            hifi_mapping[i].formatType,
            &pathID);
        if (transResult == 0) {
            pathStatus[(pathID * 4) + 3]     = (char)pathID;                               // path ID
            pathStatus[(pathID * 4 + 1) + 3] = (char)(streamStatus[i * 3]);                // state
            if (pathID < RECORDER_PCM_MIC1
                || pathID == VOIP_SPK || pathID == VOIP_CALL_HS
                || pathID == VOICE_CALL_EPC || pathID == VOIP_EPC) {
                //ignore record path (volume is always 0)
                pathStatus[(pathID * 4 + 2) + 3] = (char)(streamStatus[i * 3 + 1]);        // volume
            }
            pathStatus[(pathID * 4 + 3) + 3] = (char)(streamStatus[i * 3 + 2]>0 ? 1 : 0);  // mute
        }
    }

    close(fd);
    free(streamStatus);

    DPRINTF("***** Sending AUDIO_HW_PathsStatus...");

    DIAG_FILTER(Audio, HW, AUDIO_HW_PathsStatus, DIAG_INFORMATION)
    diagStructPrintf("", (void *)pathStatus, len);

    free(pathStatus);
}

/*******************************************************************************
* Function: GetSlopeOffset
*******************************************************************************
* Description: Send current organ volume parameters to CTAudio
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, Organ, GetSlopeOffset
void GetSlopeOffset(void *data)
{
    FILE * fp = NULL;
    NVM_Header_ts header;
    hifiParam param;
    hifiParam paramTemp;
    char * slopeOffset;
    char * pTemp;
    int len, pathID;
    int i, j;
    int flag = 1;
    int transResult;
    int * defaultDBTable;
    char *nvm_root_dir = getenv("NVM_ROOT_DIR");
    char fullpath[MAX_PATH] = { 0 };
    UNUSEPARAM(data);

    DPRINTF("***** Recv Organ_GetSlopeOffset...");

    if(nvm_root_dir != NULL)
    {
	    strcpy(fullpath, nvm_root_dir);
	    strcat(fullpath, "/");
    }
    strcat(fullpath, VOLUME_CALIBRATION_NVM_FILE);

    fp = fopen(fullpath, "rb");
    if (NULL == fp) {
        DPRINTF("***** Unable to open %s!", fullpath);
        flag = 0;
    }

    if (flag && fread(&header, sizeof(NVM_Header_ts), 1, fp) != 1) {
        DPRINTF("***** Incomplete NVM header!");
        fclose(fp);
        return;
    }

    slopeOffset = (char *)malloc(MAX_SLOPE_OFFSET_SIZE);
    if (NULL == slopeOffset) {
        DPRINTF("***** Unable to allocate buffer!");
        if (fp != NULL) {
            fclose(fp);
        }
        return;
    }

    pTemp = slopeOffset;
    len = 0;

    for (i = 0; i < ORGANPATHNUMB; i++) {
        transResult = Path2Organ(i,
                        &param.paramType,
                        &param.deviceType,
                        &param.streamType,
                        &param.formatType);

        if (transResult == 0) {
            *pTemp++ = (unsigned char)i;                                                //path ID
            *pTemp++ = (unsigned char)(MAX_STREAM_VOLUME[param.streamType - 1] + 1);    //gain number
            len += 2;

            // Organ stream type = android stream type + 1
            switch (MAX_STREAM_VOLUME[param.streamType - 1]) {
            case 15:
                defaultDBTable = DEFAULT_DB_VALUE_15;
                break;
            case 7:
                defaultDBTable = DEFAULT_DB_VALUE_7;
                break;
            case 5:
                defaultDBTable = DEFAULT_DB_VALUE_5;
                break;
            default:
                defaultDBTable = DEFAULT_DB_VALUE_1;
                break;
            }

            // Organ stream type = android stream type + 1
            for (j = 0; j <= MAX_STREAM_VOLUME[param.streamType - 1]; j++) {
                *pTemp++ = (char)j;                   //Gain ID
                *pTemp++ = (char)defaultDBTable[j];   //offset
                len += 2;
            }
        }
    }

    if (flag) {
        for (i = 0; i < (int)header.NumofStructs; i++) {
            if (fread(&param, header.StructSize, 1, fp) != 1) {
                DPRINTF("***** Incomplete NVM record!");
                fclose(fp);
                free(slopeOffset);
                return;
            }

            transResult = Organ2Path(param.paramType,
                            param.deviceType,
                            param.streamType,
                            param.formatType,
                            &pathID);
            if (transResult == 0) {
                pTemp = slopeOffset;
                for (j = 0; j < ORGANPATHNUMB; j++) {
                    if (j != pathID) {
                        transResult = Path2Organ(j,
                                        &paramTemp.paramType,
                                        &paramTemp.deviceType,
                                        &paramTemp.streamType,
                                        &paramTemp.formatType);
                        // Organ stream type = android stream type + 1
                        pTemp += ((MAX_STREAM_VOLUME[paramTemp.streamType - 1] + 1) * 2 + 2);
                        continue;
                    }

                    pTemp += 2;
                    *(pTemp + param.volumeIndex * 2 + 1) = (char)param.dBIndex;
                }
            }
        }
    }

    DPRINTF("***** Sending AUDIO_ORGAN_SlopeOffset...");

    DIAG_FILTER(Audio, Organ, AUDIO_ORGAN_SlopeOffset, DIAG_INFORMATION)
    diagStructPrintf("", (void *)slopeOffset, len);

    if (fp != NULL) {
        fclose(fp);
    }
    free(slopeOffset);
}

/*******************************************************************************
* Function: ReadFdiCalibData
*******************************************************************************
* Description: notify AudioFlinger to reload Organ NVM file
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, Organ, ReadFdiCalibData
void ReadFdiCalibData(void *data)
{
    int fd;
    struct sockaddr_un unix_addr;
    int command = RELOAD_NVM;
    int status;
    int len;
    UNUSEPARAM(data);

    DPRINTF("***** Recv Organ_ReadFdiCalibData...");

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        DPRINTF("***** Fail to create socket!");
        return;
    }

    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, ORGAN_DAEMON_SOCKET);
    len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

    if (connect(fd, (struct sockaddr *)&unix_addr, len) < 0) {
        DPRINTF("***** Fail to connect to server!");
        close(fd);
        return;
    }

    write(fd, &command, sizeof(command));
    read(fd, &status, sizeof(status));
    close(fd);

    DPRINTF("***** Sending AUDIO_ORGAN_ReadFdiDataOK...");

    if (STATUS_OK == status) {
        DIAG_FILTER(Audio, Organ, AUDIO_ORGAN_ReadFdiDataOK, DIAG_INFORMATION)
        diagStructPrintf("OK", NULL, 0);
    } else {
        DIAG_FILTER(Audio, Organ, AUDIO_ORGAN_ReadFdiDataERROR, DIAG_INFORMATION)
        diagStructPrintf("ERROR", NULL, 0);
    }
}

/*******************************************************************************
* Function: AUDIO_HW_PathEnable
*******************************************************************************
* Description: enable organ path
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_ORGAN_PathEnable
void PathEnable(void *data)
{
    int fd;
    struct sockaddr_un unix_addr;
    int command = SET_VOLUME;
    int status;
    char streamVolume[2];
    int len;
    hifiParam param;
    int transResult;

    DPRINTF("***** Recv AUDIO_HW_PathEnable...");

    transResult = Path2Organ(((char *)data)[1],
                    &param.paramType,
                    &param.deviceType,
                    &param.streamType,
                    &param.formatType);

    if (transResult < 0 || param.streamType > FM_RADIO) {
        DPRINTF("***** not playback type, ignore...");
        DIAG_FILTER(Audio, HW, AUDIO_HW_PathEnableIgnore, DIAG_INFORMATION)
        diagStructPrintf("OK", NULL, 0);
        return;
    }

    streamVolume[0] = (char)param.streamType;
    streamVolume[1] = ((char *)data)[3];

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        DPRINTF("***** Fail to create socket!");
        return;
    }

    memset(&unix_addr, 0, sizeof(unix_addr));
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, ORGAN_DAEMON_SOCKET);
    len = sizeof(unix_addr.sun_family) + strlen(unix_addr.sun_path);

    if (connect(fd, (struct sockaddr *)&unix_addr, len) < 0) {
        DPRINTF("***** Fail to connect to server!");
        close(fd);
        return;
    }

    write(fd, &command, sizeof(command));
    write(fd, streamVolume, 2);

    read(fd, &status, sizeof(status));
    close(fd);

    DPRINTF("***** Sending AUDIO_HW_PathEnable...");

    if (STATUS_OK == status) {
        DIAG_FILTER(Audio, HW, AUDIO_HW_PathEnableOK, DIAG_INFORMATION)
        diagStructPrintf("OK", NULL, 0);
    } else {
        DIAG_FILTER(Audio, HW, AUDIO_HW_PathEnableSTATUSERROR, DIAG_INFORMATION)
        diagStructPrintf("ERROR", NULL, 0);
    }
}

/*******************************************************************************
* Function: AUDIO_HW_PathDisable
*******************************************************************************
* Description: disable organ path (not used)
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_ORGAN_PathDisable
void PathDisable(void *data)
{
    UNUSEPARAM(data);
    DPRINTF("***** Recv AUDIO_HW_PathDisable...");

    DIAG_FILTER(Audio, HW, AUDIO_HW_PathDisableOK, DIAG_INFORMATION)
    diagStructPrintf("OK", NULL, 0);
}

/*******************************************************************************
* Function: AUDIO_HW_PathMute
*******************************************************************************
* Description: mute organ path (not used)
*
* Parameters: received data
*
* Return value: void
*
* Notes:
*******************************************************************************/
//ICAT EXPORTED FUNCTION - Audio, HW, AUDIO_HW_PathMute
void PathMute(void *data)
{
    DPRINTF("***** Recv AUDIO_HW_PathMute, %d...", ((char *)data)[3]);

    DIAG_FILTER(Audio, HW, AUDIO_HW_PathMuteOK, DIAG_INFORMATION)
    diagStructPrintf("OK", NULL, 0);
}

// Path translation utils (written by PHS / Peter Shao)
int Organ2Path(
    unsigned short paramType,
    unsigned short device,
    unsigned short streamType,
    unsigned short format,
    int* path)
{
    UNUSEPARAM(paramType);

    switch(streamType)
    {
    case MUSIC:
        if(device == BLUETOOTH_DEV)
            device = HEADSET_FORCEDSPEAKER_DEV;
        *path = MUSIC_MP3_SPK+(format-1)+(device-1)*10;
        break;
    case SYSTEM:
        *path = SYSTEM_MP3_SPK+(format-1)+(device-1)*7;
        break;
    case RING:
        *path = RING_MP3_SPK+(format-1)+(device-1)*7;
        break;
    case ALARM:
        *path = ALARM_MP3_SPK+(format-1)+(device-1)*7;
        break;
    case NOTIFICATION:
        *path = NOTIFICATION_MP3_SPK+(format-1)+(device-1)*7;
        break;
    case ENFORCED_AUDIBLE:
        *path = ENFORCED_MP3_SPK+(format-1)+(device-1)*7;
        break;
    case VOICE_CALL:
        if(device == EARPIECE_DEV)
            *path = VOICE_CALL_EPC;
        else
            *path = VOICE_CALL_SPK+(device-1);
        break;
    case BLUETOOTH_SCO:
        *path = BLUETOOTH_SCO_SPK+(device-1);
        break;
    case DTMF:
        *path = DTMF_SPK+(device-1);
        break;
    case TTS:
        *path = TTS_SPK+(device-1);
        break;
    case FM_RADIO:
        *path = FM_RADIO_SPK+(device-1);
        break;
    case VOICE_RECORDER:
        if(format == AAC_FAT)
            format = AMR_NB_FAT+1;
        *path = RECORDER_PCM_MIC1+(format-PCM_FAT)+(device-MIC1_DEV)*3;
        break;
    case CAMCORDER:
        if(format == AAC_FAT)
            format = AMR_NB_FAT+1;
        *path = CAMCORDER_PCM_MIC1+(format-PCM_FAT)+(device-MIC1_DEV)*3;
        break;
    case ASR:
        if(format == AAC_FAT)
            format = AMR_NB_FAT+1;
        *path = ASR_PCM_MIC1+(format-PCM_FAT)+(device-MIC1_DEV)*3;
        break;
    case VOIP:
        if(device == EARPIECE_DEV)
            *path = VOIP_EPC;
        else
            *path = VOIP_SPK+(device-1);
        break;
    case VOIP_RECORDER:
        if(format == AAC_FAT)
            format = AMR_NB_FAT+1;
        *path = VOIP_RECORDER_PCM_MIC1+(format-PCM_FAT)+(device-MIC1_DEV)*3;
        break;
    default:
        return -1;
    }

    return 0;
}

int Path2Organ(
    int path,
    unsigned short* paramType,
    unsigned short* device,
    unsigned short* streamType,
    unsigned short* format)
{
    if(path < SYSTEM_MP3_SPK)
    {
        *paramType = 0;
        *streamType = MUSIC;
        *device = path/10+1;
        if(*device == HEADSET_FORCEDSPEAKER_DEV)
            *device = BLUETOOTH_DEV;
        *format = path%10+1;
    }
    else if(path < RING_MP3_SPK)
    {
        *paramType = 0;
        *streamType = SYSTEM;
        *device = (path-SYSTEM_MP3_SPK)/7+1;
        *format = (path-SYSTEM_MP3_SPK)%7+1;
    }
    else if(path < ALARM_MP3_SPK)
    {
        *paramType = 0;
        *streamType = RING;
        *device = (path-RING_MP3_SPK)/7+1;
        *format = (path-RING_MP3_SPK)%7+1;
    }
    else if(path < NOTIFICATION_MP3_SPK)
    {
        *paramType = 0;
        *streamType = ALARM;
        *device = (path-ALARM_MP3_SPK)/7+1;
        *format = (path-ALARM_MP3_SPK)%7+1;
    }
    else if(path < ENFORCED_MP3_SPK)
    {
        *paramType = 0;
        *streamType = NOTIFICATION;
        *device = (path-NOTIFICATION_MP3_SPK)/7+1;
        *format = (path-NOTIFICATION_MP3_SPK)%7+1;
    }
    else if(path < VOICE_CALL_SPK)
    {
        *paramType = 0;
        *streamType = ENFORCED_AUDIBLE;
        *device = (path-ENFORCED_MP3_SPK)/7+1;
        *format = (path-ENFORCED_MP3_SPK)%7+1;
    }
    else if(path < BLUETOOTH_SCO_SPK)
    {
        *paramType = 0;
        *streamType = VOICE_CALL;
        *device = (path-VOICE_CALL_SPK)+1;
        *format = 0;
    }
    else if(path < DTMF_SPK)
    {
        *paramType = 0;
        *streamType = BLUETOOTH_SCO;
        *device = (path-BLUETOOTH_SCO_SPK)+1;
        *format = 0;
    }
    else if(path < TTS_SPK)
    {
        *paramType = 0;
        *streamType = DTMF;
        *device = (path-DTMF_SPK)+1;
        *format = 0;
    }
    else if(path < FM_RADIO_SPK)
    {
        *paramType = 0;
        *streamType = TTS;
        *device = (path-TTS_SPK)+1;
        *format = 0;
    }
    else if(path < RECORDER_PCM_MIC1)
    {
        *paramType = 0;
        *streamType = FM_RADIO;
        *device = (path-FM_RADIO_SPK)+1;
        *format = 0;
    }
    else if(path < CAMCORDER_PCM_MIC1)
    {
        *paramType = 1;
        *streamType = VOICE_RECORDER;
        *device = (path-RECORDER_PCM_MIC1)/3+MIC1_DEV;
        *format = (path-RECORDER_PCM_MIC1)%3+PCM_FAT;
        if(*format == (AMR_NB_FAT+1))
            *format = AAC_FAT;
    }
    else if(path < ASR_PCM_MIC1)
    {
        *paramType = 1;
        *streamType = CAMCORDER;
        *device = (path-CAMCORDER_PCM_MIC1)/3+MIC1_DEV;
        *format = (path-CAMCORDER_PCM_MIC1)%3+PCM_FAT;
        if(*format == (AMR_NB_FAT+1))
            *format = AAC_FAT;

    }
    else if(path <= ASR_AAC_HSMIC)
    {
        *paramType = 1;
        *streamType = ASR;
        *device = (path-ASR_PCM_MIC1)/3+MIC1_DEV;
        *format = (path-ASR_PCM_MIC1)%3+PCM_FAT;
        if(*format == (AMR_NB_FAT+1))
            *format = AAC_FAT;
    }
	else if(path < VOIP_RECORDER_PCM_MIC1)
	{
		*paramType = 0;
		*streamType = VOIP;
		*device = (path-VOIP_SPK)+1;
		*format = 0;
	}
	else if(path <= VOIP_RECORDER_AAC_HSMIC)
	{
		*paramType = 1;
		*streamType = VOIP_RECORDER;
		*device = (path-VOIP_RECORDER_PCM_MIC1)/3+MIC1_DEV;
		*format = (path-VOIP_RECORDER_PCM_MIC1)%3+PCM_FAT;
		if(*format == (AMR_NB_FAT+1))
			*format = AAC_FAT;
	}
    else if(path <= VOICE_CALL_EPC)
    {
        *paramType = 0;
        *streamType = VOICE_CALL;
        *device = EARPIECE_DEV;
        *format = 0;
    }
    else if(path <= VOIP_EPC)
    {
        *paramType = 0;
        *streamType = VOIP;
        *device = EARPIECE_DEV;
        *format = 0;
    }
    else {
        return -1;
    }

    return 0;
}
