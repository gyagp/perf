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
#include "sysmon.h"

/* Global variables */
int delay = DEFAULT_SYSMON_DURATION;
int count = DEFAULT_SYSMON_FRAME_COUNT;
int maxcstat = 0;
int maxpstat = 0;
int enwakeup = _FALSE;
int encpufreq = _FALSE;
int debug = _FALSE;
int loop = _FALSE;

header_t hdr;

sum_block_t bk;
flag_t sysflag;

procinfo_t sysinfo;

struct sigaction act;
int stopflag = 0;

char *logname = DEFAULT_SYSMON_LOGFILE;

void cleanup(void)
{
	GList *l = bk.pdlist;
	if (bk.sa)
		free(bk.sa);
	while (l) {
		sum_pdata_t *spd = l->data;
		GList *t = spd->tlist;
		while (t) {
			free(t->data);
			t = g_list_next(t);
		}
		g_list_free(spd->tlist);
		free(spd);
		l = g_list_next(l);
	}
	g_list_free(bk.pdlist);
	if (bk.h->frames)
		free(bk.h->frames);
}

void usage()
{
	printf(_("Usage: sysmon [OPTION...]\n"));
	printf(_
	       ("  -o, --log=File                               Output the app use into specified file\n"));
	printf(_
	       ("  -d, --delay=Second                              Specify delay time. default %d\n"),
	       delay);
	printf(_
	       ("  -c, --count=Number                              Specify times, default %d\n"),
	       count);
	printf(_
	       ("  -w, --wakeup                              	   Enable wakeup\n"));
	printf(_
	       ("  -f, --cpufreq                          	   Enable cpufreq\n"));
	printf(_
	       ("  -x, --debug                                     Specify to show more info to stdout, default false\n"));
	printf(_
	       ("  -h, --help                                      Show this help message\n"));

	exit(0);
}

void sighandler (int signum, siginfo_t *info, void *ptr) {
	printf("Received signal %d\n", signum);
	printf("Signal come from process %u\n", info->si_pid);
	stopflag = 1;	
}


int sysmon_init(header_t *head)
{
	memset(&act, 0 , sizeof(act));
	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &act, NULL);

	int size = sizeof(frame_t);

	frame_t *frames = (frame_t *) malloc(size * count);
	if (!frames) {
		fprintf(stderr, _("Fail to allocate frame_t"));
		return 1;
	}
	printf("\tPre allocate %u B memory, One frame include %u B\n", size * count, size);

	FILE *fd;
	char buf[1024], *c;
	maxcstat = 0;

	/* cpuidle flag check */
	DIR *cpudir = opendir("/sys/devices/system/cpu/cpu0/cpuidle");
	if (!cpudir) {
		sysflag.cpuidle = 0;
		maxcstat = 0;
	} else {
		struct dirent *entry;
		sysflag.cpuidle = 1;
		while ((entry = readdir(cpudir)) != 0) {
			if (strlen(entry->d_name) < 3)
				continue;
			if (!isdigit(entry->d_name[5]))
				continue;
			maxcstat++;
		}
	}
	closedir(cpudir);

	/* cpufreq flag check */
	cpudir = opendir("/sys/devices/system/cpu/cpu0/cpufreq/stats");
	if (!cpudir)
		sysflag.cpufreq = 0;
	else{
		sysflag.cpufreq = 1;
		fd = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies", "r");
		maxpstat = 0;
		fread(buf, 1, sizeof(buf), fd);
		char *str = buf;
		while ((str = strstr(str, " ")) != NULL) {
			str++;
			maxpstat++;
		}
		fclose(fd);
	}
	printf(_("\tmaxpstat = %d, maxcstat = %d\n"), maxpstat, maxcstat);
	closedir(cpudir);

	fd = fopen("/proc/meminfo", "r");
	if (!fd)
		return 1;
	
	int n;
	n = fread(buf, 1, sizeof(buf), fd);
	if (!n)
		return 1;
	fclose(fd);
	c = strstr(buf, "MemTotal");
	c += 10;		/* 8 + 2 */
	head->totalmem = strtoull(c, NULL, 10);
	if (debug)
		printf(_
		       ("Basic config: count=%d, delay=%d, maxcstat=C%d\n"),
		       count, delay, maxcstat);

	head->frames = frames;
	head->maxcstat = maxcstat;
	head->maxpstat = maxpstat;
	head->count = count;
	head->delay = delay;
	head->cpunum = sysconf(_SC_NPROCESSORS_ONLN);
	head->top = 0;
	head->tail = 0;
	gettimeofday(&(head->tv), NULL);

	return 0;
}

int main(int argc, char **argv)
{
	header_t *head = &hdr;
	frame_t *f = NULL;

	setlocale(LC_ALL, "");
	bindtextdomain("sysmon", "/usr/share/locale");
	textdomain("sysmon");

	if (argc < 0 && argv[0] == NULL)
		return EXIT_FAILURE;

	while (1) {
		static struct option opts[] = {
			{"output", 1, NULL, 'o'},
			{"delay", 1, NULL, 'd'},
			{"count", 1, NULL, 'c'},
			{"debug", 0, NULL, 'x'},
			{"wakeup", 0, NULL, 'w'},
			{"cpufreq", 0, NULL, 'f'},
			{"loop", 0, NULL, 'f'},
			{"help", 0, NULL, 'h'},
			{0, 0, NULL, 0}
		};
		int index2 = 0, c;

		c = getopt_long(argc, argv, "o:d:p:c:xwflh", opts, &index2);
		if (c == -1)
			break;
		switch (c) {
		case 'o':
			logname = optarg;
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'c':
			count = atoi(optarg);
			break;
		case 'x':
			debug = _TRUE;
			break;
		case 'w':
			enwakeup = _TRUE;
			break;
		case 'f':
			encpufreq = _TRUE;
			break;
		case 'l':
			loop = _TRUE;
			break;
		case 'h':
			usage();
			break;
		default:;
		}
	}

	if (count < 2) {
		fprintf(stderr, _("count must be larger than 1\n"));
		return EXIT_FAILURE;
	}	

	if (delay < 1) {
		fprintf(stderr, _("delay must be larger than 0\n"));
		return EXIT_FAILURE;
	}	

	if (sysmon_init(head) == 1) {
		fprintf(stderr, _("sysmon_init fail\n"));
		return EXIT_FAILURE;
	}

	printf(_("\tStart sampling\n"));

	while (!stopflag) {
		NEWFRM(f, head)
		if (read_info(&sysinfo))
			break;
		calc_info(&sysinfo, f);
		if ( !loop && FRMLEN(head) >= (count-1))
			break;
		sleep(delay);
	}

	head->len = FRMLEN(head);
	printf("\ntop=%d, tail=%d len=%d\n", head->top, head->tail, head->len);
	if (head->len < 2) {
		printf("Too few samples, ignore it\n");
		return EXIT_FAILURE;	
	}
	store_info(head, logname);

	printf(_("\tStart do summary\n"));
	bk.h = head;
	do_sum(&bk);
	printf(_("\tStart report\n"));
	store_result_by_cpu_usage(&bk, NULL);
	store_result_by_rss(&bk, NULL);

	if (encpufreq)
		store_result_by_cpu_freq(&bk, NULL);

	if (enwakeup)
		store_result_by_wakeup(&bk, NULL);
	cleanup();
	return EXIT_SUCCESS;
}
