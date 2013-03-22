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
#include "effectHunter.h"

void printUsage();

void sigQuit(int, siginfo_t *, void *);

int main(int argc, char *argv[])
{
    int     ch;
    long    winId = 0;
    int     sampleCount = 20;
    bool    winIdSet = false;
    int     mode = SAMPLE_FPS;
    
    opterr = 0;
    while ((ch = getopt (argc, argv, "w:c:h:t")) != EOF)
    {
        switch (ch)
        {
            case 'w':
                sscanf(optarg, "%lX", &winId);
                winIdSet = true;
                break;
            case 'c':
                sscanf(optarg, "%d", &sampleCount);
                if (sampleCount <= 0) {
                    printf("SampleCount must be a positive number.\n");
                    printUsage();
                    return 1;
                }
                break;
            case 't':
                mode = SAMPLE_TIMESTAMP;
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

    if (winIdSet) {
        EffectHunter * effectHunter = EffectHunter::genInstance(winId, mode, sampleCount);
        printf(" [Info]: Start sampling ...\n");
        effectHunter->collectFPS();
        delete effectHunter;
        effectHunter = NULL;
    } else {
        printf("Please set winId first.\n");
        printUsage();
    }
    
    return 0;
}

void printUsage()
{
    printf("\n[Usage]: ./effectHunter [-w winId] <-t> <-c sampleCount>\n");
    printf("-w    set the windown id of the widget to be sampled, you can obtain it with xwininfo\n");
    printf("-t    set the sampling mode to be TimeStamp sampling, default mode is FPS sampling\n");
    printf("-c    set the count of sampling, only valid in FPS sampling mode\n");
    printf("______________________________________________________________________________\n");
    printf("effectHunter is a tool used for collecting FPS and TimeStamp info for the movement effect of an active widget.\n\n");
}

void sigQuit(int, siginfo_t *, void *)
{
    EffectHunter::stop();
}
