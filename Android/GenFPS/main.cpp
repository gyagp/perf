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
#include <string>

int main(int argc, char ** argv)
{
    FILE    *fp = NULL;
    unsigned  line_length = 128;
    char    line_str[line_length];
    char    keyword_str[line_length];
    char *  keyword;
    char    time_str[line_length];
    char *  timeStamp;
    int     res = 0;
    int     len = 0;
    int     i = 0;
    char    eventFile[64] = {'\0'};
    std::string temp;

    char    fpsSlab[16] = {'\0'};
    char    fpsSlabTemp[16] = {'\0'};
    char    fpsSlabSwapBuffer[16] = {'\0'};
    unsigned fps = 0;
    bool    started = false;
    int     fpsList[100];
    int     fpsCount = 0;
    float   fpsAvg = 0.0;

    for (int index=1; index < argc; index++) {
        temp = argv[index];
        if (temp[0] == '-' && temp.length()>1) {
            switch (temp[1]) {
                case 'i':
                    if (index +1 < argc) {
                        strncpy(eventFile, argv[index+1], 64);
                    } else {
                        return 1;
                    }
                    break;
                default:;
            }
        }
    }

    if ((fp=fopen(eventFile, "r"))==NULL) {
        fprintf(stderr, "Can not open the event file\n");
        return 1;
    }

    

    while (fgets(line_str, line_length, fp)!=NULL) {
        res = sscanf(line_str, "%s %*s %s", keyword_str, time_str);
        if (res < 2) {
            continue;
        }
        len = strlen(keyword_str);
        if (len>3) {
            keyword_str[len-1] = '\0';         // bypass the '('
            keyword = keyword_str + 2;         // bypass the "I/"
        }
        //fprintf(stdout, "%s ", keyword);
        len = strlen(time_str);
        time_str[len-1] = '\0';          // bypass the ']'
        for (i=0; i< len; i++) {
            if (',' == time_str[i]) {
                timeStamp = time_str + i +1;
                break;
            }
        }
        //fprintf(stdout, "%s \n", timeStamp);

        if (strcmp(keyword, "sf_swapbuffers")==0) {
            strncpy(fpsSlabSwapBuffer, timeStamp, strlen(timeStamp));
        } else if (strcmp(keyword, "sf_repaint_done")==0 \
                   && strcmp(fpsSlabSwapBuffer, timeStamp)!=0) {  // Exclude the fake event: same timestamp for swapbuffer and repaint_done
            strncpy(fpsSlabTemp, timeStamp, strlen(timeStamp)-3);
            if (strcmp(fpsSlab, fpsSlabTemp)!=0) {
                if (started) {
                    fpsList[fpsCount ++] = fps;
                } else {
                    started = true;
                }
                strncpy(fpsSlab, fpsSlabTemp, strlen(fpsSlabTemp));
                fps = 1;
            } else {
                fps ++;
            }
        }
    }

    // Calculate the average, exclude the first one and the last one to elimate the uncompleted data
    for (i=1; i<fpsCount-1; i++) {
        fprintf(stdout, "FPS: %d\n", fpsList[i]);
        fpsAvg += fpsList[i];
    }
    fpsAvg/=(fpsCount-2);
    fprintf(stdout, "Average FPS: %.1f\n", fpsAvg);

    return 0;
}
