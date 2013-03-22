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

#ifndef __INCLUDE_GUARD_SYSMON_H__
#define __INCLUDE_GUARD_SYSMON_H__
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
#include <locale.h>
#include <libintl.h>
#include <time.h>
#include <sys/time.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>
#include <signal.h>

#define DEFAULT_SYSMON_LOGFILE "/tmp/sysmon.xml"
#define DEFAULT_SYSMON_CPU_RESULT "/tmp/sysmon_cpu.html"
#define DEFAULT_SYSMON_CPUFREQ_RESULT "/tmp/sysmon_cpufreq.html"
#define DEFAULT_SYSMON_WAKEUP_RESULT "/tmp/sysmon_wakeup.html"
#define DEFAULT_SYSMON_RSS_RESULT "/tmp/sysmon_rss.html"

#define DEFAULT_SYSMON_DURATION 2 	/* Default delay */
#define DEFAULT_SYSMON_FRAME_COUNT 100	/* Default frame count */

#define MAX_PNUM 220		/* MAX process number */
#define MAX_CSTAT 6		/* MAX C State numbe */
#define PNAME_LENGTH 16
#define MAX_PSTAT 6

#define _TRUE 1
#define _FALSE 0
#define _NO_CPUIDLE_ERR 254
#define _NO_ACPI_INFO 253

#define FOR_EACH_FRM(f, h) for (f = h->frames + h->top; f != (h->frames + h->tail); f = h->frames + (f + 1 - h->frames) % h->count)
#define FRMNO(f, h) ((f - h->frames) + h->count - h->top) % h->count
#define FRMLEN(h) (h->tail + h->count - h->top) % h->count

#define NEWFRM(f, h) {\
	f = &h->frames[h->tail];\
	h->tail = (h->tail + 1) % h->count;\
	if (h->tail == h->top)\
		h->top = (h->top + 1) % h->count;\
}

struct sysstat {
	uint64_t cus, cni, csy, cid, ciow;
	uint64_t chi, csi, cstl;
};
typedef struct sysstat sysstat_t;

struct pidstat {
	uint64_t utime, stime, cutime, cstime;
	char pname[PNAME_LENGTH];
};
typedef struct pidstat pidstat_t;

struct pidmem {
	uint64_t rss;
};
typedef struct pidmem pidmem_t;

struct pinfo {
	int32_t pid;
	pidstat_t stat;
	pidmem_t mem;
};
typedef struct pinfo pinfo_t;

struct meminfo {
	uint64_t free, buff, cache;
};
typedef struct meminfo meminfo_t;

struct cpuidle {
	uint64_t usage, duration;
};
typedef struct cpuidle cpuidle_t;

struct cpufreq {
	uint32_t freq; 
	uint64_t cnt;
};
typedef struct cpufreq cpufreq_t;


/* Main proc info */
struct procinfo {
	sysstat_t ss;
	meminfo_t mm;
	cpuidle_t idle[MAX_CSTAT];
	cpufreq_t freq[MAX_PSTAT];
	uint32_t pnum;
	pinfo_t pi[MAX_PNUM];
};
typedef struct procinfo procinfo_t;

struct pdata {
	int32_t pid;
	uint64_t cpu;		/* unit is 1 of USER_HZ */
	uint32_t rss;
	char pname[PNAME_LENGTH];
};
typedef struct pdata pdata_t;

struct sysnode {
	uint64_t clock;
	uint64_t idle_delta;	/* unit is 1 of USER_HZ */
	uint64_t wakeup_delta;
	uint64_t freq_delta[MAX_PSTAT];
};
typedef struct sysnode sysnode_t;

struct pnode {
	uint64_t cpu_delta;	/* unit is 1 of USER_HZ */
	uint32_t rss;
	sysnode_t *s;
};
typedef struct pnode pnode_t;

struct sum_pdata {
	uint64_t start;
	uint64_t cpu_0, cpu_delta, cpu_max;
	uint32_t rss_avg;
	int32_t pid;
	char pname[PNAME_LENGTH];
	GList *tlist;
};
typedef struct sum_pdata sum_pdata_t;

/* Frame info which will be stored */
struct frame {
	uint64_t id, tot /* unit is 1 of USER_HZ (100) */ ;
	uint64_t used, free, buff, cache;	/* unit is kB */
	uint64_t wakeup;	/* accumulative wakeup times */
	uint64_t freq[MAX_PSTAT];
	int16_t pnum;
	pdata_t data[MAX_PNUM];
};
typedef struct frame frame_t;

struct header {
	int delay, count, len;
	int maxcstat;
	int maxpstat;
	int totalmem;
	int cpunum;
	int top, tail;
	struct timeval tv;
	frame_t *frames;
};
typedef struct header header_t;

struct sum_block {
	uint64_t id_0, id_delta;
	sysnode_t *sa;
	uint64_t tot_0, tot_delta;
	uint64_t wakeup_0, wakeup_delta;
	GList *pdlist;
	header_t *h;
};
typedef struct sum_block sum_block_t;


// 0=don't has ** info  1= has ** info
struct flag {
	int cpuidle;
	int cpufreq;
};
typedef struct flag flag_t;

#define _(STRING)    gettext(STRING)

extern int maxcstat;
extern int maxpstat;
extern int enwakeup;
extern int encpufreq;
extern int debug;
extern flag_t sysflag;

/* prototypes */
extern int read_info(procinfo_t * p);
extern void calc_info(procinfo_t * p, frame_t * frm);
extern int store_info(header_t * phead, char *logname);
extern int do_sum(sum_block_t * sbk);
extern int do_sort(sum_block_t * sbk, char *type);
extern int store_result_by_cpu_usage(sum_block_t * sbk, char *fname);
extern int store_result_by_cpu_freq(sum_block_t * sbk, char *fname);
extern int store_result_by_rss(sum_block_t * sbk, char *fname);
extern int store_result_by_wakeup(sum_block_t * sbk, char *fname);

#endif
