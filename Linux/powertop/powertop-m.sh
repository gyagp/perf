#!/bin/bash
#
PNPLOGDIR=/usr/local/pnplog

# for powertop test
powertopLogDir="$PNPLOGDIR/powertop-logs"
[ ! -e $powertopLogDir ] && mkdir -p $powertopLogDir

mypowertop()
{
        powertop -t 60 --html >/dev/null 2>&1
        [ -f powertop*.html ] && mv powertop*.html $powertopLogDir/powertop-`date +%a%m%d%Y-%H%M%S`.html
}

rm -rf powertop*.html
sleep 60
mypowertop
sleep 20
mypowertop

sleep 200
for looptime in $(seq 1 8)
do
        echo $looptime
        mypowertop
        sleep 20
done

#Get the average value
function avg(){
  local data len
  data=$1
  len="0"
  for i in $data
  do
    sum=`awk 'BEGIN{printf '$sum'+'$i'}'`
    len=$(($len+1))
  done
  avg=`awk 'BEGIN{printf '$sum'/'$len'}'`
}

a=`grep "wakeups/second" $powertopLogDir/powertop-*.html | awk -F'<p>' '{print $2}' | awk -F',' '{print $1}'`
b=`echo "$a" | awk -F' ' '{print $1}'`
avg "$b"
echo -e "[wakeups]
value=$avg" |tee $powertopLogDir/powertop_wakeups.log
