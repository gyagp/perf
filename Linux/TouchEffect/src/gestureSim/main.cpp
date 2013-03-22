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

#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include "gestureSim.h"


void printUsage();
void sigQuit(int, siginfo_t *, void *);

int main(int argc, char *argv[])
{

    int ch;
    int timeInterval = 50;
    int gestureType = GestureSim::SWIPE;
    int delay = GestureSim::NORMAL_DELAY;
    int orientation = GestureSim::BREADTHWISE;
    long customerPid = 0;

    opterr = 0;
    while ((ch = getopt (argc, argv, "g:t:d:o:p:h")) != EOF)
    {
        switch (ch)
        {
            case 'g':
                sscanf(optarg, "%d", &gestureType);
                if (gestureType<0 || gestureType>2) {
                    printf("[Error]: gesture type must be 0, 1, 2.");
                    printUsage();
                    return 1;
                }
            case 't':
                sscanf(optarg, "%d", &timeInterval);
                if (timeInterval < 0) {
                    printf("[Error]: Time Interval must be positive.\n");
                    printUsage();
                    return 1;
                }
                break;
            case 'd':
                sscanf(optarg, "%d", &delay);
                if (delay < 0) {
                    printf("[Error]: Delay must be positive.\n");
                    printUsage();
                    return 1;
                }
                break;
            case 'p':
                sscanf(optarg, "%ld", &customerPid);
                break;
            case 'o':
                sscanf(optarg, "%d", &orientation);
                if (orientation != 0 && orientation != 1) {
                    printf("[Error]: Orientation must be 0 (Breadthwise) or 1 (Lengthways)\n");
                    printUsage();
                    return 1;
                }
                break;
            case 'h':
                printUsage();
                return 0;
            default:
                printUsage();
                return 1;
        }
    }

    struct sigaction act;
    int sig = 12;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;
    act.sa_sigaction=sigQuit;
    if (sigaction(sig, &act, NULL)<0) {
        printf("[Error]: Failed to install quit signal.\n");
    }

    GestureSim gestureSim(customerPid);
    gestureSim.sim(gestureType, orientation, delay, timeInterval);

    struct timeval timeStamp;
    gestureSim.getFirstEventTimeStamp(&timeStamp); 
    printf(" [Info]: Gesture started at %ld - %ld\n", timeStamp.tv_sec, timeStamp.tv_usec);
    gestureSim.getLastEventTimeStamp(&timeStamp); 
    printf(" [Info]: Gesture ended at   %ld - %ld\n", timeStamp.tv_sec, timeStamp.tv_usec);

    return 0;
}

void printUsage()
{
    printf("[Usage]: ./gestureSim [-g gestureType] [-t timeInterval)] [-d delay] [-o orientation]\n");
    printf("-g    set the gesture type, 0: Swipe, 1: Swiping, 2: Pan, 3: Panning, 4: Pinch. Default is 0.\n");
    printf("-t    set the interval between two gestures. Default is 50 (ms).\n");
    printf("-d    set the delay between movement, it is just the opposite of movement speed. Default is 20 (ms).\n");
    printf("-o    set the orientation of gesture, 0: Breadthwise, 1: Lengthways. Default is 0.\n");
    printf("______________________________________________________________________________\n");
    printf("gestureSim is a tool used for simulating gestures like pan, pinch, swipe.\n\n");
}


void sigQuit(int, siginfo_t *, void *)
{
    GestureSim::stop();
}
