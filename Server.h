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
/**
 * SocketIO implement DefaultIO.
 * holds file description of a socket to read and write.
 */
class SocketIO : public DefaultIO {
  int fd;
 public:
  /**
   * constructor for SocketIO.
   * @param fd file descriptor of a socket
   */
  SocketIO(int fd) noexcept(false) {
      this->fd = fd;
  }
  /**
   * read single line from socket
   * @return string line received
   */
  string read() override {
      string line;
      char c = 0;
      //read from socket until encounter '\n' character
      recv(this->fd, &c, sizeof(char), 0);
      while (c != '\n') {
          line += c;
          recv(fd, &c, sizeof(char), 0);
      }
      return line;
  }
  /**
   * read from socket to float address
   * @param f address of a float
   */
  void read(float *f) override {
      string buf = read();
      *f = stof(buf);
  }
  /**
   * write string to socket
   * @param text string to write
   */
  void write(string text) override {
      send(fd, text.c_str(), text.size(), 0);
  }
  /**
   * write float to socket
   * @param f float to write
   */
  void write(float f) override {
      ostringstream oss;
      oss << f;
      string text(oss.str());
      write(text);
  }
};

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
      SocketIO dio(clientID);
      CLI cli(&dio);
      cli.start();
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
