CC=g++
CFLAGS=-c -Wall -g -O2 
LDFLAGS= 
SOURCES=main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=genFPS

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) *.o
