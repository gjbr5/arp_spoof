#include "hostinfo.h"
#include "hwaddress.h"
#include "session.h"
#include <iostream>
#include <list>
#include <map>
#include <signal.h>

int main(int argc, char *argv[])
{
    argc = 4;
    if (argc < 4 || argc & 1) {
        std::cout << "Usage: " << argv[0] << " <dev> <sender> <target> <sender> <target>...\n";
        return 0;
    }

    //    std::string *dev = argv[1];
    std::string dev = "ens33";

    PacketCtrl::init(dev);

    std::map<InetAddress, HostInfo> hosts;
    for (int i = 2; i < argc; i++) {
        InetAddress in = inet_addr(argv[i]);
        if (hosts.find(in) == hosts.end())
            hosts.emplace(in, HostInfo(in));
    }

    std::list<Session> sessions;
    for (int i = 2; i < argc; i += 2)
        //        sessions.push_back((Session(dev, inet_addr(argv[i]), inet_addr(argv[i + 1]))));
        sessions.push_back(Session(hosts.find(inet_addr(argv[i]))->second,
                                   hosts.find(inet_addr(argv[i + 1]))->second));

    for (std::list<Session>::iterator it = sessions.begin(); it != sessions.end(); it++)
        it->open();

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = PacketCtrl::terminate;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
    while (true)
        ;

    return 0;
}
