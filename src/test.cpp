#include <iostream>
#include <string>
#include <vector>
#include "Utility.h"
#include "TorrentInfo.h"

using namespace std;

int main()
{
    string file_name = "../input_files/file1.txt";
    // vector<string> tracker_ips;
    // tracker_ips.push_back("127.0.0.1:7000");
    // tracker_ips.push_back("127.0.0.1:7002");
    // create_torrent_file(file_name, tracker_ips);
    TorrentInfo ti = get_torrent_info("../input_files/file1.mtorrent");
    print_torrent_info(ti);
    return 0;
}