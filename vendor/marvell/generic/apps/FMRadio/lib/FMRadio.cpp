#define LOG_TAG	"FMRadio"

#include "FMRadio.h"
#include <utils/Log.h>


class FMRadio::EventThread : public Thread {
public:

    EventThread(FMRadio& fm): Thread(false), mOwner(fm) {
        
    }

    virtual bool threadLoop() {
        LOGV("FM Radio EventThread start to run!");
        while(1) {
	    if (mOwner.mCount > 0) {
		processEvent();
	    } else {
                usleep(10*1000);
	    }
        }
        return true;
    }

    void processEvent() {
	mOwner.mLock.lock();
	EventEntry entry = mOwner.mEvents[mOwner.mHeader];
        switch (entry.type) {
        case FOUND_CHANNEL: {
            if (mOwner.mFoundChannel_handler) {
                mOwner.mFoundChannel_handler(entry.data);
		LOGE("Event for FOUND_CHANNEL %d", entry.data);
	    } else {
                LOGW("Unhandled Event for FOUND_CHANNEL %d", entry.data);
            }
	    break;
        }
	case GET_RSSI: {
	    if (mOwner.mGetRssi_handler) {
                mOwner.mGetRssi_handler(entry.data);
		LOGW("Event for GET_RSSI %d", entry.data);
	     } else {
                LOGW("Unhandled Event for GET_RSSI %d", entry.data);
            }
            break;
        } 
	case GET_RDS_PS: {
	    if (mOwner.mGetRdsPs_handler) {
                mOwner.mGetRdsPs_handler("CRI");
		LOGW("Event for GET_RDS_PS %d", entry.data);
	    } else {
                LOGW("Unhandled Event for GET_RDS_PS");
            }
	    break;
	}
	case MONO_STEREO_CHANGED: {
            if (mOwner.mMonoStereoChanged_handler) {
                mOwner.mMonoStereoChanged_handler(entry.data);
                LOGW("Event for MONO_STEREO_CHANGED %d", entry.data);
	    } else {
                LOGW("Unhandled Event for MONO_STEREO_CHANGED, %d", entry.data);
            }
            break;
        }
        case SCAN_FINISHED: {
	    mOwner.isScanning = false;
            if (mOwner.mScanFinished_handler) {
                mOwner.mScanFinished_handler();
                LOGE("Event for SCAN_FINISHED");
            } else {
                LOGW("Unhandled Event for SCAN_FINISHED");
            }
            break;
        }
	default:
	    break;
	}    
	mOwner.mCount--;
	mOwner.mHeader = (mOwner.mHeader + 1 ) % MAX_EVENT_SIZE;
	mOwner.mLock.unlock();
    }

    FMRadio& mOwner;
};


Mutex FMRadio::mInstanceLock;
FMRadio* FMRadio::mInstance = NULL;


FMRadio::FMRadio() {
    mFoundChannel_handler = NULL;
    mGetRssi_handler = NULL;
    mGetRdsPs_handler = NULL;
    mMonoStereoChanged_handler = NULL;
    mScanFinished_handler = NULL;
    isEnabled = false;
    isScanning = false;
    mFrequency = 87500;
    mMinFrequency = 87500;
    mMaxFrequency = 108100;
    mCount = 0;
    mHeader = 0;
    mTailer = 0;
    mEventThread = new EventThread(*this);
    mEventThread->run();
} 

FMRadio::~FMRadio() {
}

FMRadio* FMRadio::getInstance() {
    AutoMutex _l(mInstanceLock);
    if (mInstance == NULL) {
        mInstance = new FMRadio();
    }
    return mInstance;
}

int FMRadio::enable() {
    if (isEnabled) {
	return -1;
    } else {
	isEnabled = true;
        sleep(6);
        return 0;
    }
}

int FMRadio::disable() {
    if (isEnabled) {
	isEnabled = false;
        return 0;
    } else {
        sleep(3);
	return -1;
    }
}

int FMRadio::scan_all() {
    if (!isEnabled || isScanning) {
	return -1;
    } else {
	isScanning = true;
	for (unsigned int freq = mMinFrequency; freq <= mMaxFrequency; freq += 100) {
	    printf("Scanning %d ...\n", freq);
	    usleep(10*1000);
	    mFrequency = freq;
	    if (mFrequency % 1000 == 0 ) {
		generateEvent(FOUND_CHANNEL, mFrequency);
	    }

	}
	generateEvent(SCAN_FINISHED, 0);
    }

    return 0;
}

//stop_scan actually not work in stub, since scan_all will not return unless scan finished
int FMRadio::stop_scan() {
    if (!isEnabled || !isScanning) {
	return -1;
    } else {
	isScanning = false;
    }
    return 0;
}

