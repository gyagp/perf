CC=g++
CFLAGS=-c -Wall -W -O2 -D_THREAD_SAFE
LDFLAGS= -lXtst -lXdamage -lXfixes -lpthread
SOURCES=main.cpp effectHunter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=effectHunter

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@(rm -rf $(EXECUTABLE) *.o)

