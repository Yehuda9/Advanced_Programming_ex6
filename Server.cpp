#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "Server.h"

Server::Server(int port)throw (const char*) {
    fd = socket(AF_INET, SOCK_STREAM,0);
    if (fd<0){
        throw "socket failed";
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0){
        throw "bind failed";
    }
    if (listen(fd, 3)<0){
        throw "listen failure";
    }
}

void Server::start(ClientHandler& ch)throw(const char*){
    t=new thread([&ch, this] (){
        socklen_t clientSize = sizeof(client);
        //loop thought all clients
        //??????????????
        while (run){
            int aClient = accept(fd, (struct sockaddr*)&client, &clientSize);
            if (aClient < 0){
                throw "accept failure";
            }
            ch.handle(aClient);
            close(aClient);
            close(fd);
        }
    });
}

void Server::stop(){
	t->join(); // do not delete this!
    run = false;
}

Server::~Server() {
}

