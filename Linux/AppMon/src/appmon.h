/*
 * Copyright (C) 2010 Intel Corporation.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.

 * Authors:
 *    Jing, Wang <jing.j.wang@intel.com>                                                                               
 *    Chen, Guobing <guobing.chen@intel.com>
 */

#ifndef __INCLUDE_GUARD_APPUSE_H__
#define __INCLUDE_GUARD_APPUSE_H__
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/types.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <glib.h>
#include <locale.h>
#include <libintl.h>

#define DEFAULT_APPUSE_LOGFILE "appmon.log"
#define DEFAULT_APPUSE_DURATION 5 /* Default delay */
#define DEFAULT_APPUSE_FRAME_COUNT 20 /* Default frame count */
#define MAX_APP_NUM 8 /* MAX apps number*/
#define MAX_CSTAT_NUM 8 /* MAX C State numbe */
#define NEXTFRM(f, appnum) (frame_t *)((char *)(f+1)+sizeof(appdata_t)*(appnum))
#define MAX_PID_LENGTH 32
#define APP_NAME_LENGTH 32
#define _TRUE 1
#define _FALSE 0
#define _NO_CPUIDLE_ERR 254
#define _NO_ACPI_INFO 253

struct sysstat {
	uint64_t cus,cni,csy,cid,ciow;
	uint64_t chi, csi, cstl;
};
typedef struct sysstat sysstat_t;

struct pidstat {
	uint64_t utime,stime,cutime,cstime;
	int32_t pid;	
	char pname[APP_NAME_LENGTH];
};
typedef struct pidstat pidstat_t;

struct pidmem {
	uint64_t rss; 	
};
typedef struct pidmem pidmem_t;

struct meminfo {
	uint64_t free, buff,cache;
};
typedef struct meminfo meminfo_t;

struct cpuidle {
	uint64_t usage, duration;
};
typedef struct cpuidle cpuidle_t;

/* Main proc info */
struct procinfo {
	sysstat_t ss;
	meminfo_t mm;
	cpuidle_t idle[MAX_CSTAT_NUM];
	pidstat_t ps[MAX_APP_NUM];
	pidmem_t pm[MAX_APP_NUM];
};
typedef struct procinfo procinfo_t;

struct appdata {
	uint64_t cpu,rss; /* unit is 1 of USER_HZ (100) */
	int32_t	pid;
	char appname[APP_NAME_LENGTH]; 
};
typedef struct appdata appdata_t;

/* Frame info which will be stored */
struct frame {
	uint64_t 	id,tot /* unit is 1 of USER_HZ (100) */;
	uint64_t	used, free, buff, cache ; /* unit is kB */
	uint64_t 	wakeup_persec; /* unit is microsecond */
	uint64_t 	c_time[MAX_CSTAT_NUM]; /* unit is microsecond */
	int16_t		appnum;
	appdata_t data[0];
};
typedef struct frame frame_t;

struct header {
	//int appnum;
	//char appnames[MAX_APP_NUM][128];
	int delay,count;
	int maxcstate;
	int totalmem;
	int maxAppNum;
	frame_t *frames;
};
typedef struct header header_t;

// 0=don't has ** info  1= has ** info
struct flag{
	int cpuidle_flag;
	int acpi_flag;	
};
typedef struct flag flag_t;

#define _(STRING)    gettext(STRING)

//extern int count;
extern int delay;
//extern int pid;
extern int appnum;
extern int maxcstate;
extern int debug;
extern flag_t sysflag;

/* prototypes */

extern int read_info(int *pids, procinfo_t *p);
extern void calc_info(procinfo_t *infos, unsigned int tog, frame_t *frm);
extern int store_info(header_t *phead, char *fname);

#endif
