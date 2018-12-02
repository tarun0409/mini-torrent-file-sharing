#ifndef TORRENTUTILITY_H
#define TORRENTUTILITY_H

#include<mutex>
#include<vector>
#include<string>
#include "TorrentInfo.h"

class TorrentUtility
{
    private:
    std::mutex status_file_lock;
    std::vector<TorrentEntry *> merge_torrent_entries(std::vector<TorrentEntry *> te1, std::vector<TorrentEntry *> te2);

    public:
    std::vector<TorrentEntry *> get_torrent_entries();
    void save_torrent_entries(std::vector<TorrentEntry *> torrent_entries);
    std::vector<std::string> split_string(std::string input_string, char delimiter);
    void print_current_status();
};

#endif