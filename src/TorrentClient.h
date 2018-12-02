#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include<string>
// #include "TorrentUtility.h"

class TorrentClient
{
    // private:
    // TorrentUtility * torrent_utility;

    public:
    int connect_to_server(std::string server_ip, int port_number);
    void get_file(int server_socket, std::string torrent_file_name, std::string destination_file);
    // void set_torrent_utility(TorrentUtility * tu);
};

#endif