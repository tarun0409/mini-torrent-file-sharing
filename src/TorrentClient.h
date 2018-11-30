#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include<string>

class TorrentClient
{
    public:
    int connect_to_server(std::string server_ip, int port_number);
};

#endif