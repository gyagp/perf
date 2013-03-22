CFLAGS += -O2 -g -Wall -W -D_FORTIFY_SOURCE=2 -fstack-protector `pkg-config --cflags --libs glib-2.0` 

OBJS := appmon.o proc.o report.o

all: appmon


appmon:	$(OBJS) 
	mkdir -p ../bin
	gcc $(OBJS) -o appmon
	mv appmon ../bin

$(OBJS): appmon.h

clean:
	rm -rf ../bin *.o
	
