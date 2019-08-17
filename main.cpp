#include "hostinfo.h"
#include "hwaddress.h"
#include "session.h"
#include <iostream>
#include <list>
#include <map>
#include <signal.h>
#include <unistd.h>

static std::list<Session> sessions;

void terminate(int)
{
    for (Session session : sessions) {
        session.close();
    }
    PacketCtrl::terminate();
}

int main(int argc, char *argv[])
{
    if (argc < 4 || argc & 1) {
        std::cout << "Usage: " << argv[0] << " <dev> <sender> <target> <sender> <target>...\n";
        return 0;
    }
    PacketCtrl::init(argv[1]);

    std::map<InetAddress, HostInfo> hosts;
    for (int i = 2; i < argc; i++) {
        InetAddress in = inet_addr(argv[i]);
        if (hosts.find(in) == hosts.end())
            hosts.insert(std::make_pair(in, HostInfo(in)));
    }

    for (int i = 2; i < argc; i += 2)
        sessions.push_back(Session(hosts.find(inet_addr(argv[i]))->second,
                                   hosts.find(inet_addr(argv[i + 1]))->second));

    for (std::list<Session>::iterator it = sessions.begin(); it != sessions.end(); it++)
        it->open();

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = terminate;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    pause();

    return 0;
}
