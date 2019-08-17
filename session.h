#ifndef SESSION_H
#define SESSION_H

#include "hostinfo.h"
#include "packetctrl.h"
#include "packetheader.h"
#include <iostream>
#include <string>

#include <mutex>
#include <thread>
#include <queue>

class Session
{
private:
    HostInfo sender, target;

    bool isOpened = false;

public:
    Session(HostInfo sender, HostInfo target);
    ~Session();

    void open();
    void close();

    std::string to_str();
    friend std::ostream &operator<<(std::ostream &os, Session &session);
};

#endif // SESSION_H
