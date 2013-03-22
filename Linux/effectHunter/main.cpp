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
#include <cstdlib>
#include <unistd.h>
#include "effectHunter.h"

typedef struct{
    int mode;
    int sampleCount;
} ParamEffect;

void printUsage();
void * startEffectHunter(void *);

int main(int argc, char *argv[])
{
    int     ch;
    long    winId = 0;
    int     sampleCount = 20;
    int     mode = SAMPLE_FPS;
    
    opterr = 0;
    while ((ch = getopt (argc, argv, "w:c:h:t")) != EOF)
    {
        switch (ch)
        {
            case 'w':
                sscanf(optarg, "%lX", &winId);
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

    pthread_t ptEffectHunter;

    ParamEffect paramEffect;
    paramEffect.sampleCount = sampleCount;
    paramEffect.mode = mode;
    int ret = pthread_create(&ptEffectHunter, NULL, startEffectHunter, &paramEffect);
    if (ret != 0) {
        perror("Start effectHunter error:");
    }

    if (mode == SAMPLE_TIMESTAMP) {
        sleep(sampleCount);
        ret = pthread_kill(ptEffectHunter, SIGUSR1);
        if (ret != 0)
        {
            perror("Thread cancel failed");
            exit(EXIT_FAILURE);
        }
    }

    ret = pthread_join(ptEffectHunter, NULL);
    if (ret != 0)
    {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void printUsage()
{
    printf("\n[Usage]: ./effectHunter <-w winId> <-t> <-c sampleCount>\n");
    printf("-w    set the window id of the widget to be sampled, you can obtain it with xwininfo, default is root window id.\n");
    printf("-t    set the sampling mode to be TimeStamp sampling, default mode is FPS sampling.\n");
    printf("-c    set the count of sampling, also means the TimeOut (s) in TimeStamp sampling mode, default is 20.\n");
    printf("______________________________________________________________________________\n");
    printf("effectHunter is a tool used for collecting FPS and TimeStamp info for the movement effect of an active widget.\n\n");
}

void * startEffectHunter(void * param)
{
    ParamEffect * paramX = (ParamEffect *)param;
    EffectHunter * effectHunter = EffectHunter::genInstance(0, paramX->mode, paramX->sampleCount, NULL);
    effectHunter->collectFPS();
    delete effectHunter;
    return NULL;
}

