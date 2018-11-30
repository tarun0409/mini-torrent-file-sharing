#ifndef TORRENTCLIENT_H
#define TORRENTCLIENT_H

#include<string>

class TorrentClient
{
    public:
    int connect_to_server(std::string server_ip, int port_number);
    void get_file(int server_socket, std::string torrent_file_name, std::string destination_file);
};

#endif