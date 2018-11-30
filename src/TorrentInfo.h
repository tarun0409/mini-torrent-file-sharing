#ifndef TORRENTINFO_H
#define TORRENTINFO_H

#include<string>
#include<vector>
#define PIECE_SIZE 524288
#define HASH_LENGTH 2*SHA_DIGEST_LENGTH

typedef struct
{
    std::vector<std::string> tracker_ips;
    std::string file_name;
    size_t file_size;
    std::vector<std::string> piece_hashes;
    std::string file_hash;

}TorrentInfo;

#endif