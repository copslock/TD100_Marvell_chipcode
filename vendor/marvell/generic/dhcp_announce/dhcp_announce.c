#define LOG_TAG "DHCP ANNOUNCE"
#include "cutils/log.h"
#include <stdlib.h>

extern int send_dhcp_intent(char * actionDhcp,char * macAddr,char * ipAddr);

int main(int argc, char **argv) {

    int i;
    char *ptr, *event, *time;
    unsigned int seconds;
    char actionDhcp[16]="";

    /* we expect at least <add|old|del> <mac> <ip> from dnsmasq */
    if (argc < 4) {
        LOGE("Didn't get all args from dnsmasq\n");
        return -1;
    }

    /* When dnsmasq starts up, it spits out an event for each of the leases in
     * the lease file.  These can be differentiated from new leases or renewals
     * because their remaining lease time is less than the one hour configured
     * in TetherController.cpp.
     */
    time = getenv("DNSMASQ_TIME_REMAINING");
    if (time == NULL) {
        LOGW("Failed to find remaining lease time\n");
    } else {
        seconds = strtoul(time, NULL, 10);
        if (strcmp(argv[1], "old") == 0 && seconds < 3600) {
            LOGD("Ignoring launch-time lease event for %s:%s\n",
                 argv[2], argv[3]);
            return 0;
        }
    }

    if (strcmp(argv[1], "add") == 0) {
        strcat(actionDhcp, "add");
    } else if (strcmp(argv[1], "del") == 0) {
        strcat(actionDhcp, "del");
    } else if (strcmp(argv[1], "old") == 0) {
        strcat(actionDhcp, "update");
    }

    LOGD("Sending command: event %s , mac %s ,ip %s\n",actionDhcp,argv[2],argv[3]);
    return send_dhcp_intent(actionDhcp,argv[2],argv[3]);
}
