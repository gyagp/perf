#
#  Copyright (C) 2010 Intel Corporation
# 
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
#  Authors:
#     Chen, Guobing <guobing.chen@intel.com>
# 

#!/bin/perl
use Getopt::Long;
use strict;

my $winId = 0;
my $winName = "qtGUI";

my $help = 0;
my $widget = 0;
my $winId = "UNSET";
my $mode = "TIMESTAMP";
my $gestureType = "PAN";
my $orientation = "B";
my $timeInterval = 10;
my $delay = 10;
my $sampleCount = 20;
if (@ARGV > 0) {
    GetOptions('w:s'=> \$widget,
               'wi:s'=> \$winId,
               't:i'=> \$timeInterval,
               'm:s'=> \$mode,
               'g:s'=> \$gestureType,
               'o:s'=> \$orientation,
               'd:i'=> \$delay,
               'c:i'=> \$sampleCount,
               'help|h'=> \$help);
}

if ($help) {
    &printUsage();
    exit();
}

if (!$widget && $winId eq "UNSET") {
    &printUsage();
    exit();
} 
elsif ($widget eq "qt") {
    $winName = "qtGUI";
}
elsif ($widget eq "meegotouch") {
    $winName = "meegotouchGUI";
}
else {
    $winName = $widget;
}

if ($gestureType eq "SWIPE") {
    $gestureType = '0';
} elsif ($gestureType eq "SWIPING") {
    $gestureType = '1';
} elsif ($gestureType eq "PAN") {
    $gestureType = '2';
} elsif ($gestureType eq "PANNING") {
    $gestureType = '3';
} elsif ($gestureType eq "PINCH") {
    $gestureType = '4';
} else {           # default value
    $gestureType = '2';
}

if ($orientation eq "B") {
    $orientation = 0;
} elsif ($orientation eq "L") {
    $orientation = 1;
} else {           # default value
    $orientation = 0;
}

if ($mode ne "FPS" && $mode ne "TIMESTAMP") {
        &printUsage();
        exit();
}

mkdir("log");

print(" Sampling $winName for $mode\n\n");

$ENV{"DISPLAY"} = ":0.0";

if ($widget eq "qt" || $widget eq "meegotouch") {
    chdir("./bin");
    system("./$winName &");
    chdir("../");
    sleep(5);
}

if ($winId eq "UNSET") {
    $winId = `xwininfo -name $winName | grep $winName | awk '{print \$4}'`;
    chop($winId);
}

if ($mode eq "FPS") {
    system("./bin/gestureSim -g $gestureType -o $orientation -t $timeInterval -d $delay > log/gestureSim.log &");
    sleep(4);
    system("./bin/effectHunter -w $winId -c $sampleCount");

    # Send stop signal(12, handled by gestureSim) to gestureSim to make it generate ESC key press event and then quit.
    # The ESC key will be accepted by build-in touch-supported widgets which will then quit.
    system("kill -s 12 `ps -ef | grep gestureSim | grep -v grep | awk '{print \$2}'`");
} elsif ($mode eq "TIMESTAMP") {
    system("./bin/effectHunter -t -w $winId >log/effectHunter.log &");
    sleep(1);
    my $efh_pid = `pidof effectHunter`;
    chomp($efh_pid);
    system("./bin/gestureSim -g $gestureType -o $orientation -t $timeInterval -d $delay -p $efh_pid > log/gestureSim.log");
    sleep(1);
    #system("kill -s 12 `ps -ef | grep effectHunter | grep -v grep | awk '{print \$2}'`");
 
    &processTSLog();
}

# Kill them if still exsiting
if ($widget eq "qt" || $widget eq "meegotouch") {
    system("kill -9 `ps -ef | grep $winName | grep -v grep | awk '{print \$2}'`");
}


###########################################################################################################################
# Main Stop Here                                                                                                          #
###########################################################################################################################

sub printUsage
{
    print ("Usage:\n");
    print ("perl touchEffect.pl <-w=WidgetName> [-t timeInterval] [-d eventDelay]\n");
    print ("option \"w\": The name of touch widget, two build-in ones: qt, meegotouch. You can also specify your own one.\n");
    print ("option \"wi\": The id (get with xwininfo) of touch widget. You can specify either the name or the id, and id is with high priority.\n");
    print ("option \"t\": The time interval between two gestures. Default is 10 ms\n");
    print ("option \"d\": The time delay between two events. Default is 10 ms.\n");
    print ("option \"m\": The mode of sampling: FPS, TIMESTAMP.\n");
    print ("option \"g\": The gesture type: SWIPE, SWIPING, PAN, PANNING, PINCH.\n");
    print ("option \"o\": The orientation of the gesture: B - Breadthways, L - Lengthways. Default is B.\n");
    print ("option \"c\": The sampling count of collection. Default is 20.\n");
    print ("touchEffect is a benchmark used for measuring graphics performance when playing gestures like panning, pinching and swiping.\n");
}

sub processTSLog()
{
    my $efh_start_s = 0;
    my $efh_start_us = 0;
    my $efh_stop_s = 0;
    my $efh_stop_us = 0;
    my $gts_start_s = 0;
    my $gts_start_us = 0;
    my $gts_stop_s = 0;
    my $gts_stop_us = 0;

    open(EFH_HANDLER, "<log/effectHunter.log") || die("Could not open log/effectHunter.log!");

    while(<EFH_HANDLER>) {
        my($line) = $_;
        chomp($line);
        if ($line =~ /Start Refresh at (\d+) - (\d+)/) {
            print $line."\n";
            $efh_start_s = $1;
            $efh_start_us = $2;
        } elsif ($line =~ /Stop Refresh at  (\d+) - (\d+)/) {
            print $line."\n";
            $efh_stop_s = $1;
            $efh_stop_us = $2;
        }
    }
    close(EFH_HANDLER);

    open(GTS_HANDLER, "<log/gestureSim.log") || die("Could not open log/gestureSim.log!");
    while(<GTS_HANDLER>) {
        my($line) = $_;
        chomp($line);
        if ($line =~ /Gesture started at (\d+) - (\d+)/) {
            print $line."\n";
            $gts_start_s = $1;
            $gts_start_us = $2;
        } elsif ($line =~ /Gesture ended at   (\d+) - (\d+)/) {
            print $line."\n";
            $gts_stop_s = $1;
            $gts_stop_us = $2;
        }
    }
    close(GTS_HANDLER);

    my $start_latency = ($efh_start_s-$gts_start_s) + ($efh_start_us - $gts_start_us)/1000000;
    my $stop_latency = ($efh_stop_s-$gts_stop_s) + ($efh_stop_us - $gts_stop_us)/1000000;
    
    print "\n -------------------------------------\n";
    print " Start latency: ".$start_latency." s\n";
    print " End latency:   ".$stop_latency." s\n";

}
