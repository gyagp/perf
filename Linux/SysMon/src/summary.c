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
 */

#define _GNU_SOURCE
#include "sysmon.h"

static int compare_cpu(gconstpointer a, gconstpointer b)
{
	sum_pdata_t *A = (sum_pdata_t *) a;
	sum_pdata_t *B = (sum_pdata_t *) b;

	return B->cpu_delta - A->cpu_delta;
}

static int compare_rss(gconstpointer a, gconstpointer b)
{
	sum_pdata_t *A = (sum_pdata_t *) a;
	sum_pdata_t *B = (sum_pdata_t *) b;

	return B->rss_avg - A->rss_avg;
}


static int compare_pid(gconstpointer a, gconstpointer b)
{
	sum_pdata_t *A = (sum_pdata_t *) a;
	sum_pdata_t *B = (sum_pdata_t *) b;

	return (A->pid == B->pid) ? 0 : 1;
}


sum_pdata_t *new_process(pdata_t * d)
{
	sum_pdata_t *spd = malloc(sizeof(sum_pdata_t));
	spd->cpu_0 = d->cpu;
	spd->cpu_delta = 0;
	spd->cpu_max = 0;
	spd->rss_avg = d->rss;
	spd->pid = d->pid;
	spd->tlist = NULL;
	strncpy(spd->pname, d->pname, PNAME_LENGTH);
	return spd;
}

int find_pid(frame_t * f, int32_t pid, int guess)
{
	int loc = guess;
	pdata_t *p = f->data;

	if (loc < 0)
		return -1;
	if (p[loc].pid == pid)
		return loc;
	loc = guess - 1;
	if (loc >= 0 && p[loc].pid == pid)
		return loc;
	loc = guess + 1;
	if (p[loc].pid == pid)
		return loc;
	loc = guess - 2;
	if (loc >= 0 && p[loc].pid == pid)
		return loc;
	loc = guess + 2;
	if (p[loc].pid == pid)
		return loc;
	loc = guess - 3;
	if (loc >= 0 && p[loc].pid == pid)
		return loc;
	loc = guess + 3;
	if (p[loc].pid == pid)
		return loc;

	/* do slowest search */
	printf("Quick guess pid %d fail, do heavy search\n", pid);
	int i;
	for (i = 0; i < f->pnum; i++) {
		if (p[i].pid == pid)
			break;
	}
	return (p[i].pid != pid) ? -1 : i;
}

int do_sort(sum_block_t * sbk, char *type)
{
	if (strcmp(type, "cpu") == 0) {
		sbk->pdlist = g_list_sort(sbk->pdlist, compare_cpu);
		return 0;
	}
	if (strcmp(type, "rss") == 0) {
		sbk->pdlist = g_list_sort(sbk->pdlist, compare_rss);
		return 0;
	}
	printf("No %s sort type\n", type);
	return -1;
}

