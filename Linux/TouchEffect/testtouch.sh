#!/bin/sh
#create by min for touch effect test
#if any question,please mail to me
#my Email: min.a.li@intel.com
#at 2011-01-19


default_arg="B"
pan="/tmp/pan.tmp"
s_time_f="/tmp/s_time_f"
e_time_f="/tmp/e_time_f"
test_time=8

USAGE="Usage: `basename $0` [-h] [-l] [-b]"

        case $1 in
                -l) default_arg="L";;
                -h) echo "-h $USAGE";;
                -b) default_arg="B";;
                *)  echo "use default value -b";;
        esac
#echo $default_arg


export DISPLAY=:0.0

echo "notice:please make sure you have set date!"
echo "Please click at your test widget!"
getid=`xwininfo | grep "id:" | awk {'print $4'}`
echo "get the ID:$getid"


echo "start ID:$getid FPS test, please wait"
perl touchEffect.pl -wi $getid -m FPS -g SWIPING -o $default_arg | tail -2
echo "sleep about 30s!"
sleep 30

looptime=0
s_time=0
e_time=0

while [ $looptime -lt $test_time ]
do
	perl touchEffect.pl -wi  $getid  -m TIMESTAMP -g PAN -o $default_arg >>$pan
	s_time=`cat $pan | tail -2 | grep "Start latency:" | awk {'print $3'}`
	e_time=`cat $pan | tail -2 | grep "End latency:" | awk {'print $3'}` 
	echo "the $looptime start time is $s_time"
	echo "the $looptime end time is $e_time"
	echo "$s_time" >>$s_time_f
	echo "$e_time" >>$e_time_f

	looptime=`expr $looptime + 1`
	sleep 30
done

echo "the final start time is:"
cat $s_time_f |awk -v COLUMN=n '{ sum += $COLUMN } END { print sum / NR }'
echo "the final end time is:"
awk -v COLUMN=n '{ sum += $COLUMN } END { print sum / NR }' $e_time_f


rm -rf $pan $s_time_f $e_time_f

#echo "start ID:$getid FPS test, please wait"
#perl touchEffect.pl -wi $getid -m FPS -g SWIPING -o $default_arg | tail -2


