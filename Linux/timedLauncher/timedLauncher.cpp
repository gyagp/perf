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
#include <cstring>
#include <unistd.h>
#include "pthread.h"
#include "../utils/src/effectHunter/effectHunter.h"
#include "../utils/src/eventHunter/eventHunter.h"

#define GUI_LAUNCH 0
#define CMD_LAUNCH 1

typedef struct{
    char * logFileName;
    char * eventInterface;
} ParamEvent;

typedef struct{
    char * logFileName;
    int  sampleCount;
} ParamEffect;

void printUsage();
int  trim(char * cmdLine); 
void processLog(const char * logLaunchFileName, const char * logRefreshFileName, int mode, int outputLevel, int sampleCount);
void * startEventHunter(void *);
void * startEffectHunter(void *);

int main(int argc, char *argv[])
{
    int     ch;
    int     outputLevel = 0;
    int     sampleCount = 10;
    char    eventInterface[128] = {'\0'};
    char    cmdFile[128] = {'\0'};
    char    cmdLine[256] = {'\0'};
    int     mode = GUI_LAUNCH;

    char    logLaunchFileName[] = "/tmp/timedLauncher.launch.log";
    char    logRefreshFileName[] = "/tmp/timedLauncher.refresh.log";
 
    opterr = 0;
    while ((ch = getopt (argc, argv, "c:t:i:xh")) != EOF)
    {
        switch (ch)
        {
            case 't':
                sscanf(optarg, "%d", &sampleCount);
                if (sampleCount <= 0) {
                    printf("SampleCount must be a positive number.\n");
                    printUsage();
                    return 1;
                }
                break;
            case 'i':
                sscanf(optarg, "%s", eventInterface);
                break;
            case 'c':
                sscanf(optarg, "%s", cmdFile);
                mode = CMD_LAUNCH;
                break;
            case 'x':
                outputLevel = 1;
                break;
            case 'h':
                printUsage();
                return 0;
            default:
                printUsage();
                return 1;
        }
    }

    if (mode == GUI_LAUNCH) {
        if (eventInterface[0]=='\0') {
            printf(" [Error]: Please set input event interface under GUI_LAUNCH mode.\n");
            printUsage();
            return 1;
        }
        printf(" Start sampling under GUI_LAUNCH mode ...\n");
    } else if (mode == CMD_LAUNCH) {
        printf(" Start sampling under CMD_LAUNCH mode ...\n");
    }

    struct timeval launchTS;

    pthread_t ptEffectHunter, ptEventHunter;

    ParamEffect paramEffect;
    paramEffect.logFileName = logRefreshFileName;
    paramEffect.sampleCount = sampleCount;
    int ret = pthread_create(&ptEffectHunter, NULL, startEffectHunter, &paramEffect);
    if (ret != 0) {
        perror(" [Error]: Start effectHunter error");
    }

    if (mode == GUI_LAUNCH) {
        ParamEvent paramEvent;
        paramEvent.logFileName = logLaunchFileName;
        paramEvent.eventInterface = eventInterface;
        ret = pthread_create(&ptEventHunter, NULL, startEventHunter, &paramEvent);
        if (ret != 0) {
            perror(" [Error]: Start eventHunter error");
        } else {
            printf(" Please click on button to launch your application ...\n");
        }
    } else if (mode == CMD_LAUNCH) {
        FILE * cmdFP = fopen(cmdFile, "r");
        if (cmdFP != NULL) {
            while(fgets(cmdLine, sizeof(cmdLine), cmdFP) != NULL) {
                if (trim(cmdLine)!=0) {
                    break;
                }
            }
            fclose(cmdFP);
            if (strlen(cmdLine)==0) {
                printf(" [Error]: Can not read cmd from config file\n");
                return 1;
            }
            strcpy(cmdLine + strlen(cmdLine), " &>/dev/null &");
            printf(" Launch CMD: %s\n", cmdLine);
            setenv("DISPLAY",":0.0", 1);
            system(cmdLine);
            gettimeofday(&launchTS, NULL);
	} else {
            perror(" [Error]: Can not open cmd file for reading");
            return 1;
        }

    }

    if (mode == GUI_LAUNCH) {
        ret = pthread_join(ptEventHunter, NULL);
        if (ret != 0)
        {
            perror(" [Error]: Thread join failed");
            exit(EXIT_FAILURE);
        } else {
            printf(" Click event received ...\n");
        }
    } else if (mode == CMD_LAUNCH) {
        FILE * logLaunch = fopen(logLaunchFileName, "w");
        if ( logLaunch == NULL) {
            perror(" [Error]: Can not open Launch log file for writing");
            exit(EXIT_FAILURE);
        }
        fprintf(logLaunch, " [Info]: Launched CMD at  %ld - %ld\n", launchTS.tv_sec, launchTS.tv_usec);
        fclose(logLaunch);
    }

    printf(" Waiting %d second for sampling the launch process ...\n", sampleCount);
    sleep(sampleCount);
    ret = pthread_kill(ptEffectHunter, SIGUSR1);
    if (ret != 0)
    {
        perror(" [Error]: Thread cancel failed");
        exit(EXIT_FAILURE);
    }
    
    ret = pthread_join(ptEffectHunter, NULL);
    if (ret != 0)
    {
        perror(" [Error]: Thread join failed");
        exit(EXIT_FAILURE);
    }

    processLog(logLaunchFileName, logRefreshFileName, mode, outputLevel, sampleCount);

    return 0;
}

