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

import gtk
from pygtk_chart import line_chart
from pygtk_chart.line_chart import Graph
import random
import math
from pygtk_chart import graphOptions
import getopt, sys
"""
try:
    opts,args =  getopt.getopt(sys.argv[1],"NI",["noidle"])
except getopt.GetoptError, exc:
    print("error argument")
for a in opts:
    if a in ('-NI'):
        NO_IDLE= "ture"
"""
def cb_chart_title(entry, chart):
    chart.title.set_text(entry.get_text())

logFile = "./log/appmon.dat"
window = gtk.Window()
window.connect("destroy", gtk.main_quit)
window.set_default_size(1000, 400)
window.set_title("AppMon v0.1")

box = gtk.HBox()
hboxCPU = gtk.HBox()
hboxMEM = gtk.HBox()
hboxPowerWU = gtk.HBox()
hboxPowerC = gtk.HBox()
hboxMEM_ALL = gtk.HBox()

chartCPU = line_chart.LineChart()
chartCPU.set_Y_label("Percent(%)")
chartCPU.set_X_label("Round")
chartCPU.title.set_text("AppMon Data Report: CPU")
hboxCPU.pack_start(chartCPU)

chartMEM = line_chart.LineChart()
chartMEM.set_Y_label("Size(Kb)")
chartMEM.set_X_label("Round")
chartMEM.title.set_text("AppMon Data Report: MEM")
hboxMEM.pack_start(chartMEM)

chartMEM_ALL = line_chart.LineChart()
chartMEM_ALL.set_Y_label("Size(kb)")
chartMEM_ALL.set_X_label("Round")
chartMEM_ALL.title.set_text("AppMon Data Report: System MEM")
hboxMEM_ALL.pack_start(chartMEM_ALL)

chartPowerWU = line_chart.LineChart()
chartPowerWU.set_Y_label("Count(n/s)")
chartPowerWU.set_X_label("Round")
chartPowerWU.title.set_text("AppMon Data Report: Power WakeUP")
hboxPowerWU.pack_start(chartPowerWU)

chartPowerC = line_chart.LineChart()
chartPowerC.set_Y_label("Percent(%)")
chartPowerC.set_X_label("Round")
chartPowerC.title.set_text("AppMon Data Report: Power C state")
hboxPowerC.pack_start(chartPowerC)

infile = open(logFile, "r")
lineList = infile.readlines()
acpiflag = lineList[-1].split()
acpiflag = acpiflag[-1]
cpuidleflag =  lineList[-2].split()
cpuidleflag = cpuidleflag[-1]
appNameList = lineList[-3].split()
maxCState = int(lineList[-4].split()[1])
maxValueList = lineList[-5].split()
avgValueList = lineList[-6].split()
infile.close()

sysCPU = line_chart.graph_new_from_file(logFile, "sysCPU", 0, 1)
sysCPU.set_title("System")
chartCPU.add_graph(sysCPU)

sysMEM = line_chart.graph_new_from_file(logFile, "sysMEM", 0, 3)
sysMEM.set_title("System")
chartMEM_ALL.add_graph(sysMEM)

sysPowerWakeUp = line_chart.graph_new_from_file(logFile, "sysPowerWakeUp", 0, 4)
sysPowerWakeUp.set_title("WakeUp")
chartPowerWU.add_graph(sysPowerWakeUp)

memGraph = []
powerCGraph = []
memALLGraph = [sysMEM]
cpuGraph = [sysCPU]
powerWUGraph = [sysPowerWakeUp]

cpuSysAvg = avgValueList[1]
memSysAvg = avgValueList[2] + " kb"
memPureSysAvg = avgValueList[3] + " kb"
wakeupAvg = avgValueList[4] + " n/s"
cAvg = []
cpuProcAvg = []
memProcAvg = []

index = 0
for i in range(0, maxCState+1):
    index = 5+i
    cStr = "C" + str(i)
    sysPowerC = line_chart.graph_new_from_file(logFile, "sysPower"+cStr, 0, index)
    sysPowerC.set_title(cStr)
    chartPowerC.add_graph(sysPowerC)
    powerCGraph.append(sysPowerC)
    cAvg.append(avgValueList[5+i] + " ms")

index +=1
procIndex = index
for appName in appNameList:
    if appName.find("#") < 0:
        appChart = line_chart.graph_new_from_file(logFile, appName, 0, index)
        appChart.set_title(appName)
        chartCPU.add_graph(appChart)   
        cpuGraph.append(appChart) 
        appChart = line_chart.graph_new_from_file(logFile, appName, 0, index+1)
        appChart.set_title(appName)
        chartMEM.add_graph(appChart)    
        memGraph.append(appChart)
        appChart1 = line_chart.graph_new_from_file(logFile, appName, 0, index+1)
        appChart1.set_title(appName)
        chartMEM_ALL.add_graph(appChart1)
        memALLGraph.append(appChart1)
        index = index + 2 

procName = []
for i in range(0, len(appNameList)-1):
    cpuProcAvg.append(avgValueList[procIndex+i*2])
    memProcAvg.append(avgValueList[procIndex+1+i*2]+ " kb")
    procName.append(appNameList[i+1])
sum = graphOptions.Summary()
sum.setProcName(procName)
sum.setCPU(cpuSysAvg, cpuProcAvg)
sum.setMEM(memSysAvg, memPureSysAvg, memProcAvg)
sum.setWU(wakeupAvg)
sum.setCState(cAvg)
memCtrl = graphOptions.GraphControl(sum, "MEM", memGraph)
memALLCtrl = graphOptions.GraphControl(sum, "MEMA", memALLGraph)
cpuCtrl = graphOptions.GraphControl(sum, "CPU", cpuGraph)
powerWUCtrl = graphOptions.GraphControl(sum, "WU", powerWUGraph)
powerCCtrl = graphOptions.GraphControl(sum, "CC", powerCGraph)

hboxMEM.pack_start(memCtrl, False, False)
hboxMEM_ALL.pack_start(memALLCtrl, False, False)
hboxCPU.pack_start(cpuCtrl, False, False)
hboxPowerWU.pack_start(powerWUCtrl, False, False)
hboxPowerC.pack_start(powerCCtrl, False, False)
 
notebook = gtk.Notebook()
notebook.append_page(hboxCPU, gtk.Label("CPU Usage"))
notebook.append_page(hboxMEM, gtk.Label("App MEM"))
if cpuidleflag == "1" :
    notebook.append_page(hboxPowerWU, gtk.Label("CPU WakeUP"))
    notebook.append_page(hboxPowerC, gtk.Label("CPU C State"))
notebook.append_page(hboxMEM_ALL, gtk.Label("System MEM"))

box.pack_start(notebook)

window.add(box)
window.show_all()
gtk.main()