int FMRadio::set_channel(unsigned int freq) {
    if (!isEnabled || freq < mMinFrequency || freq > mMaxFrequency) {
	return -1;
    }
    if (freq % 1000 == 0 ) {
        generateEvent(GET_RSSI, 50);
        if (freq % 2000 == 0) {
            generateEvent(MONO_STEREO_CHANGED, 0);
            generateEvent(GET_RDS_PS, freq);
        } else {
            generateEvent(MONO_STEREO_CHANGED, 1);
        }
    } else {
	generateEvent(GET_RSSI, 0);
    }

    mFrequency = freq;
    return 0;
}

int FMRadio::get_channel() {
    if (!isEnabled) {
	return -1;
    }
    //update mFrequency
    return mFrequency;
}

int FMRadio::get_rssi() {
    if (!isEnabled) {
	return -1;
    }
    return 50;
}

int FMRadio::get_volume() {
    if (!isEnabled) {
        return -1;
    } 
    //update mVolume
    return mVolume;
}

int FMRadio::set_volume(int volume) {
    if (!isEnabled) {
        return -1;
    } 
    mVolume = volume;
    return 0;
}

int FMRadio::set_mute(bool flag) {
    if (!isEnabled) {
        return -1;
    } 
    return 0;
}

int FMRadio::set_band(int band) {
    if (!isEnabled) {
        return -1;
    } 
    return 0;
}

int FMRadio::scan_next() {
    if (!isEnabled || isScanning) {
        return -1;
    } else {
        isScanning = true;
        for (unsigned int freq = mFrequency + 100; freq <= mMaxFrequency; freq += 100) {
            printf("Scanning %d ...\n", freq);
	    usleep(10*1000);
	    mFrequency = freq;
            if (mFrequency % 1000 == 0 ) {
                generateEvent(FOUND_CHANNEL, mFrequency);
                generateEvent(GET_RSSI, 50);
                if (freq % 2000 == 0) {
                    generateEvent(MONO_STEREO_CHANGED, 0);
                    generateEvent(GET_RDS_PS, mFrequency);
                } else {
                    generateEvent(MONO_STEREO_CHANGED, 1);
                }
		break;
            }

        }
        generateEvent(SCAN_FINISHED, 0);
    }

    return 0;
}

int FMRadio::scan_prev(){
    if (!isEnabled || isScanning) {
        return -1;
    } else {
        isScanning = true;
        for (unsigned int freq = mFrequency - 100; freq >= mMinFrequency; freq -= 100) {
            printf("Scanning %d ...\n", freq);
	    usleep(10*1000);
	    mFrequency = freq;
            if (freq % 1000 == 0 ) {
                generateEvent(FOUND_CHANNEL, mFrequency);
                generateEvent(GET_RSSI, 50);
                if (freq % 2000 == 0) {
                    generateEvent(MONO_STEREO_CHANGED, 0);
                    generateEvent(GET_RDS_PS, mFrequency);
                } else {
                    generateEvent(MONO_STEREO_CHANGED, 1);
                }
                break;
            }

        }
        generateEvent(SCAN_FINISHED, 0);
    }

    return 0;
}

int FMRadio::register_FoundChannel_handler(tFoundChannel_handler handler) {
    LOGE("this is register_FoundChannel_handler !");
    mFoundChannel_handler = handler;
    return 0;
}

int FMRadio::register_GetRssi_handler(tGetRssi_handler handler) {
    LOGE("this is register_GetRssi_handler !");
    mGetRssi_handler = handler;
    return 0;
}

int FMRadio::register_GetRdsPs_handler(tGetRdsPs_handler handler) {
    LOGE("this is register_GetRdsPs_handler !");
    mGetRdsPs_handler = handler;
    return 0;
}

int FMRadio::register_MonoStereoChanged_handler(tMonoStereoChanged_handler handler) {
    LOGE("this is register_MonoStereoChanged_handler !");
    mMonoStereoChanged_handler = handler;
    return 0;
}

int FMRadio::register_ScanFinished_handler(tScanFinished_handler handler) {
    LOGE("this is register_ScanFinished_handler !");
    mScanFinished_handler = handler;
    return 0;
}

int FMRadio::generateEvent(EventType type, int data) {
    mLock.lock();
    mEvents[mTailer].type = type;
    mEvents[mTailer].data = data;
    mCount++;
    mTailer = (mTailer + 1) % MAX_EVENT_SIZE;
    mLock.unlock();
    return 0;    
}

int FMRadio::setSpeakerOn(bool){
    return 0;
}
