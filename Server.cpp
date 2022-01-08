#include "Server.h"

Server::Server(int port) throw(const char *) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw "socket failed";
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        throw "bind failed";
    }
    if (listen(fd, 3) < 0) {
        throw "listen failure";
    }
}
void handleSigalarm(int sig) {}
void Server::start(ClientHandler &ch) throw(const char *) {
    t = new thread([&ch, this]() {
      signal(SIGALRM, handleSigalarm);
      socklen_t clientSize = sizeof(client);
      //loop thought all clients
      while (run) {
          alarm(5);
          int aClient = accept(fd, (struct sockaddr *) &client, &clientSize);
          if (aClient < 0) {
              throw "accept failure";
          }
          alarm(0);
          thread *t1 = new thread([&aClient, this, &ch]() {
            ch.handle(aClient);
            close(aClient);
          });
          t1->join();
      }
      close(fd);
    });
}

void Server::stop() {
    run = false;
    try {
        t->join();
    } catch (exception &exception) {}
}

Server::~Server() {
}


