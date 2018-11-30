#ifndef TORRENTSERVER_H
#define TORRENTSERVER_H

#include <string>

class TorrentServer
{
    public:
    void start_torrent_server(std::string ip_address, int port_number);
    void handle_client(int client_socket);
};

#endif