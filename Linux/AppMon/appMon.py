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

import getopt,sys,string,os,time,shutil,commands

ThisScript = "appMon"
DEBUG = ""
delay = 5
display = "true"
count = 20
processName = "appmon"
logPath= "./log"
logfile= logPath + "/appmon.log"
logfile1 = logPath + "/appmon.dat"
FLAT_DELAY = 5

def usage():
    print "Usage:"
    print "%s.py [-p processName[,processName1]] [-c count] [-d delay] [-D] [-x] [-h]" %ThisScript
    print "-h: Help; display this menu"
    print "-x: Debug mode; default off."
    print "-D: Not to display the data with chart; default to display."
    print "-p: The process name of the processes to be sampled; default is this tool self. There are long process names, you can only type the first 15 letters (eliminate the path) of these kind of process name, for example, a process named /usr/bin/1234567890123456789 , you can use 123456789012345 instead."
    print "-d: The delay time (unit: s) between two sampling counts; default is 5 s."
    print "-c: Total counts for sampling, only used for process sampling stage; default is 20."
    print "-----------------------------------------------------------------------------------------"
    print "%s is a tool used to collect and analyse performance data for your specified process." %ThisScript
    sys.exit(0)


# ##############################################################################
#                                 Main body                                   ##
# ##############################################################################
if __name__ == '__main__':
    try:
        opts, args = getopt.getopt(sys.argv[1:],"xhDd:c:p:",["x","help","D","delay=","count=","processName="])
    except getopt.GetoptError, exc:
        print "%s: %s" % (sys.argv[0], exc.msg)
        usage()
    for a, param in opts:
        if a in ('-x'):
            DEBUG = "-x"
        if a in ('-h','--help'):
            usage()
        if a in ('-D'):
            display = "false"
        if a in ('-d','--delay'):
            if (param[0]=="-"):
                usage()
            else:
                delay = param
        if a in ('-c','--count'):
            if (param[0]=="-"):
                usage()
            else:
                count = param
        if a in ('-p','--processName'):
            if (param[0]=="-"):
                usage()
            else:
                processName = param
    
    if os.path.exists(logfile):
        os.unlink(logfile)
    if os.path.exists(logfile1):
        os.unlink(logfile1)
    if os.path.exists(logPath) == False:
        os.mkdir(logPath)

# ##############################################################################
#                         Sampling for the specified processes                ##
# ##############################################################################
    print "================================================================================="
    print "[Info] Start collecting data after %d seconds\n" % (FLAT_DELAY)
    time.sleep(FLAT_DELAY)
    (status,output)= commands.getstatusoutput("./bin/appmon -p %s -o %s  -d %s -c %s %s "%(processName,logfile,delay,count,DEBUG))
 
    print "================================================================================="

# ##############################################################################
#                     Data comparation/analysis and generate report           ##
# ##############################################################################

os.system("python ./pyLib/genData.py")
os.system("python ./pyLib/uniform.py")
if (display == "true"):
    os.system("python ./pyLib/chartDisplay.py")
else:
    os.system("cat ./log/report.txt")

