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

#ifndef GENFPS_H
#define GENFPS_H
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/Xdamage.h>
#include <signal.h>
#include <vector>

#define SAMPLE_FPS 0
#define SAMPLE_TIMESTAMP 1

class EffectHunter
{
public:
    static EffectHunter * genInstance(long winId, int mode, int sampleCount);
    ~EffectHunter();
    void collectFPS();

public:
    void processAlarm();
    static void stop();

private:
    EffectHunter(long winId, int mode, int sampleCount);

private:
    long     mWinId;
    int      mScreen;
    Display *   mXWindowDisplay;

    int      mMode;    
    int      mSampleCount;
    int      mFPSCount;
    int      mFPS;
    float    mAvgFPS;
    bool     mTermFlag;
    std::vector<int> mFPSs;
    struct timeval * mLastXDamageEventTimeStamp;
    struct timeval * mFirstXDamageEventTimeStamp;
    
    static   EffectHunter * smObject;
};

#endif
