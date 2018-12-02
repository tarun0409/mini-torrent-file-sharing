#ifndef TORRENTSERVER_H
#define TORRENTSERVER_H

#include <string>
// #include "TorrentUtility.h"

class TorrentServer
{
    // private:
    // TorrentUtility * torrent_utility;

    public:
    void start_torrent_server(std::string ip_address, int port_number);
    void handle_client(int client_socket);
    // void set_torrent_utility(TorrentUtility * tu);
};

#endif