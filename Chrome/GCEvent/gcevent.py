# /usr/bin/python
import sys
import os
import re

def analyze():
    if len(sys.argv) < 2:
        print "You must designate a file";
        return -1;
		
    file = open(sys.argv[1], 'r');

    patternStartTime = re.compile('"startTime":(\d+.\d+)');
    # Format is "startTime":1343964245648.726<,"stackTrace":[]>,"data":{"usedHeapSizeDelta":1091080},"endTime":1343964245653.7258,"type":"GCEvent","usedHeapSize":3725312,"totalHeapSize":9260416
    # stackTrace is optional
    patternExpect = re.compile('GCEvent');
    # Should not be greedy because there might be multiple GCEvent in a line
    pattern = re.compile('"startTime":(\d+.\d+),.*?"data":{"usedHeapSizeDelta":(\d+)},"endTime":(\d+.\d+),"type":"GCEvent"');
        
    startTime = 0;
    
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
            print "Pattern is not correct, so there might be some GCEvent is missed";
            return -1;

        for i in range(0, numberExpect):
            gcStartTimeTemp = "%.0f" % (float(match[i][0]) - float(startTime));
            gcSizeTemp = int(match[i][1]);
            gcDurationTemp = "%.3f" % (float(match[i][2]) - float(match[i][0]));
            print gcStartTimeTemp, gcSizeTemp, gcDurationTemp;
                
            
    file.close();    
    
if __name__ == "__main__":
    analyze();

    