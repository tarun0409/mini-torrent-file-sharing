#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include <mutex>
#include <fstream>
#include <sys/stat.h>
#include "TorrentUtility.h"

using namespace std;


vector<string> TorrentUtility::split_string(string input_string, char delimiter)
{
    vector<string> output_vector;
    string curr_string = "";
    for(int i=0; i<input_string.length(); i++)
    {
        char curr_char = input_string[i];
        if(curr_char == delimiter)
        {
            output_vector.push_back(curr_string);
            curr_string = "";
        }
        else
        {
            curr_string+=curr_char;
        }
    }
    output_vector.push_back(curr_string);
    return output_vector;
}

vector<TorrentEntry *> TorrentUtility::merge_torrent_entries(vector<TorrentEntry *> te1, vector<TorrentEntry * > te2)
{
    vector<TorrentEntry *> merged_te;
    for(int i=0; i<te1.size(); i++)
    {
        for(int j=0; j<te2.size(); j++)
        {
            TorrentEntry * t1 = te1[i];
            TorrentEntry * t2 = te2[j];
            if(!t1->file_hash.compare(t2->file_hash))
            {
                if(t1->status == Status::SEEDING && t2->status == Status::LEECHING)
                {
                    merged_te.push_back(t1);
                }
                else if(t1->status == Status::LEECHING && t2->status == Status::SEEDING)
                {
                    merged_te.push_back(t2);
                }
                else if(t1->available_hashes.size() > t2->available_hashes.size())
                {
                    merged_te.push_back(t1);
                }
                else if(t1->available_hashes.size() < t2->available_hashes.size())
                {
                    merged_te.push_back(t2);
                }
                else
                {
                    merged_te.push_back(t1);
                }
            }
        }
    }

    for(int i=0; i<te1.size(); i++)
    {
        bool found = false;
        for(int j=0; j<merged_te.size(); j++)
        {
            TorrentEntry * t1 = te1[i];
            TorrentEntry * t2 = merged_te[j];
            if(t1->file_hash.compare(t2->file_hash))
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            merged_te.push_back(te1[i]);
        }
    }
    for(int i=0; i<te2.size(); i++)
    {
        bool found = false;
        for(int j=0; j<merged_te.size(); j++)
        {
            TorrentEntry * t1 = te2[i];
            TorrentEntry * t2 = merged_te[j];
            if(t1->file_hash.compare(t2->file_hash))
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            merged_te.push_back(te2[i]);
        }
    }
    return merged_te;
}

vector<TorrentEntry *> TorrentUtility::get_torrent_entries()
{
    lock_guard<mutex> lock(status_file_lock);

    vector<TorrentEntry *> read_entries;
    ifstream infile("torrent_entries.txt");
    if (infile) 
    {
        string line;
        while(getline(infile, line))
        {
            TorrentEntry * te = new TorrentEntry();
            vector<string> line_split = split_string(line, ' ');
            te->file_hash = line_split[0];
            te->torrent_file_path = line_split[1];
            int st = stoi(line_split[2]);
            switch(st)
            {
                case 0:
                te->status = Status::SEEDING;
                break;
                case 1:
                te->status = Status::LEECHING;
            }
            vector<string> hash_split = split_string(line_split[3], ',');
            te->available_hashes = hash_split;
            read_entries.push_back(te);
        }
    }
    return read_entries;
}

void TorrentUtility::save_torrent_entries(vector<TorrentEntry *> torrent_entries)
{
    lock_guard<mutex> lock(status_file_lock);
    vector<TorrentEntry *> final_entries;
    vector<TorrentEntry *> read_entries = get_torrent_entries();
    if(read_entries.empty())
    {
        final_entries = torrent_entries;
    }
    else
    {
        final_entries = merge_torrent_entries(read_entries, torrent_entries);
    }
    mode_t mode = 0777 & ~umask(0);
    int w_fd = open("torrent_entries.txt",(O_WRONLY | O_CREAT | O_TRUNC),mode);
    for(int i=0; i<final_entries.size(); i++)
    {
        TorrentEntry * te = final_entries[i];
        string file_hash = te->file_hash;
        write(w_fd, file_hash.c_str(), file_hash.length());
        write(w_fd, " ", 1);
        string torrent_file_path = te->torrent_file_path;
        write(w_fd, torrent_file_path.c_str(), torrent_file_path.length());
        write(w_fd, " ", 1);
        switch(te->status)
        {
            case Status::SEEDING:
            write(w_fd, "0", 1);
            break;
            
            case Status::LEECHING:
            write(w_fd, "1", 1);
        }
        write(w_fd, " ", 1);
        vector<string> hashes = te->available_hashes;
        for(int j=0; j<hashes.size(); j++)
        {
            string hash = hashes[j];
            write(w_fd, hash.c_str(), hash.length());
            if(j!=(hashes.size() - 1))
            {
                write(w_fd, ",", 1);
            }
        }
        if(i!=(final_entries.size() - 1))
        {
            write(w_fd, "\n", 1);
        }
    }
    close(w_fd);
}

void TorrentUtility::print_current_status()
{
    vector<TorrentEntry *> tes = get_torrent_entries();
    for(int i=0; i<tes.size(); i++)
    {
        TorrentEntry * te = tes[i];
        cout<<(i+1)<<". "<<te->torrent_file_path<<"  ";
        switch(te->status)
        {
            case Status::LEECHING:
            cout<<"LEECHING"<<endl;
            break;

            case Status::SEEDING:
            cout<<"SEEDING"<<endl;
        }
    }
    cout<<endl;
}