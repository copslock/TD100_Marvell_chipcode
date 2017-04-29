/******************************************************************************
*(C) Copyright 2011 Marvell International Ltd.
* All Rights Reserved
******************************************************************************/

#include <ctype.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/capability.h>
#include <linux/prctl.h>
#include <private/android_filesystem_config.h>
#include <sys/reboot.h>
#include <hardware_legacy/power.h>

#define LOG_TAG "MARVELL_TEL_monitor"
#include <cutils/log.h>

#include <EEHandler_nvm.h>
#include <eeh_assert_notify.h>
#include <eeh_ioctl.h>

#define READ_BUF_SIZE   64
#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

static void switchUser()
{
    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    setuid(AID_SYSTEM);

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;
    cap.effective = cap.permitted = (1 << CAP_SETUID);
    cap.inheritable = 0;
    capset(&header, &cap);
}

static pid_t find_pid_by_name(const char* pidName)
{
    DIR *dir;
    struct dirent *next;
    pid_t ret = 0;

    dir = opendir("/proc");
    if (!dir) {
        LOGE("Cannot open /proc");
        exit(1);
    }

    while ((next = readdir(dir)) != NULL) {
        FILE *status;
        char filename[READ_BUF_SIZE];
        char buffer[READ_BUF_SIZE];
        char name[READ_BUF_SIZE];

        /* Must skip ".." since that is outside /proc */
        if (strcmp(next->d_name, "..") == 0)
            continue;

        /* If it isn't a number, we don't want it */
        if (!isdigit(*next->d_name))
            continue;

        sprintf(filename, "/proc/%s/status", next->d_name);
        if (! (status = fopen(filename, "r")) ) {
            continue;
        }
        if (fgets(buffer, READ_BUF_SIZE-1, status) == NULL) {
            fclose(status);
            continue;
        }
        fclose(status);

        /* Buffer should contain a string like "Name:   binary_name" */
        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, pidName) == 0) {
            ret = atoi(next->d_name);
            break;
        }
    }
    closedir(dir);
    return ret;
}

static inline int isProcessExist(pid_t pid)
{
    char filename[READ_BUF_SIZE];
    sprintf(filename, "/proc/%d", pid);
    return access(filename, F_OK) == 0;
}

static int getFinalAction()
{
    NVM_Header_ts header;
    EE_Configuration_t cfg;
    int ret = EE_RESET_COMM_SILENT;
    char path[128];
    const char* nvm_root_dir = getenv("NVM_ROOT_DIR");
    if (!nvm_root_dir) {
        LOGE("failed to find NVM_ROOT_DIR");
        return ret;
    }
    snprintf(path, sizeof(path), "%s/EEHandlerConfig_Linux.nvm", nvm_root_dir);
    FILE* file = fopen(path, "rb");
    if (!file) {
        LOGE("failed to open %s", path);
        return ret;
    }

    size_t count = fread(&header, sizeof(NVM_Header_ts), 1, file);
    LOGD("%s: count: %d, header.StructName:%s\n", __FUNCTION__, count, header.StructName);
    LOGD("%s: header.NumofStructs:%lu, header.Version:%s, header.StructSize:%lu, sizeof(EE_Configuration_t):%d\n",
           __FUNCTION__, header.NumofStructs, header.Version, header.StructSize, sizeof(EE_Configuration_t));
    if (count == 1 && header.NumofStructs == 1 && !strcmp("EE_Configuration_t", header.StructName) && !strcmp("2.0", header.Version) && header.StructSize == sizeof(EE_Configuration_t)) {
        count = fread(&cfg, sizeof(EE_Configuration_t), 1, file);
        LOGI("%s: count: %d", __FUNCTION__, count);
        if (count == 1)
            ret = cfg.finalAction;
    }
    fclose(file);
    LOGI("%s: finalAction: %d", __FUNCTION__, ret);
    return ret;
}

typedef struct process_crash_info {
    const char* name;
    pid_t pid;
    int count;
} process_crash_info_t;

int main()
{
    process_crash_info_t pci[] = {
        {"atcmdsrv", 0, 0},
        {"sm", 0, 0},
        {"audioserver", 0, 0}
    };
    switchUser();

    while (1) {
        sleep(3);
        int crashed = 0;
        unsigned int i;
        for (i = 0; i < NUM_ELEMS(pci); i++) {
            if (pci[i].pid == 0) pci[i].pid = find_pid_by_name(pci[i].name);
            if (!isProcessExist(pci[i].pid)) {
                crashed = 1;
                pci[i].count++;
                char text[128];
                snprintf(text, sizeof(text), "telmon discovered NO.%d process(%s:%d) crashed.", pci[i].count, pci[i].name, pci[i].pid);
                eeh_draw_panic_text(text, strlen(text), (int)EEH_AP_EXCEPTION);
            }
        }
        if (crashed) {
            int finalAction = getFinalAction();
            switch (finalAction) {
            case EE_RESET_COMM_SILENT:
                acquire_wake_lock(PARTIAL_WAKE_LOCK, "telephony-restart");
                setuid(AID_ROOT);
                system("/marvell/tel/exit_composite.sh");
                system("/marvell/tel/run_composite.sh");
                for (i = 0; i < NUM_ELEMS(pci); i++) {
                    pci[i].pid = 0;
                }
                switchUser();
                release_wake_lock("telephony-restart");
                break;
            case EE_RESET_GPIO_RESET:
                setuid(AID_ROOT);
                reboot(RB_AUTOBOOT);
                break;
            default:
                exit(0);
            }
        }
    }
    return 0;
}

