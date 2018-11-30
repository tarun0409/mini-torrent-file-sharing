#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "TorrentClient.h"

using namespace std;

int TorrentClient::connect_to_server(string ip_address, int port_number)
{
    struct sockaddr_in server_ip;
    struct hostent * server;
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        cout<<"\nError while creating socket\n";
    }
    server = gethostbyname(ip_address.c_str());
    if(server==NULL)
    {
        cout<<"\nNo such host identified\n";
    }
    bzero((char *) &server_ip, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &server_ip.sin_addr.s_addr, server->h_length);
    server_ip.sin_port = htons(port_number);
    if(connect(sock,(struct sockaddr *) &server_ip, sizeof(server_ip)) < 0)
    {
        cout<<"\nConnection to server failed\n";
    }
    
    return sock;
}