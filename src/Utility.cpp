#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include "Utility.h"
#include "TorrentInfo.h"

using namespace std;

vector<string> split_string(string input_string, char delimiter)
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

bool file_exists(string file_name)
{
    int r_fd = open(file_name.c_str(), O_RDONLY);
    if(r_fd <= 0)
    {
        return false;
    }
    return true;
}

string get_torrent_file_name(string file_path)
{
    file_path = get_absolute_file_path(file_path);
    vector<string> path_split = split_string(file_path, '/');
    string file_name = path_split[path_split.size()-1];
    vector<string> name_split = split_string(file_name, '.');
    string absolute_file_name = name_split[0];
    string t_file_name = absolute_file_name+".mtorrent";
    string torrent_file_name = "";
    int start_index = 0;
    if(path_split[0].empty())
    {
        torrent_file_name+='/';
        start_index = 1;
    }
    int limit = path_split.size() -1;
    for(int i = start_index; i<limit; i++)
    {
        torrent_file_name += path_split[i] + "/";
    }
    torrent_file_name += t_file_name;
    return torrent_file_name;

}

size_t get_file_size(string file_path)
{
    struct stat statbuf;
    lstat(file_path.c_str(),&statbuf);
    return statbuf.st_size;
}

string get_absolute_file_path(string file_path)
{
    char f = file_path[0];
    if(f=='/')
    {
      return file_path;
    }
    string cd = getcwd(NULL,100*sizeof(char));
    cd+='/';
    file_path = cd + file_path;
    return file_path;
}

string get_hash_hex(unsigned char * hash)
{
    string hash_hex = "";
    for(int i=0; i<SHA_DIGEST_LENGTH; i++)
    {
      char temp[3];
      sprintf(temp,"%02x",hash[i]);
      hash_hex+=temp[0];
      hash_hex+=temp[1];
    }
    return hash_hex;
}

TorrentInfo get_torrent_info(string torrent_file_name)
{
    torrent_file_name = get_absolute_file_path(torrent_file_name);
    FILE * file_ptr = fopen(torrent_file_name.c_str(), "r");
    string line;
    ifstream infile(torrent_file_name.c_str());
    int line_no = 1;
    TorrentInfo ti;
    while(getline(infile, line))
    {
        if(line.empty())
        {
            continue;
        }
        if(line_no == 1)
        {
            vector<string> ip_split = split_string(line, ' ');
            ti.tracker_ips = ip_split;
        }
        else if(line_no == 2)
        {
            ti.file_name = line;
        }
        else if(line_no == 3)
        {
            size_t file_size = stoi(line);
            ti.file_size = file_size;
        }
        else if(line_no == 4)
        {
            vector<string> piece_hashes;
            vector<string> hash_split = split_string(line, ' ');
            for(int i=0; i<hash_split.size(); i++)
            {
                if(!hash_split[i].empty())
                {
                    piece_hashes.push_back(hash_split[i]);
                }
            }
            ti.piece_hashes = piece_hashes;
        }
        else if(line_no == 5)
        {
            ti.file_hash = line;
        }
        line_no++;
    }
    return ti;
}

string create_torrent_file(string file_path, vector<string> tracker_ips)
{
    int r_fd = open(file_path.c_str(),O_RDONLY);
    if(r_fd < 0)
    {
      cout<<"\nFile does not exist/No permissions on file\n";
      return NULL;
    }

    /*open torrent file to write*/
    mode_t mode = 0777 & ~umask(0);
    string torrent_file_name = get_torrent_file_name(file_path);
    cout<<endl<<torrent_file_name<<endl;
    int w_fd = open(torrent_file_name.c_str(),(O_WRONLY | O_CREAT | O_TRUNC),mode);
    if(w_fd < 0)
    {
      cout<<"\nCould not create torrent file\n";
      return NULL;
    }

    /*add tracket info */

    for(int i=0; i<tracker_ips.size(); i++)
    {
        write(w_fd, tracker_ips[i].c_str(), tracker_ips[i].length());
        int no_of_trackers = tracker_ips.size();
        if(i!=(no_of_trackers-1))
        {
            write(w_fd, " ", 1);
        }   
    }
    write(w_fd, "\n", 1);


    /*write file name into the torrent file*/
    write(w_fd, file_path.c_str(), file_path.length());
    write(w_fd, "\n", 1);


    /* get file size and convert to string and write to file*/
    size_t file_size_t = get_file_size(file_path);
    string file_size_string = to_string(file_size_t);
    write(w_fd, file_size_string.c_str(), file_size_string.length());
    write(w_fd, "\n", 1);


    /*get no of pieces*/
    size_t last_piece_size = file_size_t%PIECE_SIZE;
    int no_of_pieces = file_size_t/PIECE_SIZE;
    no_of_pieces++;
    if(last_piece_size==0)
    {
      last_piece_size = PIECE_SIZE;
      no_of_pieces--;
    }

    char buffer[PIECE_SIZE];
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, file_path.c_str(), file_path.length());
    bzero(buffer,PIECE_SIZE);
    size_t bytes_read = read(r_fd, buffer, PIECE_SIZE);
    while( bytes_read > 0 )
    {
      string temp_buff = buffer;
      unsigned char hash[SHA_DIGEST_LENGTH];
      bzero(hash,SHA_DIGEST_LENGTH);
      SHA1((unsigned const char *)buffer, bytes_read, hash);
      string hash_hex = get_hash_hex(hash);
      SHA1_Update(&ctx, buffer, bytes_read);
      write(w_fd, hash_hex.c_str(), hash_hex.length());
      write(w_fd, " ", 1);
      bzero(buffer,PIECE_SIZE);
      bytes_read = read(r_fd, buffer, PIECE_SIZE);
    }
    unsigned char full_hash[SHA_DIGEST_LENGTH];
    bzero(full_hash,SHA_DIGEST_LENGTH);
    SHA1_Final(full_hash, &ctx);
    string hash_hex = get_hash_hex(full_hash);
    write(w_fd,"\n",1);
    write(w_fd, hash_hex.c_str(), hash_hex.length());
    close(r_fd);
    close(w_fd);
    return torrent_file_name;
}

void print_torrent_info(TorrentInfo ti)
{
    cout<<"\nTracker IPs : ";
    for(int i=0; i<ti.tracker_ips.size(); i++)
    {
        cout<<ti.tracker_ips[i]<<" ";
    }
    cout<<endl;
    cout<<"\nFile name : "<<ti.file_name<<endl;
    cout<<"\nFile size : "<<ti.file_size<<endl;
    cout<<"\nPiece hashes : "<<endl;
    for(int i=0; i<ti.piece_hashes.size(); i++)
    {
        cout<<ti.piece_hashes[i]<<endl;
    }
    cout<<"\nFile hash : "<<ti.file_hash<<endl;
}

char * get_file_piece(TorrentInfo ti, string piece_hash)
{
    vector<string> piece_hashes = ti.piece_hashes;
    int i = 0;
    for( ; i<piece_hashes.size(); i++)
    {
        if(!piece_hashes[i].compare(piece_hash))
        {
            break;
        }
    }
    off_t offset = i*PIECE_SIZE;
    string file_name = ti.file_name;
    int r_fd = open(file_name.c_str(), O_RDONLY);
    if(r_fd < 0)
    {
        return NULL;
    }
    lseek(r_fd, offset, SEEK_SET);
    char * buff = (char *)calloc((PIECE_SIZE+1), sizeof(char));
    read(r_fd, buff, PIECE_SIZE);
    return buff;
}

