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


static char buf[1024];
int read_stat(sysstat_t * s)
{
	FILE *fp;
	char *c, *str;

	/* Read /proc/stat */
	fp = fopen("/proc/stat", "r");
	if (!fp) {
		fprintf(stderr, _("Fail to open /proc/stat"));
		return 1;
	}

	str = fgets(buf, sizeof(buf) - 1, fp);
	if (!str) {
		fprintf(stderr, _("Fail to get line from /proc/stat"));
		fclose(fp);
		return 1;
	}
	fclose(fp);

	c = strstr(buf, "cpu ");
	if (c) {
		sscanf(c, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
		       &s->cus, &s->cni, &s->csy, &s->cid, &s->ciow,
		       &s->chi, &s->csi, &s->cstl);
	}
	return 0;
}

int read_meminfo(meminfo_t * m)
{
	FILE *fm;
	char *c;
	int n;

	/* Read /proc/meminfo */
	fm = fopen("/proc/meminfo", "r");
	if (!fm) {
		fprintf(stderr, _("Fail to open /proc/meminfo"));
		return 1;
	}
	n = fread(buf, 1, sizeof(buf), fm);
	if (!n) {
		return 1;
	}
	fclose(fm);
	c = strstr(buf, "MemFree");
	c += 9;			/* 7 + 2 */
	m->free = strtoull(c, NULL, 10);

	c = strstr(buf, "Buffers");
	c += 9;
	m->buff = strtoull(c, NULL, 10);

	c = strstr(buf, "Cached");
	c += 8;
	m->cache = strtoull(c, NULL, 10);

	return 0;
}

int read_pidstat(pidstat_t * t, uint32_t pid)
{
	FILE *fp;
	char *c, *d, *str;
	char filename[256];

	/* Open proc pidstat proc file */
	sprintf(filename, "/proc/%d/stat", pid);
	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, _("Fail to open %s"), filename);
		return 1;
	}

	str = fgets(buf, sizeof(buf) - 1, fp);
	if (!str) {
		fprintf(stderr, _("Fail to get line from %s"), filename);
		return 1;
	}
	fclose(fp);

	/* Data we are not interested in */
	char ca;
	int lb, lc, ld, le, lf;
	long unsigned int Lg, Lh, Li, Lj, Lk;

	c = strchr(buf, '(') + 1;
	d = strrchr(c, ')');

	int nameLength = d - c;
	if (nameLength > PNAME_LENGTH - 1) {
		nameLength = PNAME_LENGTH - 1;
	}
	strncpy(t->pname, c, nameLength);
	t->pname[nameLength] = '\0';

	c = d + 2;		/* skip ") " */

	/* Get utime stime cutime and cstime */
	sscanf(c, "%c " "%d %d %d %d %d " "%lu %lu %lu %lu %lu " "%llu %llu %llu %llu",	/* utime stime cutime cstime */
	       &ca,
	       &lb, &lc, &ld, &le, &lf,
	       &Lg, &Lh, &Li, &Lj, &Lk,
	       &t->utime, &t->stime, &t->cutime, &t->cstime);
	return 0;
}

int read_pidmem(pidmem_t * m, uint32_t pid)
{
	FILE *fp;
	char *str;
	char filename[256];

	/* Open proc pidstat proc file */
	sprintf(filename, "/proc/%d/statm", pid);
	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, _("Fail to open %s"), filename);
		return 1;
	}

	str = fgets(buf, sizeof(buf) - 1, fp);
	if (!str) {
		fprintf(stderr, _("Fail to get line from %s"), filename);
		return 1;
	}
	fclose(fp);

	/* Data we are not interested in */
	long long unsigned int la, lc, ld, le, lf;

	/* Get mem */
	sscanf(buf, "%Lu %Lu %Lu %Lu %Lu %Lu",	/* vsize rss share text data lib dirty */
	       &la, &m->rss, &lc, &ld, &le, &lf);
	return 0;
}

static int read_cpuidle(cpuidle_t * idl)
{
	DIR *cpudir, *dir;
	FILE *fd = NULL;
	struct dirent *entry;
	char buf[64];
	char filename[128], *f;
	int len, clevel = 0;

	cpudir = opendir("/sys/devices/system/cpu");
	if (!cpudir) 
		return 0;

	memset(idl, 0, sizeof(cpuidle_t) * MAX_CSTAT);
	/* Loop over cpuN entries */
	while ((entry = readdir(cpudir))) {
		if (!isdigit(entry->d_name[3])) 
			continue;

		len =
		    sprintf(filename, "/sys/devices/system/cpu/%s/cpuidle",
			    entry->d_name);

		dir = opendir(filename);
		if (!dir) {
			sysflag.cpuidle = 0;
			return 0;
		}
		clevel = 0;

		/* For each C-state, there is a stateX directory which
		 * contains a 'usage' and a 'time' (duration) file */
		while ((entry = readdir(dir))) {
			if (strlen(entry->d_name) < 3) {
				continue;
			}
			sprintf(filename + len, "/%s/usage",
				entry->d_name);
			fd = fopen(filename, "r");
			if (!fd) {
				idl[clevel].usage = 0;
			} else {
				f = fgets(buf, sizeof(buf), fd);
				fclose(fd);
				if (f == NULL) {
					idl[clevel].usage = 0;
					break;
				}
				idl[clevel].usage +=
				    1 + strtoull(buf, NULL, 10);
			}
			/*
			sprintf(filename + len, "/%s/time", entry->d_name);
			fd = fopen(filename, "r");
			if (!fd) {
				idl[clevel].duration = 0;
			} else {
				f = fgets(buf, sizeof(buf), fd);
				fclose(fd);
				if (f == NULL) {
					idl[clevel].duration = 0;
					break;
				}
				idl[clevel].duration +=
				    1 + strtoull(buf, NULL, 10);
			}
			*/
			clevel++;
		}
		closedir(dir);

	}
	closedir(cpudir);


	return 0;
}

