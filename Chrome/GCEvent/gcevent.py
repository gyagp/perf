# /usr/bin/python
import sys
import os
import re

INDEXSTARTTIME = 0;
INDEXSIZE = 1;
INDEXREASON = 2;
INDEXENDTIME = 3;


def formatTime(time, n):
    return str(round(time, n));

def printResult(result):
    number = len(result);
    for i in range(0, number):
        print "Start: " + formatTime(float(result[i][INDEXSTARTTIME]) / 1000, 2) + "s";
        print "Duration: " + formatTime(float(result[i][INDEXENDTIME]) - float(result[i][INDEXSTARTTIME]), 3) + "ms";

        if int(result[i][INDEXSIZE]) > 1048576:
            print "Size: " + str('%.2f'%(float(result[i][INDEXSIZE]) / 1048576)) + "MB";
        elif int(result[i][INDEXSIZE]) > 1024:
            print "Size: " + str('%.2f'%(float(result[i][INDEXSIZE]) / 1024)) + "KB";
        else:
            print "Size: " + result[i][INDEXSIZE] + "B";

        print "Reason: " + result[i][INDEXREASON];

        print "=============================="; 

def analyze():
    if len(sys.argv) < 2:
        print "You must designate a file";
        return -1;
		
    file = open(sys.argv[1], 'r');

    patternStartTime = re.compile('"startTime":(\d+.\d+)');
    # Format is "startTime":1343964245648.726<,"stackTrace":[]>,"data":{"usedHeapSizeDelta":1091080,"reason":"idle notification: contexts disposed"},"endTime":1343964245653.7258,"type":"GCEvent","usedHeapSize":3725312,"totalHeapSize":9260416
    # stackTrace is optional
    patternExpect = re.compile('GCEvent');
    # Should not be greedy because there might be multiple GCEvent in a line
    pattern = re.compile('"startTime":(\d+.\d+),(?:"stackTrace":\[.*?\],)?"data":{"usedHeapSizeDelta":(\d+),"reason":(.*?)},"endTime":(\d+.\d+),"type":"GCEvent"');
    
    startTime = 0;
    gcResultRaw = [];
    gcResult = [];    
    
    while 1:
        line = file.readline();
        if not line:
            break;
        
        # get the overall starttime
        if startTime == 0:
            matchStartTime = patternStartTime.search(line);
            if matchStartTime:
                startTime = matchStartTime.group(1);
            
        
        matchExpect = patternExpect.findall(line);
        numberExpect = len(matchExpect);
        if numberExpect == 0:
            continue;
        
        match = pattern.findall(line);
        
        if len(match) != numberExpect:
            print "Pattern is not correct for following line, so there might be some GCEvent missed";
            print "The expected GCEvent number is " + str(numberExpect) + ", but the actual match is " + str(len(match));
            print line;
            return -1;

        for i in range(0, numberExpect):
            gcResultRaw.append([float(match[i][INDEXSTARTTIME]) - float(startTime), match[i][INDEXSIZE], match[i][INDEXREASON], float(match[i][INDEXENDTIME]) - float(startTime)]);

            #patternIdleNotification = re.compile('idle notification:');
            #if not patternIdleNotification.search(gcReasonTemp):
            #    print gcStartTimeTemp, gcSizeTemp, gcReasonTemp;
    
    numberTotal = len(gcResultRaw);
    if numberTotal == 0:
        return 0;

    index = 0;
    gcResult.append([gcResultRaw[index][INDEXSTARTTIME], float(gcResultRaw[index][INDEXSIZE]), gcResultRaw[index][INDEXREASON], gcResultRaw[index][INDEXENDTIME]]);
    for i in range(1, numberTotal):
        diff = abs(float(gcResult[index][INDEXENDTIME]) - float(gcResultRaw[i][INDEXSTARTTIME]));
        #print diff;
        if  diff < 0: # need to do merge
            gcResult[index][INDEXSIZE] = float(gcResult[index][INDEXSIZE]) + float(gcResultRaw[i][INDEXSIZE]);
            gcResult[index][INDEXREASON] += ", " + gcResultRaw[i][INDEXREASON]
            gcResult[index][INDEXENDTIME] = gcResultRaw[i][INDEXENDTIME];
            #print "merge happened";
        else:
            index += 1;
            gcResult.append([gcResultRaw[i][INDEXSTARTTIME], gcResultRaw[i][INDEXSIZE], gcResultRaw[i][INDEXREASON], gcResultRaw[i][INDEXENDTIME]]);
    
    #printResult(gcResultRaw);
    printResult(gcResult);

    file.close();    
    
if __name__ == "__main__":
    analyze();

    
