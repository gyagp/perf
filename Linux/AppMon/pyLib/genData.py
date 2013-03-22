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

logFile = "./log/appmon.log"
outFile = "./log/appmon.dat"
frameStartKey = "[Frame"
frameEndKey = "[End"
memKey = "[mem"
cpuKey = "[cpu"
powerKey = "[power"
appKey = "[app"
sepKey = "[New Round"
maxAppKey = "[MaxAppNum"

maxValue = [0, 0, 0, 0]

index = 0
lineString = ""
maxApp = 0
appCount = 0
appNameList = ""

infile = open(logFile,"r")
outfile = open(outFile, "w")
outfile.writelines("# index CPU MemUsed MemUsedPure WakeUP C0...Cn APP1...APPn\n")
lineList = infile.readlines()

totalMemLine = lineList[4]
totalMem = int(totalMemLine.split()[2])

maxCLine = lineList[3]
maxCState = int(maxCLine.split()[2])

appNumLine = lineList[-2]
if appNumLine.find(maxAppKey) >= 0:
    maxApp = int(appNumLine.split()[1])

cpuidleLine = lineList[5]
if cpuidleLine.find("cpuidle_flag") >= 0:
    cpuidle_flag = cpuidleLine.split()[2]

acpiLine = lineList[6]
if acpiLine.find("acpi_flag") >= 0:
    acpi_flag = acpiLine.split()[2]

for i in range(0, maxCState+1):
    maxValue.append(0)
for i in range(0, maxApp):
    maxValue.append(0)
    maxValue.append(0)

memSysAvg = 0
memSysPAvg = 0
cpuSysAvg = 0
wakeupAvg = 0
cstateAvg = [0 for x in range(0,maxCState+1)] 
memAppAvg = [0 for x in range(0, maxApp)]
cpuAppAvg = [0 for x in range(0, maxApp)] 
appSampleCount = [0 for x in range(0, maxApp)]

mvIndex = 0
totalCPU = 0
sysCPU = 0
for line in lineList:
    if line.find(frameStartKey) >= 0:
        appCount = 0
        lineString = "" + str(index)
        index = index + 1
        mvIndex = 0
    if line.find(frameEndKey) >= 0:
        for i in range(appCount, maxApp):
            lineString = lineString + " 0 0"
        outfile.writelines(lineString+"\n")
    if line.find(memKey) >= 0:
        value = line.split()
        memUsed = totalMem - int(value[3])
        memSysAvg += memUsed
        memUsedPure = memUsed - int(value[5]) - int(value[7])
        memSysPAvg += memUsedPure
        if maxValue[mvIndex] < memUsed:
            maxValue[mvIndex] = memUsed
        if maxValue[mvIndex+1] < memUsedPure:
            maxValue[mvIndex+1] = memUsedPure
        mvIndex = mvIndex + 2
        lineString = lineString + " " + str(memUsed) + " " + str(memUsedPure)        
    if line.find(cpuKey) >= 0:
        value = line.split()
        sysCPU = float("%3.2f" % ((1-float(value[3])/float(value[5]))*100))
        cpuSysAvg += sysCPU
        totalCPU = int(value[5])
        if maxValue[mvIndex] < sysCPU:
            maxValue[mvIndex] = sysCPU
        mvIndex = mvIndex + 1
        lineString = lineString + " " + str(sysCPU)
    if line.find(powerKey) >= 0:
        value = line.split()
        if maxValue[mvIndex] < long(value[2]):
            maxValue[mvIndex] = long(value[2])
        mvIndex += 1
        lineString = lineString + " " + value[2]
        cstateTotal = 0.0
        for i in range(0, maxCState+1):
            cstateAvg[i] += long(value[4+2*i])
            cstateTotal += long(value[4+2*i])
            if maxValue[mvIndex+i] < long(value[4+2*i]):
                maxValue[mvIndex+i] = long(value[4+2*i])
        for i in range(0, maxCState+1):
            lineString = lineString + " " + str(float("%3.2f" % (float(value[4+2*i])/cstateTotal*100)))
        mvIndex = mvIndex + i +1
        wakeupAvg += int(value[2])
    if line.find(appKey) >= 0:
        value = line.split()
        y = float("%3.2f" % (float(value[5])/totalCPU*100))
        if y > sysCPU:
            y = sysCPU
            print "[Warning] Proc CPU overflow, limited to be sysCPU."
        if maxValue[mvIndex] < y:
            maxValue[mvIndex] = y
        if maxValue[mvIndex+1] < int(value[7]):
            maxValue[mvIndex+1] = int(value[7])
        cpuAppAvg[appCount] += y
        memAppAvg[appCount] += int(value[7])
        appSampleCount[appCount] += 1
        mvIndex = mvIndex + 2
        lineString = lineString + " " + str(y) + " " + value[7]
        appName = value[1] + ":" + value[2]
        if appNameList.find(appName) < 0:
            appNameList = appNameList + appName + " "
        appCount = appCount + 1

maxValueStr = ""
for s in maxValue:
    maxValueStr = maxValueStr + str(s) + " "

cpuSysAvg = float("%3.2f"% (float(cpuSysAvg)/index))
memSysAvg = float("%3.2f"% (float(memSysAvg)/index))
memSysPAvg = float("%3.2f"% (float(memSysPAvg)/index))
wakeupAvg = float("%3.2f"% (float(wakeupAvg)/index))
for i in range(0, maxCState+1):
    cstateAvg[i] = float("%3.2f"% (float(cstateAvg[i])/index))
avgString = "# " + str(cpuSysAvg) 
avgString += " " + str(memSysAvg) 
avgString += " " + str(memSysPAvg)
avgString += " " + str(wakeupAvg)
for i in range(0, maxCState+1):
    cstateAvg[i] = float("%3.2f"% (float(cstateAvg[i])/index))
    avgString += " " + str(cstateAvg[i])
for i in range(0, maxApp):
    cpuAppAvg[i] = float("%3.2f"% (float(cpuAppAvg[i])/appSampleCount[i]))
    memAppAvg[i] = float("%3.2f"% (float(memAppAvg[i])/appSampleCount[i]))
    avgString += " " + str(cpuAppAvg[i]) + " " + str(memAppAvg[i])

outfile.writelines(avgString + "\n")
outfile.writelines("# " + maxValueStr + "\n")
outfile.writelines("# " + str(maxCState) + "\n")
outfile.writelines("# " + appNameList + "\n")
outfile.writelines("# cpuidle_flag " + cpuidle_flag + "\n")
outfile.writelines("# acpi_flag " + acpi_flag + "\n")
infile.close()
outfile.close()
