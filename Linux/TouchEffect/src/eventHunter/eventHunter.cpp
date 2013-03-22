#include "eventHunter.h"
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>

EventHunter::EventHunter(const char * eventInterface, int logLevel):mLogLevel(logLevel),mEventInterface(eventInterface)
{
    mEventFD = open(mEventInterface, O_RDWR); 
    if(mEventFD <= 0){
        std::cerr << "Fail to open input event interface." << std::endl;
    }

}

EventHunter::~EventHunter()
{
    close(mEventFD);
}

void EventHunter::simMoveREL(int posX, int posY)
{
    static struct input_event mEvent, mEmptyEvent;
    mEvent.type = EV_REL;

    mEvent.code = REL_X;
    mEvent.value = posX;
    gettimeofday(&mEvent.time, NULL);
    if (write(mEventFD,&mEvent,sizeof(mEvent))!=sizeof(mEvent)) {
        std::cerr << "X pos write error." << std::endl;
    }

    mEvent.code = REL_Y;
    mEvent.value = posY;
    gettimeofday(&mEvent.time, NULL);
    if (write(mEventFD,&mEvent,sizeof(mEvent))!=sizeof(mEvent)) {
        std::cerr << "Y pos write error." << std::endl;
    }
   
    write(mEventFD,&mEmptyEvent,sizeof(mEmptyEvent));
}

void EventHunter::readEvent(int & posX, int & posY)
{
    struct input_event ev;
    struct timeval timeStamp;

    bool tsFlag = false;
    
    while (1) {
        if(read(mEventFD, &ev, sizeof(struct input_event)) <= 0) {
            std::cerr << "Event read error." << std::endl;
            return;
        }

        timeStamp.tv_sec = ev.time.tv_sec;
        timeStamp.tv_usec = ev.time.tv_usec;
        if (ev.type == EV_REL) {
            switch(ev.code) {
                case REL_X:
                    if (2==mLogLevel||tsFlag) {
                        log("REL x", ev);
                        tsFlag = false;
                    }
                    break;
                case REL_Y:
                    if (2==mLogLevel||tsFlag) {
                        log("REL y", ev);
                        tsFlag = false;
                    }
                    break;
                case REL_Z:
                    if (2==mLogLevel||tsFlag) {
                        log("REL z", ev);
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("REL ERROR", ev);
                        tsFlag = false;
                    }
                    break;
            }
        } else if (ev.type == EV_ABS) {
            switch(ev.code) {
                case ABS_X:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS x", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_Y:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS y", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_Z:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS z", ev);
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS ERROR", ev);
                        tsFlag = false;
                    }
                    break;
            }
        } else if (ev.type == EV_KEY) {
            switch(ev.code) {
                case BTN_LEFT:
                    log("KEY", ev);
                    if (1==ev.value) {
                        tsFlag = true;
                    } else if (0==ev.value) {
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("KEY", ev);
                        break;
                    }
            }
        } else if (ev.type == EV_SYN) {
            switch(ev.code) {
                default:
                    //if (2==mLogLevel||tsFlag) {
                        log("SYN", ev);
                    //}
                    break;
            }
        } else {
            if (2==mLogLevel||tsFlag) {
                log("ELSE", ev);
            }
        }
    }
}

void EventHunter::simKeyEvent(int key, int value)
{
    static struct input_event mEvent, mEmptyEvent;
    mEvent.type = EV_KEY;

    mEvent.code = key;
    mEvent.value = value;
    gettimeofday(&mEvent.time, NULL);
    if (write(mEventFD,&mEvent,sizeof(mEvent))!=sizeof(mEvent)) {
        std::cerr << "key write error." << std::endl;
    }

    if (value == 1) { // Only log the press event
        std::cout << "Sent Event at: " << mEvent.time.tv_sec << " - " << mEvent.time.tv_usec << std::endl;
    }
    write(mEventFD,&mEmptyEvent,sizeof(mEmptyEvent));
}

inline void EventHunter::log(std::string pre, struct input_event & ev)
{
    std::cout << "[Event]: <" << pre << "> " << ev.value << " at " << ev.time.tv_sec << " - " << ev.time.tv_usec << std::endl;
}
