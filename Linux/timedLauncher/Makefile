CC=g++
CFLAGS=-c -Wall -W -O2
LDFLAGS= -lXtst -lXdamage -lXfixes -lpthread ../utils/src/effectHunter/effectHunter.o ../utils/src/eventHunter/eventHunter.o
SOURCES=timedLauncher.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=timedLauncher

all: buildEFH buildEVH $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	@(rm -rf $(EXECUTABLE) *.o)
	@(cd ../utils/src/effectHunter && $(MAKE) clean && cd ../../../timedLauncher)
	@(cd ../utils/src/eventHunter && $(MAKE) clean && cd ../../../timedLauncher)


buildEFH:
	@(cd ../utils/src/effectHunter && $(MAKE) && cd ../../../timedLauncher)

buildEVH:
	@(cd ../utils/src/eventHunter && $(MAKE) && cd ../../../timedLauncher)
