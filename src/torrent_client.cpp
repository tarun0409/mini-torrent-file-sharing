#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <string>
#include <thread>
#include "Utility.h"
#include "TorrentServer.h"
#include "TorrentClient.h"
#include "TrackerClient.h"
// #include "TorrentUtility.h"

using namespace std;

void start_server(string server_ip, int port_number)
{
    TorrentServer ts;
    // ts.set_torrent_utility(tu);
    
    string log_string = "Starting file sharing server at "+server_ip+":"+to_string(port_number);
    log(log_string);
    ts.start_torrent_server(server_ip,port_number);
}

void start_client(string server_ip, int port_number, string torrent_file, string destination_path)
{
    TorrentClient tc;
    // tc.set_torrent_utility(tu);
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
    // TorrentUtility * tu = new TorrentUtility();
    string server_ip_string = argv[1];
    vector<string> server_ip_split = split_string(server_ip_string, ':');
    string server_ip = server_ip_split[0];
    int port_number = stoi(server_ip_split[1]);

    vector<string> tracker_ips;
    tracker_ips.push_back(argv[2]);
    tracker_ips.push_back(argv[3]);

    thread server_thread = thread(&start_server, server_ip, port_number);

    string command = "enter";
    thread t;
    while(true)
    {
        // cout<<"$";
        getline(cin,command);
        if(!command.compare("exit"))
        {
            break;
        }
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

            TrackerClient tr;
            int ss = tr.connect_to_tracker(tracker_ips[0]);
            tr.add_ip_address(ss, file_name, server_ip_string);

            TorrentInfo ti = get_torrent_info(torrent_file_name);
            // TorrentEntry * te = new TorrentEntry();
            // te->file_hash = ti.file_hash;
            // te->available_hashes = ti.piece_hashes;
            // te->status = Status::SEEDING;
            // te->torrent_file_path = torrent_file_name;
            // vector<TorrentEntry *> v;
            // v.push_back(te);
            // tu->save_torrent_entries(v);

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
            TorrentInfo ti = get_torrent_info(torrent_file);
            string destination_file = command_split[2];
            string file_server = "";
            int port = -1;
            TrackerClient tc;
            int ss = tc.connect_to_tracker(tracker_ips[0]);
            string file_name = ti.file_name;
            vector<string> ips = tc.get_ip_addresses(ss, file_name);
            for(int i=0; i<ips.size(); i++)
            {
                string ip = ips[i];
                if(ip.compare(server_ip))
                {
                    vector<string> ip_split = split_string(ip, ':');
                    file_server = ip_split[0];
                    port = stoi(ip_split[1]);
                    break;
                }
            }
            if(file_server.empty())
            {
                file_server = server_ip;
                port = port_number;
            }

            t = thread(&start_client, file_server, port, torrent_file, destination_file);
            cout<<"\nStarted!\n";
        }
    }
}