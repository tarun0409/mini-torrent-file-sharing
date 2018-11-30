#include <iostream>
#include <string>
#include <vector>
#include "Utility.h"
#include "TorrentClient.h"

using namespace std;

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        cout<<"\n\nEnter ip and port\n";
        exit(1);
    }
    vector<string> ip_split = split_string(argv[1], ':');
    string ip_address = ip_split[0];
    int port_number = stoi(ip_split[1]);
    TorrentClient tc;
    int server_socket = tc.connect_to_server(ip_address, port_number);
    tc.get_file(server_socket, "torrent_files/file1.mtorrent", "output_files/file1.txt");
}