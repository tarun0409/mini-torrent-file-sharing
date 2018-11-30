#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <string>
#include <thread>
#include "Utility.h"
#include "TorrentServer.h"
#include "TorrentClient.h"

using namespace std;

void start_server(string server_ip, int port_number)
{
    TorrentServer ts;
    
    string log_string = "Starting file sharing server at "+server_ip+":"+to_string(port_number);
    log(log_string);
    ts.start_torrent_server(server_ip,port_number);
}

void start_client(string server_ip, int port_number, string torrent_file, string destination_path)
{
    TorrentClient tc;
    int server_socket = tc.connect_to_server(server_ip, port_number);
    tc.get_file(server_socket, torrent_file, destination_path);
}

int main(int argc, char * argv[])
{
    if(argc < 4)
    {
        cout<<"\n\nEnter ip and port\n";
        exit(1);
    }
    string server_ip_string = argv[1];
    vector<string> server_ip_split = split_string(server_ip_string, ':');
    string server_ip = server_ip_split[0];
    int port_number = stoi(server_ip_split[1]);

    vector<string> tracker_ips;
    tracker_ips.push_back(argv[2]);
    tracker_ips.push_back(argv[3]);

    thread server_thread = thread(&start_server, server_ip, port_number);

    string command = "enter";
    
    while(command.compare("exit"))
    {
        cout<<"$";
        getline(cin,command);
        vector<string> command_split = split_string(command, ' ');
        string op_code = command_split[0];
        if(!op_code.compare("share"))
        {
            if(command_split.size() < 3)
            {
                cout<<"\n\nInvalid command. Please read the readme!";
                continue;
            }
            string file_name = command_split[1];
            string torrent_file_name = command_split[2];
            create_torrent_file(file_name, torrent_file_name, tracker_ips);

            cout<<"\nTorrent file create successfully\n";
        }
        else if(!op_code.compare("get"))
        {
            if(command_split.size() < 3)
            {
                cout<<"\n\nInvalid command. Please read the readme!";
                continue;
            }
            string torrent_file = command_split[1];
            string destination_file = command_split[2];
            
        }
    }
}