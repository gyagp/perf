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

static char buf[1024];
int read_stat(sysstat_t *s, int *pids, pidstat_t *h)
{
    FILE *fp[9];
    char *c,*d,*str;
    int i = 0;
    char filename[256];

    /* Read /proc/stat */
    fp[0]=fopen("/proc/stat","r");
    if (!fp[0]) {
        fprintf(stderr, _("Fail to open /proc/stat"));
        return 1;
    }

    while (i<appnum) {
        /* Open proc pidstat proc file */
        sprintf(filename, "/proc/%d/stat", pids[i]);
        fp[i+1]=fopen(filename,"r");
        if (!fp[i+1]){
            fprintf(stderr, _("Fail to open %s"), filename);
            return 1;
        }
        i++;
    }

    str = fgets(buf, sizeof(buf)-1,fp[0]);
    if (!str) {
        fprintf(stderr, _("Fail to get line from /proc/stat"));
        fclose(fp[0]);
        return 1;
    }
    fclose(fp[0]);

    c = strstr(buf, "cpu ");
    if(c) { 
        sscanf(c, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",\
            &s->cus, &s->cni, &s->csy, &s->cid, &s->ciow, &s->chi, &s->csi, &s->cstl);
    }

    i=0;
    while (i<appnum){
        pidstat_t *t=&h[i];
        str = fgets(buf, sizeof(buf)-1,fp[i+1]);
        if (!str) {
            fprintf(stderr, _("Fail to get proc cpu data"));
            fclose(fp[i+1]);
            return 1;
        }
        fclose(fp[i+1]);

        /* Data we are not interested in */
        char ca;
        int lb,lc,ld,le,lf;
        long unsigned int Lg,Lh,Li,Lj,Lk;

        c = strchr(buf, '(') + 1;
        d = strrchr(c, ')');

        int nameLength = d - c;
        if (nameLength > APP_NAME_LENGTH - 1) {
            nameLength = APP_NAME_LENGTH - 1;
        }
        strncpy(t->pname, c, nameLength);
        t->pname[nameLength] = '\0';
        t->pid = pids[i];

        c = d + 2;                 /* skip ") " */

        /* Get utime stime cutime and cstime */
        sscanf(c,
            "%c "
            "%d %d %d %d %d "
            "%lu %lu %lu %lu %lu "
            "%llu %llu %llu %llu",  /* utime stime cutime cstime */
        &ca,
        &lb,&lc,&ld,&le,&lf,
        &Lg,&Lh,&Li,&Lj,&Lk,
        &t->utime,&t->stime,&t->cutime, &t->cstime);
        i++;
    }
    return 0;
}

int read_meminfo(meminfo_t *m)
{
    FILE *fm;
    char *c;
    int n;

    /* Read /proc/meminfo */
    fm=fopen("/proc/meminfo","r");
    if (!fm) {
        fprintf(stderr, _("Fail to open /proc/meminfo"));
        return 1;
    }
    n=fread(buf,1,sizeof(buf),fm);
    if (!n) {
        return 1;
    }
    fclose(fm);
    c = strstr(buf, "MemFree");
    c+=9; /* 7 + 2 */
    m->free = strtoull(c, NULL, 10);        

    c = strstr(buf, "Buffers");
    c+=9;
    m->buff = strtoull(c, NULL, 10);        

    c = strstr(buf, "Cached");
    c+=8;
    m->cache = strtoull(c, NULL, 10);        
    
    return 0;
}

int read_pidstat(int *pids, pidstat_t *h)
{
    FILE *fp;
    char *c,*d,*str;
    int i=0;
    char filename[256];
    while (i<appnum){
        pidstat_t *t=&h[i];
        /* Open proc pidstat proc file */
        sprintf(filename, "/proc/%d/stat", pids[i]);
        fp=fopen(filename,"r");
        if (!fp) {
            fprintf(stderr, _("Fail to open %s"), filename);
            return 1;
        }

        str = fgets(buf, sizeof(buf)-1,fp);
        if (!str) {
            fprintf(stderr, _("Fail to get line from %s"), filename);
            return 1;
        }
        fclose(fp);

        /* Data we are not interested in */
        char ca;
        int lb,lc,ld,le,lf;
        long unsigned int Lg,Lh,Li,Lj,Lk;

        c = strchr(buf, '(') + 1;
        d = strrchr(c, ')');
        
        int nameLength = d - c;
        if (nameLength > APP_NAME_LENGTH - 1) {
              nameLength = APP_NAME_LENGTH - 1;
        }
        strncpy(t->pname, c, nameLength);
        t->pname[nameLength] = '\0';
        t->pid = pids[i];

        c = d + 2;                 /* skip ") " */

        /* Get utime stime cutime and cstime */
        sscanf(c,
               "%c "
               "%d %d %d %d %d "
               "%lu %lu %lu %lu %lu "
               "%llu %llu %llu %llu",  /* utime stime cutime cstime */
                &ca,
                &lb,&lc,&ld,&le,&lf,
                &Lg,&Lh,&Li,&Lj,&Lk,
                &t->utime,&t->stime,&t->cutime, &t->cstime);
        i++;
    }
    return 0;
}

int read_pidmem(int *pids, pidmem_t *pm)
{
    FILE *fp;
    char *str;
    int i=0;
    char filename[256];
    while (i<appnum) {
        pidmem_t *m=&pm[i];
        /* Open proc pidstat proc file */
        sprintf(filename, "/proc/%d/statm", pids[i]);
        fp=fopen(filename,"r");
        if (!fp){
            fprintf(stderr, _("Fail to open %s"), filename);
            return 1;
        }

        str = fgets(buf, sizeof(buf)-1,fp);
        if (!str){
            fprintf(stderr, _("Fail to get line from %s"), filename);
            return 1;
        }
        fclose(fp);

        /* Data we are not interested in */
        long long unsigned int la,lc,ld,le,lf;

        /* Get utime stime cutime and cstime */
        sscanf(buf, "%Lu %Lu %Lu %Lu %Lu %Lu",  /* vsize rss share text data lib dirty */
                     &la,&m->rss,&lc,&ld,&le,&lf);
        i++;
    }
    return 0;
}

static int read_cpuidle(cpuidle_t *idl)
{
    DIR *cpudir, *dir;
    struct dirent *entry;
    FILE *fd = NULL;
    char buf[64];
    char filename[128], *f;
    int len, clevel = 0;

    cpudir = opendir("/sys/devices/system/cpu");
    if (!cpudir) {
        return 1;
    }
    memset(idl,0,sizeof(cpuidle_t)*MAX_CSTAT_NUM);
    
    /* Loop over cpuN entries */
    while ((entry = readdir(cpudir))) {
        if (strlen(entry->d_name) < 3) {
            continue;
        }

        if (!isdigit(entry->d_name[3])) {
            continue;
        }

        len = sprintf(filename, "/sys/devices/system/cpu/%s/cpuidle", entry->d_name);

        dir = opendir(filename);
        if (!dir) {
            sysflag.cpuidle_flag = 0;
            return _NO_CPUIDLE_ERR;
        }

        clevel = 0;

        /* For each C-state, there is a stateX directory which
         * contains a 'usage' and a 'time' (duration) file */
        while ((entry = readdir(dir))) {
            if (strlen(entry->d_name) < 3) {
                continue;
            }
            sprintf(filename + len, "/%s/usage", entry->d_name);
            fd = fopen(filename, "r");
            if (!fd) {
                idl[clevel].usage=0;
            } else {
                f = fgets(buf, sizeof(buf), fd);
                fclose(fd);
                if (f == NULL) {
                    idl[clevel].usage=0;
                    break;
                }
                idl[clevel].usage += 1+strtoull(buf, NULL, 10);
            }
            sprintf(filename + len, "/%s/time", entry->d_name);
            fd = fopen(filename, "r");
            if (!fd) {
                idl[clevel].duration=0;    
            } else {
                f = fgets(buf, sizeof(buf), fd);
                fclose(fd);
                if (f == NULL) {
                    idl[clevel].duration=0;
                    break;
                }
                idl[clevel].duration += 1+strtoull(buf, NULL, 10);
            }    
            clevel++;
        }
        closedir(dir);

    }
    closedir(cpudir);
    return 0;
}

int read_info(int *pids, procinfo_t *p)
{
    int ret=0;
//    memset(bm,0,sizeof(bm));
//    memset(bp,0,sizeof(bp));
//    memset(bs,0,sizeof(bs));
    if ((ret=read_stat(&(p->ss), pids, p->ps))!=0) {
        return ret;
    }
    if ((ret=read_meminfo(&(p->mm)))!=0) {
        return ret;
    }
    if ((ret=read_pidmem(pids,p->pm))!=0) {
        return ret;
    }
    if ((ret=read_cpuidle(p->idle))!=0) {
        return ret;
    }
    return ret;
}

void calc_info(procinfo_t *infos, unsigned int tog, frame_t *frm)
{
    unsigned int curr=tog,prev=!tog;
    sysstat_t *s1=&infos[prev].ss,*s2=&infos[curr].ss;
    frm->id=s2->cid - s1->cid;
    frm->tot=(s2->cus + s2->cni + s2->csy + s2->cid + s2->ciow + s2->chi + s2->csi + s2->cstl) - \
            (s1->cus + s1->cni + s1->csy + s1->cid + s1->ciow + s1->chi + s1->csi +s1->cstl );
    if (debug) {
        printf("idle(jif)=%llu, total(jif)=%llu\n",frm->id, frm->tot);        
    }

    meminfo_t *m=&infos[curr].mm;
    frm->free= m->free;
    frm->buff= m->buff;
    frm->cache= m->cache;
    if (debug) {
        printf("Free=%Lu kB, Free+Buf/Cache=%Lu kB\n", frm->free, frm->free + frm->buff + frm->cache);
    }
    int j=0;
    uint64_t d1=0,d2=0;
    while (j<=maxcstate) {
        d1 += infos[prev].idle[j].usage;
        d2 += infos[curr].idle[j].usage;
        j++;    
    }
    frm->wakeup_persec=(d2-d1)/delay/sysconf(_SC_NPROCESSORS_ONLN);
    cpuidle_t *idl1, *idl2;
    int k=0;
    uint64_t totmic=delay * 1000000 * sysconf(_SC_NPROCESSORS_ONLN);
    int64_t n=totmic;
    while (k<=maxcstate) { 
        idl1=&infos[prev].idle[k];
        idl2=&infos[curr].idle[k];
        n -= (idl2->duration-idl1->duration);
        frm->c_time[k] = (idl2->duration-idl1->duration);
        k++;
    }
    frm->c_time[0]=(n>0?n:0);    
    //idl1=&infos[prev].idle[maxcstate];
    //idl2=&infos[curr].idle[maxcstate];
    //frm->cn_time=idl2->duration - idl1->duration;    
    if (debug) {
        printf("Wakeup_per_sec=%llu C0_time=%llu Cn_time=%llu\n",frm->wakeup_persec,frm->c_time[0], frm->c_time[maxcstate]);
    }
    int i=0;
    frm->appnum = appnum;
    while (i<appnum){
        pidstat_t *t1=&infos[prev].ps[i],*t2=&infos[curr].ps[i];
        pidmem_t *m2=&infos[curr].pm[i];

        char * pname = infos[curr].ps[i].pname;
        appdata_t *data=&frm->data[i];
        strcpy(data->appname, pname);
        data->pid = infos[curr].ps[i].pid;
        
        if (debug) {
           printf("for cpu;\n t2:utime:%llu, stime:%llu, cutime:%llu, cstime:%llu\n t1:utime:%llu, stime:%llu, cutime:%llu, cstime:%llu\n", \
                              t2->utime ,t2->stime , t2->cutime , t2->cstime,t1->utime , t1->stime , t1->cutime , t1->cstime); 
        }
        data->cpu = (t2->utime + t2->stime + t2->cutime + t2->cstime) - \
                (t1->utime + t1->stime + t1->cutime + t1->cstime);
        data->rss = m2->rss*4;
        if (debug) {
            printf("    app[%d]= name: %s cpu:%llu rss:%llu kB\n",i ,pname ,data->cpu ,data->rss);
        }
        i++;
    }        
}
