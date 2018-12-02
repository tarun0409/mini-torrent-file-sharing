#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include "TrackerClient.h"
#include "Utility.h"

using namespace std;

void TrackerClient::add_ip_address(int server_socket, string file_name, string ip_address)
{
    write(server_socket, "add_entry", 9);
    char buff[255];
    bzero(buff, 255);
    read(server_socket, buff, 255);
    string reply = buff;

    string log_string = "Reply From tracker : "+reply;
    log(log_string);

    string str = file_name+";"+ip_address;
    write(server_socket, str.c_str(), str.length());

    bzero(buff, 255);
    read(server_socket, buff, 255);
    reply = buff;

    log_string = "Reply From tracker : "+reply;
    log(log_string);

    close(server_socket);
}

int TrackerClient::connect_to_tracker(string ip_address_string)
{
    vector<string> ip_split = split_string(ip_address_string, ':');
    string ip_address = ip_split[0];
    int port_number = stoi(ip_split[1]);
    struct sockaddr_in server_ip;
    struct hostent * server;
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        string log_string = "Error while creating socket";
        log(log_string);
    }
    server = gethostbyname(ip_address.c_str());
    if(server==NULL)
    {
        string log_string = "No such host identified";
        log(log_string);
    }
    bzero((char *) &server_ip, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &server_ip.sin_addr.s_addr, server->h_length);
    server_ip.sin_port = htons(port_number);
    if(connect(sock,(struct sockaddr *) &server_ip, sizeof(server_ip)) < 0)
    {
        string log_string = "Connection to server failed";
        log(log_string);
    }

    return sock;
}