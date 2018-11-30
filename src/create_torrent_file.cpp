#include <iostream>
#include <string>
#include "Utility.h"

using namespace std;

int main(int argc, char * argv[])
{
    if(argc < 3)
    {
        cout<<"\nEnter the file name\n";
        exit(1);
    }
    string file_name = argv[1];
    string tracker_ip_string = argv[2];
    vector<string> tracker_ips = split_string(tracker_ip_string, ',');
    create_torrent_file(file_name, tracker_ips);
    return 0;
}