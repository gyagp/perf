/*
 * Copyright (C) 2010 Intel Corporation
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 * Authors:
 *    Chen, Guobing <guobing.chen@intel.com>
 */

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "effectHunter.h"

void sigAlarm(int, siginfo_t *, void *)
{
    EffectHunter::genInstance(0, SAMPLE_FPS, 0, NULL)->processAlarm();
}

void sigStop(int, siginfo_t *, void *)
{
    EffectHunter::stop();
}

EffectHunter * EffectHunter::smObject = NULL;

EffectHunter::EffectHunter(long winId, int mode, int sampleCount, const char * logFileName)
    :mFPSs(),mLog(logFileName)
{
    mWinId = winId;
    mSampleCount = sampleCount;
    mFPSCount = 0;
    mFPS = 0;
    mTermFlag = false;
    mMode = mode;
    mLastXDamageEventTimeStamp = (struct timeval *)malloc(sizeof(struct timeval));
    mFirstXDamageEventTimeStamp = (struct timeval *)malloc(sizeof(struct timeval));
    
    int eventnum = 0, errornum = 0, majornum = 0, minornum = 0;

    mXWindowDisplay = XOpenDisplay(":0.0");

    if (mXWindowDisplay == NULL) {
        std::cerr << "[Error]: Can't Open the Dispaly." << std::endl;
        exit(-1);
    }

    if (mWinId == 0) {
        mWinId = (long) RootWindow(mXWindowDisplay, DefaultScreen(mXWindowDisplay));
    }

    if (!XTestQueryExtension(mXWindowDisplay, &eventnum, &errornum, &majornum, &minornum)) {
        std::cerr << "[Error]: XServer " << DisplayString(mXWindowDisplay) << " doesn't support the XTest extensions!" << std::endl;
        exit(-1);
    }

    XSync(mXWindowDisplay, True);
    mScreen = DefaultScreen(mXWindowDisplay);

    struct sigaction act;
    if (mMode == SAMPLE_FPS) {
        int sig = 14; // SIGALRM
        sigemptyset(&act.sa_mask);
        act.sa_flags=SA_SIGINFO;
        act.sa_sigaction=sigAlarm;
        if (sigaction(sig, &act, NULL)<0) {
            std::cerr <<"[Error]: Failed to install alarm signal." << std::endl;
       }
    } else if (mMode == SAMPLE_TIMESTAMP) {
        int sig = SIGUSR1; // SIGALRM
        sigemptyset(&act.sa_mask);
        act.sa_flags=SA_SIGINFO;
        act.sa_sigaction=sigStop;
        if (sigaction(sig, &act, NULL)<0) {
            std::cerr <<"[Error]: Failed to install stop signal." << std::endl;
       }
    }
}

EffectHunter::~EffectHunter()
{
    //if (mXWindowDisplay!=NULL) {
        //XSync(mXWindowDisplay, False);
        //XCloseDisplay(mXWindowDisplay);
        //mXWindowDisplay = NULL;
    //}
    if (mMode == SAMPLE_TIMESTAMP) {
        if (mLog.is_open()) {
            mLog << " [Info]: Start Refresh at " << mFirstXDamageEventTimeStamp->tv_sec << " - " << mFirstXDamageEventTimeStamp->tv_usec << std::endl;
            mLog << " [Info]: Stop Refresh at  " << mLastXDamageEventTimeStamp->tv_sec << " - " << mLastXDamageEventTimeStamp->tv_usec << std::endl;
        } else {
            std::cout << " [Info]: Start Refresh at " << mFirstXDamageEventTimeStamp->tv_sec << " - " << mFirstXDamageEventTimeStamp->tv_usec << std::endl;
            std::cout << " [Info]: Stop Refresh at  " << mLastXDamageEventTimeStamp->tv_sec << " - " << mLastXDamageEventTimeStamp->tv_usec << std::endl;
        }
    }
    free(mLastXDamageEventTimeStamp);
    free(mFirstXDamageEventTimeStamp);
}

EffectHunter * EffectHunter::genInstance(long winId, int mode, int sampleCount, const char * logFileName)
{
    if (smObject == NULL) {
        smObject = new EffectHunter(winId, mode, sampleCount, logFileName);
    }
    return smObject;
}

void EffectHunter::collectFPS()
{
    long    fpsSum = 0;
    bool    firstDamageFlag = true;

    XDamageCreate(mXWindowDisplay, mWinId, XDamageReportRawRectangles);

    int damageEvt, damageErr;
    if (!XDamageQueryExtension (mXWindowDisplay, &damageEvt, &damageErr)){
        std::cerr << "not support damage\n";
        exit(-1);
    }
    
    XEvent e;

    if (mMode == SAMPLE_FPS) {
        struct itimerval itVal;
        itVal.it_interval.tv_sec = 1;
        itVal.it_interval.tv_usec = 0;
        itVal.it_value.tv_sec = 1;
        itVal.it_value.tv_usec = 0;
        setitimer(ITIMER_REAL, &itVal, NULL);

        while (!mTermFlag) {
            XNextEvent (mXWindowDisplay, &e);
            if (e.type == (damageEvt + XDamageNotify)) {
                mFPS++;
            }
        }

        itVal.it_interval.tv_sec = 0;
        itVal.it_value.tv_sec = 0;
        setitimer(ITIMER_REAL, &itVal, NULL);;   
 
        for(int i=0; i<mSampleCount; i++) {
            fpsSum += mFPSs[i];
        }
        std::cout << " [Info]: Average FPS - " << (double)fpsSum/mSampleCount << std::endl;
    } else if (mMode == SAMPLE_TIMESTAMP) {
        while (1) {
            XNextEvent (mXWindowDisplay, &e);
            if (e.type == (damageEvt + XDamageNotify)) {
                gettimeofday(mLastXDamageEventTimeStamp, NULL);
                if (firstDamageFlag) {
                    mFirstXDamageEventTimeStamp->tv_sec = mLastXDamageEventTimeStamp->tv_sec;
                    mFirstXDamageEventTimeStamp->tv_usec = mLastXDamageEventTimeStamp->tv_usec;
                    firstDamageFlag = false;
                }
            }
        }
    }
}

void EffectHunter::processAlarm()
{
    if (mMode == SAMPLE_FPS) {
    	mFPSs.push_back(mFPS);
    	std::cout << " [Info]: " << mFPS << " - round " << mFPSCount ++ << std::endl; 
    	mFPS = 0;
        if (mFPSCount == mSampleCount) {
            mTermFlag = true;
        }
    } else if (mMode == SAMPLE_TIMESTAMP) {
    }
}

void EffectHunter::stop()
{
    delete smObject;
    pthread_exit(NULL);
}
