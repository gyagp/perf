CC=g++
CFLAGS=-c -Wall -g -O3 -D_THREAD_SAFE  
LDFLAGS= 
SOURCES=main.cpp eventHunter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=eventHunter

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) *.o