int do_sum(sum_block_t * sbk)
{
	header_t *head = sbk->h;
	frame_t *f, *prev = NULL;
	int i;
	FOR_EACH_FRM(f, head) {
		if (FRMNO(f, head) == 0) {
			sbk->id_0 = f->id;
			sbk->id_delta = 0;
			sbk->tot_0 = f->tot;
			sbk->tot_delta = 0;
			if (enwakeup) {
				sbk->wakeup_0 = f->wakeup;
				sbk->wakeup_delta = 0;
			}
			/* system node array */
			int size = sizeof(sysnode_t) * (head->count - 1);
			sbk->sa =
	    			(sysnode_t *) malloc(size);
			memset(sbk->sa, 0, size);
			i = 0;
			while (i < f->pnum) {
				pdata_t *d = &f->data[i];
				sum_pdata_t *spd = new_process(d);
				sbk->pdlist = g_list_append(sbk->pdlist, spd);
				i++;
			}
		}else{
			sbk->id_delta = f->id - sbk->id_0;
			sbk->tot_delta = f->tot - sbk->tot_0;

			int dist = FRMNO(f, head);
			sysnode_t *s = &sbk->sa[dist - 1];
			s->idle_delta = f->id - prev->id;
			s->clock = f->tot - prev->tot;

			if (enwakeup){
				sbk->wakeup_delta = f->wakeup - sbk->wakeup_0;
				s->wakeup_delta = f->wakeup - prev->wakeup;
			}

			if (encpufreq) {
				int k;
				for (k = 0; k < maxpstat; k++)
					s->freq_delta[k] = f->freq[k] -  prev->freq[k];
			}

			i = 0;
			while (i < f->pnum) {
				pdata_t *d = &f->data[i], *d0 = NULL;
				int loc = find_pid(prev, d->pid, i);
				if (loc >= 0)
					d0 = &prev->data[loc];
				else
					printf("#########Miss find pid %u\n", d->pid);
				sum_pdata_t *spd;
		      		sum_pdata_t pd = { pid:d->pid, };
				GList *reslist =
			    		g_list_find_custom(sbk->pdlist, &pd,
					       compare_pid);
				if (reslist) {
					spd = (sum_pdata_t *) reslist->data;
					spd->cpu_delta = d->cpu - spd->cpu_0;
					spd->rss_avg =
				    		(spd->rss_avg * dist  + d->rss) / (dist + 1);
					pnode_t *n =
				    		(pnode_t *) malloc(sizeof(pnode_t));
					n->cpu_delta =
				    		(d0) ? d->cpu - d0->cpu : d->cpu;
					n->rss = d->rss;
					n->s = s;
					if (n->cpu_delta > spd->cpu_max)
						spd->cpu_max = n->cpu_delta;
					spd->tlist = g_list_append(spd->tlist, n);
				} else {
					spd = new_process(d);
					sbk->pdlist =
				    		g_list_append(sbk->pdlist, spd);
				}
				i++;
			}
		}
		prev = f;
	}

	do_sort(sbk, "cpu");

	printf("\n********************************************\n");
	printf("    delay=%d\n", head->delay);
	printf("    count=%d\n", head->count);
	printf("    cpunum=%d\n", head->cpunum);
	printf("    maxcstat=%d\n", head->maxcstat);
	printf("    maxpstat=%d\n", head->maxpstat);
	printf("    idle=%3.1f%%\n", 
	       (double) sbk->id_delta * 100 / (double) sbk->tot_delta);
	if (enwakeup)
		printf("    wakeup=%3.1f/s\n", sbk->wakeup_delta * 1.0 / (head->len - 1) / head->delay / head->cpunum);

	if (debug) {
		for (i = 0; i < head->len - 1 ; i++) {
			sysnode_t *s = &sbk->sa[i];
			printf("\t[%llu]%llu", s->clock, s->idle_delta);
			if (enwakeup)
				printf(",%llu", s->wakeup_delta);
	
			if (encpufreq){
				uint64_t freq_tot = 0;
				int k;
				for (k = 0; k < maxpstat; k++)
					freq_tot += s->freq_delta[0];
				printf(",%llu/%llu", s->freq_delta[0], freq_tot);
			}
			printf("--->");
		}
	}
	printf("\n");
	printf("\n");
	GList *item = g_list_first(sbk->pdlist);
	while (item) {
		sum_pdata_t *spd = item->data;
		if (spd->cpu_delta > 0) 
			printf
		    	("Process(%-16s), cpu=%3.1f%%, rss=%-5uK, pid=%u\n",
		     	spd->pname, 
		     	(double) spd->cpu_delta * 100 /
		     	(double) sbk->tot_delta, spd->rss_avg, spd->pid);
		if (debug) {
			if (spd->cpu_delta > 0) {
				GList *node = g_list_first(spd->tlist);
				while (node) {
					pnode_t *n = node->data;
					printf("\t[%llu]%llu,%u--->",
					       		n->s->clock, n->cpu_delta,
					       		n->rss);
					node = g_list_next(node);
				}
				printf("\n");
			}
		}
		item = g_list_next(item);
	}
	return 0;
}
