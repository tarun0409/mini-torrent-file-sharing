#ifndef TRACKERCLIENT_H
#define TRACKERCLIENT_H

#include <string>
#include <vector>

class TrackerClient
{
    public:
    std::vector<std::string> get_ip_addresses(int server_socket, std::string file_name);
    void add_ip_address(int server_socket, std::string file_name, std::string ip_address);
    int connect_to_tracker(std::string ip_address_string);

};


#endif