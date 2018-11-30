#include<iostream>
#include <vector>
#include <string>
#include "TorrentServer.h"
#include "Utility.h"

using namespace std;

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        cout<<"\nEnter the ip address and port\n";
        exit(1);
    }
    vector<string> ip_split = split_string(argv[1], ':');
    string ip_address = ip_split[0];
    int port_number = stoi(ip_split[1]);
    TorrentServer ts;
    ts.start_torrent_server(ip_address, port_number);
    return 0;
}