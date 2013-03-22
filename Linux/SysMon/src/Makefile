CFLAGS += -O2 -g -Wall -W -I/usr/include/libxml2 `pkg-config --cflags --libs glib-2.0 libxml-2.0`

OBJS := sysmon.o proc.o report.o summary.o

all: sysmon


sysmon:	$(OBJS)
	gcc $(OBJS) $(CFLAGS) -o $@

$(OBJS): sysmon.h

clean:
	rm -rf ../bin *.o sysmon
	
