#ifndef FMRADIO_H
#define FMRADIO_H

#include <utils/threads.h>
using namespace android;
class FMRadio {
public:
    static FMRadio* getInstance();
    int enable();
    int disable();
    int scan_all();
    int stop_scan();
    int set_channel(unsigned int freq);
    int get_channel();
    int get_rssi();
    int get_volume();
    int set_volume(int volume);
    int set_mute(bool flag);
    int set_band(int band);
    int scan_next();
    int scan_prev();
    int setSpeakerOn(bool on);
    typedef void (*tFoundChannel_handler)(int freq);
    typedef void (*tGetRssi_handler)(int rssi);
    typedef void (*tGetRdsPs_handler)(const char* name);
    typedef void (*tMonoStereoChanged_handler)(int monoStereo);
    typedef void (*tScanFinished_handler)();
    tFoundChannel_handler mFoundChannel_handler;
    tGetRssi_handler mGetRssi_handler;
    tGetRdsPs_handler mGetRdsPs_handler;
    tMonoStereoChanged_handler mMonoStereoChanged_handler;
    tScanFinished_handler mScanFinished_handler;
    int register_FoundChannel_handler(tFoundChannel_handler handler);
    int register_GetRssi_handler(tGetRssi_handler handler);
    int register_GetRdsPs_handler(tGetRdsPs_handler handler);
    int register_MonoStereoChanged_handler(tMonoStereoChanged_handler handler);
    int register_ScanFinished_handler(tScanFinished_handler handler);

private:
    FMRadio();
    ~FMRadio();
    bool isEnabled;
    bool isScanning;
    unsigned int mFrequency;
    unsigned int mMinFrequency;
    unsigned int mMaxFrequency;
    int mVolume;
    mutable Mutex mLock;
    static Mutex mInstanceLock;
    static FMRadio* mInstance;
    class EventThread;
    sp<Thread>  mEventThread;
    enum EventType {
	FOUND_CHANNEL = 0,
	GET_RSSI = 1,
	GET_RDS_PS = 2,
	MONO_STEREO_CHANGED = 3,
	SCAN_FINISHED = 4
    };

    struct EventEntry {
	EventType type;
	int data;
    };
    
    enum {
	MAX_EVENT_SIZE = 256
    };
    EventEntry	mEvents[MAX_EVENT_SIZE];
    int mHeader;
    int mTailer;
    int mCount;
    int generateEvent(EventType type, int data);
};

#endif // FMRADIO_H

