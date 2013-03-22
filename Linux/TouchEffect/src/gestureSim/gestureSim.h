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

#ifndef PANSIM_H
#define PANSIM_H

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <sys/time.h>

class GestureSim
{
public:
    GestureSim(pid_t customerPid);
    ~GestureSim();
    void sim(int gestureType, int orientation, int delay, int timeInterval);
    void getFirstEventTimeStamp(struct timeval * timeStamp);
    void getLastEventTimeStamp(struct timeval * timeStamp);
    static void stop();

    static const int   QUICK_DELAY;
    static const int   NORMAL_DELAY;
    static const int   SLOW_DELAY;
    static const int   BREADTHWISE;
    static const int   LENGTHWAYS;
    static const int   SWIPE;
    static const int   SWIPING;
    static const int   PAN;

private:
    Display *   mXWindowDisplay;
    int         mScreen;
    bool        mFirstEventFlag;
    pid_t       mCustomerPid;
    struct timeval *    mFirstEventTimeStamp;
    struct timeval *    mLastEventTimeStamp;
    static bool		mQuitFlag;
    void panAndSwipe(bool isSwipe, int & posX, int & posY, int direction, int delay);
    void swiping(int & posX, int & posY, int orientation, int delay, struct timespec * tsInterval);
};

#endif
