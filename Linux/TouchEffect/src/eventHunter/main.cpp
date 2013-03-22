#include "eventHunter.h"
#include <iostream>

void printUsage();

int main(int argc, char ** argv)
{
    int count = 20;
    int posX, posY;
    
    int logLevel = 1;
    const char * eventInterface = NULL;
    std::string temp;

    for (int index=1; index < argc; index++) {
        temp = argv[index];
        if (temp[0] == '-' && temp.length()>1) {
            switch (temp[1]) {
                case 'i':
                    if (index +1 < argc) {
                        eventInterface = argv[index+1];
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'x':
                    logLevel = 2;
                    break;
                default:
                    break; 
            }
        }    
    }

    if (eventInterface == NULL) {
        printUsage();
        return 1;
    }

    EventHunter eventHunter(eventInterface, logLevel);

    //simHWEvent.simMoveREL(10, 10);
    //for (int index = 0; index < count; index++) {
    //    simHWEvent.simKeyEvent(BTN_LEFT, 1);
    //    simHWEvent.simKeyEvent(BTN_LEFT, 0);
    //    usleep(500000);
    //}

    eventHunter.readEvent(posX, posY);

    return 0;
}

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "eventHunter -i input_event_interface [-x]" << std::endl;
    std::cout << "-i set the input event interface for your specified device." << std::endl;
    std::cout << "-x output information for all the events." << std::endl;
    std::cout << "Example: eventHunter /dev/input/event3 (means starting eventHunter by hooking to /dev/input/event3)" << std::endl;
    std::cout << "___________________________________________________________________________________" << std::endl;
    std::cout << "eventHunter is a tool used to hook to one input device for event listening/sending." << std::endl;
}
