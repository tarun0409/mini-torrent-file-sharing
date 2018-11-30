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

#include "TorrentServer.h"
#include "Utility.h"

using namespace std;

void TorrentServer::handle_client(int client_socket)
{
    char buffer[255];
    bzero(buffer, 255);
    read(client_socket, buffer, 255);
    string file_name = buffer;

    
    if(file_exists(file_name))
    {
        string reply = "AVAILABLE";
        write(client_socket, reply.c_str(), reply.length());

        string torrent_file_name = get_torrent_file_name(file_name);
        TorrentInfo ti = get_torrent_info(torrent_file_name);

        string status = "open";

        while(status.compare("close"))
        {
            bzero(buffer, 255);
            read(client_socket, buffer, 255);
            string client_string = buffer;

            if(client_string.compare("close"))
            {
                string piece_hash = buffer;
                string reply = "AVAILABLE";
                write(client_socket, reply.c_str(), reply.length());
                bzero(buffer, 255);
                read(client_socket, buffer, 255);
                piece_hash = buffer;

                string log_string = "Hash requested : "+piece_hash;
                log(log_string);

                char * file_piece_ptr = get_file_piece(ti, piece_hash);
                if(file_piece_ptr == NULL)
                {
                    string status = "UNAVAILABLE";
                    write(client_socket, status.c_str(), status.length());
                } 
                string file_piece = file_piece_ptr;

                string size_string = to_string(file_piece.length());
                write(client_socket, size_string.c_str(), size_string.length());

                bzero(buffer, 255);
                read(client_socket, buffer, 255);
                reply = buffer;
                if(reply.compare("OK"))
                {
                    break;
                }
                write(client_socket, file_piece.c_str(), file_piece.length());

                string log_string = "Size of file sent : "+to_string(file_piece.length());
                log(log_string);
            }
            else
            {
                status = "close";
            }
        }
        close(client_socket);
    }
    else
    {
        string reply = "UNAVAILABLE";
        write(client_socket, reply.c_str(), reply.length());
    }

    string log_string = "Thread exiting peacefully";
    log(log_string);
}

void TorrentServer::start_torrent_server(string ip_address, int port_number)
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
        
        t =  thread(&TorrentServer::handle_client,this,client_socket);
    }
   
    close(init_socket);
    string log_string = "Server exiting peacefully";
    log(log_string);
}