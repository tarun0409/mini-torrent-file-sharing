#ifndef TORRENTTRACKER_H
#define TORRENTTRACKER_H

#include<string>
#include<mutex>
#include<vector>
#include<unordered_set>

typedef struct
{
    std::string file_name;
    std::unordered_set<std::string> ip_addresses;
}TrackerEntry;

class TorrentTracker
{
    private:
    std::mutex tracker_file_lock; 
    public:
    std::vector<TrackerEntry> get_tracker_entries();
    std::vector<TrackerEntry> save_tracker_entries(std::vector<TrackerEntry> tes);
    void handle_client(int client_socket);
    void start_tracker_server(int port_number);
};

#endif