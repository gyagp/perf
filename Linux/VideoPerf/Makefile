CFLAGS += -O2 -g  -W -D_FORTIFY_SOURCE=2 -fstack-protector  `pkg-config --libs x11 xdamage` -pthread

OBJS := videoperf.o

all: videoperf

install: videoperf
	install -d $(DESTDIR)/usr/bin
	install -m 755 videoperf $(DESTDIR)/usr/bin

videoperf: $(OBJS)
	$(CC) -o videoperf $(OBJS) $(CFLAGS) 


clean:
	rm videoperf -f *~ *.o
	
