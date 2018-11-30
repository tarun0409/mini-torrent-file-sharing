#ifndef UTILITY_H
#define UTILITY_H

#include<string>
#include<vector>
#include "TorrentInfo.h"

std::string create_torrent_file(std::string file_path, std::vector<std::string> tracker_ips);
std::vector<std::string> split_string(std::string input_string, char delimiter);
std::string get_torrent_file_name(std::string file_path);
size_t get_file_size(std::string file_path);
std::string get_hash_hex(unsigned char * hash);
std::string get_absolute_file_path(std::string file_path);
TorrentInfo get_torrent_info(std::string torrent_file_name);
void print_torrent_info(TorrentInfo ti);
bool file_exists(std::string file_name);
std::string get_file_piece(TorrentInfo ti, std::string piece_hash);

#endif