/*
 * Author: 208994285 Yehuda Schwartz
 * and 318960168 Avital Gololobov
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include "CLI.h"

using namespace std;


class ClientHandler {
 public:
  virtual void handle(int clientID) = 0;
};

class AnomalyDetectionHandler : public ClientHandler {
 public:
  /**
   * AnomalyDetectionHandler implement handle method by starting new CLI.
   * @param clientID file descriptor of client socket.
   */
  virtual void handle(int clientID) {
      DefaultIO *dio = new SocketIO(clientID);
      CLI *cli = new CLI(dio);
      cli->start();
  }
};

class Server {
  thread *t; // the thread to run the start() method in
  int fd;
  sockaddr_in server;
  sockaddr_in client;
  volatile bool run = true;
 public:
  Server(int port) throw(const char *);
  virtual ~Server();
  void start(ClientHandler &ch) throw(const char *);
  void stop();
};

#endif /* SERVER_H_ */
