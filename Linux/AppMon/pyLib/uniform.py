#!/usr/bin/python

# Copyright (C) 2010 Intel Corporation
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# Authors:                                                                               
#    Chen, Guobing <guobing.chen@intel.com>

import sys, os,string
def getval(lofile,report):
    f=open(logfile,"r")
    lines= f.readlines();
    app=lines[-3];
    cn=lines[-4];
    max=lines[-5];
    ave=lines[-6];
    acpi=lines[-1];
    cpuidle=lines[-2];
    f.close()
    #retrieve the applications' names and put them into appstr, applist
    if app!='#\n':
        applist=app.split(' ')
        applist.remove("#")
        applist.remove("\n")
        appstr=[]
        rep=open(report,"w")
	appNameStr=''
        for x in range(len(applist)):
            y=x+1
            appstr.append('App%s_CPU'%y)
            appstr.append('App%s_Mem'%y)
            if x==0:
                appNameStr += "Sampled applications: App%d -"%y + applist[x]
            else:
                appNameStr += "App%d -"%y + applist[x]
	    appNameStr += " "
        print >> rep, appNameStr
	rep.close()
    #retrieve the average     
    avelist=ave.split(' ')
    avelist.remove("#")
    avelist.insert(0,"ave")
    #retrieve the max line
    maxlist=max.split(' ')
    maxlist.remove("#")
    maxlist.remove('\n')
    maxlist.insert(0,"max")
    #retrieve cn and get cnstr 
    cnlist=cn.split(' ')
    cnlist.remove("#")
    cn=int(cnlist[0])
    cnstr=[]
    for x in range(cn+1):
        cnstr.append("C%s"%x)
    #generate the title line
    title=['NO.','CPU%','Mem','Mem(Pure)','Wakeups']
    title+=cnstr
    if app!='#\n':
        title+=appstr
    else:
        os.system("rm %s" %report)
    #generate the report
    tmp=open("tmp.txt","w+")
    print >>tmp, ' '.join(title)
    print >>tmp, ' '.join(avelist), ' '.join(maxlist)
    tmp.close()
    tmp=open("tmp.txt","a+")
    os.system("grep '^[^#]' %s >>tmp.txt" %logfile) 
    tmp.close()
    ######The following codes is added by xuehua######
    #this block of code is used to remove serveral colume of date from tmp.txt
    #retrive the cpuidle flag
    if acpi[-2]=='0' or cpuidle[-2]=='0':
        tmpf=open("tmp.txt","r")
        lines=tmpf.readlines()
        tmpf.close()
        ff=open("tmp.txt","w+")
        count=len(lines)
        for i in range(0,count):
            line=lines[i]
            linelist=line.split(' ')
            linetmp = ''
            for x in range(len(linelist)):
                if x in [4,5,6]:
                    continue;
                else:
                    linetmp +='%s '%linelist[x]
            ff.write(linetmp)
        ff.close()
    ######End by xuehua######
    os.system("column -t tmp.txt >>%s" %report)
    os.system("rm tmp.txt")
def insertsep(report,finalreport):
    rep=open(report,"r")
    alines=rep.readlines()
    rep.close()
    alines.insert(1,"------------------------------------------------------------------------------------------\n")
    alines.insert(3,"------------------------------------------------------------------------------------------\n")
    alines.insert(6,"------------------------------------------------------------------------------------------\n")
    fr=open(finalreport,"w")
    fr.writelines(alines)
    fr.close()
    os.system("rm %s"%report)
    
def tail(report):
    rep=open(report,"a+")
    print >>rep, '---------------------------------------------------------------------------------------------'    
    print >>rep, "Note: This is a list showing the data about process CPU, Memory.Mem(Pure)=MemoryUsed-Buffer-cache"
    print >>rep, "Wakeups=Wakeups per second; Cx=CPU Cx state (whether these three variables are provided or not  depend on different kernels)"
    print >>rep, "App1_CPU=Application 1 CPU%, App1_Mem=Application 1 Used Memory. Similar for App2_CPU, App2_Mem..."
    rep.close() 
if __name__=='__main__':
    logfile="./log/appmon.dat"
    report="./log/reporttmp.txt"
    finalreport="./log/report.txt"
    getval(logfile,report)
    tail(report)
    insertsep(report,finalreport)