static int read_cpufreq(cpufreq_t *fqt)
{
	FILE *fd = NULL;
	char line[128];
	
	fd = fopen("/sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state",
		   "r");
	if (!fd){
		printf("Not load cpufreq_stat module!!!\n");
		return 1;
	}
	int i = 0;
	while (!feof(fd)) {
		memset(line, 0, sizeof(line));
		if (!fgets(line, sizeof(line)-1, fd))
			break;
		sscanf(line, "%u %llu",
		       &fqt[i].freq, &fqt[i].cnt);
		i++;			
	}
	
	return 0;

}

int read_pinfo(pinfo_t * pi, uint32_t *n)
{
	DIR *dir;
	struct dirent *ent;
	int num=0;

	num = 0;
	dir = opendir("/proc");
	if (!dir) {
		fprintf(stderr, _("Failed to open /proc!\n"));
		return 1;
	}
	do {
		uint32_t tmp;
		ent = readdir(dir);
		if (!ent)
			break;
		tmp = strtoul(ent->d_name, NULL, 10);
		if (tmp > 0){
			pinfo_t *t = &pi[num];
			t->pid = tmp;
			if (read_pidstat(&t->stat, t->pid) != 0)
				continue;
			if (read_pidmem(&t->mem, t->pid) != 0)
				continue;
		}
		num++;
	} while (1);
	closedir(dir);
	*n=num;
	printf(_("Totally have %d processes***\n"), num);
	return 0;
}

int read_info(procinfo_t * p)
{
	int ret = 0;
	if ((ret = read_stat(&p->ss)) != 0)
		return ret;

	if ((ret = read_meminfo(&p->mm)) != 0)
		return ret;

	p->pnum = 0;
	if ((ret = read_pinfo(p->pi, &p->pnum)) != 0)
		return ret;

	if (enwakeup && sysflag.cpuidle != 0) {
		if ((ret = read_cpuidle(p->idle)) != 0)
			return ret;
	}

	if (encpufreq && sysflag.cpufreq != 0) {
		if ((ret = read_cpufreq(p->freq)) != 0)
			return ret;
	}

	return ret;
}

/*
int binSearch(pinfo_t *p, int cnt, uint32_t pid) {
	int start = 0;
	int stop = cnt -1;
	int mid = (start + stop) / 2;
	while(p[mid].pid != pid && start < stop) {
		if (pid < p[mid].pid)
			stop = mid - 1;
		else if (pid > p[mid].pid)
			start = mid + 1;
		mid = (start + stop) / 2;
	}
	return (p[mid].pid != pid)? -1 : mid;
}
*/

void calc_info(procinfo_t * p, frame_t * frm)
{
	sysstat_t *s = &p->ss;

	frm->id = s->cid + s->ciow;	/* Real idle time includes idle and iowait */
	frm->tot =
	    (s->cus + s->cni + s->csy + s->cid + s->ciow + s->chi +
	     s->csi + s->cstl);
	if (debug) {
		printf("idle(jif)=%llu, total(jif)=%llu\n", frm->id,
		       frm->tot);
	}

	meminfo_t *m = &p->mm;
	frm->free = m->free;
	frm->buff = m->buff;
	frm->cache = m->cache;
	if (debug) {
		printf("Free=%Lu kB, Free+Buf/Cache=%Lu kB\n", frm->free,
		       frm->free + frm->buff + frm->cache);
	}
	int j;
	uint64_t d = 0;

	if (enwakeup) {
		for (j = 0;j < maxcstat; j++) 
			d += p->idle[j].usage;
		frm->wakeup = d;
	}

	if (encpufreq) {
		for (j = 0; j < maxpstat; j++)
			frm->freq[j] = p->freq[j].cnt; 
	}

	int i = 0;
	frm->pnum = p->pnum;
	while (i < frm->pnum) {
		pidstat_t *t = &p->pi[i].stat;
		char *pname = p->pi[i].stat.pname;
		pdata_t *data = &frm->data[i];
		strcpy(data->pname, pname);
		data->pid = p->pi[i].pid;

		if (debug) {
			printf
			    ("for cpu;\n t:utime:%llu, stime:%llu, cutime:%llu, cstime:%llu\n",
			     t->utime, t->stime, t->cutime, t->cstime);
		}
		data->cpu = t->utime + t->stime;

		pidmem_t *m2 = &p->pi[i].mem;
		data->rss = m2->rss * 4;
		if (debug) 
			printf
			    ("    app[%d]= name: %s cpu:%llu rss:%u kB\n",
			     i, pname, data->cpu, data->rss);
		i++;
	}
}
