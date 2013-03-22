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

int store_info(header_t *phead, char *fname)
{        
    FILE *fd;
    printf("\n      Sampling finished.\n      Result can be found at %s\n",fname);
    fd=fopen(fname,"w");
    if (!fd){
        fprintf(stderr,_("Fail open %s"), fname);
        return 1;
    }
    fprintf(fd, "[header]\n");
    fprintf(fd, "\tdelay = %d\n" , phead->delay);
    fprintf(fd, "\tcount = %d\n" , phead->count);
    fprintf(fd, "\tmaxcstate = %d\n" , phead->maxcstate);
    fprintf(fd, "\ttotalmem = %d\n" , phead->totalmem);
    fprintf(fd, "\tcpuidle_flag = %d\n", sysflag.cpuidle_flag);
    fprintf(fd, "\tacpi_flag = %d\n", sysflag.acpi_flag);
    
    int j=0, index=0;
    frame_t *f=phead->frames;
    
    f=NEXTFRM(f,MAX_APP_NUM);   // to fix the first point has no app date issue

    if (debug) {
        printf ("the count is : %d \n",phead->count);
    }
    
    while (j<phead->count){
        fprintf(fd,"[Frame %d]\n", j);
        fprintf(fd,"\t[cpu  ] idle= %llu total= %llu\n",f->id, f->tot);
        fprintf(fd,"\t[mem  ] free= %llu buffered= %llu cache= %llu\n", f->free,f->buff,f->cache);
        fprintf(fd,"\t[power] wakeup_persec= %llu", f->wakeup_persec);
        for (index=0; index<=phead->maxcstate; index++) {
            fprintf(fd," C%d_time= %llu", index,f->c_time[index]);
        }
        fprintf(fd,"\n");
        int i=0;
        while (i<f->appnum){
            appdata_t *d=&f->data[i];
            fprintf(fd,"\t[app  %s %d ] cpu= %llu rss= %llu\n",d->appname, d->pid, d->cpu, d->rss);
            i++;
        }        
        fprintf(fd,"[End]\n");
        f=NEXTFRM(f,MAX_APP_NUM);
        j++;
    }

    fprintf(fd, "[MaxAppNum %d ]\n" , phead->maxAppNum);
    fprintf(fd, "[Total %d frames]\n", j);
    fclose(fd);

    return 0;
}


