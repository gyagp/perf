#ifndef __EVENTHUNTER_H
#define __EVENTHUNTER_H
#include <linux/input.h>
#include <unistd.h>
#include <string>

class EventHunter {
public:
    EventHunter(const char * eventInterface, int logLevel);
    ~EventHunter();
    void simMoveREL(int posX, int posY);
    void simKeyEvent(int key, int value);
    void readEvent(int & posX, int & posY);

private:
    int mEventFD;
    const int mLogLevel;
    const char * mEventInterface;

    void log(std::string pre, struct input_event & ev);
};

#endif
