#include <stdio.h>
#include <utils/Log.h>
#include "FMRadio.h"
using namespace std;

FMRadio* fm = NULL;

int usage() {
    printf(
    "|---------------------FM Radio Command list---------------------\n"
    "|enable      enable FM radio                                    |\n"
    "|disable     disable FM radio                                   |\n"
    "|getch       get current channel (in KHz)                       |\n"
    "|band n      set new band                                       |\n"
    "|setch n     tune to channel n (in KHz)                         |\n"
    "|rssi        get current rssi                                   |\n"
    "|getvo       get current volume                                 |\n"
    "|setvo       set a new volume                                   |\n"
    "|mute        mute fm audio                                      |\n"
    "|unmute      unmute fm audio                                    |\n"
    "|scan        scan all available channels                        |\n"
    "|next        scan next available channel                        |\n"
    "|prev        scan previous available                            |\n"
    "|exit        exit FM Radio test                                 |\n"
    "|help        show this help                                     |\n"
    "|---------------------------------------------------------------|\n\n");
    return 0;
}

void onFoundChannel(int freq) {
    printf("##Found Channel %d\n", freq);
    return;
}

void onGetRssi(int rssi) {
    printf("##Get Rssi %d\n", rssi);
    return;
}

void onGetRdsPs(const char* name) {
    printf("##Get RDS PS %s\n", name);
    return;
}

void onMonoStereoChanged(int monoStereo) {
    if (monoStereo == 0) {
	printf("##Using Mono\n");
    } else {
	printf("##Using Stereo\n");
    }
    return;
}

void onScanFinished() {
    printf("##Scan Finished\n");
    return;
}

bool processCommand() {
    printf("FMRadio>");
    char words[80];
    fgets(words, 80, stdin);

    if (0 == strncmp(words, "enable", 6)) {
	int rtn = fm->enable();	
	if (rtn == 0) {
	    printf("FM Radio is enabled\n");
	} else {
	    printf("failed\n");
	}
	return true;
    }
    
    if (0 == strncmp(words, "disable", 7)) {
	int rtn = fm->disable();
	if (rtn == 0) {
	    printf("FM Radio is disabled\n");
	} else {
	    printf("failed\n");
	}
	return true;
    }

    if (0 == strncmp(words, "getch", 5)) {
	int ch = fm->get_channel();
	printf("Current channel is %d KHz\n", ch);
	return true;
    }

    if (0 == strncmp(words, "setch", 5)) {
	int ch = atoi(words + 6);
	int rtn = fm->set_channel(ch);
	if (0 == rtn) {
	    printf("Set Channel %d KHz success\n", ch);
	} else {
	    printf("failed\n");
	}
	return true;
    }

    if (0 == strncmp(words, "rssi", 4)) {
        int rssi = fm->get_rssi();
	printf("Get Rssi %d\n", rssi);
	return true;
    }

    if (0 == strncmp(words, "getvo", 5)) {
	int vo = fm->get_volume();
	printf("Get Volume %d\n", vo);
	return true;
    }

    if (0 == strncmp(words, "setvo", 5)) {
	int vo = atoi(words + 6);
	int rtn = fm->set_volume(vo);
	if (0 == rtn) {
	    printf("Set Volume %d success\n", vo);
	} else {
	    printf("failed\n");
	}
	return true;
    }

    if (0 == strncmp(words, "mute", 4)) {
	fm->set_mute(true);
	return true;
    }

    if (0 == strncmp(words, "unmute", 6)) {
	fm->set_mute(false);
	return true;
    }

    if (0 == strncmp(words, "band", 4)) {
	int bd = atoi(words + 8);
	fm->set_band(bd);
	return true;
    }

    if (0 == strncmp(words, "next", 4)) {
	fm->scan_next();
	return true;
    }

    if (0 == strncmp(words, "prev", 4)) {
	fm->scan_prev();
	return true;
    }

    if (0 == strncmp(words, "scan", 4)) {
	fm->scan_all();
	return true;
    }

    if (0 == strncmp(words, "exit", 4)) {
        return false;
    } 
    
    usage();
    return true;
    
}

int main(int argc, const char** argv) {
    fm = FMRadio::getInstance();
    fm->register_FoundChannel_handler(onFoundChannel);
    fm->register_GetRssi_handler(onGetRssi);
    fm->register_GetRdsPs_handler(onGetRdsPs);
    fm->register_MonoStereoChanged_handler(onMonoStereoChanged);
    fm->register_ScanFinished_handler(onScanFinished);
    bool running = true;
    do {
	running = processCommand();
    } while (running);
    return 0;
}
