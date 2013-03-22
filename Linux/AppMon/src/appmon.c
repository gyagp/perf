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
 *    Wang, Jing <jing.j.wang@intel.com>                                                                               
 *    Chen, Guobing <guobing.chen@intel.com>
 */


#define _GNU_SOURCE 
#include "appmon.h"

/* Global variables */
int delay=DEFAULT_APPUSE_DURATION;
int count=DEFAULT_APPUSE_FRAME_COUNT; 
int pids[MAX_APP_NUM + 1]={0,};
int pidCount = 0;
int appnum = 0;
int maxcstate = 0;
int debug = _FALSE;

frame_t *frames=NULL;
header_t watchhead;
flag_t sysflag;

procinfo_t pinfo[2];
unsigned int tog=0;

char *fname=DEFAULT_APPUSE_LOGFILE;
void cleanup(void)
{
    free(frames);
}

void usage()
{
        printf(_("Usage: appmon [OPTION...]\n"));
        printf(_("  -o, --output=File                               Output the app use into specified file\n"));
        printf(_("  -p, --pname=ProcessName1[,ProcessName2...]      Specify target process name list\n"));
        printf(_("  -d, --delay=Second                              Specify delay time. default %d\n"), delay);
        printf(_("  -c, --count=Number                              Specify times, default %d\n"),count);
        printf(_("  -x, --debug                                     Specify to show more info to stdout, default false\n"));
        printf(_("  -h, --help                                      Show this help message\n"));
        exit(0);
}

int appmon_init()
{        
        int size= sizeof(frame_t) + sizeof(appdata_t)*MAX_APP_NUM;
        frames=(frame_t *)malloc(size*count);
        if (!frames) {
                fprintf(stderr,_("Fail to allocate frame_t"));
                return 1;
        }
        FILE *fd;
        char buf[1024];
        char *c;
        int n;
        DIR *cpudir;
        struct dirent *entry;
        maxcstate = 0;
        fd=fopen("/proc/acpi/processor/CPU0/power","r");
        if (!fd){   
            cpudir = opendir("/sys/devices/system/cpu/cpu0/cpuidle");
            if(!cpudir){
                sysflag.acpi_flag = 0;
                maxcstate = 0 ;
            } 
            else {
                while ((entry = readdir(cpudir)) != 0){
                    if (strlen(entry->d_name) < 3 )
                        continue;
                    if (!isdigit(entry->d_name[5]))
                        continue;
                    maxcstate++;
                }
            }
            closedir(cpudir);
        }
        else{
            n=fread(buf, 1, sizeof(buf), fd);
            if (!n)
                    return 1;
            fclose(fd);
            maxcstate=MAX_CSTAT_NUM;
            while (maxcstate>0) {
                    char s[8];
                    sprintf(s,"C%d:",maxcstate);
                    if (strstr(buf, s)!=0)
                            break;
                    maxcstate--;
            }
        }
        fd=fopen("/proc/meminfo","r");
        if (!fd)
                return 1;
        n=fread(buf, 1, sizeof(buf),fd);
        if (!n)
                return 1;
        fclose(fd);
        c = strstr(buf, "MemTotal");
        c+=10; /* 8 + 2 */
        watchhead.totalmem = strtoull(c, NULL, 10);                
           if(debug) 
            printf(_("Basic config: count=%d, delay=%d, maxcstate=C%d\n"), count,delay,maxcstate);        
        watchhead.frames=frames;
        watchhead.maxcstate=maxcstate;
        watchhead.count=count;
        watchhead.delay=delay;        
        if (sysflag.acpi_flag == 1 )
            return 0;
        else    
            return _NO_ACPI_INFO;
}

static int pidof(char * threadName)
{
    int i=0;
    char pid[MAX_PID_LENGTH];

    FILE *fp = NULL;
    char cmdBase[] = "pgrep -d , ";
    char * cmd;
    cmd = (char *)malloc((strlen(cmdBase) + strlen(threadName) + 1)
                         *sizeof(char));
    strcpy(cmd, cmdBase);
    strcat(cmd, threadName);

    fp = popen(cmd, "r");

    while ((pid[i++] = fgetc(fp)) != EOF);
    pid[i-1] ='\0';

    free(cmd);
    pclose(fp);

    char* tid = strtok(pid, ",");
    while(tid != NULL) {
        if (pidCount == MAX_APP_NUM) {
            printf(_("WARNING: Only first %d threads will be sampled, others will be ignore.\n"), pidCount);
            break;
        }
        pids[pidCount++] = atoi(tid);
        tid = strtok(NULL, ",");
    }

    return 0;
}

int main(int argc, char **argv)
{
    setlocale (LC_ALL, "");
    bindtextdomain ("appmon", "/usr/share/locale");
    textdomain ("appmon");

    char * pnames = NULL;
        
    if (argc < 0 && argv[0] == NULL)
        return EXIT_FAILURE; 

    while (1) {
        static struct option opts[] = {
            { "output", 1, NULL, 'o' },
            { "delay", 1, NULL, 'd' },
            { "pnames", 1, NULL, 'p' },
            { "count", 1, NULL, 'c' },
            { "debug", 0, NULL, 'x' },
            { "help", 0, NULL, 'h' },
            { 0, 0, NULL, 0 }
            };
        int index2 = 0, c;

        c = getopt_long(argc, argv, "o:d:p:c:xh", opts, &index2);
        if (c == -1)
           break;
        switch (c) {
            case 'o':
                fname= optarg;
                break;
            case 'p':
                pnames = optarg;
                break;
            case 'd':
                delay= atoi(optarg);
                break;
            case 'c':
                count= atoi(optarg);
                break;
            case 'x':
                debug = _TRUE;
                break;
            case 'h':
                usage();
                break;
            default:;
        }
    }

    sysflag.acpi_flag = 1;
    sysflag.cpuidle_flag = 1;
    count ++;   // to fix the first point has no app date issue

    if (appmon_init() == 1){
        fprintf(stderr, _("appmon_init fail\n"));
        return EXIT_FAILURE; 
    }
    char* pname = strtok(pnames, ",");
    int pnameCount = 0;
    char* pnameList[MAX_APP_NUM];
    while( pname != NULL ) {
        if (pnameCount == MAX_APP_NUM) {
            printf(_("WARNING: Only first %d processes will be sampled, others will be ignore.\n"), pnameCount);
            break;
        }
        pnameList[pnameCount++] = pname;
        pname = strtok(NULL, ",");
    }
       
    int j = 0;
    printf(_("      Start sampling "));
        
    appnum = pidCount;
        
    int i=0;
    int maxAppNum = 0;
    read_info(pids, &pinfo[tog]);
    frame_t *f=frames;
    int cpuidleFlag = 0;

    while (i<count) {        
        sleep(delay);
        tog=!tog;
                
        pidCount = 0;
        for(j=0; j<pnameCount; j++) {
            pidof(pnameList[j]);
        }
        appnum = pidCount;
        if (appnum > maxAppNum) {
            maxAppNum = appnum;
        }

        if ((cpuidleFlag = read_info(pids, &pinfo[tog])) == 1)   
            continue;
        calc_info(pinfo,tog,f);
        f=NEXTFRM(f,MAX_APP_NUM);
        i++;
        if (!debug)
            printf(_("."));
    }
    watchhead.maxAppNum = maxAppNum;
    watchhead.count=(count-1); 
    store_info(&watchhead,fname);
    cleanup();
    if (cpuidleFlag == _NO_CPUIDLE_ERR)
    {
        return _NO_CPUIDLE_ERR;
    }
    return EXIT_SUCCESS;
}
