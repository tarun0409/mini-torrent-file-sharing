#include <iostream>
#include <string>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <sys/stat.h>
#include <unordered_set>
#include <mutex>
#include "Utility.h"
#include "TorrentTracker.h"


using namespace std;

vector<TrackerEntry> TorrentTracker::get_tracker_entries()
{
    // lock_guard<mutex> lock(tracker_file_lock);
    
    string log_string = "Locked on tracker_file_lock from get_tracker_entries";
    log(log_string);
    
    vector<TrackerEntry> tes;
    int rd = open("tracker_entries.txt", O_RDONLY);
    if(rd <= 0)
    {
        string log_string = "tracker_entries.txt does not exist. Returning an empty array";
        log(log_string);
        

        return tes;
    }

    log_string = "tracker_entries.txt does not exist. Returning an empty array";
    log(log_string);

    ifstream infile("tracker_entries.txt");
    string line;
    while(getline(infile, line))
    {
        vector<string> s = split_string(line, ' ');
        TrackerEntry te;
        te.file_name = s[0];
        vector<string> ips = split_string(s[1], ',');
        for(int i=0; i<ips.size(); i++)
        {
            string ip = ips[i];
            te.ip_addresses.insert(ip);
        }
        tes.push_back(te);
    }
    return tes;
}

vector<TrackerEntry> TorrentTracker::save_tracker_entries(vector<TrackerEntry> tes)
{
    lock_guard<mutex> lock(tracker_file_lock);

    string log_string = "Locked on tracker_file_lock from save_tracker_entries";
    log(log_string);
    
    mode_t mode = 0777 & ~umask(0);
    vector<TrackerEntry> gtes = get_tracker_entries();
    for(int i=0; i<tes.size(); i++)
    {
        int index = -1;
        for(int j=0; j<gtes.size(); j++)
        {
            TrackerEntry t1 = tes[i];
            TrackerEntry t2 = gtes[j];
            if(!t1.file_name.compare(t2.file_name))
            {
                index = j;
                break;
            }
        }
        if(index>=0)
        {
            for(unordered_set<string>::iterator it = tes[i].ip_addresses.begin(); it!=tes[i].ip_addresses.end(); ++it)
            {
                gtes[index].ip_addresses.insert(*it);
            }
        }
        else
        {
            gtes.push_back(tes[i]);
        }
    }

    log_string = "Going to write "+to_string(gtes.size())+"into tracker_entries.txt";
    log(log_string);

    int wd = open("tracker_entries.txt", (O_WRONLY | O_CREAT | O_TRUNC), mode);
    for(int i=0; i<gtes.size(); i++)
    {
        TrackerEntry te = gtes[i];
        write(wd, te.file_name.c_str(), te.file_name.length());
        write(wd, " ", 1);
        int s = te.ip_addresses.size();
        int j = 0;
        for(unordered_set<string>::iterator it = te.ip_addresses.begin(); it!=te.ip_addresses.end(); ++it)
        {
            string ip = *it;
            write(wd, ip.c_str(), ip.length());
            if(j!=(s-1))
            {
                write(wd, ",", 1);
            }
            j++;
        }
        write(wd, "\n", 1);
    }
    close(wd);
    return gtes;
}

void TorrentTracker::handle_client(int client_socket)
{
    char buffer[255];
    bzero(buffer, 255);
    read(client_socket, buffer, 255);
    string request = buffer;

    string log_string = "Request from client : "+request;
    log(log_string);

    if(!request.compare("add_entry"))
    {
        write(client_socket, "OK", 2);
        bzero(buffer, 255);
        read(client_socket, buffer, 255);
        string entry = buffer;

        log_string = "Entry from client : "+entry;
        log(log_string);
        
        vector<string> ent = split_string(entry, ';');
        TrackerEntry te;
        te.file_name = ent[0];
        te.ip_addresses.insert(ent[1]);
        vector<TrackerEntry> xxx;
        xxx.push_back(te);
        save_tracker_entries(xxx);
        write(client_socket, "OK", 2);
    }
    close(client_socket);
}

void TorrentTracker::start_tracker_server(int port_number)
{


    int init_socket,client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_length;
    init_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(init_socket < 0)
    {
        string log_string = "Error while creating file socket";
        log(log_string);
        exit(1);
    }
    bzero((char *)(&server_address), sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);
    if( bind(init_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 )
    {
        string log_string = "Unable to bind to file server address!";
        log(log_string);
        exit(1);
    }
    listen(init_socket,4);
    client_length = sizeof(client_address);
    int thread_count = 0;
    thread t;
    while( true )
    {
        client_socket = accept(init_socket, (struct sockaddr *) &client_address, &client_length);
        
        string log_string = "New connection received. Socket : "+to_string(client_socket);
        log(log_string);
        
        t =  thread(&TorrentTracker::handle_client,this, client_socket);
        t.detach();
    }
   
    close(init_socket);
    string log_string = "Server exiting peacefully";
    log(log_string);
}