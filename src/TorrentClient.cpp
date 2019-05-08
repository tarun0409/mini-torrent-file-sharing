#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include "TorrentClient.h"
#include "Utility.h"
// #include "TorrentInfo.h"

using namespace std;

int TorrentClient::connect_to_server(string ip_address, int port_number)
{
    struct sockaddr_in server_ip;
    struct hostent * server;
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        string log_string = "Error while creating socket";
        log(log_string);
    }
    server = gethostbyname(ip_address.c_str());
    if(server==NULL)
    {
        string log_string = "No such host identified";
        log(log_string);
    }
    bzero((char *) &server_ip, sizeof(server_ip));
    server_ip.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *) &server_ip.sin_addr.s_addr, server->h_length);
    server_ip.sin_port = htons(port_number);
    if(connect(sock,(struct sockaddr *) &server_ip, sizeof(server_ip)) < 0)
    {
        string log_string = "Connection to server failed";
        log(log_string);
    }

    return sock;
}

void TorrentClient::get_file(int server_socket, string torrent_file_name, string destination_file)
{
    TorrentInfo ti = get_torrent_info(torrent_file_name);
    string file_name = ti.file_name;
    size_t file_size = ti.file_size;
    vector<string> piece_hashes = ti.piece_hashes;
    mode_t mode = 0777 & ~umask(0);
    int w_fd = open(destination_file.c_str(),(O_WRONLY | O_CREAT | O_TRUNC), mode);
    for(int i=0; i<file_size; i++)
    {
        write(w_fd, "0", 1);
    }
    close(w_fd);
    write(server_socket, file_name.c_str(), file_name.length());
    char buff[255];
    bzero(buff, 255);
    read(server_socket, buff, 255);
    string status = buff;
    if(status.compare("AVAILABLE"))
    {
        return;
        close(server_socket);
    }

    string log_string = "File is available";
    log(log_string);

    w_fd = open(destination_file.c_str(),O_WRONLY, mode);    
    for(int i=0; i<piece_hashes.size(); i++)
    {
        string log_string = "Requesting for hash : "+piece_hashes[i];
        log(log_string);

        write(server_socket, piece_hashes[i].c_str(), piece_hashes[i].length());
        bzero(buff, 255);
        read(server_socket, buff, 255);
        status = buff;
        if(status.compare("AVAILABLE"))
        {
            continue;
        }
        
        log_string = piece_hashes[i] + " is available";
        log(log_string);
        
        write(server_socket, piece_hashes[i].c_str(), piece_hashes[i].length());

        bzero(buff, 255);
        read(server_socket, buff, 255);
        int bytes_to_be_read = stoi(buff);

        string reply = "OK";
        write(server_socket, reply.c_str(), reply.length());

        off_t offset = i*PIECE_SIZE;
        lseek(w_fd, offset, SEEK_SET);

        for(int i=0; i<bytes_to_be_read; i++)
        {
            char single_buff[2];
            bzero(single_buff, 2);
            read(server_socket, single_buff, 1);
            write(w_fd, single_buff, 1);
        }

        log_string = "Received : "+to_string(bytes_to_be_read)+" B from the server";
        log(log_string);

    }
    string reply = "close";
    write(server_socket, reply.c_str(), reply.length());
    close(w_fd);
}

// void TorrentClient::set_torrent_utility(TorrentUtility * tu)
// {
//     this->torrent_utility = tu;
// }