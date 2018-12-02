#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

#include <string>

class TrackerClient
{
    public:
    void add_ip_address(int server_socket, std::string file_name, std::string ip_address);
    int connect_to_tracker(std::string ip_address_string);

};


#endif