void printUsage()
{
    printf("\n[Usage]: ./timedLauncher [-c CMD_FILE | -i INPUT_EVENT_INTERFACE] <-t timeOut> <-x>\n");
    printf("-t    set the time out for launch time sampling, default mode is 10s.\n");
    printf("-c    set the file containing cmd line to launch application. Turn into GUI click mode if not set.\n");
    printf("-i    set the input event interface for your specified device. E.X.: /dev/input/event6\n");
    printf("-x    output all the raw log.\n");
    printf("-h    output help info.\n");
    printf("______________________________________________________________________________\n");
    printf("timedLauncher is a tool used for sampling the launch time of a GUI-based application.\n\n");
}

int trim(char * cmdLine) 
{
    char *end;
    int len;

    len = strlen(cmdLine);
    while (*cmdLine && len!=0)
    {
        end = cmdLine + len -1;
        if(' '==*end || '\n'==*end || '\t'==*end)
        {
            *end = '\0';
        } else {
            break;
        }
        len = strlen(cmdLine);
    }
    return (len);
}

void processLog(const char * logLaunchFileName, const char * logRefreshFileName, int mode, int outputLevel, int sampleCount)
{
    char line[64] = {'\0'};
    int  ret = 0;

    char event[32] = {'\0'};
    int  is_click = 1;
    long tsLaunchSecond = 0;
    long tsLaunchUSecond = 0;
    long tsRefreshSecond = 0;
    long tsRefreshUSecond = 0;
    
    FILE * launchFP = fopen(logLaunchFileName, "r");
    if (launchFP == NULL) {
        perror("Can not open Launch log File for reading");
        return;
    }
    while(fgets(line, sizeof(line), launchFP) != NULL) {
        if (outputLevel == 1) {
            printf("%s", line);
        }
        if (mode == CMD_LAUNCH) {
            ret = sscanf(line, "%*s %s %*s %*s %ld %*c %ld", event, &tsLaunchSecond, &tsLaunchUSecond);
            if (ret==7 && strcmp(event, "Launched")==0) {
                break;
            }
        } else if (mode == GUI_LAUNCH) {
            ret = sscanf(line, "%*s %s %d %*s %ld %*c %ld", event, &is_click, &tsLaunchSecond, &tsLaunchUSecond);
            if (ret==7 && strcmp(event, "<KEY>")==0 && is_click==0) {
                break;
            }
        }
    } 
    while(fgets(line, sizeof(line), launchFP) != NULL) {
        if (outputLevel == 1) {
            printf("%s", line);
        }
    }
    fclose(launchFP);

    FILE * refreshFP = fopen(logRefreshFileName, "r");
    if (refreshFP == NULL) {
        perror("Can not open Refresh log File for reading");
        return;
    }
    while(fgets(line, sizeof(line), refreshFP) != NULL) {
        if (outputLevel == 1) {
            printf("%s", line);
        }
        ret = sscanf(line, "%*s %s %*s %*s %ld %*c %ld", event, &tsRefreshSecond, &tsRefreshUSecond);
        if (ret==7 && strcmp(event, "Stop")==0) {
            break;
        }
    }
    while(fgets(line, sizeof(line), refreshFP) != NULL) {
        if (outputLevel == 1) {
            printf("%s", line);
        }
    }
    fclose(refreshFP);

    double launchTime = (tsRefreshSecond-tsLaunchSecond) + (double)(tsRefreshUSecond-tsLaunchUSecond)/1000000;

    printf(" ---------------------------\n");
    printf("  Launch Time: %.3lf\n", launchTime);
    if (sampleCount - launchTime < 0.5) {
        printf(" [Warning]: The difference of Launch Time and TimeOut is too small that the data is NOT reliable. \n [Warning]: Please re-sample data with BIGGER TimeOut!\n");
    }

}

void * startEffectHunter(void * param)
{
    ParamEffect * paramX = (ParamEffect *)param;
    EffectHunter * effectHunter = EffectHunter::genInstance(0, SAMPLE_TIMESTAMP, paramX->sampleCount, paramX->logFileName);
    effectHunter->collectFPS();
    delete effectHunter;
    return NULL;
}

void * startEventHunter(void * param)
{
    ParamEvent * paramX = (ParamEvent *)param;
    EventHunter eventHunter(paramX->eventInterface, 0, 1, paramX->logFileName);
    int posX, posY;
    eventHunter.readEvent(posX, posY);
    return NULL;
}

